//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextControl.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "8 Byte Floating Point Number";


void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	char mime[256];
	if (length == 0) {
		mime[0] = '0';
		mime[1] = '.';
		mime[2] = '0';
		mime[3] = 0;
	} else {
		double dataa;
		dataa = *((double *)(data));
		sprintf(mime,"%lf",dataa);
	}
	bkgview->Window()->ResizeTo(300,40);
	BTextControl *v = new BTextControl(BRect(20,5,280,35),"Integer","Data: ",mime,NULL);
	v->SetDivider(be_plain_font->StringWidth("Data: "));
	bkgview->AddChild(v);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set length to the size of data
	BTextControl *text = (BTextControl *)(bkgview->ChildAt(0));
	double *data = new double;
	sscanf(text->Text(),"%lf",data);
	*length = sizeof(double);
	return (unsigned char*)(data);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}