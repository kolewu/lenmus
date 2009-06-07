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
#pragma implementation "GMObject.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <algorithm>

#include "../app/global.h"
#include "GMObject.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "Handlers.h"
#include "../app/Paper.h"
#include "../score/StaffObj.h"
#include "../app/ScoreCanvas.h"
#include "agg_basics.h"         // agg::path_cmd_stop

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

// global variables defined in this file
bool g_fDrawSelRect = false;    //draw selection rectangles around staff objects
bool g_fDrawBounds = false;     //draw bounds rectangles around staff objects
bool g_fShowMargins = false;    //draw margins in scores, so user can change them
bool g_fDrawAnchors = false;    //draw anchors, to see them in the score
bool g_fFreeMove = false;		//the shapes can be dragged without restrictions

bool g_fDrawBoundsBoxSystem = false;        //draw bound rectangles for systems
bool g_fDrawBoundsBoxSlice = false;         //draw bound rectangles for slices
bool g_fDrawBoundsBoxSliceInstr = false;    //draw bound rectangles for SliceInstr
bool g_fDrawBoundsBoxSliceVStaff = false;   //draw bound rectangles for SliceVStaff
bool g_fDrawBoundsShapes = false;           //draw bound rectangles for non boxes


//========================================================================================
//helper class to represent an attached shape
//========================================================================================
class lmAttachPoint
{
public:
    lmAttachPoint(lmShape* shape, lmEAttachType tag) : pShape(shape), nType(tag) {}
    ~lmAttachPoint() {}

	lmShape*		pShape;
	lmEAttachType	nType;
};


//========================================================================================
//Implementation of class lmGMObject: the root object for the graphical model
//========================================================================================

static int m_IdCounter = 0;         //to assign unique IDs to GMObjects
static long m_nShapeOrder = 0;       //to assign an ordering to shapes, when they are added to a box


lmGMObject::lmGMObject(lmScoreObj* pOwner, lmEGMOType nType, bool fDraggable,
					   bool fSelectable, wxString sName, int nOwnerIdx)
{
    m_nId = m_IdCounter++;      // give it an ID
    m_nType = nType;
	m_pOwner = pOwner;
    m_nOwnerIdx = nOwnerIdx;
	m_sGMOName = sName;

	//initializations
	m_uBoundsBottom = lmUPoint(0.0f, 0.0f);
    m_uBoundsTop = lmUPoint(0.0f, 0.0f);
    m_uUserShift = lmUPoint(0.0f, 0.0f);
	m_fSelected = false;
    m_fSelectable = fSelectable;
	m_fLeftDraggable = fDraggable;
}

lmGMObject::~lmGMObject()
{
}

bool lmGMObject::BoundsContainsPoint(lmUPoint& uPoint)
{
    //returns true if point received is within the limits of this object bounds

    return GetBounds().Contains(uPoint);
}

bool lmGMObject::HitTest(lmUPoint& uPoint)
{
    //returns true if point received is within the limits of this object
    //selection rectangle

    return GetSelRectangle().Contains(uPoint);
}

void lmGMObject::DrawBounds(lmPaper* pPaper, wxColour color)
{
    //draw a border around object region
    pPaper->SketchRectangle(m_uBoundsTop,
                            lmUSize(m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y),
                            color);

}

void lmGMObject::Render(lmPaper* pPaper, wxColour colorC)
{
    // Code common to all shapes renderization. Must be invoked after specific code at
    // each shape renderization method

    // draw selection rectangle
    if (g_fDrawSelRect)
        DrawSelRectangle(pPaper, g_pColors->ScoreSelected() );

    if (g_fDrawBounds)
    {
        if ( (g_fDrawBoundsBoxSystem && this->IsBoxSystem()) 
             || (g_fDrawBoundsBoxSlice && this->IsBoxSlice()) 
             || (g_fDrawBoundsBoxSliceInstr && this->IsBoxSliceInstr()) 
             || (g_fDrawBoundsBoxSliceVStaff && this->IsBoxSliceVStaff()) 
             || (g_fDrawBoundsShapes && this->IsShape()) )
        {
            if (this->IsBox())
                ((lmBox*)this)->DrawLimits(pPaper, *wxGREEN);
            DrawBounds(pPaper, *wxRED); //colorC);
        }
    }
}

