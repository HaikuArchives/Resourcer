//------------------------------------------------------------------------------
//	WIND.cp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdio.h>
#include <string.h>

// System Includes -------------------------------------------------------------
#include <app/Application.h>
#include <app/Message.h>
#include <app/MessageFilter.h>

#include <interface/Alert.h>
#include <interface/Bitmap.h>
#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>
#include <interface/ColorControl.h>
#include <interface/ListItem.h>
#include <interface/MenuBar.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/OutlineListView.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/TabView.h>
#include <interface/TextControl.h>
#include <interface/Screen.h>
#include <interface/ScrollView.h>
#include <interface/Slider.h>
#include <interface/StringView.h>
#include <interface/View.h>
#include <interface/Window.h>

#include <kernel/image.h>

#include <storage/Resources.h>

#include <support/Beep.h>
#include <support/ClassInfo.h>

#include <translation/TranslationUtils.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "DragOutlineListView.h"
#include "EditingControl.h"
#include "EditingPanel.h"
#include "EditingWindow.h"
#include "Filters.h"
#include "WIND.h"
#include "WindowEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------
extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Window";
extern "C" _EXPORT BResources *file;
extern "C" _EXPORT image_id me;

#if __POWERPC__
#else
int32 flipcode (int32 original)
{
	int32 type = original;
	char *one = (char *)(&original);
	char *two = one + 1;
	char *three = two + 1;
	char *four = three + 1;
	*((char *)(&type)) = *four;
	*((char *)(&type) + 1) = *three;
	*((char *)(&type) + 2) = *two;
	*((char *)(&type) + 3) = *one;
	return type;
}
#endif
BResources *file;
image_id me;
BResources *resources;
BMessage *widemsg;

class EditingWindow;

BMessage *current;
EditingWindow *editingwin;
WindowEditor *wineditor;
BWindow *main;
window_feel danger_feel = B_NORMAL_WINDOW_FEEL;
int32 track_over(void *view);

const pattern STIPPLED = {{240,240,240,240,15,15,15,15}};


class InterfaceItem : public BStringItem
{
	public:
		InterfaceItem(char *label,BView *tocreate) : BStringItem(label)
		{
			what_a_drag = new BMessage;
			tocreate->ResizeToPreferred();
			tocreate->Archive(what_a_drag,true);
			drag_size = tocreate->Bounds();
			delete tocreate;	
		}

		BMessage *what_a_drag;
		BRect drag_size;
};

class InterfaceListView : public BOutlineListView
{
	public:
		InterfaceListView(void) : BOutlineListView(BRect(20,20,230,200),"elements") {}
		bool InitiateDrag(BPoint /*point*/, int32 index, bool /*wasSelected*/)
		{
			if (ItemAt(index)->OutlineLevel() != 1)
				return false;
			BRect temp = ((InterfaceItem *)(ItemAt(index)))->drag_size;
			BMessage *msg = ((InterfaceItem *)(ItemAt(index)))->what_a_drag;
			BBitmap *bits = new BBitmap(temp,BScreen(Window()).ColorSpace(),true);
			BView *view = (BView *)(instantiate_object(msg));
			bits->AddChild(view);
			view->Window()->Lock();
			view->Draw(view->Bounds());
			view->Sync();
			if (view->Window()->IsLocked())
				view->Window()->Unlock();
			DragMessage(msg,bits,BPoint(0,0));
			return true;
		}
};

int32 track_over(void *view) {
	DragOutlineListView *lview = (DragOutlineListView *)(view);
	lview->WatchMouse();
	return B_OK;
}

BCheckBox *menubox;
EditingPanel *editpanel;

uint32 make_bitmask(void);
bool startingup;

