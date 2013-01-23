#include "includes.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "preamble.h"
#include "class.h"
#include "res.h"
#include "window.h"

int main(void) {
	openmwindows = 0;       //---This tracks open documents, so we can quit when it's zero--
	openpanel = NULL;
	itemLista = NULL;
	new BRApplication;
	newpanel = new BFilePanel(B_SAVE_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,NULL,false,true);
	openpanel = new BFilePanel(B_OPEN_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,new appFilter,false,true);
	be_app->Run();
	delete be_app;
	return 0;
}

void DoubleItem::Invoke(bool generic) {
	reswindow *x = (reswindow *)parent->Window(); //----Context is important
	bool genoverride = generic;					//------whether to use hex editor or not
	if (win != NULL) {							//------If we are open, we only need to activate our window---
		win->Hide();
		win->Show();
		win->Activate(true);
		return;
	}
	app_info info;                        //-Find the editors directory
	be_app->GetAppInfo(&info);            //-|
	BEntry entry(&(info.ref));            //-|
	BDirectory dir;						  //-|
	entry.GetParent(&dir);				  //-|
	entry.SetTo(&dir,"editors");          //-|
	dir.SetTo(&entry);                    //-|
	TypeItem *super = (TypeItem *)parent->Superitem(this);  //-Get our type code
	type_code type = super->type;                           //-|
	BPath path(&dir,super->TypeCode());					//--editors/typecode, the path of our editor
	plug_in plugin;										//--handy little struct, holds useful information, the def is in preamble.h
	plugin.win = x;                                     //-Set it up
	entry.SetTo(path.Path());
	BEntry entry2(&dir,"unknown");                      //-The hex editor
	if ((entry.Exists() == false) || (genoverride == true)) { //-Do we not have an editor or do we want to use the hex editor?
		if (entry2.Exists() == false) {                       //-Does the hex editor exist?
			BAlert *alert = new BAlert("noedit","No editor found.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT); //-Yikes!
			alert->Go();
			return;
		} else {
			entry2.GetPath(&path); //--Let's use the hex editor
		}
	}
	plugin.plugin = load_add_on(path.Path()); //--Load the add-on
	BResources **tfile;
	if (get_image_symbol(plugin.plugin,"file",B_SYMBOL_TYPE_DATA,(void **)(&tfile)) == B_NO_ERROR)
		*tfile = x->openres;  //--------WIND uses this to get info on bits and CSTR resources
	image_id *tplugin;
	if (get_image_symbol(plugin.plugin,"me",B_SYMBOL_TYPE_DATA,(void **)(&tplugin)) == B_NO_ERROR)
		*tplugin = plugin.plugin; //---No one uses, but you could to get your plug-in id
	if (get_image_symbol(plugin.plugin,"loaddata",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.loaddata))) != B_NO_ERROR) {
			BAlert *alert = new BAlert("noedit","The editor could not be launched.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
			alert->Go();
			return;         //---Load Data loads the data. If it doesn't exist, there's a problem
	}
	get_image_symbol(plugin.plugin,"savedata",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.savedata)));
	get_image_symbol(plugin.plugin,"messaging",B_SYMBOL_TYPE_TEXT,(void **)(&(plugin.messaging)));
	unsigned char *data; //---Send in our data, we'll delete it later
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
	plugin.olddata = data;        //---To check if it's changed, we need the original data
	plugin.oldlength = length;    //-|
	char winname[255];            //--Set the window name
	if (idstring == NULL)
		sprintf(winname,"%s Attribute Name: %s",super->TypeCode(),name);
	else {
		if (name[0] == 0)
			sprintf(winname,"%s Resource Id: %ld",super->TypeCode(),id);
		else
			sprintf(winname,"%s Resource Id: %ld Name: %s",super->TypeCode(),id,name);
	}
	pluginwindow *win = new pluginwindow(&(this->win),plugin,winname,id,type,name,(idstring == NULL));
	win->gray = new BView(BRect(0,0,300,300),"graybkgrd",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS);
	win->gray->SetViewColor(216,216,216);
	win->AddChild(win->gray);
	if (length != 0) {	
		(*(plugin.loaddata))(data,length,win->gray);  //--Send in data
	} else {
		(*(plugin.loaddata))(NULL,0,win->gray);       //--It's a new resource
	}
	win->Show();
}

void BRApplication::RefsReceived(BMessage *message) {
		entry_ref ref;
		message->FindRef("refs",&ref);
		char title[B_FILE_NAME_LENGTH];
		BEntry entry(&ref,true);
		entry.GetName(title);
		new reswindow(title,ref); //---reswindow does all the dirty work
		openpan = false;          //---All these booleans could probably be consolidated
		saveas = false;
	}
	
void BRApplication::dissectwindow(BMessage *msg) { //---Do panel close
		if (!saveas)
			return;
		reswindow *x;
		msg->FindPointer("reswindow",(void **)(&x));
		if (x->istempfile) {
			BEntry(&(x->fileref)).Remove();  //---------Remove the temp file
			x->changes = false;
		}
		if (x->isquitting)					//----------Was the window quitting and we hit save as?
			x->PostMessage(B_QUIT_REQUESTED);//---------Close the window
	}
	
void BRApplication::getready(BMessage *message,bool istempfile) { //---Do save as dirty work
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
				delete [] buffer;
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
		saveas = false;
	}