wxString lmGMObject::DumpBounds()
{
    return wxString::Format(_T("Bounds=(%.2f, %.2f, %.2f, %.2f)"),
        	m_uBoundsTop.x, m_uBoundsTop.y,
            m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y);
}

void lmGMObject::NormaliceBoundsRectangle()
{
	// Ensure that TopLeft point is at left-top of BottomRight point

	if (m_uBoundsBottom.x < m_uBoundsTop.x)
	{
		lmLUnits uxMin = m_uBoundsBottom.x;
		m_uBoundsBottom.x = m_uBoundsTop.x;
		m_uBoundsTop.x = uxMin;
	}
	if (m_uBoundsBottom.y < m_uBoundsTop.y)
	{
		lmLUnits uyMin = m_uBoundsBottom.y;
		m_uBoundsBottom.y = m_uBoundsTop.y;
		m_uBoundsTop.y = uyMin;
	}
}

lmUPoint lmGMObject::GetObjectOrigin()
{
	//returns the origin of this object
	return m_uBoundsTop;
}

void lmGMObject::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must
	// send a move object command to the controller.

    WXUNUSED(pPaper);
	pCanvas->MoveObject(this, uPos);
}

void lmGMObject::ApplyUserShift(lmUPoint uUserShift)
{
    lmUPoint uShift = uUserShift - m_uUserShift;
    Shift(uShift.x, uShift.y);
    m_uUserShift = uUserShift;
}

void lmGMObject::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    ShiftBoundsAndSelRec(xIncr, yIncr);
}

void lmGMObject::ShiftOrigin(lmUPoint uNewOrg)
{
	lmUPoint uShift = uNewOrg - m_pOwner->GetLayoutRefPos();
	Shift(uShift.x, uShift.y);
}

void lmGMObject::ShiftBoundsAndSelRec(lmLUnits xIncr, lmLUnits yIncr)
{
	// Auxiliary method to be used by derived classes to perform common actions when the
	// shape is shifted

	m_uSelRect.x += xIncr;		//AWARE: As it is a rectangle, changing its origin does not change
    m_uSelRect.y += yIncr;		//       its width/height. So no need to adjust bottom right point

	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
	m_uBoundsTop.y += yIncr;
	m_uBoundsBottom.y += yIncr;
}

void lmGMObject::SetSelRectangle(lmLUnits x, lmLUnits y, lmLUnits uWidth, lmLUnits uHeight)
{
    m_uSelRect.x = x;
    m_uSelRect.y = y;
    m_uSelRect.width = uWidth;
    m_uSelRect.height = uHeight;
}

void lmGMObject::DrawSelRectangle(lmPaper* pPaper, wxColour colorC)
{
    pPaper->SketchRectangle(m_uSelRect.GetPosition(), m_uSelRect.GetSize(), colorC);
}

void lmGMObject::OnRightClick(lmController* pCanvas, const lmDPoint& vPos, int nKeys)
{
	WXUNUSED(nKeys);
    m_pOwner->PopupMenu(pCanvas, this, vPos);
}

void lmGMObject::SetSelected(bool fValue)
{
    if (m_fSelected == fValue) return;      //nothing to do

    //change selection status
    m_fSelected = fValue;
    OnSelectionStatusChanged();         //inform derived class

    //add/remove object from global list
    lmBoxScore* pBS = this->GetOwnerBoxScore();
    wxASSERT(pBS);
    if (fValue)
        pBS->AddToSelection(this);
    else
        pBS->RemoveFromSelection(this);
}



