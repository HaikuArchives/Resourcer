//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/ColorControl.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "RGB Color";

void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	BColorControl *colorPicker = new BColorControl(BPoint(10,10),B_CELLS_32x8,8,"colorPicker");
	float x,y;
	colorPicker->GetPreferredSize(&x,&y);
	x += 20; y += 20;
	bkgview->Window()->ResizeTo(x,y);
	if (length == 0) {
		rgb_color white;
		white.red = 0xff; white.blue = 0xff; white.green = 0xff;
		colorPicker->SetValue(white);
	} else {
		colorPicker->SetValue(*((rgb_color *)(data)));
	}
	bkgview->AddChild(colorPicker);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set length to the size of data
	BColorControl *color = (BColorControl *)(bkgview->FindView("colorPicker"));
	rgb_color *toreturn = new rgb_color;
	*toreturn = color->ValueAsColor();
	*length = sizeof(rgb_color);
	return (unsigned char*)(toreturn);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}