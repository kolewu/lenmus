//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_SCORE_H__        //to avoid nested includes
#define __LM_SCORE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Score.cpp"
#endif

#include <vector>
#include <list>

#include "wx/debug.h"

#include "defs.h"
#include "Pitch.h"
#include "../app/global.h"


// aligments
enum lmEAlignment
{
    lmALIGN_DEFAULT = 0,
    lmALIGN_LEFT,
    lmALIGN_RIGHT,
    lmALIGN_CENTER
};


//Play modes: instrument to use to play a score
enum lmEPlayMode
{
    ePM_NormalInstrument = 1,        //play using normal instrument
    ePM_RhythmInstrument,
    ePM_RhythmPercussion,
    ePM_RhythmHumanVoice
};

//Highlight when playing a score
enum lmEHighlightType
{
    eVisualOff = 0,
    eVisualOn,
    eRemoveAllHighlight,
    ePrepareForHighlight,
};

//for paper size and margins settings
enum lmEPageScope
{
    lmSCOPE_PAGE = 0,       //for current page
    lmSCOPE_SECTION,        //for current section
    lmSCOPE_SCORE,          //for the whole document
};

/*  Renderization options
    ---------------------
    eRenderJustified
        Render a score justifying measures so that they fit exactly in the width of the
        staff

    eRenderSimple
        Render a score without bar justification and without breaking it into systems.
        That is, it draws all the score in a single system without taking into consideration
        paper length limitations.
        This very simple renderer is usefull for simple scores and in some rare occations
*/
enum ERenderizationType
{
    eRenderJustified = 1,
    eRenderSimple
};

// Spacing methods for rendering scores
// -------------------------------------
//    Two basic methods:
//    1. Fixed: the spacing between notes is constant, independently of note duration.
//    2. Proportional: the spacing is adjusted so that note position is proportional to time.
//
//    In the proportional method several alternatives are posible:
//    1. ProportionalConstant: the proportion factor between time and space is fixed. Two alternative
//        methods for fixing this factor:
//        a) Fixed: is given by the vaule of a parameter
//        b) ShortNote: is computed for the shorter note in the score
//    2. ProportionalVariable: the proportion factor is computed for each bar. Two alternatives:
//        a) ShortNote: is computed for the shorter note in the bar
//        b) NumBars: Computed so taht the number of bars in the system is a predefined number

enum lmESpacingMethod
{
    esm_Fixed = 1,
    esm_PropConstantFixed,
    //esm_PropConstantShortNote,
    //esm_PropVariableShortNote,
    //esm_PropVariableNumBars,
};


// font specification
enum lmETextStyle           // text styles
{
    lmTEXT_DEFAULT = 0,
    lmTEXT_NORMAL,
    lmTEXT_BOLD,
    lmTEXT_ITALIC,
    lmTEXT_ITALIC_BOLD,
};

typedef struct lmFontInfoStruct {
    wxString sFontName;
    int nFontSize;
    lmETextStyle nStyle;
} lmFontInfo;

    //global variables used as default initializators
extern lmFontInfo tLyricDefaultFont;            // defined in NoteRestObj.cpp
extern lmFontInfo g_tInstrumentDefaultFont;       // defined in Instrument.cpp
extern lmFontInfo tBasicTextDefaultFont;        // defined in NoteRestObj.cpp


//Constants
#define XML_DURATION_TO_LDP  64        //factor to convert between LDP duration and MusicXML duration

//options for Play() method
#define lmVISUAL_TRACKING           true        //highligth notes on the score as they are played
#define lmNO_VISUAL_TRACKING        false
#define NO_MARCAR_COMPAS_PREVIO     false

#define lmLDP_INDENT_STEP   3       //indent step for Source LDP generation
#define lmXML_INDENT_STEP   3       //indent step for MusicXML generation

// forward declarations
class lmPaper;
class lmPageInfo;
class lmScore;
class lmVStaff;
class lmInstrument;
class lmStaffObj;
class lmSOIterator;

class lmBasicText;
class lmScoreText;
class lmSOControl;
class lmClef;
class lmTimeSignature;
class lmKeySignature;
class lmBarline;
class lmNoteRest;
class lmBeam;
class lmTupletBracket;
class lmChord;
struct lmTBeamInfo;
class lmNote;
class lmRest;
class lmInstrGroup;
class lmStaff;
class lmContext;
class lmSoundManager;
class lmObjOptions;