//========================================================================================
//Implementation of class lmBox: the container objects root
//========================================================================================


lmBox::lmBox(lmScoreObj* pOwner, lmEGMOType nType, wxString sName)
	: lmGMObject(pOwner, nType, lmNO_DRAGGABLE, lmNO_SELECTABLE, sName)
    , m_uTopSpace(0.0f)
    , m_uBottomSpace(0.0f)
    , m_uLeftSpace(0.0f)
    , m_uRightSpace(0.0f)
{
}

lmBox::~lmBox()
{
    if (!lmPRESERVE_SHAPES)
    {
        //delete shapes collection
        for (int i=0; i < (int)m_Shapes.size(); i++)
        {
            delete m_Shapes[i];
        }
        m_Shapes.clear();
    }

    //delete boxes
    std::vector<lmBox*>::iterator itB;
    for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
    {
        delete *itB;
    }
    m_Boxes.clear();
}

void lmBox::AddShape(lmShape* pShape, long nLayer)
{
    //add shape to this box collection
    m_Shapes.push_back(pShape);
	pShape->SetOwnerBox(this);
    pShape->SetOrder(++m_nShapeOrder);
    pShape->SetLayer(nLayer);
    wxASSERT(nLayer >= lm_eLayerBackground && nLayer <= lm_eLayerTop);

    ////add shape to layer
    //lmBoxPage* pBoxPage = this->GetOwnerBoxPage();
    //wxASSERT(pBoxPage);
    //pBoxPage->AddShapeToLayer(pShape, nLayer);
}

void lmBox::AddBox(lmBox* pBox)
{
    m_Boxes.push_back(pBox);
}

void lmBox::DrawLimits(lmPaper* pPaper, wxColour color)
{
    //draw a border around box limits

    lmUPoint uTopLeft(m_uBoundsTop.x - m_uLeftSpace, m_uBoundsTop.y - m_uTopSpace);
    lmUSize uSize( GetWidth() + m_uLeftSpace + m_uRightSpace,
                   GetHeight() + m_uTopSpace + m_uBottomSpace );
    pPaper->SketchRectangle(uTopLeft, uSize, color);
}

void lmBox::Render(lmPaper* pPaper, lmUPoint uPos)
{
    //RenderShapes(pPaper);

    //render contained boxes
    std::vector<lmBox*>::iterator itB;
    for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        (*itB)->Render(pPaper, uPos);

    //base class
    lmGMObject::Render(pPaper, *wxBLACK);
}

void lmBox::RemoveShape(lmShape* pShape)
{
    std::vector<lmShape*>::iterator it;
    it = std::find(m_Shapes.begin(), m_Shapes.end(), pShape);
    wxASSERT(it != m_Shapes.end());
    m_Shapes.erase(it);
}

lmBox* lmBox::FindBoxAtPos(lmUPoint& uPoint)
{
    //look for most inner box (minimal size box: i.e. BoxSliceVStaff) that contains received point.
    //If not box found returns NULL.

    //loop to look up in the child boxes collection
    std::vector<lmBox*>::iterator it;
    for(it = m_Boxes.begin(); it != m_Boxes.end(); ++it)
    {
        lmBox* pBox = (*it)->FindBoxAtPos(uPoint);
        if (pBox)
			return pBox;    //found
    }

    // no object found. Verify if the point is in this box
    if (BoundsContainsPoint(uPoint))
        return this;
    else
        return (lmBox*)NULL;

}

void lmBox::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                            lmLUnits uYMin, lmLUnits uYMax)
{
    //loop to look up in the shapes collection
    lmURect selRect(uXMin, uYMin, uXMax-uXMin, uYMax-uYMin);
    std::vector<lmShape*>::iterator it;
    for(it = m_Shapes.begin(); it != m_Shapes.end(); ++it)
    {
        if ((*it)->IsSelectable() && (*it)->IsInRectangle(selRect))
			(*it)->SetSelected(fSelect);
    }
}

