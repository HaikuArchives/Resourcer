//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextControl.h>
#include <interface/ScrollView.h>
#include <interface/OutlineListView.h>
#include <storage/Directory.h>
#include <storage/Path.h>
#include <interface/Alert.h>
#include <interface/ListItem.h>
#include <app/Message.h>
#include <app/Application.h>
#include <app/Roster.h>
//-------------------------------------------

//------------Prototypes---------------------
class TypeItem;

void InitListView(void);
void GetSelected(type_code *type, int32 *index, char **name);
TypeItem *FindType(type_code type);
//-------------------------------------------

//------------Globals------------------------
BMessage msg;
BOutlineListView *msgview;
BList openWindows;
BTextControl *what;
//-------------------------------------------

//------------Export Functions---------------
extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Archived BMessage";
//-------------------------------------------

//------------Class Definitions--------------
class TypeItem : public BStringItem {
	public:
		TypeItem(type_code code);
		TypeItem(const char *string_code);
		void SetTypeCode(type_code code);
		void SetTypeCode(const char *string_code);
		char *TypeAsString(void);
		type_code Type(void);
	private:
		void SetDescription(void);
		char *description;
		type_code type;
};

class PlugWin : public BWindow {
	public:
		PlugWin(type_code code, char *pointer, int32 ind, unsigned char* (*save)(size_t *,BView *), void (*messages)(BMessage *,BView *), char *name);
		bool QuitRequested(void);
		void MessageReceived(BMessage *msg);
		BView *gray;
		type_code type;
		char *name;
		int32 index;
	private:
		unsigned char* (*savedata)(size_t *,BView *);
		void (*messaging)(BMessage *,BView *);
};
//-------------------------------------------

//------------C Functions--------------------
void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	if (length != 0)
		msg.Unflatten((const char *)(data));
	BRect newFrame = bkgview->Bounds();
	newFrame.top += 30;
	newFrame.right -= B_V_SCROLL_BAR_WIDTH;
	newFrame.bottom -= B_H_SCROLL_BAR_HEIGHT;
	msgview = new BOutlineListView(newFrame,"msgView",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES);
	InitListView();
	BScrollView *scrolly = new BScrollView("msgViewScroll",msgview,B_FOLLOW_ALL_SIDES,0,true,true,B_NO_BORDER);
	scrolly->ScrollBar(B_HORIZONTAL)->SetRange(0,0);
	bkgview->AddChild(scrolly);
	what = new BTextControl(BRect(10,5,150,30),"what","Command Constant: ",TypeItem(msg.what).TypeAsString(),NULL);
	what->SetDivider(be_plain_font->StringWidth(what->Label()));
	what->TextView()->SetMaxBytes(4);
	bkgview->AddChild(what);
}


unsigned char* savedata(size_t *length,BView *bkgview) {
	PlugWin *win;
	msg.what = TypeItem(what->Text()).Type();
	for (int32 i = openWindows.CountItems()-1; i >= 0; i--) {
		win = ((PlugWin *)(openWindows.ItemAt(i)));
		win->Lock();
		win->QuitRequested();
		win->Quit();
	}
	ssize_t size = msg.FlattenedSize();
	unsigned char *data = new unsigned char[size];
	msg.Flatten((char *)data,size);
	*length = size;
	return data;
}

