//------------------------------------------------------------------------------
//	MenuEditor.h
//
//------------------------------------------------------------------------------

#ifndef MENUEDITOR_H
#define MENUEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/Box.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class DragOutlineListView;

//------------------------------------------------------------------------------
class MenuEditor : public BBox
{
	public:
		MenuEditor(BMenu *men,BRect rect);
		void MessageReceived(BMessage *msg);
		uint32 make_mod_mask(void);	

	private:
		DragOutlineListView *lview;
		BMenu *menu;
};
//------------------------------------------------------------------------------

#endif	// MENUEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