void lmBox::RenderShapes(lmPaper* pPaper)
{
	//render the shapes
    std::vector<lmShape*>::iterator it;
	for (it=m_Shapes.begin(); it != m_Shapes.end(); ++it)
	{
		(*it)->Render(pPaper);
	}
}

void lmBox::AddShapesToLayers(lmBoxPage* pBoxPage)
{
    //add shapes to box page layers

    std::vector<lmShape*>::iterator it;
    for (it = m_Shapes.begin(); it != m_Shapes.end(); ++it)
        pBoxPage->AddShapeToLayer(*it, (*it)->GetLayer());

    //add shapes to box page layers
    std::vector<lmBox*>::iterator itB;
    for (itB = m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        (*itB)->AddShapesToLayers(pBoxPage);
}

wxString lmBox::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += DumpBounds();
    sDump += _T("\n");

	nIndent++;

	//dump the shapes in this box
    std::vector<lmShape*>::iterator it;
	for (it=m_Shapes.begin(); it != m_Shapes.end(); ++it)
    {
        sDump += (*it)->Dump(nIndent);
    }

    //loop to dump the boxes
    std::vector<lmBox*>::iterator itB;
	for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        sDump += (*itB)->Dump(nIndent);

	return sDump;
}

bool lmBox::ContainsXPos(lmLUnits uxPos)
{
	//return true if position uxPos is within the limits of this Box

	return (uxPos >= GetXLeft() && uxPos <= GetXRight());
}

lmBox* lmBox::FindChildBoxAt(lmLUnits uxPos)
{
	//return the first box located at xPos

    std::vector<lmBox*>::iterator itB;
	for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
    {
        if ((*itB)->ContainsXPos(uxPos))
			return *itB;		//found
    }
	return (lmBox*)NULL;
}

void lmBox::UpdateXRight(lmLUnits xRight)
{
	// During layout there is a need to update initial computations about this
	// box position. This update must be propagated to all contained boxes

    SetXRight(xRight);

	//propagate change
    std::vector<lmBox*>::iterator itB;
	for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        (*itB)->UpdateXRight(xRight);
}

void lmBox::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);

	//propagate change
    std::vector<lmBox*>::iterator itB;
	for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        (*itB)->UpdateXLeft(xLeft);
}



//========================================================================================
// Implementation of class lmShape: any renderizable object, such as a line,
// a glyph, a note head, an arch, etc.
//========================================================================================


lmShape::lmShape(lmEGMOType nType, lmScoreObj* pOwner, int nOwnerIdx, wxString sName,
                 bool fDraggable, bool fSelectable, wxColour color, bool fVisible)
	: lmGMObject(pOwner, nType, fDraggable, fSelectable, sName, nOwnerIdx)
    , m_pOwnerBox((lmBox*)NULL)
	, m_color(color)
	, m_fVisible(fVisible)
	, m_pParentShape((lmShape*)NULL)
    , m_pMouseCursorWindow((wxWindow*)NULL)
    , m_nOrder(0)
{
}

lmShape::~lmShape()
{
	//delete attachment data
	std::list<lmAttachPoint*>::iterator pItem;
	for (pItem = m_cAttachments.begin(); pItem != m_cAttachments.end(); ++pItem)
	{
		(*pItem)->pShape->OnDetached(this);     //inform: this attachment removed
		delete *pItem;
    }
    m_cAttachments.clear();

    //if this shape is attached to others, ask for removal
	std::list<lmShape*>::iterator it;
	for (it = m_cAttachedTo.begin(); it != m_cAttachedTo.end(); ++it)
	{
		(*it)->Detach(this, false);     //false: do not inform back this shape
    }

    //restore mouse cursor if necessary
    if (m_pMouseCursorWindow)
    {
        lmUPoint anyPoint(0.0f, 0.0f);
        OnMouseOut(m_pMouseCursorWindow, anyPoint);
    }
}

