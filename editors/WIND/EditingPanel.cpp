//------------------------------------------------------------------------------
//	EditingPanel.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdio.h>
#include <string.h>

// System Includes -------------------------------------------------------------
#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/ColorControl.h>
#include <interface/MenuBar.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/Slider.h>
#include <interface/StringView.h>
#include <interface/TextControl.h>
#include <interface/Window.h>

#include <support/Autolock.h>
#include <support/Beep.h>
#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "EditingPanel.h"
#include "MenuEditor.h"
//#include "ResImageView.h"
//#include "ResourceStringView.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
EditingPanel::EditingPanel(void) : BView(BRect(0,0,300,350),"editpanel",B_FOLLOW_ALL_SIDES,0) {
	pointer = NULL;
	SetViewColor(216,216,216);
	iscontrol = false;
	AddChild(new BStringView(BRect(25,100,175,125),"noactive","No Focus View"));
}
//------------------------------------------------------------------------------
void EditingPanel::AttachedToWindow()
{
	BControl* ctrl;
	for (uint32 i = 0; i < CountChildren(); ++i)
	{
		ctrl = dynamic_cast<BControl*>(ChildAt(i));
		if (ctrl)
		{
			ctrl->SetTarget(this);
		}
	}
}
//------------------------------------------------------------------------------
void EditingPanel::MessageReceived(BMessage *msg)
{
	BAutolock locker(pointer->Window());
	// TODO: bring back when necessary
//	int32 idnum;
	switch(msg->what)
	{
		case MSG_VIEW_SET_NAME:
		{
			BMessage notify(MSG_VIEW_SET_NAME);
			notify.AddString("name", ((BTextControl *)(FindView("name")))->Text());
			notify.AddPointer("view", (const void*)pointer);
			
			pointer->Window()->PostMessage(&notify);
			break;
		}
		case MSG_CONTROL_SET_LABEL:
			((BControl *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			if (is_kind_of(pointer,BTextControl))
				((BTextControl *)(pointer))->SetDivider(be_plain_font->StringWidth(((BTextControl *)(pointer))->Label())); 
			break;
		case MSG_CONTROL_SET_MSG:
			((BControl *)(pointer))->SetMessage(new BMessage(flipcode(*(int32 *)(((BTextControl *)(FindView("msg")))->Text()))));
			break;
		case MSG_SLIDER_SET_BLOCK_THUMB:
			((BSlider *)(pointer))->SetStyle(B_BLOCK_THUMB);
			((BSlider *)(pointer))->Invalidate(pointer->Bounds());
			break;
		case MSG_SLIDER_SET_TRI_THUMB:
			((BSlider *)(pointer))->SetStyle(B_TRIANGLE_THUMB);
			((BSlider *)(pointer))->Invalidate(pointer->Bounds());
			break;
		case MSG_SLIDER_SET_LIMIT_LABELS:
			((BSlider *)(pointer))->SetLimitLabels(((BTextControl *)(FindView("minlabel")))->Text(),((BTextControl *)(FindView("maxlabel")))->Text());
			break;
		case MSG_STRING_VIEW_SET_TEXT:
			((BStringView *)(pointer))->SetText(((BTextControl *)(FindView("text")))->Text());
			break;
		case MSG_BOX_SET_LABEL:
			((BBox *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			break;
		case MSG_BOX_FANCY_BORDER:
			((BBox *)(pointer))->SetBorder(B_FANCY_BORDER);
			break;
		case MSG_BOX_PLAIN_BORDER:
			((BBox *)(pointer))->SetBorder(B_PLAIN_BORDER);
			break;
		case MSG_BOX_NO_BORDER:
			((BBox *)(pointer))->SetBorder(B_NO_BORDER);
			break;
		case MSG_RES_IMAGE_VIEW_SET_ID:
			// TODO: Figure how this works then refactor it
#if 0
			sscanf(((BTextControl *)(FindView("idnum")))->Text(),"%ld",&idnum);
			if (resources->HasResource('bits',idnum))
			{
				((ResImageView *)(pointer))->SetTo(idnum,true); 
				//editingwin->cntrl->ResizeTo(pointer->Bounds().right+10,pointer->Bounds().bottom+10);
				editingwin->cntrl->controlled = NULL;
				editingwin->cntrl->notInit = true;
				pointer->MakeFocus();
				editingwin->cntrl->Pulse();
				//editingwin->cntrl->Draw(editingwin->cntrl->Bounds());
			}
			else
			{
				beep();
			}
#endif
			break;
		case MSG_MENU_FIELD_SET_LABEL:
			((BMenuField *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			((BMenuField *)(pointer))->SetDivider(be_plain_font->StringWidth(((BMenuField *)(pointer))->Label()));
			break; 
		case MSG_MENU_SET_DEFAULT_TEXT:
			((BMenuField *)(pointer))->Menu()->SetName(((BTextControl *)(FindView("deftext")))->Text());
			((BMenuField *)(pointer))->Menu()->Superitem()->SetLabel(((BTextControl *)(FindView("deftext")))->Text());
			break;
		case MSG_RES_STRING_VIEW_SET_ID:
			// TODO: Figure how this works then refactor it
#if 0
			sscanf(((BTextControl *)(FindView("idnum")))->Text(),"%ld",&idnum);
			if (resources->HasResource('CSTR',idnum)) {
				((ResourceStringView *)(pointer))->SetTo(idnum); 
				//editingwin->cntrl->ResizeTo(pointer->Bounds().right+10,pointer->Bounds().bottom+10);
				editingwin->cntrl->controlled = NULL;
				editingwin->cntrl->notInit = true;
				pointer->MakeFocus();
				editingwin->cntrl->Pulse();
				//editingwin->cntrl->Draw(editingwin->cntrl->Bounds());
			} else
				beep();
#endif
			break;

		default:
			BView::MessageReceived(msg);
			break;
	}
}
//------------------------------------------------------------------------------
void EditingPanel::SetTo(BView* active)
{
	BAutolock locker(Window());
	SetPart2(active);
}
//------------------------------------------------------------------------------
void EditingPanel::SetPart2(BView* active)
{
	for (BView *dead = ChildAt(0); dead != NULL; dead = ChildAt(0)) {
		RemoveChild(dead);
	}
	pointer = active;
	if (active == NULL) {
		AddChild(new BStringView(BRect(25,100,175,125),"noactive","No Focus View"));
		return;
	}
	//-----------General BView------------
	char classname[512];
	strcpy(classname,class_name(pointer));
	char *temp = new char[strlen("Element Type: ")+strlen(classname)+1];
	sprintf(temp,"Element Type: %s",classname);
	AddChild(new BStringView(BRect(10,10,190,30),"classname",temp));
	delete temp;
	BTextControl *name = new BTextControl(BRect(10,30,190,50),"name","View Name: ",((BButton *)(pointer))->Name(),new BMessage(MSG_VIEW_SET_NAME));
	name->SetDivider(be_plain_font->StringWidth("View Name: "));
	AddChild(name);
	//-----------Base Classes----------------
	//-----------BControl------------------
	if (is_kind_of(pointer,BControl)) {
		name = new BTextControl(BRect(10,50,190,70),"label","Label: ",((BControl *)(pointer))->Label(),new BMessage(MSG_CONTROL_SET_LABEL));
		name->SetDivider(be_plain_font->StringWidth("Label: "));
		AddChild(name);
		char msg[5];
		msg[0] = 0;
		if (((BControl *)(pointer))->Message() != NULL)
			strncpy(msg,(char *)(new int32(flipcode(((BControl *)(pointer))->Message()->what))),4);
		name = new BTextControl(BRect(10,70,190,90),"msg","Message: ",msg,new BMessage(MSG_CONTROL_SET_MSG));
		name->TextView()->SetMaxBytes(4);
		name->SetDivider(be_plain_font->StringWidth("Message: "));
		AddChild(name);
		iscontrol = true;
	}
	//----------Individuals--------------
	//----------BSlider------------------
	if (is_kind_of(pointer,BSlider)) {
		BSlider *slider = (BSlider *)(pointer);
		name = new BTextControl(BRect(10,90,190,110),"minlabel","Left Label: ",slider->MinLimitLabel(),new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		name->SetDivider(be_plain_font->StringWidth("Left Label: "));
		AddChild(name);
		name = new BTextControl(BRect(10,110,190,130),"maxlabel","Right Label: ",slider->MaxLimitLabel(),new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		name->SetDivider(be_plain_font->StringWidth("Right Label: "));
		AddChild(name);
		BRadioButton *but = new BRadioButton(BRect(10,130,190,150),"rect","Rectangular Thumb",new BMessage(MSG_SLIDER_SET_BLOCK_THUMB));
		BRadioButton *but2 = new BRadioButton(BRect(10,150,190,170),"rect","Triangular Thumb",new BMessage(MSG_SLIDER_SET_TRI_THUMB));
		if (slider->Style() == B_BLOCK_THUMB)
			but->SetValue(B_CONTROL_ON);
		else
			but2->SetValue(B_CONTROL_ON);
		AddChild(but);
		AddChild(but2);
	}
	//----------BStringView-------------
	if ((is_kind_of(pointer,BStringView)))// && (!(is_kind_of(pointer,ResourceStringView))))
	{
		BStringView *strview = (BStringView *)(pointer);
		name = new BTextControl(BRect(10,50,190,70),"text","Text: ",strview->Text(),new BMessage(MSG_STRING_VIEW_SET_TEXT));
		name->SetDivider(be_plain_font->StringWidth("Text: "));
		AddChild(name);
	}
	//---------ResourceStringView-------
	// TODO: redo when ResourceStringView is ready
#if 0
	if (is_kind_of(pointer,ResourceStringView)) {
		ResourceStringView *img = (ResourceStringView *)pointer;
		int32 id = img->ResourceID();
		char idtext[15];
		sprintf(idtext,"%ld",id);
		name = new BTextControl(BRect(10,50,190,70),"idnum","Resource ID: ",idtext,new BMessage(MSG_RES_STRING_VIEW_SET_ID));
		name->SetDivider(be_plain_font->StringWidth("Resource ID: "));
		AddChild(name);
	}
#endif
	//---------BBox---------------------
	if (is_kind_of(pointer,BBox)) {
		BBox *box = (BBox *)(pointer);
		name = new BTextControl(BRect(10,50,190,70),"label","Label: ",box->Label(),new BMessage(MSG_BOX_SET_LABEL));
		name->SetDivider(be_plain_font->StringWidth("Label: "));
		BRadioButton *but1 = new BRadioButton(BRect(10,70,190,90),"fancy","Fancy Border",new BMessage(MSG_BOX_FANCY_BORDER));
		BRadioButton *but2 = new BRadioButton(BRect(10,90,190,110),"plain","Plain Border",new BMessage(MSG_BOX_PLAIN_BORDER));
		BRadioButton *but3 = new BRadioButton(BRect(10,110,190,130),"none","No Border",new BMessage(MSG_BOX_NO_BORDER));
		AddChild(name);
		AddChild(but1);
		AddChild(but2);
		AddChild(but3);
		Window()->Lock();
		switch (box->Border()) {
			case B_NO_BORDER:
				but3->SetValue(B_CONTROL_ON);
				break;
			case B_PLAIN_BORDER:
				but2->SetValue(B_CONTROL_ON);
				break;
			case B_FANCY_BORDER:
				but1->SetValue(B_CONTROL_ON);
		}
		if (Window()->IsLocked())
			Window()->Unlock();
	}
	//---------ResImageView---------------
	// TODO: redo when ResImageView is ready
#if 0
	if (is_kind_of(pointer,ResImageView)) {
		ResImageView *img = (ResImageView *)(pointer);
		int32 id = img->ResourceID();
		char idtext[15];
		sprintf(idtext,"%ld",id);
		name = new BTextControl(BRect(10,50,190,70),"idnum","Resource ID: ",idtext,new BMessage(MSG_RES_IMAGE_VIEW_SET_ID));
		name->SetDivider(be_plain_font->StringWidth("Resource ID: "));
		AddChild(name);
	}
#endif
	//----------BMenuField----------------
	if (is_kind_of(pointer,BMenuField)) {
		BMenuField *mfield = (BMenuField *)(pointer);
		name = new BTextControl(BRect(10,50,190,70),"label","Label: ",mfield->Label(),new BMessage(MSG_MENU_FIELD_SET_LABEL));
		name->SetDivider(be_plain_font->StringWidth("Label: "));
		AddChild(name);
		name = new BTextControl(BRect(10,70,190,90),"deftext","Default Menu Text: ",mfield->Menu()->Name(),new BMessage(MSG_MENU_SET_DEFAULT_TEXT));
		name->SetDivider(be_plain_font->StringWidth(name->Label()));
		AddChild(name);
		MenuEditor *items = new MenuEditor(mfield->Menu(),BRect(20,90,240,320));
		AddChild(items);
	}
	//----------BMenuBar------------------
	if (is_kind_of(pointer,BMenuBar)) {
		BMenuBar *mbar = (BMenuBar *)(pointer);
		MenuEditor *items = new MenuEditor(mbar,BRect(20,50,240,280));
		AddChild(items);
	}
	//-----------BColorControl------------
	if (is_kind_of(pointer,BColorControl)) {
		FindView("label")->RemoveSelf();
		FindView("msg")->MoveTo(10,50);
		BColorControl *colpicker = (BColorControl *)(pointer);
		BPopUpMenu *alignment = new BPopUpMenu("Item Alignment");
		// TODO: add message handling for these
		alignment->AddItem(new BMenuItem("4 by 64",new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		alignment->AddItem(new BMenuItem("8 by 32",new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		alignment->AddItem(new BMenuItem("16 by 16",new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		alignment->AddItem(new BMenuItem("64 by 4",new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		alignment->AddItem(new BMenuItem("32 by 8",new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		BMenuField *items = new BMenuField(BRect(10,80,190,100),"alignpicker","Cell Alignment: ",alignment);
		items->SetDivider(be_plain_font->StringWidth(items->Label()));
		switch (colpicker->Layout()) {
			case B_CELLS_4x64:
				alignment->ItemAt(0)->SetMarked(true);
				break;
			case B_CELLS_8x32:
				alignment->ItemAt(1)->SetMarked(true);
				break;
			case B_CELLS_16x16:
				alignment->ItemAt(2)->SetMarked(true);
				break;	
			case B_CELLS_64x4:
				alignment->ItemAt(3)->SetMarked(true);
				break;	
			case B_CELLS_32x8:
				alignment->ItemAt(4)->SetMarked(true);
				break;
		}
		AddChild(items);
	}
}
//------------------------------------------------------------------------------
void EditingPanel::SetTo(BMessage* active)
{
	BView* view = NULL;
	if (active->FindPointer("view", (void**)&view) == B_OK)
	{
		fControl = view;

		// TODO: remove
		SetTo(view);

		BMessage ctrl;
		if (active->FindMessage("ctrl", &ctrl) == B_OK)
		{
			SetPart2(&ctrl);
		}
		else
		{
			// Houston, we have a problem ...
			debugger("Archive of currently selected control not found");
		}
	}
	else
	{
		fControl = BMessenger();

		// TODO: remove
		SetTo((BView*)NULL);
	}
}
//------------------------------------------------------------------------------
void EditingPanel::SetPart2(BMessage* active)
{
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

