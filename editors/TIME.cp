//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextControl.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "time_t Value";


void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	char mime[256];
	if (length == 0) {
		mime[0] = '0';
		mime[1] = 0;
	} else {
		int32 dataa;
		dataa = *(int32 *)(data);
		sprintf(mime,"%ld",dataa);
	}
	bkgview->Window()->ResizeTo(300,40);
	BTextControl *v = new BTextControl(BRect(20,5,280,35),"Integer","Data: ",mime,NULL);
	v->SetDivider(be_plain_font->StringWidth("Data: "));
	bkgview->AddChild(v);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set length to the size of data
	BTextControl *text = (BTextControl *)(bkgview->ChildAt(0));
	char *toreturn = new char[4];
	int32 data;
	sscanf(text->Text(),"%ld",&data);
	char *one = (char *)(&data);
	char *two = one + 1;
	char *three = two + 1;
	char *four = three + 1;
	toreturn[0] = *one;
	toreturn[1] = *two;
	toreturn[2] = *three;
	toreturn[3] = *four;
	*length = 4;
	return (unsigned char*)(toreturn);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}