bool lmShape::Collision(lmShape* pShape)
{
    lmURect rect1 = GetBounds();
    return rect1.Intersects( pShape->GetBounds() );
}

bool lmShape::IsInRectangle(lmURect& rect)
{
    lmURect rect1 = GetBounds();
    //wxLogMessage(_T("[lmShape::IsInRectangle] Bounds(x=%.2f, y=%.2f, w=%.2f, h=%.2f), sel=(x=%.2f, y=%.2f, w=%.2f, h=%.2f)"),
    //    rect1.x, rect1.y, rect1.width, rect1.height,
    //    rect.x, rect.y, rect.width, rect.height );
    return rect.Contains(rect1);
}

void lmShape::Render(lmPaper* pPaper)
{
    if (IsVisible())
        Render(pPaper, (IsSelected() ? g_pColors->ScoreSelected() : m_color) );
}

void lmShape::OnMouseIn(wxWindow* pWindow, lmUPoint& uPoint)
{
    if (IsSelected() && IsLeftDraggable())
    {
	    pWindow->SetCursor( wxCursor(wxCURSOR_SIZING));
        m_pMouseCursorWindow = pWindow;
    }
}

void lmShape::OnMouseOut(wxWindow* pWindow, lmUPoint& uPoint)
{
    if (m_pMouseCursorWindow)
    {
	    pWindow->SetCursor(*wxSTANDARD_CURSOR);
        m_pMouseCursorWindow = (wxWindow*)NULL;
    }
}

wxString lmShape::DumpSelRect()
{
    return wxString::Format(_T("SelRect=(%.2f, %.2f, %.2f, %.2f)"),
        	m_uSelRect.x, m_uSelRect.y, m_uSelRect.width, m_uSelRect.height);

}

int lmShape::Attach(lmShape* pShape, lmEAttachType nTag)
{
	lmAttachPoint* pData = new lmAttachPoint(pShape, nTag);
    m_cAttachments.push_back(pData);

    //inform pShape that has been attached to this shape
    pShape->OnAttached(this);

	//return index to attached shape
	return (int)m_cAttachments.size() - 1;
}

void lmShape::Detach(lmShape* pShape, bool fInform)
{
    //detach shape pShape. If flag fInform is true, inform pShape of the
    //detachment

	std::list<lmAttachPoint*>::iterator it;
	for (it = m_cAttachments.begin(); it != m_cAttachments.end(); ++it)
	{
		if ( (*it)->pShape->GetID() == pShape->GetID() ) break;
    }
	if (it != m_cAttachments.end())
    {
        if (fInform)
            (*it)->pShape->OnDetached(this);
        lmAttachPoint* pData = *it;
		m_cAttachments.erase(it);
        delete pData;
    }
}

void lmShape::OnAttached(lmShape* pShape)
{
    //this shape has been attached to shape pShape. Log it
    m_cAttachedTo.push_back(pShape);
}

void lmShape::OnDetached(lmShape* pShape)
{
    //this shape has been detached from shape pShape. Remove from log

	std::list<lmShape*>::iterator it;
	for (it = m_cAttachedTo.begin(); it != m_cAttachedTo.end(); ++it)
	{
		if ( (*it)->GetID() == pShape->GetID() ) break;
    }
	if (it != m_cAttachedTo.end())
		m_cAttachedTo.erase(it);
}

void lmShape::InformAttachedShapes(lmLUnits uxShift, lmLUnits uyShift, lmEParentEvent nEvent)
{
    //Parent shape has been shifted by (uxShift, uyShift). Inform attached shapes

	std::list<lmAttachPoint*>::iterator pItem;
	for (pItem = m_cAttachments.begin(); pItem != m_cAttachments.end(); pItem++)
	{
		lmAttachPoint* pData = *pItem;
        pData->pShape->OnAttachmentPointMoved(this, pData->nType, uxShift, uyShift, nEvent);
    }
}

