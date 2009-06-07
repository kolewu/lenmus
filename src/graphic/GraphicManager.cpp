//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "GraphicManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/image.h"

#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../score/ObjOptions.h"
#include "../app/ScoreDoc.h"
#include "GraphicManager.h"
#include "AggDrawer.h"
#include "GMObject.h"
#include "BoxScore.h"
#include "BoxPage.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;



//-----------------------------------------------------------------------------------------

lmGraphicManager::lmGraphicManager()
{
    Create((lmScore*)NULL, (lmPaper*)NULL);
}

lmGraphicManager::lmGraphicManager(lmScore* pScore, lmPaper* pPaper)
{
    Create(pScore, pPaper);
}

void lmGraphicManager::Create(lmScore* pScore, lmPaper* pPaper)
{
    m_pScore = pScore;
    m_pPaper = pPaper;

    //initializations
    m_pBoxScore = (lmBoxScore*) NULL;
    m_rScale = 0;
    m_fReLayout = true;
    m_nLastScoreID = -1;
    m_xPageSize = 0;
    m_yPageSize = 0;
}

lmGraphicManager::~lmGraphicManager()
{
    if (!lmPRESERVE_SHAPES)     //BoxScore will be deleted when deleting the score
    {
        if (m_pBoxScore)      
            delete m_pBoxScore;
    }
    DeleteBitmaps();
    m_Selection.ClearSelection();
}

int lmGraphicManager::GetNumPages()
{
    if (m_pBoxScore)
        return m_pBoxScore->GetNumPages();
    else
        return 0;
}

void lmGraphicManager::Layout()
{
    // The goal of this method is to parse the internal representation of a score
	// (internal model, lmScore) and generate the graphical representation so that
	// it can be displayed or printed.
    // The result is a lmBoxScore object.

    if (!lmPRESERVE_SHAPES)     //BoxScore will be deleted when deleting the score
    {
        if(m_pBoxScore)
        {
            delete m_pBoxScore;
            m_pBoxScore = (lmBoxScore*) NULL;
        }
    }
    lmAggDrawer* pDrawer = new lmAggDrawer(m_xPageSize, m_yPageSize, m_rScale);
    m_pPaper->SetDrawer(pDrawer);
    m_pBoxScore = m_pScore->Layout(m_pPaper);
    m_fReLayout = false;
}

wxBitmap* lmGraphicManager::RenderScore(int nPage, int nOptions,
                                        wxWindow* pRenderWindow, wxPoint vOffset)
{
    //Renders page 1..n
    //Options are those defined for lmUpdateHint (see ScoreDoc.h)

    //get options
    bool fUseBitmaps = !(nOptions & lmNO_BITMAPS);
    bool fReDraw = nOptions & lmDO_ONLY_REDRAW;

    wxBitmap* pBitmap = (wxBitmap*)NULL;
    if (fUseBitmaps)
        pBitmap = GetPageBitmap(nPage);

    if (!pBitmap || fReDraw)
    {
        // anti-aliased renderization
        wxStopWatch oTimer;
        lmAggDrawer* pDrawer = new lmAggDrawer(m_xPageSize, m_yPageSize, m_rScale);
        m_pPaper->SetDrawer(pDrawer);
        wxASSERT(m_pBoxScore);  //Layout phase omitted?
        m_pBoxScore->RenderPage(nPage, m_pPaper, pRenderWindow, vOffset);
        oTimer.Pause();
        wxLogMessage(_T("[lmGraphicManager::RenderScore] %ld ms required for rendering page %d"),
                    oTimer.Time(), nPage);

        //Make room for the new bitmap
        //TODO

        //Add bitmap to the offscreen collection
        pBitmap = new wxBitmap(pDrawer->GetImageBuffer());
        AddBitmap(nPage, pBitmap);
    }
    return pBitmap;
}

void lmGraphicManager::PrepareForHighlight()
{
    // The score is going to be highlighted while played back.
	// Do here any needed preparatory work
}

