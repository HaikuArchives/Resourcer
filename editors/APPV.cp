//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextControl.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <storage/AppFileInfo.h>
//------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Application Version";

version_info *info2;

void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	bkgview->Window()->ResizeTo(300,120);
	version_info *info = (version_info *)(data);
	if (length == 680)
		info2 = new version_info(*((version_info *)(data + 340)));
	else
		info2 = new version_info;
	if (length == 0) {
		info = new version_info;
		info->major = 0;
		info->minor = 0;
		info->middle = 0;
		info->variety = 0;
		info->internal = 0;
		info->short_info[0] = 0;
		info->long_info[0] = 0;
	}
	char buffer[80];
	float vers_width = be_plain_font->StringWidth("Version: 1000");
	float dot_width = be_plain_font->StringWidth(".1000");
	sprintf(buffer,"%ld",info->major);
	BTextControl* version = new BTextControl(BRect(0,20,vers_width,40), "vers","Version: ",buffer,NULL);
	version->SetDivider(be_plain_font->StringWidth("Version: "));
	bkgview->AddChild(version);
	sprintf(buffer,"%ld",info->middle);
	version = new BTextControl(BRect(vers_width,20,vers_width + dot_width,40), "middle",".",buffer,NULL);
	version->SetDivider(be_plain_font->StringWidth("."));
	bkgview->AddChild(version);
	sprintf(buffer,"%ld",info->minor);
	version = new BTextControl(BRect(vers_width + dot_width,20,vers_width + dot_width + dot_width,40), "minor",".",buffer,NULL);
	version->SetDivider(be_plain_font->StringWidth("."));
	bkgview->AddChild(version);
	char *states[6];
	states[0] = "Development";
	states[1] = "Alpha";
	states[2] = "Beta";
	states[3] = "Gamma";
	states[4] = "Golden master";
	states[5] = "Final";
	BPopUpMenu *menu = new BPopUpMenu("states");
	menu->AddItem(new BMenuItem(states[0],new BMessage(-200)));
	menu->AddItem(new BMenuItem(states[1],new BMessage(-200)));
	menu->AddItem(new BMenuItem(states[2],new BMessage(-200)));
	menu->AddItem(new BMenuItem(states[3],new BMessage(-200)));
	menu->AddItem(new BMenuItem(states[4],new BMessage(-200)));
	menu->AddItem(new BMenuItem(states[5],new BMessage(-200)));
	menu->ItemAt(info->variety)->SetMarked(true);
	BMenuField *variety = new BMenuField(BRect(vers_width + dot_width + dot_width +10,20,vers_width + dot_width + dot_width + 175,40),"Variety","Release: ",menu);
	variety->SetDivider(be_plain_font->StringWidth("Release: "));
	bkgview->AddChild(variety);
	sprintf(buffer,"%ld",info->internal);
	version = new BTextControl(BRect(vers_width + dot_width + 175,20,vers_width + dot_width + dot_width + 180,40), "internal",NULL,buffer,NULL);
	version->SetDivider(0);
	bkgview->AddChild(version);
	BTextControl* short_info = new BTextControl(BRect(0,50,250,70), "short","Short Info: ",info->short_info,NULL);
	short_info->SetDivider(be_plain_font->StringWidth("Short Info: "));
	bkgview->AddChild(short_info);
	BTextControl* long_info = new BTextControl(BRect(0,80,250,120), "long","Long Info: ",info->long_info,NULL);
	long_info->SetDivider(be_plain_font->StringWidth("Long Info: "));
	bkgview->AddChild(long_info);
}

unsigned char* savedata(size_t *length,BView *bkgview) {
	version_info *data = new version_info;
	sscanf(((BTextControl*)(bkgview->ChildAt(0)))->Text(),"%ld",&(data->major));
	sscanf(((BTextControl*)(bkgview->ChildAt(1)))->Text(),"%ld",&(data->middle));
	sscanf(((BTextControl*)(bkgview->ChildAt(2)))->Text(),"%ld",&(data->minor));
	sscanf(((BTextControl*)(bkgview->ChildAt(4)))->Text(),"%ld",&(data->internal));
	strcpy(data->short_info,((BTextControl*)(bkgview->ChildAt(5)))->Text());
	strcpy(data->long_info,((BTextControl*)(bkgview->ChildAt(6)))->Text());
	BMenu *menu = ((BMenuField *)(bkgview->ChildAt(3)))->Menu();
	data->variety = menu->IndexOf(menu->FindMarked());
	*length = 680;
	unsigned char *info = new unsigned char[680];
	memcpy(info,data,340);
	delete data;
	memcpy(info + 340,info2,340);
	delete info2;
	return info;
}

void messaging(BMessage *message,BView *bkgview) {
}