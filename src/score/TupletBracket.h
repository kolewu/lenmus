//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LM_TUPLETBRACKET_H__        //to avoid nested includes
#define __LM_TUPLETBRACKET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TupletBracket.cpp"
#endif

#include <vector>

#include "../graphic/ShapeTuplet.h"


// lmTupletBracket
//    A lmTupletBracket represents the optional bracket graphically associated
//    to tuplets. The lmTupletBracket object does not have any effect on sound. It is
//    only to describe how a tuplet must be displayed.

class lmTupletBracket
{
public:
    lmTupletBracket(bool fShowNumber, int nNumber, bool fBracket, bool fAbove,
                    int nActualNotes, int nNormalNotes);
    ~lmTupletBracket();

    void Include(lmNoteRest* pNR);
    void Remove(lmNoteRest* pNR);
    inline int NumNotes() { return (int)m_cNotes.size(); }
    inline lmNoteRest* GetStartNote() { return m_cNotes.front(); }
    inline lmNoteRest* GetEndNote() { return m_cNotes.back(); }
    inline int GetTupletNumber() { return m_nTupletNumber; }
    inline int GetActualNotes() { return m_nActualNotes; }
    inline int GetNormalNotes() { return m_nNormalNotes; }

    //layout
	lmShape* LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color);
	lmShape* GetShape() { return m_pShape; }


private:
	int FindNote(lmNoteRest* pNR);

	//notes/rests in this bracket (if chord, only base note)
	std::vector<lmNoteRest*>	m_cNotes;

    //time modifiers
    int     m_nActualNotes;     //number of notes to play in the time ...
    int     m_nNormalNotes;     //... allotted for this number of normal notes

    // graphical attributes
    bool    	m_fShowNumber;      // display tuplet number
    int     	m_nTupletNumber;    // number to display
    bool    	m_fBracket;         // display bracket
    bool    	m_fAbove;           // bracket positioned above the notes
    wxString    m_sFontName;		// font info for rendering tuplet number
    int         m_nFontSize;
    bool        m_fBold;
    bool        m_fItalic;

	//
    lmShapeTuplet*	m_pShape;			//the shape to render the tuplet bracket

};

#endif    // __LM_TUPLETBRACKET_H__

