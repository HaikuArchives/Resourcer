//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/ColorControl.h>
#include <interface/Box.h>
#include <app/MessageFilter.h>
#include <support/ClassInfo.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "RGB Color";

filter_result dummy_patch(BMessage *message,BHandler **target,BMessageFilter *messageFilter) {
	messaging(message,((BWindow *)(*target))->ChildAt(0));
	return B_DISPATCH_MESSAGE;
}

void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	BColorControl *colorPicker = new BColorControl(BPoint(10,10),B_CELLS_32x8,8,"colorPicker",new BMessage('COLR'));
	BMessageFilter *filter = new BMessageFilter(B_ANY_DELIVERY,B_ANY_SOURCE,&dummy_patch);
	colorPicker->SetTarget(bkgview->Window());
	bkgview->Window()->AddFilter(filter);
	float x,y;
	colorPicker->GetPreferredSize(&x,&y);
	x += 20; y += 60;
	bkgview->Window()->ResizeTo(x,y);
	if (length == 0) {
		rgb_color white;
		white.red = 0xff; white.blue = 0xff; white.green = 0xff;
		colorPicker->SetValue(white);
	} else {
		colorPicker->SetValue(*((rgb_color *)(data)));
	}
	bkgview->AddChild(colorPicker);
	BRect rect = BRect(colorPicker->Bounds().left+10,colorPicker->Bounds().bottom + 20,colorPicker->Bounds().right+10,colorPicker->Bounds().bottom+50);
	BBox *color2 = new BBox(rect,"color");
	rect.InsetBy(5,15);
	rect.bottom += 10;
	color2->SetLabel("Color");
	bkgview->AddChild(color2);
	BView *color3 = new BView(color2->ConvertFromParent(rect),"color2",B_FOLLOW_NONE,B_WILL_DRAW);
	color3->SetViewColor(colorPicker->ValueAsColor());
	color2->AddChild(color3);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set length to the size of data
	BColorControl *color = (BColorControl *)(bkgview->FindView("colorPicker"));
	rgb_color *toreturn = new rgb_color;
	*toreturn = color->ValueAsColor();
	*length = sizeof(rgb_color);
	return (unsigned char*)(toreturn);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
//	if (message->what == 'COLR') {
		BColorControl *color = (BColorControl *)(bkgview->FindView("colorPicker"));
		rgb_color new_color = color->ValueAsColor();
		BView *view = bkgview->FindView("color")->FindView("color2");
		if ((new_color.green == view->ViewColor().green) && (new_color.blue == view->ViewColor().blue) && (new_color.red == view->ViewColor().red))
			return;
		view->SetViewColor(new_color);
		view->Invalidate(view->Bounds());
		
//	}
}