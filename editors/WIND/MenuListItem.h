//------------------------------------------------------------------------------
//	MenuListItem.h
//
//------------------------------------------------------------------------------

#ifndef MENULISTITEM_H
#define MENULISTITEM_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/ListItem.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BOutlineListView;
class BView;

//------------------------------------------------------------------------------
class MenuListItem : public BStringItem
{
	public:
		MenuListItem(BMenuItem *ite,BOutlineListView *list);

		void CheckSub(void);
		void MarkAsOver(bool state,BOutlineListView *owner);
		void DrawItem(BView *owner, BRect itemRect, bool drawEverything = false);

		BMenuItem *item;

	private:
		BOutlineListView *lview;
		bool asover;
};
//------------------------------------------------------------------------------

#endif	// MENULISTITEM_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