int lmShape::GetPageNumber() const
{
	if (!m_pOwnerBox || !m_pOwnerBox->IsBox() ) return 0;
	return m_pOwnerBox->GetPageNumber();
}

unsigned lmShape::GetVertex(lmLUnits* pux, lmLUnits* puy)
{
    return agg::path_cmd_stop;
}

lmBoxScore* lmShape::GetOwnerBoxScore()
{
    if (m_pOwnerBox)
        return m_pOwnerBox->GetOwnerBoxScore();
    else
        return (lmBoxScore*)NULL;
}

lmBoxPage* lmShape::GetOwnerBoxPage()
{
    if (m_pOwnerBox)
        return m_pOwnerBox->GetOwnerBoxPage();
    else if (IsChildShape())
        return GetParentShape()->GetOwnerBoxPage();
    else
        return (lmBoxPage*)NULL;
}



//========================================================================================
// Implementation of class lmSimpleShape
//========================================================================================

lmSimpleShape::lmSimpleShape(lmEGMOType nType, lmScoreObj* pOwner, int nOwnerIdx,
                             wxString sName, bool fDraggable, bool fSelectable,
                             wxColour color, bool fVisible)
	: lmShape(nType, pOwner, nOwnerIdx, sName, fDraggable, fSelectable, color, fVisible)
{
}

lmSimpleShape::~lmSimpleShape()
{
}

void lmSimpleShape::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//Default behaviour is to shift bounding and selection rectangles

    m_uSelRect.x += xIncr;		//AWARE: As it is a rectangle, changing its origin does not
    m_uSelRect.y += yIncr;		//       change its width/height

	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
	m_uBoundsTop.y += yIncr;
	m_uBoundsBottom.y += yIncr;

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}



//========================================================================================
// Implementation of class lmCompositeShape
//========================================================================================


lmCompositeShape::lmCompositeShape(lmScoreObj* pOwner, int nOwnerIdx, wxColour color,
                                   wxString sName, bool fDraggable, lmEGMOType nType, bool fVisible)
	: lmShape(nType, pOwner, nOwnerIdx, sName, fDraggable, lmSELECTABLE, color, fVisible)
{
    m_fGrouped = true;	//by default all constituent shapes are grouped
	m_fDoingShift = false;
}

lmCompositeShape::~lmCompositeShape()
{
    //delete Components collection
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        delete m_Components[i];
    }
    m_Components.clear();
}

int lmCompositeShape::Add(lmShape* pShape)
{
    m_Components.push_back(pShape);

	if (m_Components.size() == 1)
	{
		//compute new selection rectangle
		m_uSelRect = pShape->GetSelRectangle();

		// compute outer rectangle for bounds
		m_uBoundsTop.x = pShape->GetXLeft();
		m_uBoundsTop.y = pShape->GetYTop();
		m_uBoundsBottom.x = pShape->GetXRight();
		m_uBoundsBottom.y = pShape->GetYBottom();
	}
	else
	{
		//compute new selection rectangle by union of individual selection rectangles
		m_uSelRect.Union(pShape->GetSelRectangle());

		// compute outer rectangle for bounds
		m_uBoundsTop.x = wxMin(m_uBoundsTop.x, pShape->GetXLeft());
		m_uBoundsTop.y = wxMin(m_uBoundsTop.y, pShape->GetYTop());
		m_uBoundsBottom.x = wxMax(m_uBoundsBottom.x, pShape->GetXRight());
		m_uBoundsBottom.y = wxMax(m_uBoundsBottom.y, pShape->GetYBottom());
	}

	//link to parent
	pShape->SetParentShape(this);

	//return index to added shape
	return (int)m_Components.size() - 1;

}

void lmCompositeShape::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//Default behaviour is to shift all components
	m_fDoingShift = true;		//semaphore to avoid recomputing constantly the bounds
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->Shift(xIncr, yIncr);
    }
	m_fDoingShift = false;

	ShiftBoundsAndSelRec(xIncr, yIncr);
}