void messaging(BMessage *msg,BView *bkgview) {
	if (msg->what != -100)
		return;
	int32 index;
	char *name;
	type_code type;
	if (msgview->CurrentSelection() < 0)
		return;
	if (msgview->CountItemsUnder(msgview->ItemAt(msgview->CurrentSelection()),true) != 0) {
		if (msgview->ItemAt(msgview->CurrentSelection())->IsExpanded())
			msgview->Collapse(msgview->ItemAt(msgview->CurrentSelection()));
		else
			msgview->Expand(msgview->ItemAt(msgview->CurrentSelection()));
		return;
	}
	GetSelected(&type,&index,&name);
	//-----------Check if already open------------------
	PlugWin *win;
	for (int i = 0;(win = (PlugWin *)openWindows.ItemAt(i)) != NULL; i++) {
		if ((win->type == type) && (win->index == index) && (strcmp(name,win->name) == 0)) {
			win->Show();
			return;
		}
	}
	//-----------Get Add-Ons directory------------------
		app_info info;
		be_app->GetAppInfo(&info);
		BEntry entry(&(info.ref));
		BDirectory dir;
		entry.GetParent(&dir);
		entry.SetTo(&dir,"editors");
		dir.SetTo(&entry);
	//----------Load Editor-----------------------------
		BPath path(&dir,TypeItem(type).TypeAsString());
		image_id plugin;
		entry.SetTo(path.Path());
		BEntry entry2(&dir,"unknown");
		if (entry.Exists() == false) {
			if (entry2.Exists() == false) {
				BAlert *alert = new BAlert("noedit","No editor found.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
				alert->Go();
				return;
			} else {
				entry2.GetPath(&path);
			}
		}
		plugin = load_add_on(path.Path());
	//----------Launch Editor----------------------------
		void (*loaddata)(unsigned char *,size_t,BView *);
		unsigned char* (*savedata)(size_t *,BView *);
		void (*messaging)(BMessage *,BView *);
		BResources **tfile;
		if (get_image_symbol(plugin,"file",B_SYMBOL_TYPE_DATA,(void **)(&tfile)) == B_NO_ERROR)
			*tfile = NULL;
		if (get_image_symbol(plugin,"loaddata",B_SYMBOL_TYPE_TEXT,(void **)(&loaddata)) != B_NO_ERROR) {
				BAlert *alert = new BAlert("noedit","The editor could not be launched.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
				alert->Go();
				return;
		}
		get_image_symbol(plugin,"savedata",B_SYMBOL_TYPE_TEXT,(void **)(&(savedata)));
		get_image_symbol(plugin,"messaging",B_SYMBOL_TYPE_TEXT,(void **)(&(messaging)));
	//---------Read Data----------------------------------
		unsigned char *data;
		ssize_t length;
		::msg.FindData(name,type,index,(const void **)(&data),&length);
		char title[255];
		sprintf(title,"Field Type: %s Name: %s Index: %ld",TypeItem(type).TypeAsString(),name,index);
		win = new PlugWin(type,name,index,savedata,messaging,title);
		win->gray = new BView(BRect(0,0,300,300),"graybkgrd",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS);
		win->gray->SetViewColor(216,216,216);
		win->AddChild(win->gray);
		if (length != 0) {	
			(*(loaddata))(data,length,win->gray);
		} else {
			(*(loaddata))(NULL,0,win->gray);
		}
		win->Show();
}

void InitListView(void) {
	int32 i,c,j;
	char *name;
	type_code type;
	BStringItem *super;
	BStringItem *superb;
	char count[12];
	for (i = 0;msg.GetInfo(B_ANY_TYPE,i,&name,&type,&c) == B_OK;i++) {
		super = FindType(type);
		superb = new BStringItem(name);
		msgview->AddUnder(superb,super);
		if (c > 1) {
			for (j = 0;j < c;j++) {
				sprintf(count,"%ld",(c-1) - j);
				msgview->AddUnder(new BStringItem(count),superb);
			}
		}
	}
	msgview->SetInvocationMessage(new BMessage(-100));
}

TypeItem *FindType(type_code type) {
	TypeItem *candidate;
	for (int32 i = msgview->CountItems() - 1;i >= 0;i--) {
		if (((TypeItem *)msgview->ItemAt(i))->OutlineLevel() != 0)
			continue;
		candidate = (TypeItem *)msgview->ItemAt(i);
		if (candidate->Type() == type)
			return candidate;
	}
	candidate = new TypeItem(type);
	msgview->AddItem(candidate);
	msgview->Collapse(candidate);
	return candidate;
}

void GetSelected(type_code *type, int32 *index, char **name) {
	int32 i = msgview->FullListCurrentSelection();
	BStringItem *selected = (BStringItem *)msgview->FullListItemAt(i);
	switch (selected->OutlineLevel()) {
		case 0:
			msgview->Expand(selected);
			*index = -1;
			break;
		case 1:
			*index = 0;
			*name = new char[strlen(selected->Text()) + 1];
			strcpy(*name,selected->Text());
			*type = ((TypeItem *)(msgview->Superitem(selected)))->Type();
			break;
		case 2:
			sscanf(selected->Text(),"%ld",index);
			*name = new char[strlen(((BStringItem *)(msgview->Superitem(selected)))->Text()) + 1];
			strcpy(*name,((BStringItem *)(msgview->Superitem(selected)))->Text());
			*type = ((TypeItem *)(msgview->Superitem(msgview->Superitem(selected))))->Type();
	}
}
	
//------------------------------------------

//------------Member Functions--------------

//------------TypeItem----------------------
TypeItem::TypeItem(type_code code) : BStringItem("NULL") {
	type = code;
	SetDescription();
}

TypeItem::TypeItem(const char *string_code) : BStringItem(string_code) {
	SetTypeCode(string_code);
	SetDescription();
}

void TypeItem::SetTypeCode(type_code code) {
	type = code;
	SetDescription();
}

void TypeItem::SetDescription(void) {
	//-----------Get Add-Ons directory------------------
		app_info info;
		be_app->GetAppInfo(&info);
		BEntry entry(&(info.ref));
		BDirectory dir;
		entry.GetParent(&dir);
		entry.SetTo(&dir,"editors");
		dir.SetTo(&entry);
	//----------Load Editor-----------------------------
		BPath path(&dir,TypeAsString());
		image_id plugin;
		entry.SetTo(path.Path());
		if (entry.Exists() == false) {
			description = "Unknown";
			return;
		}
		plugin = load_add_on(path.Path());
		image_id editor = load_add_on(path.Path());
		char *temp;
		if (get_image_symbol(editor,"description",B_SYMBOL_TYPE_DATA,(void **)(&temp)) != B_NO_ERROR)
			description = "Unknown";
		else {
			description = new char[strlen(temp) + 1];
			strcpy(description,temp);
		}
		char *tempy = new char[10 + strlen(description)];
		sprintf(tempy,"%s (%s)",TypeAsString(),description);
		SetText(tempy);
}

void TypeItem::SetTypeCode(const char *string_code) {
	memcpy(&type,string_code,4);
	swap_data(B_UINT32_TYPE,&type,4,B_SWAP_HOST_TO_BENDIAN);
	SetDescription();
}

char *TypeItem::TypeAsString(void) {
	char *AsString = new char[5];
	memcpy(AsString,&type,4);
	swap_data(B_UINT32_TYPE,AsString,4,B_SWAP_BENDIAN_TO_HOST);
	AsString[4] = 0;
	return AsString;
}

type_code TypeItem::Type(void) {
	type_code temp = type;
	return temp;
}
//-------------------------------------------

//------------------PlugInWin----------------
PlugWin::PlugWin(type_code code, char *pointer, int32 ind, unsigned char* (*save)(size_t *,BView *), void (*messages)(BMessage *,BView *), char *name) : BWindow(BRect(400,300,700,600),name,B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE) {
	savedata = save;
	messaging = messages;
	index = ind;
	type = code;
	this->name = new char[strlen(pointer) + 1];
	strcpy(this->name,pointer);
	openWindows.AddItem(this);
}

void PlugWin::MessageReceived(BMessage *msg) {
	(*messaging)(msg,gray);
	BWindow::MessageReceived(msg);
}

bool PlugWin::QuitRequested(void) {
	size_t length;
	unsigned char *data = (*savedata)(&length,gray);
	msg.ReplaceData(name,type,index,data,length);
	delete data;
	delete name;
	openWindows.RemoveItem(this);
	return true;
}