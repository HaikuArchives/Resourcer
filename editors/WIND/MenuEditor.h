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
class MenuListItem;
class BButton;
class BCheckBox;
class BMenuField;
class BMenuItem;
class BTextControl;

//------------------------------------------------------------------------------
class MenuEditor : public BBox
{
	public:
		MenuEditor(BRect rect, BMenu* men = NULL);

		void AttachedToWindow();
		void MessageReceived(BMessage* msg);

		void SetMenu(BMenu* menu);
		BMenu* Menu();

	private:
		uint32 MakeModMask(void);
		BMenuItem* CurrentItem();
		void OnSelectionChanged(BMessage* msg);

		DragOutlineListView*	fListView;
		BMenu*					fMenu;
		BTextControl*			fLabelText;
		BTextControl*			fMsgText;
		BTextControl*			fShortcutText;
		BMenuField*				fModifierField;
		BCheckBox*				fEnabledBox;
		BButton*				fAddItemBtn;
		BButton*				fAddSeparatorBtn;
		bool					fInit;
};
//------------------------------------------------------------------------------

#endif	// MENUEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

