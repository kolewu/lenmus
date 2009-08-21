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
#pragma implementation "Processor.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#else
    #include <wx/sizer.h>
    #include <wx/panel.h>
#endif


#include "Processor.h"

// access to main frame
#include "../app/MainFrame.h"
extern lmMainFrame* GetMainFrame();

#include "toolbox/ToolsBox.h"
#include "ScoreDoc.h"
#include "../exercises/auxctrols/UrlAuxCtrol.h"
#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../score/AuxObj.h"        //lmScoreLine
#include "../graphic/ShapeNote.h"
#include "ScoreCommand.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;
#include "../auxmusic/ChordManager.h"
#include "../auxmusic/HarmonyExercisesData.h"

//-------------------------------------------------------------------------------------------
// Implementation of class lmScoreProcessor
//-------------------------------------------------------------------------------------------

//IDs for controls
const int lmID_DO_PROCESS = wxNewId();
const int lmID_UNDO_PROCESS = wxNewId();

IMPLEMENT_ABSTRACT_CLASS(lmScoreProcessor, wxEvtHandler)

lmScoreProcessor::lmScoreProcessor()
    : wxEvtHandler()
{
}

lmScoreProcessor::~lmScoreProcessor()
{
}

void lmScoreProcessor::DoProcess()
{
    //get the score
    lmMainFrame* pMainFrame = GetMainFrame();
    lmDocument* pDoc = pMainFrame->GetActiveDoc();
    if (!pDoc) return;
    lmScore* pScore = pDoc->GetScore();
    if (!pScore) return;

    //create and issue the command
    wxCommandProcessor* pCP = pDoc->GetCommandProcessor();
	pCP->Submit(new lmCmdScoreProcessor(lmCMD_NORMAL, pDoc, this) );
}

bool lmScoreProcessor::CreateToolsPanel(wxString sTitle, wxString sDoLink)
{
    lmMainFrame* pMainFrame = GetMainFrame();
    lmToolBox* pToolbox = pMainFrame->GetActiveToolBox();
    if (!pToolbox) return false;        //false -> failure

    m_pToolsPanel = new wxPanel(pToolbox, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                wxSUNKEN_BORDER|wxTAB_TRAVERSAL );

	m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    AddDoLink(m_pMainSizer, sDoLink);

    return true;
}
void lmScoreProcessor::AddDoLink(wxBoxSizer* pSizer, wxString sDoLink)
{
    // 'Do process' link
    if (sDoLink != wxEmptyString)
    {
        m_pDoLink = new lmUrlAuxCtrol(m_pToolsPanel, lmID_DO_PROCESS, 1.0, sDoLink);
        pSizer->Add(m_pDoLink, 0, wxALL|wxEXPAND, 5);
        m_pToolsPanel->Connect(lmID_DO_PROCESS, lmEVT_URL_CLICK,
                               wxObjectEventFunction(&lmScoreProcessor::DoProcess),
                               NULL, this);
    }
}

void lmScoreProcessor::RealizePanel()
{
    m_pToolsPanel->SetSizer( m_pMainSizer );
	m_pToolsPanel->Layout();
	m_pMainSizer->Fit( m_pToolsPanel );

    lmMainFrame* pMainFrame = GetMainFrame();
    lmToolBox* pToolbox = pMainFrame->GetActiveToolBox();
    if (pToolbox)
        pToolbox->AddSpecialTools(m_pToolsPanel, this);
}


//-------------------------------------------------------------------------------------------
// Implementation of class lmTestProcessor
//-------------------------------------------------------------------------------------------
#ifdef __WXDEBUG__

IMPLEMENT_DYNAMIC_CLASS(lmTestProcessor, lmScoreProcessor)

lmTestProcessor::lmTestProcessor()
    : lmScoreProcessor()
{
}

lmTestProcessor::~lmTestProcessor()
{
}

bool lmTestProcessor::SetTools()
{
    //Create a panel with the exercise buttons, and place it on the ToolBox
    //Returns false if failure.

    return true;
}