class lmBox;
class lmBoxScore;
class lmScoreView;


#include "StaffObj.h"

#include "SOControl.h"
#include "AuxObj.h"
#include "Accidental.h"
#include "Instrument.h"
#include "Slur.h"            // Slurs and Ties
#include "Direction.h"
#include "Barline.h"
#include "Clef.h"
#include "TimeSignature.h"
#include "KeySignature.h"
#include "NoteRest.h"
#include "Beam.h"
#include "TupletBracket.h"
#include "Note.h"
#include "Rest.h"
#include "Chord.h"
#include "ColStaffObjs.h"   //for lmVCursorState;
#include "../app/Paper.h"
#include "../sound/SoundManager.h"


// global unique variables used during score building
// TODO: Replace for lmScore/lmNote member funtions
extern lmNoteRest* g_pLastNoteRest;
extern lmBeam* g_pCurBeam;

class lmScoreCursor
{
public:
    lmScoreCursor(lmScore* pOwnerScore);
    ~lmScoreCursor() {}

    //attachment to a ScoreView
	void AttachCursor(lmScoreView* pView);
	void DetachCursor();

    //positioning
    void ResetCursor();
    void MoveRight(bool fNextObject = true);
    void MoveLeft(bool fPrevObject = true);
    void MoveFirst();
    void MoveUp();
    void MoveDown();
	void MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure);
    void MoveCursorToObject(lmStaffObj* pSO);

    //current position info
    float GetCursorTime();
    lmStaffObj* GetCursorSO();
    lmUPoint GetCursorPoint();
    lmStaff* GetCursorStaff();
    int GetCursorNumStaff();
    lmVStaff* GetVStaff();
	inline int GetCursorInstrumentNumber() { return m_nCursorInstr; }
	inline lmScore* GetCursorScore() { return m_pScore; }
    inline lmVStaffCursor* GetVCursor() { return m_pVCursor; }
    void SetNewCursorState(lmVCursorState* pState);
    void SelectCursor(lmVStaffCursor* pVCursor);
    int GetPageNumber();


private:
    void SelectCursorFromInstr(int nInstr);

    lmScore*            m_pScore;           //owner score
    lmScoreView*        m_pView;            //View using this cursor
	lmVStaffCursor*		m_pVCursor;		    //current cursor
	int					m_nCursorInstr;		//instrument number (1..n) of current cursor

};

