//------------------------------------------------------------------------------
//	DragOutlineListView.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <string.h>

// System Includes -------------------------------------------------------------
#include <interface/Alert.h>
#include <interface/CheckBox.h>
#include <interface/Menu.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/TextControl.h>
#include <interface/Window.h>

#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "DragOutlineListView.h"
#include "MenuListItem.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
DragOutlineListView::DragOutlineListView(BRect rect, BMenu* men)
	:	BOutlineListView(rect, "itemeditor")
{
	SetMenu(men);
}
//------------------------------------------------------------------------------
void DragOutlineListView::Refresh(bool noItemsAdded)
{
	int32 old;
	if (noItemsAdded)
		old = FullListCurrentSelection();
	MakeEmpty();
	SelectionChanged();
	LoadMenu();
	if (noItemsAdded)
		Select(old);
}
//------------------------------------------------------------------------------
void DragOutlineListView::SelectionChanged(void)
{
#if 0
	Window()->Lock();
	if ((CurrentSelection() < 0) || is_kind_of(((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item,BSeparatorItem)) {
		((BTextControl *)(Parent()->Parent()->FindView("label")))->SetText("");
		((BTextControl *)(Parent()->Parent()->FindView("label")))->SetEnabled(false);
		((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetText("none");
		((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetEnabled(false);
		((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetText("");
		((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(false);
		((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetEnabled(false);
		((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_OFF);
		if (Window()->IsLocked())
			Window()->Unlock();
		return;
	} else {
		((BTextControl *)(Parent()->Parent()->FindView("label")))->SetEnabled(true);
		((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetEnabled(true);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(false);
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(false);
		((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetEnabled(true);
		if (((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->IsEnabled())
			((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_ON);
		else
			((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_OFF);
		if (((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->Submenu() == NULL) {
			((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(true);
			((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(true);
		} else {
			((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(false);
			((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(false);
		}
	}
	((BTextControl *)(Parent()->Parent()->FindView("label")))->SetText(((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->Label());
	char msg[5];
	BMenuItem *item = ((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item;
	if (item->Message() != NULL)
		strncpy(msg,(char *)(new int32(flipcode(item->Message()->what))),4);
	else
		strcpy(msg,"none");
	msg[4] = 0;
	BTextControl *tcont = (BTextControl *)(Parent()->Parent()->FindView("msg"));
	tcont->SetText(msg);
	uint32 mod;
	msg[1] = 0;
	msg[0] = item->Shortcut(&mod);
	tcont = (BTextControl *)(Parent()->Parent()->FindView("shrtct"));
	tcont->SetText(msg);
	if (mod & B_OPTION_KEY)
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(true);
	if (mod & B_SHIFT_KEY)
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(true);
	if (mod & B_CONTROL_KEY)
		((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(true);
	if (Window()->IsLocked())
		Window()->Unlock();
#endif
}
//------------------------------------------------------------------------------
bool DragOutlineListView::InitiateDrag(BPoint /*point*/, int32 index,
									   bool wasSelected)
{
	if (!wasSelected)
		return false;
	BMessage *msg = new BMessage('dolv');
	msg->AddPointer("item",ItemAt(index));
	DragMessage(msg, ItemFrame(index));
	thread_id thread = spawn_thread(track_over,"watch_mouse_over",B_NORMAL_PRIORITY,this);
	if ((thread) < B_OK) 
		(new BAlert("ck","Could not spawn watch_mouse_over! You are probably out of memory.","OK"))->Go();
	else 
		resume_thread(thread);
	return true;
}
//------------------------------------------------------------------------------
void DragOutlineListView::KeyDown(const char *bytes,int32 /*numBytes*/)
{

	MenuListItem *litem;
	if ((*bytes == B_DELETE) || (*bytes == B_BACKSPACE)) {
		litem = ((MenuListItem *)(ItemAt(CurrentSelection())));
		litem->item->Menu()->RemoveItem(litem->item);
		Refresh(false);
	}
}
//------------------------------------------------------------------------------
void DragOutlineListView::WatchMouse(void)
{
	BPoint newloc;
	BRect hello;
	float issue;
	int32 ind = -1024;
	uint32 buttons;
	float y = -20;
	MenuListItem *temp = NULL;
	for (;true;) {
		Window()->Lock();
		GetMouse(&newloc,&buttons,false);
		if (buttons != B_PRIMARY_MOUSE_BUTTON) {
			if (temp != NULL)
				temp->MarkAsOver(false,this);
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
			if (Window()->IsLocked())
				Window()->Unlock();
			return;
		}
		ind = IndexOf(newloc);
		if (ind < 0) {
			if (y == ItemFrame(IndexOf(LastItem())).bottom + 1) {
				if (Window()->IsLocked())
					Window()->Unlock();
				continue;
			}
			if (temp != NULL)
				temp->MarkAsOver(false,this);
			temp = NULL;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
			y = ItemFrame(IndexOf(LastItem())).bottom + 1;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS); 
			if (Window()->IsLocked())
				Window()->Unlock();
			continue;
		}
		hello = ItemFrame(ind);
		issue = (hello.Height() / 3);
		if (newloc.y > (hello.top + (issue * 2))) {
			if (y == hello.bottom) {
				if (Window()->IsLocked())
					Window()->Unlock();
				continue;
			}
			if (temp != NULL)
				temp->MarkAsOver(false,this);
			temp = NULL;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
			y = hello.bottom;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS); 
			if (Window()->IsLocked())
				Window()->Unlock();
			continue;
		}
		if (newloc.y > (hello.top + issue)) {
			if (y == -283 + ind) {
				if (Window()->IsLocked())
					Window()->Unlock();
				continue;
			}
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
			y = -283 + ind;
			if (temp != NULL)
				temp->MarkAsOver(false,this);
			if (is_kind_of(((MenuListItem *)(ItemAt(ind)))->item,BSeparatorItem)) {
				temp = NULL;
				if (Window()->IsLocked())
					Window()->Unlock();
				continue;
			}
			temp = ((MenuListItem *)(ItemAt(ind)));
			temp->MarkAsOver(true,this);
			if (Window()->IsLocked())
				Window()->Unlock();
			continue;
		}
		if (newloc.y > hello.top) {
			if (y == hello.top) {
				if (Window()->IsLocked())
					Window()->Unlock();
				continue;
			}
			if (temp != NULL)
				temp->MarkAsOver(false,this);
			temp = NULL;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
			y = hello.top;
			StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS);
			if (Window()->IsLocked())
				Window()->Unlock();
			continue;
		}
		if (Window()->IsLocked())
				Window()->Unlock();
	}
}
//------------------------------------------------------------------------------
void DragOutlineListView::MessageReceived(BMessage *msg)
{
	if (!(msg->WasDropped()) || msg->what != 'dolv')
	{
		BOutlineListView::MessageReceived(msg);
		return;
	}

	MenuListItem *item;
	msg->FindPointer("item",(void **)(&item));
	BPoint newloc = msg->DropPoint();
	newloc = ConvertFromScreen(newloc);
	int32 ind = IndexOf(newloc);
	BMenu *men;
	if (ind < 0) {
		if (!fMenu)
		{
			debugger("DragOutlineListView::fMenu is NULL");
		}
		BMenuItem *menite = item->item->Menu()->Superitem();
		men = item->item->Menu();
		men->RemoveItem(item->item);
		fMenu->AddItem(item->item);
		if (menite != NULL) {
			if (menite->Submenu() != NULL) {
				if (menite->Submenu()->CountItems() == 0)
					RemoveSubMenu(menite);
			}
		}
		Refresh(false);
		return;
	}
	if (ItemAt(ind) == item)
		return;
	BRect hello = ItemFrame(ind);
	float issue = (hello.Height() / 3);
	if (newloc.y > (hello.top + (issue * 2))) {
		MenuListItem *lite = (MenuListItem *)(ItemAt(ind));
		BMenuItem *menite = item->item->Menu()->Superitem();
		men = item->item->Menu();
		men->RemoveItem(item->item);
		lite->item->Menu()->AddItem(item->item,lite->item->Menu()->IndexOf(lite->item) + 1);
		if (menite != NULL) {
			if (menite->Submenu() != NULL) {
				if (menite->Submenu()->CountItems() == 0)
					RemoveSubMenu(menite);
			}
		}
		Refresh(false);
		return;
	}
	if (newloc.y > (hello.top + issue)) {
		if (is_kind_of(((MenuListItem *)(ItemAt(ind)))->item,BSeparatorItem))
			return;
		BMenuItem *menite = item->item->Menu()->Superitem();
		men = item->item->Menu();
		AddSubItem(item,(MenuListItem *)(ItemAt(ind)));
		if (menite != NULL) {
			if (menite->Submenu() != NULL) {
				if (menite->Submenu()->CountItems() == 0)
					RemoveSubMenu(menite);
			}
		}
		Refresh(false);
		return;
	}
	if (newloc.y > hello.top) {
		BMenuItem *menite = item->item->Menu()->Superitem();
		MenuListItem *lite = (MenuListItem *)(ItemAt(ind));
		item->item->Menu()->RemoveItem(item->item);
		lite->item->Menu()->AddItem(item->item,lite->item->Menu()->IndexOf(lite->item));
		if (menite != NULL) {
			if (menite->Submenu() != NULL) {
				if (menite->Submenu()->CountItems() == 0)
					RemoveSubMenu(menite);
			}
		}
		Refresh(false);
		return;
	}
	if (Window()->IsLocked())
		Window()->Unlock();
}
//------------------------------------------------------------------------------
void DragOutlineListView::AddSubItem(MenuListItem *add, MenuListItem *tothis)
{
	BMenuItem *toadd = add->item;
	BMenuItem *addunder = tothis->item;
	BMenuItem *temp;
	BMenu *men;
	int32 ind;
	if (addunder->Submenu() == NULL) {
		temp = new BMenuItem(new BMenu(addunder->Label()),new BMessage(*addunder->Message()));
		men = addunder->Menu();
		ind = men->IndexOf(addunder);
		men->RemoveItem(addunder);
		delete addunder;
		addunder = temp;
		men->AddItem(addunder,ind);
	}
	toadd->Menu()->RemoveItem(toadd);
	addunder->Submenu()->AddItem(toadd);
	tothis->item = addunder;
}
//------------------------------------------------------------------------------
void DragOutlineListView::RemoveSubMenu(BMenuItem *toremfrom)
{
	BMenuItem *temp;
	int32 ind;
	BMenu *men;
	BMenuItem *addunder = toremfrom;
	if (addunder->Submenu() == NULL)
		return;
	temp = new BMenuItem(addunder->Label(),new BMessage(*addunder->Message()));
	men = addunder->Menu();
	ind = men->IndexOf(addunder);
	men->RemoveItem(addunder);
	delete addunder;
	addunder = temp;
	men->AddItem(addunder,ind);
}
//------------------------------------------------------------------------------
void DragOutlineListView::SetMenu(BMenu* menu)
{
	fMenu = menu;
	LoadMenu();
}
//------------------------------------------------------------------------------
BMenu* DragOutlineListView::Menu()
{
	return fMenu;
}
//------------------------------------------------------------------------------
void DragOutlineListView::LoadMenu()
{
	if (fMenu)
	{
		BMenuItem *cur = NULL;
		for (int32 i = 0; fMenu->ItemAt(i) != NULL; i++)
		{
			cur = fMenu->ItemAt(i);
			MenuListItem *toadd = new MenuListItem(cur,this);
			AddItem(toadd);
			toadd->CheckSub();
		}
	}
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