bool lmTestProcessor::ProcessScore(lmScore* pScore)
{
    //This method checks the score and show errors
    //Returns true if it has done any change in the score

	bool fScoreModified = false;

	//Get the instrument
	lmInstrument* pInstr = pScore->GetFirstInstrument();
	lmVStaff* pVStaff = pInstr->GetVStaff();

    //notes to join with arrows
	int nNote = 0;
    lmNote* pNote1;
    lmNote* pNote2;
    lmNote* pNote3;
    lmNote* pNote4;
    lmNote* pNote5;
    lmNote* pNote6;

	// Loop to process notes/rests in first staff of first instrument
	lmSOIterator* pIter = pVStaff->CreateIterator();
	while(!pIter->EndOfCollection())
	{
	  	lmStaffObj* pSO = pIter->GetCurrent();
	  	if (pSO->IsNote())
	  	{
            // It is a note. Count it
			++nNote;
            if (nNote == 2)
                pNote1 = (lmNote*)pSO;
            else if (nNote == 6)
                pNote2 = (lmNote*)pSO;
            else if (nNote == 3)
                pNote3 = (lmNote*)pSO;
            else if (nNote == 7)
                pNote4 = (lmNote*)pSO;
            else if (nNote == 5)
                pNote5 = (lmNote*)pSO;
            else if (nNote == 12)
                pNote6 = (lmNote*)pSO;

		}
		pIter->MoveNext();
	}
	delete pIter;       //Do not forget this. We are not using smart pointers!

    //Add line between pairs of notes

    if (pNote1 && pNote2)
    {
        DrawArrow(pNote1, pNote2, *wxRED);
	    fScoreModified = true;
    }
    if (pNote3 && pNote4)
    {
        DrawArrow(pNote3, pNote4, wxColour(0, 170, 0) );
	    fScoreModified = true;
    }
    if (pNote5 && pNote6)
    {
        DrawArrow(pNote5, pNote6, wxColour(0, 0, 200) );
	    fScoreModified = true;
    }

	return fScoreModified;      //true -> score modified
}

void lmTestProcessor::DrawArrow(lmNote* pNote1, lmNote* pNote2, wxColour color)
{
    //get VStaff
    lmVStaff* pVStaff = pNote1->GetVStaff();

    //get note heads positions
    lmURect uBounds1 = pNote1->GetNoteheadShape()->GetBounds();
    lmURect uBounds2 = pNote2->GetNoteheadShape()->GetBounds();

    //start point
    lmUPoint uStart( uBounds1.GetWidth(), 0);
    uStart.y = pNote1->GetShiftToNotehead();        //center of notehead

    //end point
    lmUPoint uEnd(uBounds2.GetRightTop() - uBounds1.GetRightTop());
    uEnd.y += uStart.y;

    //convert to tenths
    lmTenths xtStart = pVStaff->LogicalToTenths(uStart.x) + 8.0;
    lmTenths ytStart = pVStaff->LogicalToTenths(uStart.y);
    lmTenths xtEnd = pVStaff->LogicalToTenths(uEnd.x) - 8.0;
    lmTenths ytEnd = pVStaff->LogicalToTenths(uEnd.y);

    //create arrow
    pNote1->AttachLine(xtStart, ytStart, xtEnd, ytEnd, 2, lm_eLineCap_None,
                       lm_eLineCap_Arrowhead, lm_eLine_Solid, color);
    pNote1->SetColour(color);
    pNote2->SetColour(color);
}
#endif


//-------------------------------------------------------------------------------------------
// Implementation of class lmHarmonyProcessor
//-------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(lmHarmonyProcessor, lmScoreProcessor)

