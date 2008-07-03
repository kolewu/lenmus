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

#ifndef __LM_SCOREWIZARD_H__
#define __LM_SCOREWIZARD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreWizard.cpp"
#endif

#include "wx/wizard.h"
#include "wx/statline.h"

#include "../widgets/Wizard.h"

class lmScoreWizardLayout;
class lmScoreWizardInstrPage;
class lmScoreWizardClefPage;
class lmScoreWizardTimePage;
class lmScore;


// control identifiers
enum 
{
    //ScoreWizard
    lmID_SCORE_WIZARD = 10000,

    //lmScoreWizardLayout
    lmID_LIST_ENSEMBLE,

    //ScoreWizardInstrPage
    lmID_WIZARD_INSTR_PAGE,
    lmID_COMBO_OUT_DEVICES,
    lmID_COMBO_IN_DEVICES,

    //ScoreWizardClefPage
    lmID_WIZARD_CLEF_PAGE,
    lmID_COMBO_CHANNEL,
    lmID_COMBO_SECTION,
    lmID_COMBO_INSTRUMENT,
    lmID_BUTTON_TEST_SOUND,

    //ScoreWizardTimePage
    lmID_WIZARD_TIME_PAGE,
    lmID_COMBO_MTR_CHANNEL,
    lmID_COMBO_MTR_INSTR1,
    lmID_COMBO_MTR_INSTR2,
    lmID_BUTTON,
};


// lmScoreWizard class declaration
class lmScoreWizard: public lmWizard
{
    DECLARE_EVENT_TABLE()

public:
    lmScoreWizard(wxWindow* parent, lmScore** pPtrScore, wxWindowID id = lmID_SCORE_WIZARD, const wxPoint& pos = wxDefaultPosition );
    ~lmScoreWizard();

    //event handlers
    void OnWizardCancel( wxWizardEvent& event );
    void OnWizardFinished( wxWizardEvent& event );

private:
    lmScore**       m_pPtrScore;
};


//----------------------------------------------------------------------------------
// lmScoreWizardLayout class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardLayout : public lmWizardPage 
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardLayout )

public:
    lmScoreWizardLayout();
    lmScoreWizardLayout(wxWizard* parent);

    //event handlers
    void OnEnsembleSelected(wxCommandEvent& event);

    bool TransferDataFromWindow();

protected:
    bool Create(wxWizard* parent);
    void CreateControls();

    wxListBox* m_pLstEnsemble;
	wxStaticText* m_pLblPaper;
	wxChoice* m_pCboPaper;
	wxRadioBox* m_pRadOrientation;
	wxStaticBitmap* m_pBmpPreview;
	
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------------
// lmScoreWizardInstrPage class declaration
//----------------------------------------------------------------------------------

class lmScoreWizardInstrPage: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardInstrPage )
    DECLARE_EVENT_TABLE()

public:
    //constructors
    lmScoreWizardInstrPage( );

    lmScoreWizardInstrPage( wxWizard* parent);

    //creation
    bool Create(wxWizard* parent);

    //creates the controls and sizers
    void CreateControls();

    //event handlers

    bool TransferDataFromWindow();

    //public member variables
    wxComboBox* m_pOutCombo;
    wxComboBox* m_pInCombo;
};


/*
//---------------------------------------------------------------------------------------
// lmScoreWizardClefPage class declaration
//---------------------------------------------------------------------------------------

class lmScoreWizardClefPage: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardClefPage )
    DECLARE_EVENT_TABLE()

public:
    //constructors
    lmScoreWizardClefPage( );

    lmScoreWizardClefPage( wxWizard* parent);

    //creation
    bool Create(wxWizard* parent);

    //creates the controls and sizers
    void CreateControls();

////@begin lmScoreWizardClefPage event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for lmID_COMBO_SECTION
    void OnComboSection( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for lmID_COMBO_INSTRUMENT
    void OnComboInstrument( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for lmID_BUTTON_TEST_SOUND
    void OnButtonTestSoundClick( wxCommandEvent& event );

////@end lmScoreWizardClefPage event handler declarations

////@begin lmScoreWizardClefPage member function declarations

    bool TransferDataFromWindow();
    void DoProgramChange();

////@begin lmScoreWizardClefPage member variables
    wxComboBox* m_pVoiceChannelCombo;
    wxComboBox* m_pSectCombo;
    wxComboBox* m_pInstrCombo;
////@end lmScoreWizardClefPage member variables
};

//---------------------------------------------------------------------------------------
// lmScoreWizardTimePage class declaration
//---------------------------------------------------------------------------------------

class lmScoreWizardTimePage: public lmWizardPage
{
    DECLARE_DYNAMIC_CLASS( lmScoreWizardTimePage )
    DECLARE_EVENT_TABLE()

public:
    //constructors
    lmScoreWizardTimePage( );

    lmScoreWizardTimePage( wxWizard* parent);

    //creation
    bool Create(wxWizard* parent);

    //creates the controls and sizers
    void CreateControls();

////@begin lmScoreWizardTimePage event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for lmID_COMBO_MTR_INSTR1
    void OnComboMtrInstr1Selected( wxCommandEvent& event );
    void OnComboMtrInstr2Selected( wxCommandEvent& event );
    void OnButtonClick( wxCommandEvent& event );

////@begin lmScoreWizardTimePage member variables
    wxComboBox* m_pMtrInstr1Combo;
    wxComboBox* m_pMtrInstr2Combo;
////@end lmScoreWizardTimePage member variables
};
*/

#endif  // __LM_SCOREWIZARD_H__
