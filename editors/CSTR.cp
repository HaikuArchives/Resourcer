//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextView.h>
#include <interface/ScrollView.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Text String";


void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	BTextView *view = new BTextView(BRect(20,20,280,280),"String",BRect(0,0,230,230),B_FOLLOW_NONE,B_WILL_DRAW);
	unsigned char *data2;
	if (length == 0) {
		data2 = new unsigned char[2];
		data2[0] = 0;
	} else {
		data2 = data;
	}
	view->SetText((char*)(data2)/*,int32(length)*/);
	//}
	bkgview->AddChild(new BScrollView("StringScroll",view));
	//window->Lock();
	//view->Draw(BRect(20,20,280,280));
	//window->Unlock();
}

unsigned char* savedata(size_t *length,BView *bkgview) {
	BTextView *view = (BTextView *)(bkgview->FindView("String"));
	char *hjk = new char[strlen(view->Text()) + 1];
	strcpy(hjk,view->Text());
	*length = strlen(hjk) + 1;
	return (unsigned char*)(hjk);
}

void messaging(BMessage *message,BView *bkgview) {
}