bool lmGraphicManager::PrepareToRender(lmScore* pScore, lmLUnits paperWidth, lmLUnits paperHeight,
                               double rScale, lmPaper* pPaper, int nOptions)
{
    //This method informs GraphicManager about the common parameters (the score,
    //the paper, the scale, etc..) for a series of subsequent RenderScore()
    //invocations.
    //The GraphicManager must verify if all stored values are stil valid and, if not,
    //do whatever is necessary, i.e. delete invalid offscreen bitmaps.
    //Also, it must prepare anything that could be necessary, i.e. force a re-layout
    //of the score.

    //returns true if a re-alyout has been forced. This implies that the graphical
    //model has been rebuild and, therefore, all pointers to lmGMObjects are no
    //longer valid.



    //Is it necessary to force a re-layout?
    // Yes in following cases:
    // - the first time a score is going to be rendered
	// - if the score has been modified since last re-layout
    // - if paper size has changed and not re-layout prevented (option lmNO_RELAYOUT_ON_PAPER_RESIZE)
    // - if explicitly requested (option lmFORCE_RELAYOUT)
    bool fLayoutScore = !m_pScore || m_fReLayout || m_nLastScoreID != pScore->GetID()
				|| ( m_pScore->IsModified() && !(nOptions & lmDO_ONLY_REDRAW) )
                || (nOptions & lmFORCE_RELAYOUT)
                || ( (m_xPageSize != paperWidth || m_yPageSize != paperHeight) &&
                     !(nOptions & lmNO_RELAYOUT_ON_PAPER_RESIZE) );

    //Is it necessary to delete stored offscreen bitmaps?
    //Yes in following cases:
    // - if the scale (zooming factor) has changed
    // - if a re-layout will take place
    bool fDeleteBitmaps = (m_rScale != rScale) || fLayoutScore;

    //store received values
    m_pPaper = pPaper;
    m_rScale = rScale;
    m_xPageSize = (int)paperWidth;
    m_yPageSize = (int)paperHeight;
    m_pScore = pScore;
    m_nLastScoreID = m_pScore->GetID();
	m_pScore->SetModified(false);


    //re-layout the score if necesary
	if (fLayoutScore)
	{
		Layout();
		//m_pScore->SetModified(false);		//reset flag to avoid new relayouts until new changes
	}


    //delete existing offscreen bitmaps if necessary
    if (fDeleteBitmaps) DeleteBitmaps();

    //wxLogMessage(_T("[lmGraphicManager::PrepareToRender] fLayoutScore=%s, fDeleteBitmaps=%s, Hay BoxScore=%s"),
    //    (fLayoutScore ? _T("Yes") : _T("No")),
    //    (fDeleteBitmaps ? _T("Yes") : _T("No")),
    //    (m_pBoxScore ? _T("Yes") : _T("No")) );

    return fLayoutScore;
}

void lmGraphicManager::DeleteBitmaps()
{
    std::list<lmBitmapPage*>::iterator it = m_Bitmaps.begin();
    while (it != m_Bitmaps.end()) {
        delete (*it)->pBitmap;
        delete (*it);
        ++it;
    }
    m_Bitmaps.clear();
}

wxBitmap* lmGraphicManager::GetPageBitmap(int nPage)
{
    // nPage = 1 .. n
    // Get the bitmap for requested page, or NULL if no bitmap exits for that page.

    wxASSERT(nPage > 0);

    std::list<lmBitmapPage*>::iterator it = m_Bitmaps.begin();
    for (it = m_Bitmaps.begin(); it != m_Bitmaps.end(); ++it)
    {
        if ((*it)->nPage == nPage)
            return (*it)->pBitmap;
    }
    return (wxBitmap*)NULL;
}

wxBitmap* lmGraphicManager::NewBitmap(int nPage)
{
    // nPage = 1 .. n
    // Makes room for a new bitmap, for page nPage
    // and returns it (empty bitmap)

    //wxLogMessage(_T("[lmGraphicManager::NewBitmap] Page = %d"), nPage);

    //Make room for the new bitmap
    //TODO

    //Allocate the new bit map
    wxBitmap* pBitmap = new wxBitmap(m_xPageSize, m_yPageSize);
    //wxLogMessage(_T("[lmGraphicManager::NewBitmap] Allocated bitmap (%d, %d) pixels, %d bits/pixel. Size= %.02f MB"),
    //    m_xPageSize, m_yPageSize, pBitmap->GetDepth(), (double)((m_xPageSize * m_yPageSize * pBitmap->GetDepth())/8000000.) );
    if (!pBitmap || !pBitmap->Ok()) {
        if (pBitmap) {
            delete pBitmap;
            pBitmap = (wxBitmap *) NULL;
        }
        wxLogMessage(_T("[lmGraphicManager::NewBitmap] Bitmap size (%d, %d) pixels."), m_xPageSize, m_yPageSize);
        wxMessageBox(_("Sorry, not enough memory to create a Bitmap to display the page."),
            _T("lmGraphicManager::NewBitmap"), wxOK);
        ::wxExit();
    }

    // add the new bitmap to the list and store its size
    AddBitmap(nPage, pBitmap);

    return pBitmap;
}

