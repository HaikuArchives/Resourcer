//------------------------------------------------------------------------------
//	MenuListItem.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/Menu.h>
#include <interface/MenuItem.h>
#include <interface/OutlineListView.h>
#include <interface/View.h>

#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "MenuListItem.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
MenuListItem::MenuListItem(BMenuItem *ite,BOutlineListView *list)
	:	BStringItem(ite->Label())
{
	if (is_kind_of(ite,BSeparatorItem))
		SetText("-------------");
	item = ite;
	lview = list;
}
//------------------------------------------------------------------------------
void MenuListItem::CheckSub(void) {
	if (item->Submenu() != NULL) {
		BMenu *menu = item->Submenu();
		BMenuItem *cur = NULL;
		MenuListItem *toadd;
		for (int32 i = menu->CountItems()-1; menu->ItemAt(i) != NULL; i--) {
			cur = menu->ItemAt(i);
			toadd = new MenuListItem(cur,lview);
			lview->AddUnder(toadd,this);
			toadd->CheckSub();
		}
	}
}
//------------------------------------------------------------------------------
void MenuListItem::MarkAsOver(bool state,BOutlineListView *owner) {
	if (is_kind_of(item,BSeparatorItem))
		return;
	asover = state;
	owner->Invalidate(owner->ItemFrame(owner->IndexOf(this)));
}
//------------------------------------------------------------------------------
void MenuListItem::DrawItem(BView *owner, BRect itemRect, bool drawEverything = false) {
	if (asover)
		owner->SetLowColor(51,152,255);
	owner->FillRect(itemRect,B_SOLID_LOW);
	BStringItem::DrawItem(owner,itemRect,drawEverything);
	owner->SetLowColor(255,255,255);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

