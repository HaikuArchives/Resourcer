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

void setwindoweditor(BView *x);
uint32 make_bitmask(void);
bool startingup;

#if 0
void setwindoweditor(BView *x) {
	startingup = true;
	x->SetViewColor(216,216,216);
	BTextControl *name = new BTextControl(BRect(10,10,190,30),"title","Title: ",editingwin->Title(),new BMessage(MSG_WINDOW_SET_TITLE));
	name->SetDivider(be_plain_font->StringWidth("Title: "));
	x->AddChild(name);
	//----------Window Look-------------
		BPopUpMenu *menu = new BPopUpMenu("Document Window");
		BMessage *msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_DOCUMENT_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Document Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_TITLED_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Titled Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_FLOATING_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Floating Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_MODAL_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Modal Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_BORDERED_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Bordered Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_LOOK);
		msg->AddInt32("newlook",B_NO_BORDER_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Borderless Window",msg));
		switch (editingwin->Look()) {
			case B_DOCUMENT_WINDOW_LOOK:
				menu->ItemAt(0)->SetMarked(true);
				break;
			case B_TITLED_WINDOW_LOOK:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_FLOATING_WINDOW_LOOK:
				menu->ItemAt(2)->SetMarked(true);
				break;
			case B_MODAL_WINDOW_LOOK:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_BORDERED_WINDOW_LOOK:
				menu->ItemAt(4)->SetMarked(true);
				break;
			case B_NO_BORDER_WINDOW_LOOK:
				menu->ItemAt(5)->SetMarked(true);
				break;
		}
		BMenuField *field = new BMenuField(BRect(10,40,240,60),"look","Window Look: ",menu);
		field->SetDivider(be_plain_font->StringWidth("Window Look: "));
		x->AddChild(field);
	//----------Window Feel-------------
		menu = new BPopUpMenu("Normal");
		msg = new BMessage(MSG_WINDOW_SET_FEEL);
		msg->AddInt32("newfeel",B_NORMAL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Normal",msg));
		msg = new BMessage(MSG_WINDOW_SET_FEEL);
		msg->AddInt32("newfeel",B_MODAL_APP_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Modal Across Application",msg));
		msg = new BMessage(MSG_WINDOW_SET_FEEL);
		msg->AddInt32("newfeel",B_MODAL_ALL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Modal Across System",msg));
		msg = new BMessage(MSG_WINDOW_SET_FEEL);
		msg->AddInt32("newfeel",B_FLOATING_APP_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Floating Across Application",msg));
		msg = new BMessage(MSG_WINDOW_SET_FEEL);
		msg->AddInt32("newfeel",B_FLOATING_ALL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Floating Across System",msg));
		switch (editingwin->Feel()) {
			case B_NORMAL_WINDOW_FEEL:
				menu->ItemAt(0)->SetMarked(true);
				break;
			case B_MODAL_APP_WINDOW_FEEL:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_MODAL_SUBSET_WINDOW_FEEL:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_MODAL_ALL_WINDOW_FEEL:
				menu->ItemAt(2)->SetMarked(true);
				break;
			case B_FLOATING_APP_WINDOW_FEEL:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_FLOATING_SUBSET_WINDOW_FEEL:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_FLOATING_ALL_WINDOW_FEEL:
				menu->ItemAt(4)->SetMarked(true);
				break;
		}
		field = new BMenuField(BRect(10,70,240,90),"look","Window Behavior: ",menu);
		field->SetDivider(be_plain_font->StringWidth("Window Behavior: "));
		x->AddChild(field);
	//----------Window Flags------------
		BRect work = x->Bounds();
		work.left = 10;
		work.top = 100;
		work.bottom -= 5;
		work.right -= 10;
		work.right -= 10;
		int i = 20;
		int inc = 10;
		BBox *box = new BBox(work,"flags");
		box->SetLabel("Window Flags");
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		msg->AddInt32("flags",B_NOT_MOVABLE);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"nmov","Movable",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		msg->AddInt32("flags",B_NOT_CLOSABLE);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"clos","Closable",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_ZOOMABLE);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"zoom","Zoomable",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		msg->AddInt32("flags",B_NOT_MINIMIZABLE);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"mini","Minimizable",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_H_RESIZABLE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"hres","Horizontally Resizable",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_V_RESIZABLE);
		box->AddChild(new BCheckBox(BRect(10,i,125,i+inc),"vres","Vertically Resizable",msg));
		menubox = new BCheckBox(BRect(130,i,210,i+inc),"menus","Menu Bar",new BMessage(MSG_WINDOW_ADD_MENU));
		box->AddChild(menubox);
		if (editingwin->KeyMenuBar() != NULL)
			menubox->SetValue(B_CONTROL_ON);
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_OUTLINE_RESIZE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"roiw","Resize with Outline Instead of Window",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_WILL_ACCEPT_FIRST_CLICK);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"wafc","Will Accept First Click",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_AVOID_FRONT);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"avfr","Avoid Front",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		msg->AddInt32("flags",B_AVOID_FOCUS);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"avfo","Avoid Focus",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_NO_WORKSPACE_ACTIVATION);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"nwoa","Do Not Activate Workspace",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_ANCHORED_ON_ACTIVATE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"brcu","Bring Window To Current Workspace",msg));
		msg = new BMessage(MSG_WINDOW_SET_FLAG);
		i+= inc*2;
		msg->AddInt32("flags",B_ASYNCHRONOUS_CONTROLS);
		box->AddChild(new BCheckBox(BRect(10,i,210,+inc),"async","Asynchronous Controls (Should Be On)",msg));
		uint32 flags = editingwin->Flags();
		if (!(flags & B_NOT_MOVABLE))
			((BCheckBox *)(box->ChildAt(0)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_CLOSABLE))
			((BCheckBox *)(box->ChildAt(1)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_ZOOMABLE))
			((BCheckBox *)(box->ChildAt(2)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_MINIMIZABLE))
			((BCheckBox *)(box->ChildAt(3)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_H_RESIZABLE))
			((BCheckBox *)(box->ChildAt(4)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_V_RESIZABLE))
			((BCheckBox *)(box->ChildAt(5)))->SetValue(B_CONTROL_ON);
		if (flags & B_OUTLINE_RESIZE)
			((BCheckBox *)(box->ChildAt(7)))->SetValue(B_CONTROL_ON);
		if (flags & B_WILL_ACCEPT_FIRST_CLICK)
			((BCheckBox *)(box->ChildAt(8)))->SetValue(B_CONTROL_ON);
		if (flags & B_AVOID_FRONT)
			((BCheckBox *)(box->ChildAt(9)))->SetValue(B_CONTROL_ON);
		if (flags & B_AVOID_FOCUS)
			((BCheckBox *)(box->ChildAt(10)))->SetValue(B_CONTROL_ON);
		if (flags & B_NO_WORKSPACE_ACTIVATION)
			((BCheckBox *)(box->ChildAt(11)))->SetValue(B_CONTROL_ON);
		if (flags & B_NOT_ANCHORED_ON_ACTIVATE)
			((BCheckBox *)(box->ChildAt(12)))->SetValue(B_CONTROL_ON);
		if (flags & B_ASYNCHRONOUS_CONTROLS)
			((BCheckBox *)(box->ChildAt(13)))->SetValue(B_CONTROL_ON);
		x->AddChild(box);
		startingup = false;
}
#endif