lmHarmonyProcessor::lmHarmonyProcessor()
    : lmScoreProcessor()
{
  pBoxSize = new wxSize(400, 60);
  pErrorBoxSize = new wxSize(510, 60);

  tFont.sFontName = _("Comic Sans MS");
  tFont.nFontSize = 6;
  tFont.nFontStyle = wxFONTSTYLE_NORMAL;
  tFont.nFontWeight = wxFONTWEIGHT_NORMAL;

 // Text box for general information
 // -200 --> Box X position shifted to left
 //  400 --> Initial Box Y position at the bottom
 //    0 --> Line X end position: centered with chord
 //  100 --> Line Y end position: slightly shifted down
  pInfoBox = new ChordInfoBox(pBoxSize, &tFont, &m_markup, -200, 500, 0, 100, -50);

 // Text box for errror information
 // -200 --> Box X position shifted to left
 // -200 --> Initial Box Y position at the TOP
 //    0 --> Line X end position: centered with chord
 //  100 --> Line Y end position: slightly shifted down
 //  +50 --> Increment y position after each use --> go downwards
  pChordErrorBox = new ChordInfoBox(pErrorBoxSize, &tFont, &m_markup, -150, -200, 0, 100, +50);
}

lmHarmonyProcessor::~lmHarmonyProcessor()
{
    delete pBoxSize;
    delete pErrorBoxSize;
    delete pInfoBox;
    delete pChordErrorBox;

    //delete all markup pairs
    std::list<lmMarkup*>::iterator it = m_markup.begin();
    int nCount = 0;
    while (it != m_markup.end())
    {
        lmMarkup* pError = *it;
        m_markup.erase(it++);
        delete pError;
        nCount++;
    }

    assert(nNumChords<lmMAX_NUM_CHORDS);
    for (int i = 0; i <nNumChords; i++)
        delete tChordDescriptor[i].pChord;

    wxLogMessage(_T("TODO: remove (debug) @@@@@@ ~lmHarmonyProcessor completed; deleted %d markups")
    , nCount);
}

void lmHarmonyProcessor::ResetChordDescriptor()
{
    assert(nNumChords<lmMAX_NUM_CHORDS);
    for (int i = 0; i <nNumChords; i++)
        delete tChordDescriptor[i].pChord;

    nNumChords = 0;
}

bool lmHarmonyProcessor::SetTools()
{
    //Create a panel with the exercise buttons, and place it on the ToolBox
    //Returns false if failure.

    if (!CreateToolsPanel(_("Harmony exercise"),
                          _("Check harmony")) )
        return false;

    //No more tools to add. Show panel in toolbox
    RealizePanel();
    return true;
}


// All chord processing:
//  analysis of chord notes and intervals
//  chord creation
//  results: display messages...
//  TODO: consider to improve return: status/reason
bool lmHarmonyProcessor::ProccessChord(lmScore* pScore, lmChordDescriptor* ptChordDescriptorArray
                                       , int* pNumChords, wxString &sStatusStr)
{
    bool fOk = false;
    bool fCanBeCreated = false;
    lmChordDescriptor* ptChordDescriptor = &ptChordDescriptorArray[*pNumChords]; //@@@ TODO MEJORAR...
    int nNumChordNotes = ptChordDescriptor->nNumChordNotes;

    if (nNumChordNotes < 3)
       return false;

    wxLogMessage(_T("ProccessChord %d, num CHORD NOTES: %d")
    , *pNumChords, nNumChordNotes);

    lmChordInfo tChordInfo;
    tChordInfo.Initalize();

    assert(ptChordDescriptor != NULL);
    assert(ptChordDescriptor->pChordNotes != NULL);

    // Create Chord
    fCanBeCreated = TryChordCreation(nNumChordNotes, ptChordDescriptor->pChordNotes, &tChordInfo,  sStatusStr);
    
    wxColour colour;

    if (fCanBeCreated)
    {
        lmNote* pChordBaseNote = ptChordDescriptor->pChordNotes[0]; // Even with inversions, the first note is the root
        ptChordDescriptor->pChord = new lmChordManager(pChordBaseNote, tChordInfo);


        wxString sStatusStr = wxString::Format(
                _("Chord %d: %s"),(*pNumChords)+1,  ptChordDescriptor->ToString() );
        // todo: remove: sStatusStr = ptChordDescriptor->ToString();
        (*pNumChords)++;
// todo: set definitive colour       colour = *wxGREEN;
        colour = wxColour(10,255,0,128); // R, G, B, Transparency
        // todo: consider, no necessary to display good chords?
        pInfoBox->DisplayChordInfo(pScore, ptChordDescriptor, colour, sStatusStr);
        fOk = true;
    }
    else
    {
       // Even with errors, the chord is created and used for analysis of progression  (TODO: confirm this)
        lmNote* pChordBaseNote = ptChordDescriptor->pChordNotes[0];
        ptChordDescriptor->pChord = new lmChordManager(pChordBaseNote, tChordInfo);
        sStatusStr = ptChordDescriptor->ToString();
        (*pNumChords)++;
// todo: set definitive colour       colour = *wxRED;
        colour = wxColour(255,0,0,128); // R, G, B, Transparency
        pInfoBox->DisplayChordInfo(pScore, ptChordDescriptor, colour, sStatusStr);
        fOk = false;
    }

    wxLogMessage(sStatusStr);



    return fOk;

}

