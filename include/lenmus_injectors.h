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

#ifndef __LENMUS_INJECTORS_H__
#define __LENMUS_INJECTORS_H__

//lomse
#include <lomse_doorway.h>
#include <lomse_score_player.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/string.h>
#include <wx/config.h>

#include <iostream>
#include <sstream>
using namespace std;

namespace lenmus
{

//forward declarations
class Paths;
class MidiServer;
class Logger;
class Colors;


//---------------------------------------------------------------------------------------
class ApplicationScope
{
protected:
    ostream& m_reporter;
    LomseDoorway m_lomse;
    Paths* m_pPaths;
    wxConfigBase* m_pPrefs;
    MidiServer* m_pMidi;
    ScorePlayer* m_pPlayer;
    LibraryScope* m_pLomseScope;
    Logger* m_pLogger;
    Colors* m_pColors;

    wxString m_sAppName;
    wxString m_sVendorName;
    wxString m_sVersionString;
    wxString m_sHomeDir;

    ostringstream   m_lomseReporter;    //to have access to error messages
    streambuf*      m_cout_buffer;      //to restore cout

    //some global options
    bool m_fAnswerSoundsEnabled;
    bool m_fAutoNewProblem;
    bool m_fForceReleaseBehaviour;
    bool m_fShowDebugLinks;
    //bool m_fReleaseVersion;

public:
    ApplicationScope(ostream& reporter=cout);
    ~ApplicationScope();


    //settings
    void set_bin_folder(const wxString& sBinPath);
    void create_preferences_object();
    void create_logger();

    //access to global objects/variables
    Paths* get_paths();
    wxConfigBase* get_preferences();
    MidiServer* get_midi_server();
    ScorePlayer* get_score_player();
    Colors* get_colors();

//    inline ostream& default_reporter() { return m_reporter; }
    inline LomseDoorway& get_lomse() { return m_lomse; }
    inline ostringstream& get_lomse_reporter() { return m_lomseReporter; }

    //app info
    inline wxString& get_app_name() { return m_sAppName; }
    inline wxString& get_vendor_name() { return m_sVendorName; }
    inline wxString& get_version_string() { return m_sVersionString; }
    wxString get_app_full_name();

    //global options
    inline bool are_answer_sounds_enabled() { return m_fAnswerSoundsEnabled; }
    inline void enable_answer_sounds(bool value) { m_fAnswerSoundsEnabled = value; }
    inline bool is_auto_new_problem_enabled() { return m_fAutoNewProblem; }
    inline void enable_auto_new_problem(bool value) { m_fAutoNewProblem = value; }
    inline void set_force_release_behaviour(bool value) { m_fForceReleaseBehaviour = value; }
    inline bool force_release_behaviour() { return m_fForceReleaseBehaviour; }
    inline void set_show_debug_links(bool value) { m_fShowDebugLinks = value; }
    inline bool show_debug_links() { return m_fShowDebugLinks; }

    //global optionf for debug
    //inline void set_justify_systems(bool value) { m_sAppName = value; }

//    inline void set_dump_column_tables(bool value) { m_fDumpColumnTables = value; }
//    inline bool dump_column_tables() { return m_fDumpColumnTables; }

protected:
    void set_version_string();
    void initialize_lomse();

};

////---------------------------------------------------------------------------------------
//class DocumentScope
//{
//public:
//    DocumentScope(ostream& reporter=cout) : m_reporter(reporter) {}
//    ~DocumentScope() {}
//
//    ostream& default_reporter() { return m_reporter; }
//    IdAssigner* id_assigner() { return &m_idAssigner; }
//
//protected:
//    ostream& m_reporter;
//    IdAssigner m_idAssigner;
//
//};

//---------------------------------------------------------------------------------------
class Injector
{
public:
    Injector() {}
    ~Injector() {}

//    static LdpParser* inject_LdpParser(ApplicationScope& libraryScope,
//                                       DocumentScope& documentScope);
//    static Analyser* inject_Analyser(ApplicationScope& libraryScope,
//                                     DocumentScope& documentScope);
//    static ModelBuilder* inject_ModelBuilder(DocumentScope& documentScope);
//    static LdpCompiler* inject_LdpCompiler(ApplicationScope& libraryScope,
//                                           DocumentScope& documentScope);
//    static Document* inject_Document(ApplicationScope& libraryScope);
//    static ScreenDrawer* inject_ScreenDrawer(ApplicationScope& libraryScope);
////    static UserCommandExecuter* inject_UserCommandExecuter(Document* pDoc);
//    static View* inject_View(ApplicationScope& libraryScope, int viewType, Document* pDoc);  //UserCommandExecuter* pExec)
//    static SimpleView* inject_SimpleView(ApplicationScope& libraryScope, Document* pDoc);  //UserCommandExecuter* pExec)
//    static VerticalBookView* inject_VerticalBookView(ApplicationScope& libraryScope,
//                                                     Document* pDoc);  //UserCommandExecuter* pExec)
//    static HorizontalBookView* inject_HorizontalBookView(ApplicationScope& libraryScope,
//                                                         Document* pDoc);  //UserCommandExecuter* pExec)
//    static Interactor* inject_Interactor(ApplicationScope& libraryScope,
//                                         Document* pDoc, View* pView);   //, UserCommandExecuter* pExec);
//    static Presenter* inject_Presenter(ApplicationScope& libraryScope,
//                                       int viewType, Document* pDoc);
//    static Task* inject_Task(int taskType, Interactor* pIntor);

};



}   //namespace lenmus

#endif      //__LENMUS_INJECTORS_H__
