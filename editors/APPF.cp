//------------Includes----------------------
#include <image.h>
#include <app/Roster.h>
#include <interface/RadioButton.h>
#include <interface/CheckBox.h> 
#include <interface/Window.h>
#include <stdio.h>
//------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Application Flags";


void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	uint32 value;
	uint32 app_flags = 0;
	if (length == 0) {
		value = 0;
	} else {
		value = (*((uint32 *)(data))) & B_LAUNCH_MASK;
		switch (value) {
			case B_EXCLUSIVE_LAUNCH:
				value = 2;
				break;
			case B_SINGLE_LAUNCH:
				value = 0;
				break;
			case B_MULTIPLE_LAUNCH:
				value = 1;
				break;
			default:
				value = 0;
		}
	}
	bkgview->Window()->ResizeTo(300,100);
	BRadioButton *buts[3];
	buts[0] = new BRadioButton(BRect(20,20,130,40),"1","Single Launch",new BMessage(-100));
	buts[1] = new BRadioButton(BRect(20,40,130,60),"2","Multiple Launch",new BMessage(-200));
	buts[2] = new BRadioButton(BRect(20,60,130,80),"3","Exclusive Launch",new BMessage(-200));
	bkgview->AddChild(buts[0]);
	bkgview->AddChild(buts[1]);
	bkgview->AddChild(buts[2]);
	bkgview->Window()->Lock();
	buts[value]->SetValue(true);
	bkgview->Window()->Unlock();
	BCheckBox *boxes[2];
	boxes[0] = new BCheckBox(BRect(170,20,280,40),"4","Background App",new BMessage(-200));
	boxes[1] = new BCheckBox(BRect(170,40,280,60),"5","ARGV Only",new BMessage(-200));
	if (length != 0)
		app_flags = *((uint32 *)(data));
	if (app_flags & B_BACKGROUND_APP)
		boxes[0]->SetValue(true);
	if (app_flags & B_ARGV_ONLY)
		boxes[1]->SetValue(true);
	bkgview->AddChild(boxes[0]);
	bkgview->AddChild(boxes[1]);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	uint32 one = 0;
	for(int i = 0; i <= 2; i++) {
		BRadioButton *but = (BRadioButton *)(bkgview->ChildAt(i));
		if (but->Value() == true) {
			one = i;
			break;
		}
	}
	uint32 value = one;
	switch (value) {
		case 2:
			value = B_EXCLUSIVE_LAUNCH;
			break;
		case 0:
			value = B_SINGLE_LAUNCH;
			break;
		case 1:
			value = B_MULTIPLE_LAUNCH;
			break;
		default:
			value = B_SINGLE_LAUNCH;
	}
	one = value;
	BCheckBox *boxes[2];
	boxes[0] = (BCheckBox *)(bkgview->ChildAt(3));
	boxes[1] = (BCheckBox *)(bkgview->ChildAt(4));
	uint32 a = boxes[0]->Value();
	if (a) {
		a = B_BACKGROUND_APP;
	}
	uint32 b = boxes[1]->Value();
	if (b) {
		b = B_ARGV_ONLY;
	}
	value = one | a | b;
	*length = 4;
	return (unsigned char *)(new uint32(value));
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}