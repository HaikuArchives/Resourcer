#include "includes.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "preamble.h"
#include "class.h"
#include "res.h"
#include "window.h"

int main(void) {
	openmwindows = 0;
	openpanel = NULL;
	itemLista = NULL;
	new BRApplication;
	openpan = false;
	newpanel = new BFilePanel(B_SAVE_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,NULL,false,true);
	openpanel = new BFilePanel(B_OPEN_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,new appFilter,false,true);
	be_app->Run();
	delete be_app;
	return 0;
}

void DoubleItem::Invoke(bool generic) {
	reswindow *x = (reswindow *)parent->Window();
	bool genoverride = generic;
	if (win != NULL) {
		if (win != NULL) {
			win->Hide();
			win->Show();
			win->Activate(true);
		}
		return;
	}
	app_info info;
	be_app->GetAppInfo(&info);
	BEntry entry(&(info.ref));
	BDirectory dir;
	entry.GetParent(&dir);
	entry.SetTo(&dir,"editors");
	dir.SetTo(&entry);
	TypeItem *super = (TypeItem *)parent->Superitem(this);
	type_code type = super->type;
	BPath path(&dir,super->TypeCode());
	plug_in plugin;
	plugin.win = x;
	entry.SetTo(path.Path());
	BEntry entry2(&dir,"unknown");
	if ((entry.Exists() == false) || (genoverride == true)) {
		if (entry2.Exists() == false) {
			BAlert *alert = new BAlert("noedit","No editor found.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
			alert->Go();
			return;
		} else {
			entry2.GetPath(&path);
		}
	}
	plugin.plugin = load_add_on(path.Path());
	BResources **tfile;
	if (get_image_symbol(plugin.plugin,"file",B_SYMBOL_TYPE_DATA,(void **)(&tfile)) == B_NO_ERROR)
		*tfile = x->openres;
	image_id *tplugin;
	if (get_image_symbol(plugin.plugin,"me",B_SYMBOL_TYPE_DATA,(void **)(&tplugin)) == B_NO_ERROR)
		*tplugin = plugin.plugin;
	if (get_image_symbol(plugin.plugin,"loaddata",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.loaddata))) != B_NO_ERROR) {
			BAlert *alert = new BAlert("noedit","The editor could not be launched.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
			alert->Go();
			return;
	}
	get_image_symbol(plugin.plugin,"savedata",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.savedata)));
	get_image_symbol(plugin.plugin,"messaging",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.messaging)));
	unsigned char *data;
	const void *dat;
	size_t length;
	if (idstring != NULL) {
		dat = x->openres->LoadResource(type,id,&length);
		if (dat == NULL) {
			data = NULL;
			length = 0;
		} else {
			data = new unsigned char[length];
			memcpy(data,dat,length);
		}
	} else {
		attr_info inf;
		if (x->file->GetAttrInfo(name,&inf) == B_OK) {
			length = inf.size;
			data = new unsigned char[length];
			x->file->ReadAttr(name,type,0,data,length);
		} else {
			data = NULL;
			length = 0;
		}
	}
	plugin.olddata = data;
	plugin.oldlength = length;
	char winname[255];
	if (idstring == NULL)
		sprintf(winname,"%s Attribute Name: %s",super->TypeCode(),name);
	else {
		if (name[0] == 0)
			sprintf(winname,"%s Resource Id: %ld",super->TypeCode(),id);
		else
			sprintf(winname,"%s Resource Id: %ld Name: %s",super->TypeCode(),id,name);
	}
	bool isattr;
	if (idstring != NULL)
		isattr = false;
	else
		isattr = true;
	pluginwindow *win = new pluginwindow(&(this->win),plugin,winname,id,type,name,isattr);
	win->gray = new BView(BRect(0,0,300,300),"graybkgrd",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS);
	win->gray->SetViewColor(216,216,216);
	win->AddChild(win->gray);
	if (length != 0) {	
		(*(plugin.loaddata))(data,length,win->gray);
	} else {
		(*(plugin.loaddata))(NULL,0,win->gray);
	}
	win->Show();
}

void BRApplication::RefsReceived(BMessage *message) {
		entry_ref ref;
		message->FindRef("refs",&ref);
		char title[B_FILE_NAME_LENGTH];
		BEntry entry(&ref,true);
		entry.GetName(title);
		new reswindow(title,ref);
		openpan = false;
		saveas = false;
		alredopen = true;
	}
	
void BRApplication::dissectwindow(BMessage *msg) {
		if (!saveas)
			return;
		reswindow *x;
		msg->FindPointer("reswindow",(void **)(&x));
		if (x->istempfile) {
			BEntry(&(x->fileref)).Remove();
			x->changes = false;
		}
		if (x->isquitting)
			x->PostMessage(B_QUIT_REQUESTED);
	}
	
void BRApplication::getready(BMessage *message,bool istempfile) { //come back
		entry_ref dira;
		char *name;
		message->FindRef("directory",&dira);
		message->FindString("name",(const char **)(&name));
		BDirectory dir(&dira);
		BEntry entrya(&dir,name);
		entrya.GetRef(&dira);
		BFile entry(&entrya,B_READ_WRITE | B_CREATE_FILE);
		BNodeInfo info(&entry);
		info.SetType("application/x-be-resource");
		if (saveas) {
			saveas = false;
			char title[B_FILE_NAME_LENGTH];
			char name[B_ATTR_NAME_LENGTH];
			reswindow *x;
			message->FindPointer("reswindow",(void **)(&x));
			unsigned char *buffer;
			entrya.GetName(title);
			x->SetTitle(title);
			x->openres->WriteTo(&entry);
			x->file->RewindAttrs();
			attr_info info;
			for (;x->file->GetNextAttrName(name) == B_OK;) {
				x->file->GetAttrInfo(name,&info);
				buffer = new unsigned char[info.size];
				x->file->ReadAttr(name,info.type,0,buffer,info.size);
				entry.WriteAttr(name,info.type,0,buffer,info.size);
				delete buffer;
			}
			if (x->istempfile) {
				BEntry entry(&(x->fileref));
				entry.Remove();
			}
			if (x->isquitting)
				x->PostMessage(B_QUIT_REQUESTED);
			delete x->file;
			x->file = new BFile(entry);
			x->fileref = dira;
			x->istempfile = false;
		} else {
			BResources res(&entry,true);
			char title[B_FILE_NAME_LENGTH];
			entrya.GetName(title);
			if (istempfile)
				strcpy(title,"Untitled");
			reswindow *tempder = new reswindow(title,dira);
			tempder->istempfile = istempfile;
		}
		alredopen = true;
		saveas = false;
	}