//------------------------------------------------------------------------------
void loaddata(unsigned char* data,size_t length, BView* bkgview)
{
	resources = file;
	startingup = false;
	main = bkgview->Window();

	//-------------Init Focus View & Window Options------
	bkgview->Window()->SetFlags(B_NOT_H_RESIZABLE | B_NOT_ZOOMABLE |
								B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK);
	bkgview->Window()->SetLook(B_FLOATING_WINDOW_LOOK);
	bkgview->Window()->ResizeTo(250,350);
	BTabView *tabs = new BTabView(BRect(0,0,350,350),"tabs",B_WIDTH_FROM_WIDEST);
	editpanel = new EditingPanel;
	tabs->AddTab(editpanel);
	tabs->TabAt(0)->SetLabel("Focus View");

	//-------------Init Editable Window------------------
	if (length == 0)
	{
		wineditor = new WindowEditor(BRect(0, 0, 250, 330), "wineditpanel",
									 main, editpanel);
	}
	else
	{
		wineditor = new WindowEditor(BRect(0, 0, 250, 330), "wineditpanel",
									 main, editpanel, data);
	}
	tabs->AddTab(wineditor);
	tabs->TabAt(1)->SetLabel("Window");

	//-------------Init Toolbar---------------------------
	BView *y = new BView(BRect(0,0,250,300),"viewtoolbar",B_FOLLOW_ALL_SIDES,0);
	y->SetViewColor(216,216,216);
	y->AddChild(new BStringView(BRect(20,210,20+be_plain_font->StringWidth("Drag a List Item to the Window to Add It"),230),"instructions","Drag a List Item to the Window to Add It",B_FOLLOW_TOP | B_FOLLOW_H_CENTER));
	BOutlineListView *list = new InterfaceListView;
	BStringItem *controls = new BStringItem("Controls");
	BStringItem *visuals = new BStringItem("Visual Organizers");
	BStringItem *editable = new BStringItem("Editable Items");
		list->AddItem(controls);
			list->AddUnder(new InterfaceItem("Button",new BButton(BRect(0,0,100,50),"newbutton","New Button",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Checkbox",new BCheckBox(BRect(0,0,100,50),"newcheckbox","New Checkbox",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Radio Button",new BRadioButton(BRect(0,0,100,50),"newrbutton","New Radio Button",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Slider",new BSlider(BRect(0,0,100,50),"newslider","New Slider",new BMessage('none'),0,100)),controls);
			BMenuField *tempx = new BMenuField(BRect(0,0,100,50),"newpopupmenu","New Pop-Up Menu: ",new BPopUpMenu("newpopupmenu"));
			tempx->SetDivider(be_plain_font->StringWidth(tempx->Label()));
			list->AddUnder(new InterfaceItem("Pop-Up Menu",tempx),controls);
			list->AddUnder(new InterfaceItem("Color Picker",new BColorControl(BPoint(0,0),B_CELLS_32x8,8,"newcolpicker",new BMessage('none'))),controls);
		list->AddItem(visuals);
			list->AddUnder(new InterfaceItem("Static Text",new BStringView(BRect(0,0,100,50),"newstatictext","Static Text")),visuals);
//			list->AddUnder(new InterfaceItem("Resource Text",new ResourceStringView()),visuals);
			list->AddUnder(new InterfaceItem("Box",new BBox(BRect(0,0,100,50),"newbox")),visuals);
//			list->AddUnder(new InterfaceItem("Bitmap",new ResImageView(resources)),visuals);
		list->AddItem(editable);
			list->AddUnder(new InterfaceItem("Text Box",new BTextView(BRect(0,0,100,50),"newtextbox",BRect(5,5,95,45),B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED)),editable);
			BTextControl *tempy = new BTextControl(BRect(0,0,100,50),"newtextfield","Text Field ",NULL,new BMessage('none'));
			tempy->SetDivider(be_plain_font->StringWidth(tempy->Label()));
			list->AddUnder(new InterfaceItem("Text Field",tempy),editable);
	y->AddChild(new BScrollView("scroller",list,B_FOLLOW_ALL_SIDES,0,false,true));
	tabs->AddTab(y);
	tabs->TabAt(2)->SetLabel("Elements");
	bkgview->AddChild(tabs);
}
	
	
unsigned char* savedata(size_t* length, BView* bkgview)
{
	BMessage *archive = new BMessage;
	BTabView* Tabs = dynamic_cast<BTabView*>(bkgview->FindView("tabs"));
	if (!Tabs)
	{
		// Without the tabs, we're basically up a creek
		debugger("No tabview found");
	}
	// Because of the massive lameness of BTabView, we have to do
	// a bait and switch if the "Window" is not the currently
	// selected tab.  There is the remote possibility that the user
	// will select another tab while we're saving, but we'll hope
	// that's negligible
	WindowEditor* WinEditor = dynamic_cast<WindowEditor*>(Tabs->TabAt(1)->View());
	if (!WinEditor)
	{
		// Here again, without the window editor, we're in a bad way
		debugger("No window editor found");
	}
	bool detach = false;
	if (!WinEditor->Parent())
	{
		detach = true;
		WinEditor->Hide();
		main->AddChild(WinEditor);
	}
	if (WinEditor->ArchiveWindow(archive) != B_OK)
	{
		(new BAlert("yikes", "Archive is failing"B_UTF8_ELLIPSIS, "OK"))->Go();
	}
	if (detach)
	{
		WinEditor->RemoveSelf();
	}
	BMallocIO buffer;
	ssize_t xyz;
	archive->Flatten(&buffer,&xyz);
	*length = xyz;
	unsigned char *data = new unsigned char[*length];
	memcpy(data,buffer.Buffer(),*length);
	delete archive;
//	editingwin->Quit();
	//(new BAlert("helo","savedata executing properly.","OK"))->Go();
	return data;
}

/*
 * $Log $
 *
 * $Id  $
 *
 */