#if 0
uint32 make_bitmask(void) {
	BView *box = ((BTabView *)(main->FindView("tabs")))->TabAt(1)->View()->FindView("flags");
	uint32 bitmask = 0;
	if (((BCheckBox *)(box->ChildAt(0)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_MOVABLE;
	if (((BCheckBox *)(box->ChildAt(1)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_CLOSABLE;
	if (((BCheckBox *)(box->ChildAt(2)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_ZOOMABLE;
	if (((BCheckBox *)(box->ChildAt(3)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_MINIMIZABLE;
	if (((BCheckBox *)(box->ChildAt(4)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_H_RESIZABLE;
	if (((BCheckBox *)(box->ChildAt(5)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_V_RESIZABLE;
	if (((BCheckBox *)(box->ChildAt(7)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_OUTLINE_RESIZE;
	if (((BCheckBox *)(box->ChildAt(8)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_WILL_ACCEPT_FIRST_CLICK;
	if (((BCheckBox *)(box->ChildAt(9)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_AVOID_FRONT;
	if (((BCheckBox *)(box->ChildAt(10)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_AVOID_FOCUS;
	if (((BCheckBox *)(box->ChildAt(11)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_NO_WORKSPACE_ACTIVATION;
	if (((BCheckBox *)(box->ChildAt(12)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_NOT_ANCHORED_ON_ACTIVATE;
	if (((BCheckBox *)(box->ChildAt(13)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_ASYNCHRONOUS_CONTROLS;
	return bitmask;
}
#endif
		

void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	resources = file;
	startingup = false;
	main = bkgview->Window();
	//-------------Init Editable Window------------------
#if 0
	if (length == 0) {
		editingwin = new EditingWindow;
	} else {
		current = new BMessage;
		current->Unflatten((char *)data);
		editingwin = new EditingWindow(current);
	}
	danger_feel = editingwin->Feel();
	editingwin->Show();
#endif
	//-------------Init Focus View & Window Options------
	bkgview->Window()->SetFlags(B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK);
	bkgview->Window()->SetLook(B_FLOATING_WINDOW_LOOK);
	bkgview->Window()->ResizeTo(250,350);
	BTabView *tabs = new BTabView(BRect(0,0,350,350),"tabs",B_WIDTH_FROM_WIDEST);
	editpanel = new EditingPanel;
	tabs->AddTab(editpanel);
	tabs->TabAt(0)->SetLabel("Focus View");
#if 0
	BView *x = new BView(BRect(0,0,250,330),"wineditpanel",B_FOLLOW_ALL_SIDES,0);
	setwindoweditor(x);
	editingwin->Lock();
	if ((danger_feel == B_MODAL_APP_WINDOW_FEEL) || (danger_feel == B_MODAL_ALL_WINDOW_FEEL))
		editingwin->SetFeel(B_NORMAL_WINDOW_FEEL);
	editingwin->Unlock();
	tabs->AddTab(x);
	tabs->TabAt(1)->SetLabel("Window");
	wineditor = x;
#endif
	if (length == 0)
	{
		wineditor = new WindowEditor(BRect(0, 0, 250, 330), "wineditpanel", main, editpanel);
	}
	else
	{
		wineditor = new WindowEditor(BRect(0, 0, 250, 330), "wineditpanel", main, editpanel, data);
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
#if 0
	editingwin->SetFeel(danger_feel);
	editingwin->Lock();
	editingwin->cntrl->RemoveSelf();
	editingwin->RemoveCommonFilter(editingwin->delfilt);
	editingwin->RemoveCommonFilter(editingwin->actfilt);
	editingwin->RemoveCommonFilter(editingwin->dropfilt);
	delete editingwin->cntrl;
	if (editingwin->Archive(archive,true) != B_OK)
		(new BAlert("yikes","Archive is failing...","OK"))->Go();
	//archive = current;
	/**length = archive->FlattenedSize();
	unsigned char *data = new unsigned char[*length];
	if (archive->Flatten((char *)(data),NULL) != B_OK)
		(new BAlert("whoa","Flatten is failing...","OK"))->Go();*/
	editingwin->Unlock();
#endif
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

#if 0
void messaging(BMessage *msg,BView *bkgview){
	if (msg->what <= -4000) {
		BView *x = bkgview->FindView("menueditor");
		if (x != NULL) {
			x->MessageReceived(msg);
			return;
		}
	}
	if (msg->what <= -400) {
		editpanel->messages(msg);
		return;
	}
	//(new BAlert("helo","Code executing properly.","OK"))->Go();
	int32 temp;
	BCheckBox *cbox;
	BMenuBar *mbar;
	switch (msg->what) {
		case MSG_WINDOW_SET_TITLE:
			editingwin->SetTitle(((BTextControl *)(wineditor->FindView("title")))->Text());
			break;
		//case MSG_WINDOW_SET_LOOK:
		//	editingwin->AddChild(new BButton(BRect(50,50,150,100),"blank","Blank Button",new BMessage(MSG_WINDOW_SET_LOOK)));
		//	break;
		case MSG_WINDOW_SET_LOOK:
			msg->FindInt32("newlook",&temp);
			editingwin->SetLook(window_look(temp));
			break;
		case MSG_WINDOW_SET_FLAG:
			editingwin->Lock();
			editingwin->SetFlags(make_bitmask());
			editingwin->Unlock();
			break;
		case MSG_WINDOW_ADD_MENU:
			if (startingup)
				break;
			msg->FindPointer("source",(void **)(&cbox));
			if (cbox->Value() == B_CONTROL_ON) {
				mbar = new BMenuBar(BRect(0,0,editingwin->Frame().right,30),"menubar");
				mbar->AddItem(new BMenuItem("New Menu Bar",new BMessage('none')));
				editingwin->AddChild(mbar);
			} else {
				editingwin->Lock();
				if (editingwin->cntrl->controlled == editingwin->KeyMenuBar())
					editingwin->cntrl->KeyDown(new char(B_DELETE),1);
				else
					editingwin->KeyMenuBar()->RemoveSelf();
				editingwin->Unlock();
			}
			break;
		case MSG_WINDOW_SET_FEEL:
			msg->FindInt32("newfeel",&temp);
			danger_feel = window_feel(temp);
			if (!((temp == B_MODAL_APP_WINDOW_FEEL) || (temp == B_MODAL_ALL_WINDOW_FEEL)))
				editingwin->SetFeel(window_feel(temp));
			else
				editingwin->SetFeel(B_NORMAL_WINDOW_FEEL);
			break;
	}
}
#endif


/*
 * $Log $
 *
 * $Id  $
 *
 */

