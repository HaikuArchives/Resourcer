//------------------------------------------------------------------------------
//	MenuEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/Menu.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/ScrollView.h>
#include <interface/TextControl.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "DragOutlineListView.h"
#include "MenuEditor.h"
#include "MenuListItem.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
MenuEditor::MenuEditor(BMenu *men,BRect rect) : BBox(rect,"menueditor") {
	menu = men;
	SetViewColor(216,216,216);
	lview = new DragOutlineListView(BRect(10,10,rect.Width() - 10 - B_V_SCROLL_BAR_WIDTH,100),menu);
	AddChild(new BScrollView("itemscroll",lview,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true));
	BTextControl *tcont = new BTextControl(BRect(10,110,rect.Width() - 10,130),"label","Label: ","",new BMessage(MSG_MENU_SET_ITEM_LABEL));
	tcont->SetDivider(be_plain_font->StringWidth(tcont->Label()));
	tcont->SetEnabled(false);
	AddChild(tcont);
	char msg[5];
	msg[0] = 0;
	strcpy(msg,"none");
	/*if (((BControl *)(pointer))->Message() != NULL)
		strncpy(msg,(char *)(new int32(((BControl *)(pointer))->Message()->what)),4);*/
	BTextControl *name = new BTextControl(BRect(10,130,rect.Width() - 10,150),"msg","Message: ",msg,new BMessage(MSG_MENU_SET_ITEM_MSG));
	name->TextView()->SetMaxBytes(4);
	name->SetDivider(be_plain_font->StringWidth("Message: "));
	name->SetEnabled(false);
	AddChild(name);
	tcont = new BTextControl(BRect(10,150,be_plain_font->StringWidth("Shortcut: ") + 40,170),"shrtct","Shortcut: ","",new BMessage(MSG_MENU_SET_ITEM_SHORTCUT));
	tcont->SetDivider(be_plain_font->StringWidth(tcont->Label()));
	tcont->SetEnabled(false);
	tcont->TextView()->SetMaxBytes(1);
	AddChild(tcont);
	BMenu *meniu = new BMenu("Modifier Keys");
	meniu->AddItem(new BMenuItem("Option",new BMessage(MSG_MENU_SET_ITEM_MARKED)));
	meniu->AddItem(new BMenuItem("Shift",new BMessage(MSG_MENU_SET_ITEM_MARKED)));
	meniu->AddItem(new BMenuItem("Control",new BMessage(MSG_MENU_SET_ITEM_MARKED)));
	AddChild(new BMenuField(BRect(be_plain_font->StringWidth("Shortcut: ") + 50,150,rect.Width() - 10,170),"modchoice",NULL,meniu));
	((BMenuField *)(FindView("modchoice")))->SetEnabled(false);
	meniu->SetRadioMode(false);
	meniu->SetLabelFromMarked(false);
	AddChild(new BCheckBox(BRect(10,175,rect.Width() - 10,195),"enabled","Enabled",new BMessage(MSG_MENU_SET_ITEM_ENABLED)));
	((BCheckBox *)(FindView("enabled")))->SetEnabled(false);
	AddChild(new BButton(BRect(10,200,(rect.Width()/2)-10,220),"addiantem","Add Item",new BMessage(MSG_MENU_NEW_ITEM)));
	AddChild(new BButton(BRect((rect.Width()/2)+10,200,rect.Width() - 10,220),"addsepitem","Add Separator",new BMessage(MSG_MENU_ADD_SEPARATOR)));
}
//------------------------------------------------------------------------------
void MenuEditor::MessageReceived(BMessage *msg) {
	BMenuItem *item;
	switch(msg->what) {
		case MSG_MENU_NEW_ITEM:
			item = new BMenuItem("New Item",new BMessage('none'));
			menu->AddItem(item);
			lview->Refresh(false);
			break;
		case MSG_MENU_SET_ITEM_LABEL:
			item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
			item->SetLabel(((BTextControl *)(FindView("label")))->Text());
			lview->Refresh(true);
			break;
		case MSG_MENU_SET_ITEM_MSG:
			item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
			item->SetMessage(new BMessage(flipcode(*((int32 *)(((BTextControl *)(FindView("msg")))->Text())))));
			break;
		case MSG_MENU_SET_ITEM_MARKED:
			msg->FindPointer("source",(void **)(&item));
			if (item->IsMarked())
				item->SetMarked(false);
			else
				item->SetMarked(true);
		case MSG_MENU_SET_ITEM_SHORTCUT:
			item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
			item->SetShortcut(*(((BTextControl *)(FindView("shrtct")))->Text()),make_mod_mask());
			break;
		case MSG_MENU_SET_ITEM_ENABLED:
			item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
			if (((BCheckBox *)(FindView("enabled")))->Value() == B_CONTROL_ON)
				item->SetEnabled(true);
			else
				item->SetEnabled(false);
			break;
		case MSG_MENU_ADD_SEPARATOR:
			item = new BSeparatorItem;
			menu->AddItem(item);
			lview->Refresh(false);
			break;
	}
}
//------------------------------------------------------------------------------
uint32 MenuEditor::make_mod_mask(void) {
	BMenu *men = ((BMenuField *)(FindView("modchoice")))->Menu();
	uint32 modifiers = 0;
	if (men->ItemAt(0)->IsMarked())
		modifiers = modifiers | B_OPTION_KEY;
	if (men->ItemAt(1)->IsMarked())
		modifiers = modifiers | B_SHIFT_KEY;
	if (men->ItemAt(2)->IsMarked())
		modifiers = modifiers | B_CONTROL_KEY;
	return modifiers;
}	
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

