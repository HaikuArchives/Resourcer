//------------------------------------------------------------------------------
//	DragOutlineListView.h
//
//------------------------------------------------------------------------------

#ifndef DRAGOUTLINELISTVIEW_H
#define DRAGOUTLINELISTVIEW_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/OutlineListView.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BMenu;
class MenuListItem;

//------------------------------------------------------------------------------
class DragOutlineListView : public BOutlineListView
{
	public:
		DragOutlineListView(BRect rect,BMenu *men);

		void Refresh(bool noItemsAdded);
		void SelectionChanged(void);
		bool InitiateDrag(BPoint point, int32 index, bool wasSelected);
		void KeyDown(const char *bytes,int32 numBytes);
		void WatchMouse(void);
		void MessageReceived(BMessage *msg);
		void AddSubItem(MenuListItem *add, MenuListItem *tothis);
		void RemoveSubMenu(BMenuItem *toremfrom);

	private:
		BMenu *menu;
};
//------------------------------------------------------------------------------

#endif	// DRAGOUTLINELISTVIEW_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