void lmGraphicManager::AddBitmap(int nPage, wxBitmap* pBitmap)
{
    // nPage = 1 .. n
    // Adds the received bitmap to the list, associating it to page nPage

    //wxLogMessage(_T("[lmGraphicManager::AddBitmap] Page = %d"), nPage);

    //if a bitmap for that page already exists, remove it
    wxASSERT(nPage > 0);
    std::list<lmBitmapPage*>::iterator it;
    for (it = m_Bitmaps.begin(); it != m_Bitmaps.end(); ++it)
    {
        if ((*it)->nPage == nPage)
        {
            delete (*it)->pBitmap;
            delete *it;
            m_Bitmaps.erase(it);
            break;
        }
    }

    // add the new bitmap to the list and store its size
    lmBitmapPage* pBP = new lmBitmapPage;
    pBP->nPage = nPage;
    pBP->pBitmap = pBitmap;
    m_Bitmaps.push_back(pBP);

    m_xBitmapSize = m_xPageSize;
    m_yBitmapSize = m_yPageSize;
}

void lmGraphicManager::BitmapsToFile(wxString& sFilename, wxString& sExt, int nImgType)
{
    wxASSERT(nImgType == wxBITMAP_TYPE_BMP || nImgType == wxBITMAP_TYPE_JPEG
             || nImgType == wxBITMAP_TYPE_PNG || nImgType == wxBITMAP_TYPE_PCX
             || nImgType == wxBITMAP_TYPE_PNM);

    std::list<lmBitmapPage*>::iterator it = m_Bitmaps.begin();
    int i = 1;
    while (it != m_Bitmaps.end())
    {
        wxBitmap* pBitmap = (*it)->pBitmap;
        wxImage oImg = pBitmap->ConvertToImage();
        wxString sName = wxString::Format(_T("%s_%d.%s"),
                                sFilename.c_str(), (*it)->nPage, sExt.c_str());
        oImg.SaveFile(sName, nImgType);
        ++it;
        i++;
    }
}

void lmGraphicManager::ExportAsImage(wxString& sFilename, wxString& sExt, int nImgType)
{
    //Before invoking this method, PrepareToRender() must be invoked

    wxASSERT(nImgType == wxBITMAP_TYPE_BMP || nImgType == wxBITMAP_TYPE_JPEG
             || nImgType == wxBITMAP_TYPE_PNG || nImgType == wxBITMAP_TYPE_PCX
             || nImgType == wxBITMAP_TYPE_PNM);

    int i;
    for(i=1; i <= GetNumPages(); i++) {
        wxBitmap* pBitmap = RenderScore(i);
        wxImage oImg = pBitmap->ConvertToImage();
        wxString sName = wxString::Format(_T("%s_%d.%s"), sFilename.c_str(),
                                i, sExt.c_str());
        oImg.SaveFile(sName, nImgType);
    }
}

lmGMObject* lmGraphicManager::FindShapeAtPagePos(int nNumPage, lmUPoint uPos, bool fSelectable)
{
	if (!m_pBoxScore) return (lmGMObject*)NULL;

    lmBoxPage* pBPage = m_pBoxScore->GetPage(nNumPage);
    if (pBPage)
        return pBPage->FindShapeAtPos(uPos, fSelectable);
    else
        return (lmGMObject*)NULL;
}

lmBox* lmGraphicManager::FindBoxAtPagePos(int nNumPage, lmUPoint uPos)
{
    //look for most inner box (minimal size box: i.e. BoxSliceVStaff) that contains received point.
    //Returned box is smaller than lmBoxPage. If point is only found in lmBoxPage) NULL is returned.

	if (!m_pBoxScore)
        return (lmBox*)NULL;

    lmBoxPage* pBPage = m_pBoxScore->GetPage(nNumPage);
    lmBox* pBox = (lmBox*)NULL;
    if (pBPage)
    {
        pBox = pBPage->FindBoxAtPos(uPos);
        if (pBox == pBPage)
            return (lmBox*)NULL;
    }
    return pBox;
}

//lmGMSelection* lmGraphicManager::CreateSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
//                                                lmLUnits uYMin, lmLUnits uYMax)
//{
//    m_Selection.ClearSelection();
//    return AddToSelection(nNumPage, uXMin, uXMax, uYMin, uYMax);
//}


//lmGMSelection* lmGraphicManager::AddToSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
//                                               lmLUnits uYMin, lmLUnits uYMax)
//{
//	if (m_pBoxScore)
//    {
//        lmBoxPage* pBPage = m_pBoxScore->GetPage(nNumPage);
//        pBPage->AddToSelection(&m_Selection, uXMin, uXMax, uYMin, uYMax);
//    }
//    return &m_Selection;
//}

void lmGraphicManager::ClearSelection()
{
	if (!m_pBoxScore) return;

    m_pBoxScore->ClearSelection();
}