wxString lmCompositeShape::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: grouped=%s, "), m_nOwnerIdx, m_sGMOName.c_str(),
        (m_fGrouped ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");

    //dump all its components
    nIndent++;
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        sDump += m_Components[i]->Dump(nIndent);
    }
	return sDump;
}

void lmCompositeShape::Render(lmPaper* pPaper,  wxColour color)
{
    lmShape::Render(pPaper, color);

	//Default behaviour: render all components
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->Render(pPaper, color);
    }
}

void lmCompositeShape::RenderHighlighted(wxDC* pDC, wxColour color)
{
    lmShape::RenderHighlighted(pDC, color);

	//Default behaviour: render all components
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->RenderHighlighted(pDC, color);
    }
}

void lmCompositeShape::RenderWithHandlers(lmPaper* pPaper)
{
    lmShape::RenderWithHandlers(pPaper);

	//Default behaviour: render all components
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->RenderWithHandlers(pPaper);
    }
}

void lmCompositeShape::SetSelected(bool fValue)
{
    if (m_fSelected == fValue) return;      //nothing to do

    //change selection status
    m_fSelected = fValue;
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->Restricted_SetSelected(fValue);
    }

    //add/remove object from global list
    lmBoxScore* pBS = this->GetOwnerBoxScore();
    wxASSERT(pBS);
    if (fValue)
        pBS->AddToSelection(this);
    else
        pBS->RemoveFromSelection(this);
}

bool lmCompositeShape::BoundsContainsPoint(lmUPoint& uPoint)
{
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        if (m_Components[i]->BoundsContainsPoint(uPoint))
			return true;
    }
	return false;

}

bool lmCompositeShape::Collision(lmShape* pShape)
{
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        if (m_Components[i]->Collision(pShape))
			return true;
    }
	return false;
}

lmShape* lmCompositeShape::GetShape(int nShape)
{
	wxASSERT(nShape < (int)m_Components.size());
	return m_Components[nShape];
}

void lmCompositeShape::RecomputeBounds()
{
	if (m_fDoingShift) return;

	if (m_Components.size() > 0)
	{
		lmShape* pShape = m_Components[0];

		//initilaize selection rectangle
		m_uSelRect = pShape->GetSelRectangle();

		// initialize bounds
		m_uBoundsTop.x = pShape->GetXLeft();
		m_uBoundsTop.y = pShape->GetYTop();
		m_uBoundsBottom.x = pShape->GetXRight();
		m_uBoundsBottom.y = pShape->GetYBottom();
	}

    for (int i=1; i < (int)m_Components.size(); i++)
    {
		lmShape* pShape = m_Components[i];

		//compute new selection rectangle by union of individual selection rectangles
		m_uSelRect.Union(pShape->GetSelRectangle());

		// compute outer rectangle for bounds
		m_uBoundsTop.x = wxMin(m_uBoundsTop.x, pShape->GetXLeft());
		m_uBoundsTop.y = wxMin(m_uBoundsTop.y, pShape->GetYTop());
		m_uBoundsBottom.x = wxMax(m_uBoundsBottom.x, pShape->GetXRight());
		m_uBoundsBottom.y = wxMax(m_uBoundsBottom.y, pShape->GetYBottom());
	}

}