class lmScore : public lmScoreObj
{
public:
    //ctor and dtor
    lmScore();
    ~lmScore();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Score; }


	//---- specific methods of this class ------------------------

    int GetNumMeasures();

    // play methods
    void Play(bool fVisualTracking = lmNO_VISUAL_TRACKING,
              bool fMarcarCompasPrevio = NO_MARCAR_COMPAS_PREVIO,
              lmEPlayMode nPlayMode = ePM_NormalInstrument,
              long nMM = 0,
              wxWindow* pWindow = (wxWindow*)NULL );
    void PlayMeasure(int nMeasure,
                     bool fVisualTracking = lmNO_VISUAL_TRACKING,
                     lmEPlayMode nPlayMode = ePM_NormalInstrument,
                     long nMM = 0,
                     wxWindow* pWindow = (wxWindow*)NULL );
    void Pause();
    void Stop();
    void WaitForTermination();


    // serving highlight events
    void ScoreHighlight(lmStaffObj* pSO, lmPaper* pPaper, lmEHighlightType nHighlightType);
	void RemoveAllHighlight(wxWindow* pCanvas);


    // Debug methods. If filename provided writes also to file
    wxString Dump() { return Dump(_T("")); }
    wxString Dump(wxString sFilename);
    wxString SourceLDP(wxString sFilename = _T(""));
    wxString SourceXML(wxString sFilename = _T(""));
    wxString DumpMidiEvents(wxString sFilename = _T(""));

	// instrument related
    int GetNumInstruments() { return (int)m_cInstruments.size(); }
    lmInstrument* GetInstrument(int nInstr);
    lmInstrument* GetFirstInstrument();
    lmInstrument* GetNextInstrument();
    lmInstrument* AddInstrument(int nMIDIChannel, int nMIDIInstr,
                                wxString sName, wxString sAbbrev=_T(""),
                                lmInstrGroup* pGroup = (lmInstrGroup*)NULL );
    lmInstrument* AddInstrument(int nMIDIChannel, int nMIDIInstr,
                                lmScoreText* pName, lmScoreText* pAbbrev,
                                lmInstrGroup* pGroup = (lmInstrGroup*)NULL );


    // titles related methods
    lmScoreText* AddTitle(wxString sTitle, lmEAlignment nAlign, lmLocation pos,
                  wxString sFontName, int nFontSize, lmETextStyle nStyle);
	void LayoutTitles(lmBox* pBox, lmPaper *pPaper);

    // identification
    wxString GetScoreName();
    void SetScoreName(wxString sName);
    inline long GetID() const { return m_nID; }

    // methods related to MusicXML import/export
    lmInstrument* XML_FindInstrument(wxString sId);

    //layout related methods
    lmBoxScore* Layout(lmPaper* pPaper);
    lmLUnits TopSystemDistance() { return m_nTopSystemDistance + m_nHeadersHeight; }
    void SetTopSystemDistance(lmLUnits nDistance) { m_nTopSystemDistance = nDistance; }

    //renderization options
    void SetRenderizationType(ERenderizationType nType) { m_nRenderizationType = nType; }
    //SetSpacingMethod(ESpacingMethod nMethod) { m_nSpacingMethod = nMethod; }
        // accessors for lmFormatter only
    ERenderizationType GetRenderizationType() const { return m_nRenderizationType; }
    //ESpacingMethod GetSpacingMethod() const { return m_nSpacingMethod; }
	inline void SetModified(bool fValue) { m_fModified = fValue; }
	inline bool IsModified() { return m_fModified; }

	//methods used for renderization
	void SetMeasureModified(int nMeasure, bool fModified = true);
	bool IsMeasureModified(int nMeasure);
	void ResetMeasuresModified();

	//cursor management
    lmScoreCursor* SetCursor(lmVStaffCursor* pVCursor);
    inline void ResetCursor() { m_SCursor.ResetCursor(); }
    inline lmScoreCursor* GetCursor() { return &m_SCursor; }
        //attachment to a ScoreView
	lmScoreCursor* AttachCursor(lmScoreView* pView);
	void DetachCursor();
    lmScoreCursor* SetNewCursorState(lmVCursorState* pState);

    //paper size and margins
    //lmEPageScope nScope = lmSCOPE_PAGE
    void SetPageInfo(lmPageInfo* pPageInfo);
    inline lmPageInfo* GetPageInfo() { return m_pPageInfo; }




private:
    friend class lmScoreCursor;

    void WriteToFile(wxString sFilename, wxString sContent);
    void ComputeMidiEvents();
    void RemoveHighlight(lmStaffObj* pSO, lmPaper* pPaper);
	lmLUnits CreateTitleShape(lmBox* pBox, lmPaper *pPaper, lmScoreText* pTitle,
							  lmLUnits nPrevTitleHeight);
	void DoAddInstrument(lmInstrument* pInstr, lmInstrGroup* pGroup);


        //
        // member variables
        //

    // a lmScore is, mainly, a collection of Instruments plus some data (composer, title, ...)
    std::vector<lmInstrument*>	m_cInstruments;     //list of instruments that form this score
    std::vector<int>			m_nTitles;          //indexes (over attached AuxObjs) to titles

    //Variables related to polyphonic interpretation
    lmSoundManager*			m_pSoundMngr;       //Sound events table & manager
    std::list<lmStaffObj*>	m_cHighlighted;     //list of highlighted staffobjs

    //Layout related variables
    lmLUnits			    m_nTopSystemDistance;
    lmLUnits			    m_nHeadersHeight;
    lmVStaff*               m_pTenthsConverter;     //for lmTenths <-> lmLUnits conversion

    //renderization options
    ERenderizationType      m_nRenderizationType;
	bool				    m_fModified;            //to force a repaint

    //paper size and margins
    lmPageInfo*             m_pPageInfo;

    //other variables
	int					    m_nCurNode;     //last returned instrument node
    long				    m_nID;          //unique ID for this score
    wxString			    m_sScoreName;   //for user identification

	//temporary data used for edition/renderization
	std::list<int>          m_aMeasureModified;		//list of measures modified

	//for edition
	lmScoreCursor		m_SCursor;			//Active cursor pointing to current position

};



#endif    // __LM_SCORE_H__
