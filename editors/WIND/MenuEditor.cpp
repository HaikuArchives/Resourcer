//------------------------------------------------------------------------------
//	MenuEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <string.h>

// System Includes -------------------------------------------------------------
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/Menu.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/ScrollView.h>
#include <interface/TextControl.h>

#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "DragOutlineListView.h"
#include "MenuEditor.h"
#include "MenuListItem.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
MenuEditor::MenuEditor(BRect rect, BMenu* men)
	:	BBox(rect, "menueditor"),
		fListView(NULL),
		fMenu(men),
		fLabelText(NULL),
		fMsgText(NULL),
		fShortcutText(NULL),
		fModifierField(NULL),
		fEnabledBox(NULL),
		fAddItemBtn(NULL),
		fAddSeparatorBtn(NULL),
		fInit(false)
{
	SetViewColor(216,216,216);

	BRect frame(10, 10, rect.Width() - 10 - B_V_SCROLL_BAR_WIDTH, 100);
	fListView = new DragOutlineListView(frame, Menu());
	fListView->SetSelectionMessage(new BMessage(MSG_MENU_SELECTION_CHANGED));
	AddChild(new BScrollView("itemscroll", fListView,
							 B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
}
//------------------------------------------------------------------------------
void MenuEditor::AttachedToWindow()
{
	if (!fInit)
	{
		BRect rect(Frame());
		BRect frame(10, 110, rect.Width() - 10, 130);
		fLabelText = new BTextControl(frame, "label", "Label: ", "",
									  new BMessage(MSG_MENU_SET_ITEM_LABEL));
		fLabelText->SetDivider(be_plain_font->StringWidth("Label: "));
		fLabelText->SetEnabled(false);
		AddChild(fLabelText);

		char msg[5];
		msg[0] = 0;
		strcpy(msg,"none");
		/*if (((BControl *)(pointer))->Message() != NULL)
			strncpy(msg,(char *)(new int32(((BControl *)(pointer))->Message()->what)),4);*/

		frame.Set(10, 130, rect.Width() - 10, 150);
		fMsgText = new BTextControl(frame, "msg", "Message: ", msg,
									new BMessage(MSG_MENU_SET_ITEM_MSG));
		fMsgText->TextView()->SetMaxBytes(4);
		fMsgText->SetDivider(be_plain_font->StringWidth("Message: "));
		fMsgText->SetEnabled(false);
		AddChild(fMsgText);

		frame.Set(10, 150, be_plain_font->StringWidth("Shortcut: ") + 40, 170);
		fShortcutText = new BTextControl(frame, "shrtct", "Shortcut: ", "",
										 new BMessage(MSG_MENU_SET_ITEM_SHORTCUT));
		fShortcutText->SetDivider(be_plain_font->StringWidth("Shortcut: "));
		fShortcutText->SetEnabled(false);
		fShortcutText->TextView()->SetMaxBytes(1);
		AddChild(fShortcutText);

		BMenu *meniu = new BMenu("Modifier Keys");
		meniu->AddItem(new BMenuItem("Option", new BMessage(MSG_MENU_SET_ITEM_MARKED)));
		meniu->AddItem(new BMenuItem("Shift", new BMessage(MSG_MENU_SET_ITEM_MARKED)));
		meniu->AddItem(new BMenuItem("Control", new BMessage(MSG_MENU_SET_ITEM_MARKED)));
		frame.Set(be_plain_font->StringWidth("Shortcut: ") + 50, 150,
					rect.Width() - 10, 170);
		fModifierField = new BMenuField(frame, "modchoice", NULL, meniu);
		fModifierField->SetEnabled(false);
		meniu->SetRadioMode(false);
		meniu->SetLabelFromMarked(false);
		AddChild(fModifierField);

		frame.Set(10, 175, rect.Width() - 10, 195);
		fEnabledBox = new BCheckBox(frame, "enabled", "Enabled",
									new BMessage(MSG_MENU_SET_ITEM_ENABLED));
		fEnabledBox->SetEnabled(false);
		AddChild(fEnabledBox);

		frame.Set(10, 200, (rect.Width() / 2) - 10, 220);
		fAddItemBtn = new BButton(frame, "addiantem", "Add Item",
								  new BMessage(MSG_MENU_NEW_ITEM));
		AddChild(fAddItemBtn);
		frame.Set((rect.Width() / 2) + 10, 200, rect.Width() - 10, 220);
		fAddSeparatorBtn = new BButton(frame, "addsepitem", "Add Separator",
									   new BMessage(MSG_MENU_ADD_SEPARATOR));
		AddChild(fAddSeparatorBtn);
	}

	fListView->SetTarget(this);
	fLabelText->SetTarget(this);
	fMsgText->SetTarget(this);
	fShortcutText->SetTarget(this);
	fModifierField->Menu()->SetTargetForItems(this);
	fEnabledBox->SetTarget(this);
	fAddItemBtn->SetTarget(this);
	fAddSeparatorBtn->SetTarget(this);
}
//------------------------------------------------------------------------------
void MenuEditor::MessageReceived(BMessage* msg)
{
	BMenuItem* item;
	switch (msg->what)
	{
		case MSG_MENU_NEW_ITEM:
		{
			if (fMenu)
			{
				fMenu->AddItem(new BMenuItem("New Item",
											 new BMessage('none')));
				fListView->Refresh(false);
			}
			break;
		}

		case MSG_MENU_SET_ITEM_LABEL:
			CurrentItem()->SetLabel(fLabelText->Text());
			fListView->Refresh(true);
			break;

		case MSG_MENU_SET_ITEM_MSG:
		{
			int32 what = flipcode(*((int32 *)(fMsgText->Text())));
			CurrentItem()->SetMessage(new BMessage(what));
			break;
		}

		case MSG_MENU_SET_ITEM_MARKED:
			msg->FindPointer("source", (void **)(&item));
			item->SetMarked(!item->IsMarked());

		case MSG_MENU_SET_ITEM_SHORTCUT:
		{
			if (fShortcutText->TextView()->TextLength() == 0)
			{
				fModifierField->Menu()->ItemAt(0)->SetMarked(false);
				fModifierField->Menu()->ItemAt(1)->SetMarked(false);
				fModifierField->Menu()->ItemAt(2)->SetMarked(false);
				fModifierField->SetEnabled(false);
			}
			else if (!fModifierField->IsEnabled())
			{
				fModifierField->SetEnabled(true);
			}
			CurrentItem()->SetShortcut(*(fShortcutText->Text()), MakeModMask());

			break;
		}

		case MSG_MENU_SET_ITEM_ENABLED:
		{
			CurrentItem()->SetEnabled(fEnabledBox->Value() == B_CONTROL_ON);
			break;
		}

		case MSG_MENU_ADD_SEPARATOR:
		{
			if (fMenu)
			{
				item = new BSeparatorItem;
				fMenu->AddItem(item);
				fListView->Refresh(false);
			}
			break;
		}

		case MSG_MENU_SELECTION_CHANGED:
		{
			OnSelectionChanged(msg);
			break;
		}

		default:
			BBox::MessageReceived(msg);
			break;
	}
}
//------------------------------------------------------------------------------
void MenuEditor::SetMenu(BMenu* menu)
{
	fMenu = menu;
	fListView->SetMenu(menu);
}
//------------------------------------------------------------------------------
BMenu* MenuEditor::Menu()
{
	return fMenu;
}
//------------------------------------------------------------------------------
uint32 MenuEditor::MakeModMask(void)
{
	uint32 modifiers = 0;
	if (fModifierField->Menu()->ItemAt(0)->IsMarked())
	{
		modifiers |= B_OPTION_KEY;
	}
	if (fModifierField->Menu()->ItemAt(1)->IsMarked())
	{
		modifiers |= B_SHIFT_KEY;
	}
	if (fModifierField->Menu()->ItemAt(2)->IsMarked())
	{
		modifiers |= B_CONTROL_KEY;
	}
	return modifiers;
}	
//------------------------------------------------------------------------------
BMenuItem* MenuEditor::CurrentItem()
{
	int32 index = fListView->FullListCurrentSelection();
	return ((MenuListItem *)(fListView->FullListItemAt(index)))->item;
}
//------------------------------------------------------------------------------
void MenuEditor::OnSelectionChanged(BMessage* /*msg*/)
{
	if ((fListView->CurrentSelection() < 0) ||
		is_kind_of(CurrentItem(), BSeparatorItem))
	{
		fLabelText->SetText("");
		fLabelText->SetEnabled(false);
		fMsgText->SetText("none");
		fMsgText->SetEnabled(false);
		fShortcutText->SetText("");
		fShortcutText->SetEnabled(false);
		fModifierField->Menu()->ItemAt(0)->SetMarked(false);
		fModifierField->Menu()->ItemAt(1)->SetMarked(false);
		fModifierField->Menu()->ItemAt(2)->SetMarked(false);
		fModifierField->SetEnabled(false);
		fEnabledBox->SetEnabled(false);
		fEnabledBox->SetValue(B_CONTROL_OFF);
		return;
	}
	else
	{
		fLabelText->SetEnabled(true);
		fMsgText->SetEnabled(true);

		fModifierField->Menu()->ItemAt(0)->SetMarked(false);
		fModifierField->Menu()->ItemAt(1)->SetMarked(false);
		fModifierField->Menu()->ItemAt(2)->SetMarked(false);

		fEnabledBox->SetEnabled(true);
		fEnabledBox->SetValue(CurrentItem()->IsEnabled());

		fShortcutText->SetEnabled(CurrentItem()->Submenu() == NULL);
	}
	fLabelText->SetText(CurrentItem()->Label());
	char temp[5];
	BMenuItem *item = CurrentItem();
	if (item->Message() != NULL)
		strncpy(temp, (char *)(new int32(flipcode(item->Message()->what))), 4);
	else
		strcpy(temp, "none");
	temp[4] = 0;
	fMsgText->SetText(temp);

	uint32 mod;
	temp[1] = 0;
	temp[0] = item->Shortcut(&mod);
	fShortcutText->SetText(temp);

	fModifierField->SetEnabled(*temp);
	if (*temp)
	{
		fModifierField->Menu()->ItemAt(0)->SetMarked(mod & B_OPTION_KEY);
		fModifierField->Menu()->ItemAt(1)->SetMarked(mod & B_SHIFT_KEY);
		fModifierField->Menu()->ItemAt(2)->SetMarked(mod & B_CONTROL_KEY);
	}
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