int lmGraphicManager::GetNumObjectsSelected()
{
	if (m_pBoxScore)
        return m_pBoxScore->GetNumObjectsSelected();
    else
        return 0;
}

void lmGraphicManager::NewSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                                              lmLUnits uYMin, lmLUnits uYMax)
{
	if (!m_pBoxScore) return;

    m_pBoxScore->ClearSelection();
    m_pBoxScore->AddToSelection(nNumPage, uXMin, uXMax, uYMin, uYMax);
}

void lmGraphicManager::NewSelection(lmGMObject* pGMO)
{
	if (!m_pBoxScore) return;

    m_pBoxScore->ClearSelection();
    pGMO->SetSelected(true);
}


//--------------------------------------------------------------------------------------------
// global functions related to rendering scores
//--------------------------------------------------------------------------------------------

wxBitmap GenerateBitmapForKeyCtrol(wxString& sKeyName, lmEKeySignatures nKey)
{
    //create a score with an invisible G clef and the key signature
    lmScore oScore;
    lmInstrument* pInstr = oScore.AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pInstr->GetVStaff();
    oScore.SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
    pVStaff->AddClef( lmE_Sol, 1, lmNO_VISIBLE );
    pVStaff->AddKeySignature(nKey);

	return GenerateBitmap(&oScore, sKeyName, wxSize(108, 64));
}

wxBitmap GenerateBitmapForClefCtrol(wxString& sClefName, lmEClefType nClef)
{
    //create a score with a clef
    lmScore oScore;
    lmInstrument* pInstr = oScore.AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pInstr->GetVStaff();
    oScore.SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
    pVStaff->AddClef( nClef );

	return GenerateBitmap(&oScore, sClefName, wxSize(108, 64), wxSize(0, 10));
}

wxBitmap GenerateBitmapForBarlineCtrol(wxString& sName, lmEBarline nBarlineType)
{
    //create a score with a barline
    lmScore oScore;
	oScore.SetOption(_T("Staff.DrawLeftBarline"), false);
    lmInstrument* pInstr = oScore.AddInstrument(0,0,_T(""));   //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pInstr->GetVStaff();
    oScore.SetTopSystemDistance( pVStaff->TenthsToLogical(20, 1) );     // 2 lines
	pVStaff->AddSpacer(40);
    pVStaff->AddBarline(nBarlineType);
	pVStaff->AddSpacer(40);

	return GenerateBitmap(&oScore, sName, wxSize(108, 64));
}

wxBitmap GenerateBitmap(lmScore* pScore, wxString& sName, wxSize size, wxSize shift)
{
	//use a graphic manager object to render the score in a memory DC
	//and return the bitmap
	// size - (in pixels) is the size of the bitmap
	// shift - (in pixels) is the shift to apply to the text

    //allocate a memory dc
    wxMemoryDC dc;
	wxBitmap bmp(size.x, size.y);

    //fill bitmap in white
    dc.SelectObject(bmp);
    dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

    // prepare and do renderization
    double rScale = 1.0 * lmSCALE;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( rScale, rScale );

    lmPaper oPaper;
    lmLUnits xLU = (lmLUnits)dc.DeviceToLogicalXRel(size.x);
    lmLUnits yLU = (lmLUnits)dc.DeviceToLogicalYRel(size.y);
    pScore->SetPageSize(xLU, yLU);

    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff *pVStaff = pInstr->GetVStaff();
    pScore->SetPageTopMargin(0.0f);
    pScore->SetPageLeftMargin( pVStaff->TenthsToLogical(15.0) );     //1.5 lines
    pScore->SetPageRightMargin( pVStaff->TenthsToLogical(15.0) );    //1.5 lines
    //oPaper.SetDrawer(new lmDirectDrawer(&dc));

    lmGraphicManager oGraphMngr(pScore, &oPaper);
    oGraphMngr.PrepareToRender(pScore, size.x, size.y, rScale, &oPaper,
                                lmFORCE_RELAYOUT);
    wxBitmap* pBitmap = oGraphMngr.RenderScore(1);
    wxASSERT(pBitmap && pBitmap->Ok());


        //write key signature name in black
    dc.SelectObject(*pBitmap);
    dc.SetMapMode(wxMM_TEXT);
    dc.SetUserScale(1.0, 1.0);

    int h, w;
    dc.SetPen(*wxBLACK);
    dc.SetFont(*wxNORMAL_FONT);
    dc.GetTextExtent(sName, &w, &h);
    dc.DrawText(sName, shift.x+(size.x-w)/2, shift.y+(size.y-h)/2 + h);


        //clean up and return new bitmap
    dc.SelectObject(wxNullBitmap);
    return *pBitmap;
}