bool lmHarmonyProcessor::ProcessScore(lmScore* pScore)
{
    //This method checks the score and show errors
    //Returns true if it has done any change in the score

    //As an example, I will put red and a green lines pointing to fourth and
    //sixth notes, respectively, and add some texts
    bool fScoreModified = false;
    ResetChordDescriptor();

    float rAbsTime = 0.0f;
    float rTimeAtStartOfMeasure = 0.0f;

    //Get the instrument
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    lmVStaff* pVStaff = pInstr->GetVStaff();

	lmNote* pCurrentNote;
    float rCurrentNoteAbsTime = -2.0f;
    float rRelativeTime = -2.0f;
    wxString sStatusStr;

    // TODO: improve postioning of the textbox...
    pInfoBox->ResetPosition();

    /*---

      Algorithm of chord detection, keeping a list of "active notes"

       When NEW NOTE
	      if NEW TIME (i.e. higher) then
             update current list of active notes: remove notes with end-time < TIME
             analize possible chord in previous state: with current list of active notes
             set new time
          add new note to the list of active notes

    ---*/

    // loop to process notes/rests in first staff of first instrument
    int nNote = 0;
    lmSOIterator* pIter = pVStaff->CreateIterator();
    while(!pIter->EndOfCollection())
    {
        // process only notes, rests and barlines
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsBarline())
        {
            // new measure starts. Update current time
            rTimeAtStartOfMeasure += pSO->GetTimePos();
            rAbsTime = rTimeAtStartOfMeasure;
        }
        else if (pSO->IsNoteRest())
        {
            // we continue in previous measure. Update current time if necessary
            rRelativeTime = pSO->GetTimePos();
            rAbsTime = rTimeAtStartOfMeasure + rRelativeTime;

            // process notes
            if (pSO->IsNote())
            {
                // It is a note. Count it
                ++nNote;
			    pCurrentNote  = (lmNote*) pSO;

                // calculate note's absolute time
                rCurrentNoteAbsTime = rTimeAtStartOfMeasure + rRelativeTime;

                // check new starting time (to analyze previous chord candidate) 
                if (  IsHigherTime(rCurrentNoteAbsTime, ActiveNotesList.GetTime())  )
                {
                    /*-----
                      if NEW-TIME (i.e. higher) then
                         update current list of active notes: remove notes with end-time < CURRENT-TIME
                         analize possible chord in previous state: with current list of active notes
                         set new time
                      add new note to the list of active notes
                    ---*/

               
                    // analyze possible chord with current list of active notes
                    ActiveNotesList.GetChordDescriptor(&tChordDescriptor[nNumChords]);

                    bool fChordOk = ProccessChord(pScore, tChordDescriptor, &nNumChords, sStatusStr);

                    // set new time and recalculate list of active notes
                    ActiveNotesList.SetTime( rCurrentNoteAbsTime );

                    fScoreModified = true; // repaint
                }

                // add new note to the list of active notes
                ActiveNotesList.AddNote(pCurrentNote, rCurrentNoteAbsTime + pCurrentNote->GetDuration());

            } //  if (pSO->IsNote())

        } // [else if (!pSO->IsNoteRest()) ] ignore other staff objects

        pIter->MoveNext();

    } // while
    delete pIter;       //Do not forget this. We are not using smart pointers!

    // Analyze the remaining notes
    // 
    ActiveNotesList.RecalculateActiveNotes( );
    ActiveNotesList.GetChordDescriptor(&tChordDescriptor[nNumChords]);
    bool fChordOk = ProccessChord(pScore, tChordDescriptor, &nNumChords, sStatusStr);

    int nNumErrors = AnalyzeChordsLinks(&tChordDescriptor[0], nNumChords);

    wxLogMessage(_T("ANALYSIS RESULT of %d chords:  ok: %d, Num Errors:%d ")
        ,nNumChords, fChordOk, nNumErrors);

    if (fChordOk && nNumErrors == 0)
    {
       wxString sOkMsg = _T(" Harmony is OK.");
       wxLogMessage( sOkMsg );
       pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nNumChords-1], *wxGREEN, sOkMsg );
       fScoreModified = true; // repaint
    }

    // todo: where to locate the box?  restart: pChordErrorBox->SetYPosition(-200);

    ////////////////////////////
    //
    // Exercise specific checks
    //
    ////////////////////////////

    if (nHarmonyExercise1ChordsToCheck)
    {
        int nExerciseErrors = nNumErrors;

        wxLogMessage(_T(" *** EXERCISE 1 or 2 CHECK ***"));

        if ( nHarmonyExercise1ChordsToCheck != nNumChords)
        {
            wxString sMsg = wxString::Format(
                _T("Missing chords; Expected:%d, Actual: %d")
                ,nNumChords, nHarmonyExercise1ChordsToCheck);
            wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
            pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nNumChords-1], colour, sMsg);
            wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
        }

        lmEChordType nChordType;
        int nNotePitch;
        int nInversions;
        // Check
        for (int nChordCount=0;
            nChordCount<nNumChords && nChordCount<nMAX_E1BCHORDS && nChordCount < nHarmonyExercise1ChordsToCheck; 
            nChordCount++)
        {
            nChordType = tChordDescriptor[nChordCount].pChord->GetChordType();
            nInversions = tChordDescriptor[nChordCount].pChord->GetInversion();
            wxLogMessage(_T("Chord %d  Type:%d, %d inversions. Expected:%d,0")
                ,nChordCount+1, nChordType, nInversions, nExercise1ChordType[nChordCount]);

            // if chord is not valid, no need to say anything: the error message was already shown
            if ( nChordType !=  lmINVALID_CHORD_TYPE )
            {
                if ( nInversions > 0)
                {
                    nExerciseErrors++;
                    wxString sMsg = wxString::Format(
                        _("Chord %d is not at root position: %d inversions")
                        ,nChordCount+1, nInversions);
                    wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
                    pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                }
                if ( nChordType != nExercise1ChordType[nChordCount])
                {
                    nExerciseErrors++;
                    wxString sMsg = wxString::Format(
                    _("Chord %d: The type is %s, but the expected was: %s. Wrong root note?")
                        ,nChordCount+1, ChordTypeToName( nChordType ), ChordTypeToName( nExercise1ChordType[nChordCount] ));
                    wxColour colour = wxColour(255,10,0,128); // R, G, B, Transparency: RED
                    pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                    wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                }
               /*-- todo: remove this "else"; only for debugging
                else
                {
                    wxColour colour = wxColour(10,255,0,128); // R, G, B, Transparency: GREEN
                    pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                } --*/
            }


            wxLogMessage( _T(" Chord %d has %d notes ")
                , nChordCount+1
                , tChordDescriptor[nChordCount].nNumChordNotes );
            for (int nxc=0; nxc<tChordDescriptor[nChordCount].nNumChordNotes; nxc++)
            {
                wxLogMessage( _T(" NOTE %d: %s")
                    , nxc, NoteId(*tChordDescriptor[nChordCount].pChordNotes[nxc]).c_str() );
            }

            // CHECK BASS NOTE
            nNotePitch = tChordDescriptor[nChordCount].pChordNotes[0]->GetFPitch();
            wxString sMsg = wxString::Format( _("Chord %d: bass note:%s, expected:%s ")
                , nChordCount+1
                , FPitch_ToAbsLDPName(nNotePitch).c_str()
                , FPitch_ToAbsLDPName(nExerciseBassNotesFPitch[nChordCount]).c_str()
                    );
            wxLogMessage( sMsg ); 
            if ( nNotePitch !=  nExerciseBassNotesFPitch[nChordCount] )
            {
                nExerciseErrors++;
                wxColour colour = wxColour(255,10,0,128);
                pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                wxLogMessage(_T(" Error: %s"), sMsg );
            }

            // Exercise 2: check another note...
            if ( nHarmonyExcerciseType == 2 )
            {
                if (tChordDescriptor[nChordCount].nNumChordNotes <= 3)
                {
                    sMsg = wxString::Format( _("Chord %d, Soprano voice is missing"), nChordCount+1);
                }
                else
                {
                    // CHECK SOPRANO NOTE ( index: 3 )
                    nNotePitch = tChordDescriptor[nChordCount].pChordNotes[3]->GetFPitch();
                    // note : remem
                    sMsg = wxString::Format( _("Chord %d: soprano note:%s, expected:%s  ")
                        , nChordCount+1
                        , FPitch_ToAbsLDPName(nNotePitch).c_str()
                        , FPitch_ToAbsLDPName(nExercise2NotesFPitch[nChordCount]).c_str()
                            );
                    wxLogMessage( sMsg ); 
                    // 2 possible distancies bass-soprano: 0 (lm_p8) and the second interval of the chord
                    if ( nNotePitch !=  nExercise2NotesFPitch[nChordCount] )
                    {
                        // here: not the 2nd interval
                        if ( nNotePitch % nExercise2NotesFPitch[nChordCount] != lm_p8 )
                        {
                            nExerciseErrors++;
                            // here: not the 2nd interval and not one octave: ERROR
                            wxColour colour = wxColour(255,10,0,128);
                            pChordErrorBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
                            wxLogMessage(_T(" Error: %s"), sMsg.c_str() );
                            wxLogMessage(_T(" Error: %s"), sMsg );
                        }
                    }
                }
           }


            /*-- else  TODO: remove
            {
                wxColour colour = wxColour(10,255,0,128);
                pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nChordCount], colour, sMsg);
            }--*/
        }
        nHarmonyExercise1ChordsToCheck = 0;

        if (nExerciseErrors > 0)
        {
            wxString sOkMsg = wxString::Format( _(" Exercise errors: %d"), nExerciseErrors);
           pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nNumChords-1], *wxRED, sOkMsg );
        }
        else
        {
           wxString sOkMsg = _T(" Exercise is OK.");
           pInfoBox->DisplayChordInfo(pScore, &tChordDescriptor[nNumChords-1], *wxGREEN, sOkMsg );
        }

    }
    return fScoreModified;      //true -> score modified
}

