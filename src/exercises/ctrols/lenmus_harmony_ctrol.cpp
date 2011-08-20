//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
//---------------------------------------------------------------------------------------

//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "TheoHarmonyCtrol.h"
//#endif
//
//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "TheoHarmonyCtrol.h"
//
//#include "../app/Processor.h"
//#include "../app/toolbox/ToolNotes.h"
//#include "../score/VStaff.h"
//#include "../score/Instrument.h"
//#include "Constrains.h"
//#include "Generators.h"
//#include "../auxmusic/Conversion.h"
//
//#include "../ldp_parser/LDPParser.h"
//
//
//#include "lenmus_injectors.h"
//#include "lenmus_colors.h"
//
////access to error's logger
//#include "../app/Logger.h"
//extern lmLogger* g_pLogger;
//
////access to MIDI manager to get default settings for instrument to use
//#include "../sound/MidiManager.h"
//
//// access to main frame
//#include "../app/MainFrame.h"
//extern lmMainFrame* GetMainFrame();
//
//#include "../app/toolbox/ToolsBox.h"
//
//#include "../auxmusic/HarmonyExercisesData.h"
//
//class lmEditorMode;
//
////------------------------------------------------------------------------------------
//// Implementation of lmTheoHarmonyCtrol
////------------------------------------------------------------------------------------
//
////IDs for controls
//const int lmID_LINK_SETTINGS = wxNewId();
//const int lmID_LINK_GO_BACK = wxNewId();
//const int lmID_LINK_NEW_PROBLEM = wxNewId();
//
//IMPLEMENT_CLASS(lmTheoHarmonyCtrol, lmFullEditorExercise)
//
//BEGIN_EVENT_TABLE(lmTheoHarmonyCtrol, lmFullEditorExercise)
//    LM_EVT_URL_CLICK    (lmID_LINK_SETTINGS, lmEBookCtrol::OnSettingsButton)
//    LM_EVT_URL_CLICK    (lmID_LINK_GO_BACK, lmEBookCtrol::OnGoBackButton)
//    LM_EVT_URL_CLICK    (lmID_LINK_NEW_PROBLEM, lmFullEditorExercise::OnNewProblem)
//END_EVENT_TABLE()
//
//
//lmTheoHarmonyCtrol::lmTheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
//                            lmHarmonyConstrains* pConstrains, wxSize nDisplaySize,
//                            const wxPoint& pos, const wxSize& size, int style)
//    : lmFullEditorExercise(parent, id, pConstrains, pos, size, style )
//{
//    //initializations
//    m_pConstrains = pConstrains;
//    m_pProblemScore = (ImoScore*)NULL;
//
//    CreateControls();
//}
//
//lmTheoHarmonyCtrol::~lmTheoHarmonyCtrol()
//{
//    //AWARE: As score and EditMode ownership is transferred to the Score Editor window,
//    //they MUST NOT be deleted here.
//}
//
//lmEditorMode* lmTheoHarmonyCtrol::CreateEditMode()
//{
//    //This method is invoked each time a new problem is generated, in order to define
//    //editor customizations for the created exercise. Ownership of created lmEditorMode object
//    //is transferred to the document and deleted there.
//
//    lmEditorMode* pEditMode = new lmEditorMode( CLASSINFO(ImoDocumentCanvas), CLASSINFO(lmHarmonyProcessor) );
//    pEditMode->ChangeToolPage(lmPAGE_NOTES, CLASSINFO(lmToolPageNotesHarmony) );
//    pEditMode->SetModeName(_T("TheoHarmonyCtrol"));
//    pEditMode->SetModeVers(_T("1"));
//
//    return pEditMode;
//}
//
//wxDialog* lmTheoHarmonyCtrol::GetSettingsDlg()
//{
//    //Returns a pointer to the dialog for customizing the exercise.
//
//    //TODO: Create the dialog class and implement it. The uncomment following code:
//    //wxDialog* pDlg = new lmDlgCfgTheoHarmony(this, m_pConstrains, m_pConstrains->IsTheoryMode());
//    //return pDlg;
//    return (wxDialog*)NULL;
//}
//
//void lmTheoHarmonyCtrol::SetNewProblem()
//{
//    //This method creates a problem score, satisfiying the restrictions imposed
//    //by exercise options and user customizations.
//
//    // TODO: Possible exercise options:
//    //         exercise 1,2:
//    //             inversions allowed
//    //             elision allowed?
//    //          allowed random key signature (if not: use fixed C major)
//    //
//    // TODO: Possible improvements:
//    //         generalize for chords of N notes
//    //         make "number of measures" dependant from the "key signature"?
//    //         calculate numerals from chord info + key signature + mode
//
//
//    //  all-exercises generic data
//    const int lmNUM_HARMONY_EXERCISES = 3;
//    const int nNUM_INTERVALS_IN_N_HARMONY_EXERCISE = 2;
//    int nNumMeasures = 2;
//    //  each-exercise specific data
//    wxString sExerciseDescription;
//    wxString sNote = _T("q"); // todo: calculate note note duration from time signature
//    wxString sLDPGoBack = wxString::Format(_T("(musicData (goBack %s) )"), sNote.c_str());
//
//    // Carlos  jun-09
//    //  Three types of problem
//    //   1) fixed bass
//    //   2) fixed soprano
//    //   3) chord notation
//
//    // select a random exercise type
//    lmRandomGenerator oGenerator;
///*@@ cambiado provisionalmente
//    nHarmonyExcerciseType = oGenerator.RandomNumber(1, lmNUM_HARMONY_EXERCISES);
//*/
//    static int nExType = 0;
//    if ( ++nExType > lmNUM_HARMONY_EXERCISES )
//        nExType = 1;
//    nHarmonyExcerciseType = nExType;
//
//    // todo: inversions allowed: make it an exercise option
//    if ( nHarmonyExcerciseType == 3)
//        bInversionsAllowedInHarmonyExercises = true;
//    else
//        bInversionsAllowedInHarmonyExercises = false;
//
//
//    wxString sPattern;
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//    ImoNote* pNoteToAttach = NULL;
//    wxString sExerciseTitle;
//    bool bDebugDisplayHiddenNotes = false; // activate only for debug
//
//    if ( nHarmonyExcerciseType >= 1 && nHarmonyExcerciseType <= lmNUM_HARMONY_EXERCISES )
//    {
//        // Prepare a score that meets the restrictions
//
//        m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
//
//        if (nHarmonyExcerciseType == 3)
//            sExerciseDescription  =  wxString::Format(_T(" Cipher the chords"));
//        else
//            sExerciseDescription  =  wxString::Format(
//                _T(" Fixed %s; root position. Complete the chord notes.")
//                , (nHarmonyExcerciseType == 1? _T("bass"): _T("soprano")) );
//
//        sExerciseTitle = wxString::Format(_T(" Exercise type %d : %s ")
//            , nHarmonyExcerciseType, sExerciseDescription.c_str());
//
//        //create a score with a bass line
//
//        wxLogMessage(_T(" CLEARING DATA "));
//        for (int nC=0; nC < nMAX_HARMONY_EXERCISE_CHORDS; nC++)
//        {
//            if (pHE_Chords[nC] != NULL)
//            {
//                wxLogMessage(_T("  deleting chord %d"), nC);
//                //@ todo: to do? delete pHE_Chords[nC];
//                pHE_Chords[nC] = NULL;
//            }
//            for (int nV=0; nV < nNUM_VOICES_IN_HARMONY_EXERCISE; nV++)
//            {
//                if (pHE_Notes[nC][nV] != NULL)
//                {
//                    wxLogMessage(_T("  deleting note chord %d v:%d"), nC, nV);
//                    //@ todo: to do? delete pHE_Notes[nC][nV];
//                    pHE_Notes[nC][nV] = NULL;
//                }
//                nHE_NotesFPitch[nC][nV] = 0;
//                sHE_Notes[nC][nV] = _T("");
//                sHE_Pattern[nC][nV] = _T("");
//            }
//        }
//
//        m_pProblemScore = new_score();
//        ImoInstrument* pInstr = m_pProblemScore->AddInstrument(
//                                    g_pMidi->DefaultVoiceChannel(),
//						            g_pMidi->DefaultVoiceInstr(), _T(""));
//
//        pVStaff = pInstr->GetVStaff();
//        pVStaff->AddStaff(5);               //add second staff: five lines, standard size
//        pVStaff->AddClef( lmE_Sol, 1 );     //G clef on first staff
//        pVStaff->AddClef( lmE_Fa4, 2 );     //F clef on second staff
//        pVStaff->AddKeySignature( m_nKey ); //key signature
//        pVStaff->AddTimeSignature(2 ,4);    //2/4 time signature
//
//
//        lmFontInfo tNumeralFont = {_T("Times New Roman"), 11, wxFONTSTYLE_NORMAL,
//                                    wxFONTWEIGHT_BOLD };
//        lmTextStyle* pNumeralStyle = m_pProblemScore->GetStyleName(tNumeralFont);
//
//        wxString sNumeralsDegrees[7] =
//        {_T(" I"), _T(" II"), _T("III"), _T(" IV"), _T("  V"), _T(" VI"), _T("VII")};
//        const int aCadences[][2] =
//        {
//            { 3, 0}, // plagal    IV I
//            { 4, 0}, // perfect   V I
//            { 4, 3}, // deceptive V IV
//            { 4, 5}, // deceptive V VI
//            { 4, 1}, // deceptive V II
//            { 3, 4}, // half     IV V
//            { 0, 4}, // half     I  V
//            { 1, 4}, // half     II V
//        };
//       const int nTotalNumCadences = sizeof(aCadences) / sizeof(aCadences[0]);
//
//        wxString sNumerals;
//
//        //loop the add notes
//        int nChordCount = 0;
//        int nOctave = 0;
//        int nVoice = 0;
//        int nRootNoteStep = 0;
//        int nStaff;
//        // cadences (TODO: improve)
//        int nCadence = 0;
//        int nInconclusive = 0;
//
//        for (int iN=0; iN < (nNumMeasures*2); iN+=2)
//        {
//            //add barline for previous measure
//            if (iN != 0)
//                pVStaff->AddBarline(lm_eBarlineSimple);
//            else
//                pVStaff->AddSpacer(20);
//
//            //two chords per measure (time signature is 2 / 4)
//            for (int iM=0; iM < 2; iM++)
//            {
//                // Generate chords with no harmonic progression errors:
//                //  Loop:
//                //     generate a random chord
//                //     create voices in FPitch, obviously matching the chord notes
//                //   Until chord ok: no errors
//                //   With the new chord:
//                //     Create figured bass
//                //     Calculate numeral
//                //     Create the score notes for the voices
//                //
//                //   Display
//                //       Exercise 1: bass note and numeral
//                //       Exercise 2: soprano note and numeral
//                //       Exercise 3: all notes and numeral
//                //
//
//                int nNumChordLinkErros = -1;
//                int nAttempts = 0;
//                int nMaxAttempts = 20;
//                int nInversions = 0;
//
//                // try to create a new chord until no link error with previous chords
//                pHE_Chords[nChordCount] = 0;
//                wxLogMessage(_T(" ====== START WITH CHORD %d ======= "), nChordCount );
//                while ( nNumChordLinkErros != 0 && nAttempts < nMaxAttempts)
//                {
//                    if (nAttempts)
//                        wxLogMessage(_T("   ***** NEW ATTEMPT (%d) for CHORD %d *****"), nAttempts, nChordCount );
//                    // Root note
//                    if (nChordCount > 1 && nChordCount < nMAX_HARMONY_EXERCISE_CHORDS) // 2 last chords: use a cadence
//                    {
//                        if (nChordCount == 2) // choose cadence
//                        {
//                            nInconclusive = oGenerator.RandomNumber(0, 1);
//                            if (nInconclusive)
//                            {
//                                nCadence = oGenerator.RandomNumber(2, nTotalNumCadences-1);
//                            }
//                            else
//                            {
//                                nCadence = oGenerator.RandomNumber(0, 1);
//                            }
//                            wxLogMessage(_T(" @CHORD %d CADENCE:%d"), nChordCount, nCadence );
//                        }
//                        nRootNoteStep = aCadences[nCadence][nChordCount-2];
//                        if (nRootNoteStep < 3)
//                            nOctave = 3;
//                        else
//                            nOctave = 2;
//                        wxLogMessage(_T(" @CHORD %d Cadence:%d nInconclusive:%d  rootStep:%d  octave:%d")
//                            , nChordCount, nCadence, nInconclusive, nRootNoteStep, nOctave );
//                    }
//                    else
//                    {
//                        nOctave = oGenerator.RandomNumber(2, 3);
//                        // this is done to make the notes appear more centered in the bass staff
//                        if (nOctave == 3 ) // octave 3 : notes c,d,e
//                           nRootNoteStep = oGenerator.RandomNumber(0, 2);
//                        else // octave 2 : notes f,g,a,b
//                           nRootNoteStep = oGenerator.RandomNumber(3, 6);
//                    }
//                    nInversions = 0;
//                    if (bInversionsAllowedInHarmonyExercises)
//                    {
//                        // Calculate a random number of inversions and apply them
//                        nInversions = oGenerator.RandomNumber(0, nNUM_INTERVALS_IN_N_HARMONY_EXERCISE);
//                    }
//
//                    if (pHE_Chords[nChordCount])
//                    {
//                        if ( nAttempts == 0)
//                        {
//                            wxLogMessage(_T(" @@ ERROR: impossible:  chord no emtpy but 0 previous attempts ") );
//                        }
//                        else
//                        {
//                            wxLogMessage(_T(" deleting chord %d of previous attempt "), nChordCount );
//                        }
//                        delete pHE_Chords[nChordCount];
//                        pHE_Chords[nChordCount] = 0;
//                    }
//
//                    //
//                    // create the chord
//                    //
//                    wxLogMessage(_T("Creating_ImoScoreChord: step:%d octave:%d inversions:%d  key:%d")
//                        , nRootNoteStep, nOctave, nInversions, m_nKey );
//                    pHE_Chords[nChordCount] = new ImoScoreChord(nRootNoteStep, m_nKey
//                        , nNUM_INTERVALS_IN_N_HARMONY_EXERCISE, nInversions, nOctave);
//
//
//                    // This is the bass voice (root note)
//                    nHE_NotesFPitch[nChordCount][nBassVoiceIndex] = pHE_Chords[nChordCount]->GetNote(0);
//                    // but... additional limitation:   bass note might be too high (if there are 2 inversions for example)
//                    //   we set a limit in d3
//                    if (nHE_NotesFPitch[nChordCount][nBassVoiceIndex] > FPitchK(lmSTEP_D, 3, m_nKey) )
//                    {
//                        nHE_NotesFPitch[nChordCount][nBassVoiceIndex] -= lm_p8;
//                        wxLogMessage(_T(" Bass reduced one octave to : %d ")
//                            , nHE_NotesFPitch[nChordCount][nBassVoiceIndex]
//                            );
//                    }
//                    wxLogMessage(_T(" Bass voice V%d,FINAL: %d (%s)")
//                        , nBassVoice
//                        , nHE_NotesFPitch[nChordCount][nBassVoiceIndex]
//                        , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).c_str());
//
//                    // Bass voice final value calculated
//                    // Set the bass voice note in the chord
//                    pHE_Chords[nChordCount]->AddNoteLmFPitch(nHE_NotesFPitch[nChordCount][nBassVoiceIndex]);
//
//                    wxLogMessage(_T("<><>  CHORD %d: bass STEP:%d, octave:%d, key:%d inversions:%d ROOT:%d (%s) ===")
//                        ,nChordCount, nRootNoteStep, nOctave, m_nKey, nInversions
//                        ,FPitchK(nRootNoteStep, nOctave, m_nKey)
//                        ,FPitch_ToAbsLDPName( nHE_NotesFPitch[nChordCount][nBassVoiceIndex]).c_str()
//                        );
//                    wxLogMessage(_T("\t lmChord: [%s]"), pHE_Chords[nChordCount]->ToString().c_str());
//
//
//                    //
//                    // Create the rest of voices: 3,2,1 (tenor, baritone, soprano)
//                    //   by adding intervals (and octaves) to the bass
//                    //
//
//                    //  There are three possible values, based on intervals,  for each voice:
//                    //    1: bass + N octaves (0 interval: duplicate root)
//                    //    2: bass + N octaves + 1st interval
//                    //    3: bass + N octaves + 2nd interval
//                    //  Notes:
//                    //    if there is 5th elided: do not apply rule 3
//                    //    if root note is NOT duplicated: do not apply rule 1
//                    //  For this exercise we intend to generate 'normal' (not very strange) chords, so
//                    //    better to calculate each voice using all 3 rules and not repeating any of them
//                    //     (it implies duplicate always the root)
//                    lmFPitch nIntvB[3];
//                    nIntvB[0] = oGenerator.RandomNumber(0, 2);
//                    nIntvB[1] = (nIntvB[0] + oGenerator.RandomNumber(1, 2)) % 3 ;
//                    nIntvB[2] = 3 - (nIntvB[0] + nIntvB[1] ) ;
//                    wxLogMessage(_T("\t\t nIntvB %d %d %d"), nIntvB[0], nIntvB[1], nIntvB[2]);
//
//                    int ni = 0; // index to nIntvB
//                    for (nVoice = nNUM_VOICES_IN_HARMONY_EXERCISE-1; nVoice>=1; nVoice--,ni++)
//                    {
//                        assert(pHE_Chords[nChordCount] != NULL);
//
//                        int nVoiceIndex = nVoice - 1;
//
//                        // start with BASS NOTE
//                        nHE_NotesFPitch[nChordCount][nVoiceIndex] = nHE_NotesFPitch[nChordCount][nBassVoiceIndex];
//
//                        // Apply the calculated intervals
//                        if (nIntvB[ni])
//                        {
//                            nHE_NotesFPitch[nChordCount][nVoiceIndex] += pHE_Chords[nChordCount]->GetInterval(nIntvB[ni]);
//                        }
//
//                        wxLogMessage(_T("  \tV%d, after applying interval %d : %d (%s), [ni:%d]")
//                            , nVoice
//                            , nIntvB[ni]
//                            , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).c_str()
//                            , ni
//                            );
//
//                        // Limitation: a voice can not be lower than the previous...
//                        while ( nHE_NotesFPitch[nChordCount][nVoiceIndex+1] >= nHE_NotesFPitch[nChordCount][nVoiceIndex])
//                        {
//                            nHE_NotesFPitch[nChordCount][nVoiceIndex] += lm_p8;
//                            wxLogMessage(_T(" Added octave to voice V%d: %d ")
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nVoiceIndex]);
//                        }
//
//                        // additional limitation: increase tenor voice a octave if bass is low and tenor is close to bass
//                        //  to avoid voice distance problems
//                        //  aware: tenor-bass is the only consecutive voices allowed to have a distance higher than octave
//                        if ( nVoiceIndex == nTenorVoiceIndex && nOctave < 3
//                            && (nHE_NotesFPitch[nChordCount][nTenorVoiceIndex] - nHE_NotesFPitch[nChordCount][nBassVoiceIndex]) <= lm_M3 )
//                        {
//                            nHE_NotesFPitch[nChordCount][nTenorVoiceIndex] += lm_p8;
//                            wxLogMessage(_T(" Raise Tenor: added octave to voice V%d: %d ")
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nTenorVoiceIndex]
//                                );
//                        }
//
//                        // additional limitation: baritone voice should be in upper staff (aprox. octave should be > 3)
//                        const int fUpperStaffLimit = (lm_p8*4)-lm_M3;
//                        // aware: do not raise more than one octave; otherwise a rule is broken (octave distance)
//                        if ( nVoiceIndex == nBaritoneVoiceIndex && nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex] < fUpperStaffLimit )
//                        {
//                            nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex] += lm_p8;
//                            wxLogMessage(_T(" Raise to 2nd staff: added octave to voice V%d: %d (min:%d) ")
//                                , nVoice
//                                , nHE_NotesFPitch[nChordCount][nBaritoneVoiceIndex]
//                                , fUpperStaffLimit);
//                        }
//
//                        wxLogMessage(_T("V%d,    FINAL: %d (%s)")
//                            , nVoice
//                            , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                            , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).c_str());
//
//                       // set this note in the chord
//                       pHE_Chords[nChordCount]->AddNoteLmFPitch(nHE_NotesFPitch[nChordCount][nVoiceIndex]);
//
//                    } // for voice...
//
//                    // check harmonic progression errors
//                    wxLogMessage(_T("###BEFORE %d ANALYSIS OF CHORD LINK ERRORS OF  CHORD %d: %s")
//                         ,nAttempts, nChordCount , pHE_Chords[nChordCount]->ToString().c_str());
//                    nNumChordLinkErros =  AnalyzeHarmonicProgression(pHE_Chords, nChordCount+1, 0);
//
//                    wxLogMessage(_T("##RESULT: %d LINK ERRORS"), nNumChordLinkErros);
//
//                    if (nNumChordLinkErros == 0)
//                         wxLogMessage(_T("<<<<< CHORD %d OK!!! after %d attempts >>>>>>>>>")
//                              , nChordCount , nAttempts, nNumChordLinkErros);
//
//                    nAttempts++;
//
//                    if  ( nAttempts > nMaxAttempts)
//                    {
//                       wxLogMessage(_T("\n **** CHORD %d TO MANY TRIES ******\n"), nChordCount);
//                    }
//
//                }
//
//                // Calculate the figured bass
//                //
//                //  build a chord from a list of notes in LDP source code
//                //    lmChord(int nNumNotes, wxString* pNotes, lmEKeySignatures nKey = earmDo);
//                pHE_FiguredBass[nChordCount] = new lmFiguredBass(pVStaff, lmNEW_ID
//                    , pHE_Chords[nChordCount], m_nKey);
//
//                wxLogMessage(_T("  FIGURED BASS:%s")
//                   , pHE_FiguredBass[nChordCount]->GetFiguredBassString().c_str());
//
//                // At this point we already have the notes in FPitch. Now:
//                //  Create the notes of the score
//                //  Display notes and numerals
//
//                //
//                // Create each ImoNote
//                //
//                wxString sUpDown[2] = {  _T("up"), _T("down")};
//                nVoice=1; // Voices 1(soprano)(index:0) to 4(bass)(index:3)
//                int nNumDisplayedNotesInChord = 0;
//                for (nStaff=1; nStaff<3; nStaff++)  // V1 (soprano) in P1 (upper staff)
//                {
//                    for (int nVoiceInStaff=1; nVoiceInStaff<3; nVoiceInStaff++,nVoice++)
//                    {
//                        assert(nVoice >= 1 && nVoice <= nNUM_VOICES_IN_HARMONY_EXERCISE);
//
//                        // aware: bass note: voice 4
//                       int nVoiceIndex = nVoice-1;
//                        sHE_Notes[nChordCount][nVoiceIndex] = wxString::Format(_T("%s")
//                           , FPitch_ToAbsLDPName(nHE_NotesFPitch[nChordCount][nVoiceIndex]).c_str());
//
//                        // Calculate the pattern required to create the note in the score
//                        sHE_Pattern[nChordCount][nVoiceIndex] = wxString::Format(_T("(n %s %s p%d v%d (stem %s))")
//                           , sHE_Notes[nChordCount][nVoiceIndex].c_str()
//                           , sNote.c_str()
//                           , nStaff, nVoice, sUpDown[nVoiceInStaff-1].c_str());
//
//                        wxLogMessage(_T("\t Staff %d, V%d (index:%d) %d [%s], pattern: %s")
//                              , nStaff
//                              , nVoice
//                              , nVoiceIndex
//                              , nHE_NotesFPitch[nChordCount][nVoiceIndex]
//                              , sHE_Notes[nChordCount][nVoiceIndex].c_str()
//                              , sHE_Pattern[nChordCount][nVoiceIndex].c_str()
//                              );
//
//
//
//                        // Display the notes in the score
//                        //  Exercise 1: only bass (voice 1)
//                        //  Exercise 2: only soprano (voice 4)
//                        //  Exercise 3: all
//                        // Go back:
//                        // Exercise 1, 2: Never (only one voice is displayed per chord)
//                        // Exercise 3: go back after voices 1,2,3
//
//                        if (
//                            (nHarmonyExcerciseType == 1 && nVoice == nBassVoice ) ||
//                            (nHarmonyExcerciseType == 2 && nVoice == nSopranoVoice ) ||
//                             nHarmonyExcerciseType == 3
//                            )
//                        {
//                            if ( nNumDisplayedNotesInChord > 0 && nNumDisplayedNotesInChord < nNUM_VOICES_IN_HARMONY_EXERCISE)
//                            {
//                                pNode = parserLDP.ParseText( sLDPGoBack );
//                                parserLDP.AnalyzeMusicData(pNode, pVStaff);
//                            }
//                            pNode = parserLDP.ParseText(  sHE_Pattern[nChordCount][nVoiceIndex] );
//                            pHE_Notes[nChordCount][nVoiceIndex] = parserLDP.AnalyzeNote(pNode, pVStaff);
//                            nNumDisplayedNotesInChord++;
//                            pHE_Notes[nChordCount][nVoiceIndex]->SetVoice(nVoice);
//                            pNoteToAttach = pHE_Notes[nChordCount][nVoiceIndex];
//                            wxLogMessage(_T(" V:%d added pattern: %s ***")
//                                , nVoice, sHE_Pattern[nChordCount][nVoiceIndex].c_str());
//
//                            // Set the actual ImoNote in the chord
//                            pHE_Chords[nChordCount]->SetLmNote(pNoteToAttach);
//
//                            if (  bDebugDisplayHiddenNotes ) // only for the debug
//                            {
//                                wxColour myBlue( 10, 10, 200);
//                                pNoteToAttach->SetColour(myBlue);
//                                wxLogMessage(_T("    Ej:%d V:%d AZUL"), nHarmonyExcerciseType, nVoice);
//                            }
//                            else    wxLogMessage(_T("    Ej:%d V:%d NEGRO"), nHarmonyExcerciseType, nVoice);
//                        }
//                        else    wxLogMessage(_T("    Ej:%d V:%d OCULTA"), nHarmonyExcerciseType, nVoice);
//
//                    }
//
//                }
//
//                //    Display the numeral, according the root step
////todo remove                lmStepType nRootStep = FPitch_Step(pHE_Chords[nChordCount]->GetNormalizedRoot());
//                lmStepType nRootStep = pHE_Chords[nChordCount]->GetChordDegree();
//                // todo: use GetDegreeString( pHE_Chords[nChordCount]->GetChordDegree() )
//                lmTextItem* pNumeralText = new lmTextItem(
//                    pNoteToAttach, lmNEW_ID, sNumeralsDegrees[nRootStep],
//                    lmHALIGN_DEFAULT, pNumeralStyle);
//                pNoteToAttach->AttachAuxObj(pNumeralText);
//                pNumeralText->SetUserLocation(0.0f, 230.0f );
//
//                wxLogMessage(_T("FINAL_CHORD %d: %s")
//                    , nChordCount, pHE_Chords[nChordCount]->ToString().c_str());
//
//                nChordCount++;
//            }
//        }
//        nHarmonyExerciseChordsToCheck = nChordCount;
//    }
//    wxLogMessage(_T(" CREATED EXERCISE %d with %d chords")
//        ,nHarmonyExcerciseType, nHarmonyExerciseChordsToCheck);
//    for (int i=0; i<nHarmonyExerciseChordsToCheck; i++)
//        wxLogMessage(_T(" CHORD %d: %s"), i, pHE_Chords[i]->lmFPitchChord::ToString().c_str());
//
//
//    //add final barline
//    pVStaff->AddBarline(lm_eBarlineEnd);
//
//    lmFontInfo tTitleFont = {_T("Times New Roman"), 10, wxFONTSTYLE_NORMAL,
//                                wxFONTWEIGHT_BOLD };
//    lmTextStyle* pTitleStyle = m_pProblemScore->GetStyleName(tTitleFont);
//    ImoScoreTitle* pTitle = m_pProblemScore->AddTitle(sExerciseTitle, lmHALIGN_CENTER, pTitleStyle);
//    lmLocation tTitlePos = g_tDefaultPos;
//    pTitle->SetUserLocation(tTitlePos); // only necessary if wanted to be positioned at a specific point
//
//    //set the name and the title of the score
//    m_pProblemScore->SetScoreName( sExerciseTitle );
//}
//
//void lmTheoHarmonyCtrol::OnSettingsChanged()
//{
//    //This method is invoked when user clicks on the 'Accept' button in
//    //the exercise setting dialog. You receives control just in case
//    //you would like to do something (i.e. reconfigure exercise displayed
//    //buttons to take into account the new exercise options chosen by the user).
//
//    //In this exercise there is no needed to do anything
//}
//
//void lmTheoHarmonyCtrol::InitializeStrings()
//{
//    //This method is invoked only once: at control creation time.
//    //Its purpose is to initialize any variables containing strings, so that
//    //they are translated to the language chosen by user. Take into account
//    //that those strings requiring translation can not be statically initialized,
//    //as at compilation time we know nothing about desired language.
//
//    //In this exercise there is no needed to translate anything
//}