wxBitmap* lmCompositeShape::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
    // The received DC is used for logical units to pixels conversions
	//
	// So this method returns the bitmap to use with the drag image.


    // allocate a memory DC for drawing onto a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(wxMM_TEXT);			// each logical unit is 1 pixel

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel( (wxCoord)GetWidth() );
    int hD = (int)pDC->LogicalToDeviceYRel( (wxCoord)GetHeight() );
    wxBitmap bitmap(wD, hD);

	//clear the bitmap
    dc2.SelectObject(bitmap);
    dc2.SetBackground(*wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);

    //loop to get each shape bitmap and to merge it
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        //get shape bitmap
        lmShape* pShape = m_Components[i];
		wxBitmap* pBMS = pShape->OnBeginDrag(rScale, pDC);

        //merge it
        if (pBMS)
		{
            lmPixels vxPos = pDC->LogicalToDeviceXRel( (wxCoord)(pShape->GetXLeft() - GetXLeft()) );
            lmPixels vyPos = pDC->LogicalToDeviceXRel( (wxCoord)(pShape->GetYTop() - GetYTop()) );
            dc2.DrawBitmap(*pBMS, vxPos, vyPos, true);       //true = transparent

            delete pBMS;    //bitmap no longer needed
        }
    }
    dc2.SelectObject(wxNullBitmap);

    // the common bitmap is prepared. Make it masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);
    ////DBG -----------
    //wxString sFileName = _T("CompositeShape.bmp");
    //image.SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;
}

lmUPoint lmCompositeShape::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws
	// the drag image at that position. No action must be performed by the shape on
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.

	return uPos;

}



//--------------------------------------------------------------------------------------------
// lmGMSelection is data holder with information about a selection.
//--------------------------------------------------------------------------------------------

lmGMSelection::lmGMSelection()
{
}

lmGMSelection::lmGMSelection(lmGMSelection* pSel)
{
    lmGMObject* pGMO = pSel->GetFirst();
    while(pGMO)
    {
        m_Selection.push_back(pGMO);
        pGMO = pSel->GetNext();
    }
}

lmGMSelection::~lmGMSelection()
{
    m_Selection.clear();
}

void lmGMSelection::AddToSelection(lmGMObject* pGMO)
{
    m_Selection.push_back(pGMO);
}

void lmGMSelection::RemoveFromSelection(lmGMObject* pGMO)
{
    std::list<lmGMObject*>::iterator it;
    it = std::find(m_Selection.begin(), m_Selection.end(), pGMO);
    wxASSERT(it != m_Selection.end());
    m_Selection.erase(it);
}

void lmGMSelection::ClearSelection()
{
    if (m_Selection.empty()) return;

    //Unselect all selected objects
    std::list<lmGMObject*>::iterator it = m_Selection.begin();
    for (it = m_Selection.begin(); it != m_Selection.end(); ++it)
    {
        (*it)->DoSetSelected(false);
        (*it)->OnSelectionStatusChanged();
    }
    m_Selection.clear();
}

wxString lmGMSelection::Dump()
{
    wxString sDump = wxString::Format(_T("Selection dump. %d objects selected:\n"), m_Selection.size() );
    std::list<lmGMObject*>::iterator it;
    for (it = m_Selection.begin(); it != m_Selection.end(); ++it)
    {
        sDump += (*it)->Dump(3);
    }
    sDump += _T("End of selection dump");
    return sDump;
}

lmGMObject* lmGMSelection::GetFirst()
{
    m_it = m_Selection.begin();
    if (m_it == m_Selection.end())
        return (lmGMObject*)NULL;

    return *m_it;
}

lmGMObject* lmGMSelection::GetNext()
{
    //advance to next one
    ++m_it;
    if (m_it != m_Selection.end())
        return *m_it;

    //no more items
    return (lmGMObject*)NULL;
}

lmStaffObj* lmGMSelection::GetFirstOwnerStaffObj()
{
    //returns the first staffobj in the selection or NULL

    std::list<lmGMObject*>::iterator it;
    for (it = m_Selection.begin(); it != m_Selection.end(); ++it)
    {
        lmScoreObj* pSCO = (*it)->GetScoreOwner();
        if (pSCO->GetScoreObjType() == lmSOT_ComponentObj &&
            ((lmComponentObj*)pSCO)->GetType() == lm_eStaffObj )
        {
            return (lmStaffObj*)pSCO;
        }
    }
    return (lmStaffObj*)NULL;
}