// return: total number of errors
int lmHarmonyProcessor::AnalyzeChordsLinks(lmChordDescriptor* pChordDescriptor, int nNCH)
{
    wxLogMessage(_T("AnalyzeChordsLinks N:%d "), nNCH);

    int nNumChordError[lmMAX_NUM_CHORDS]; // number of errors in each chord

    lmRuleList tRules(pChordDescriptor, nNCH);

    wxString sStr;
    sStr.clear();

    int nNumErros = 0; // TODO: decide: total num of errors or num of chords with error
    lmRule* pRule;
    // TODO: create a method of the list to evaluate ALL the rules ?
    for (int nR = lmCVR_FirstChordValidationRule; nR<lmCVR_LastChordValidationRule; nR++)
    {
        pRule = tRules.GetRule(nR);
        if ( pRule == NULL)
        {
//todo: remove this message?            wxLogMessage(_T(" Rule %d is NULL !!!"), nR);
        }
        else if (pRule->IsEnabled())
        {
            wxLogMessage(_T("Evaluating rule %d, description: %s")
                , pRule->GetRuleId(), pRule->GetDescription().c_str());
            nNumErros += pRule->Evaluate(sStr, &nNumChordError[0], pChordErrorBox);
            wxLogMessage(sStr);
            wxLogMessage(_T("   RESULT of Rule %d: %d errors"), pRule->GetRuleId(), nNumErros   );
            if (nNumErros > 0)
            {
              // TODO: anything else here?
            }
        }
    }

    return nNumErros;
}

