//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/RadioButton.h>
//--------Export Functions------------------------------------------
extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
//-----------------------------Human Readable Description----------------------
extern "C" _EXPORT const char description[] = "Boolean Value";//-
//-----------------------------------------------------------------------------


void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	bool value;
	if (length == 0) {
		value = false;
	} else {
		if (*data == false) {
			value = false;
		} else {
			value = true;
		}
	}
	bkgview->Window()->ResizeTo(300,80);
	BRadioButton *truebut = new BRadioButton(BRect(20,20,130,60),"True","True",new BMessage(-100));
	BRadioButton *falsebut = new BRadioButton(BRect(170,20,280,60),"False","False",new BMessage(-200));
	bkgview->AddChild(truebut);
	bkgview->AddChild(falsebut);
	bkgview->Window()->Lock();
	if (value == false) {
		falsebut->SetValue(true);
	} else {
		truebut->SetValue(true);
	}
	bkgview->Window()->Unlock();
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	BRadioButton *view = (BRadioButton *)(bkgview->FindView("True"));
	char *toreturn = new char[2];
	if(view->Value() == 0) {
		toreturn[0] = false;
	} else {
		toreturn[0] = true;
	}
	*length = 1;
	return (unsigned char*)(toreturn);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}