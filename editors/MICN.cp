//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/CheckBox.h>
#include <interface/ColorControl.h>
#include <Screen.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "16x16 Icon";

class pixel : public BView {
	public:
	pixel(BRect rect, char *name, unsigned char color) : BView(rect,name,B_FOLLOW_NONE,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
		state = color;
		if (state == B_TRANSPARENT_8_BIT) {
			SetViewColor(255,255,255);
			SetHighColor(0,127,255);
			FillRect(BRect(0,0,16,16),B_MIXED_COLORS);
		} else {
			SetViewColor(BScreen().ColorForIndex(state));
		}
		Invalidate();
	}
	void MouseDown(BPoint point);
	void Draw(BRect updateRect);
	unsigned char state;
};

class icon : public BView {
	public:
	icon(unsigned char *data) : BView(BRect(4,4,260,260),"icon",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {
		int row = 0;
		pixel *pix;
		for (int i = 0;!((i == 16) && (row == 15)); i++) {
			if (i == 16) {
				row++;
				i = 0;
			}
			pix = new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,data[i + (row *16)]);
			AddChild(pix);
			pix->Invalidate();
		}
	}
};

void pixel::MouseDown(BPoint point) {
		rgb_color col = ((BColorControl *)(Window()->ChildAt(0)->FindView("iconcolors")))->ValueAsColor();
		BScreen screen;
		uint8 x = screen.IndexForColor(col);
		col = screen.ColorForIndex(x);
		if (((BCheckBox *)(Window()->ChildAt(0)->FindView("Tp")))->Value() == 1) {
			x = B_TRANSPARENT_8_BIT;
			SetViewColor(255,255,255);
			SetHighColor(0,127,255);
			FillRect(BRect(0,0,16,16),B_MIXED_COLORS);
		} else {
			SetViewColor(col);
		}
		state = x;
		Invalidate();
	}

void pixel::Draw(BRect updateRect) {
		if (state == B_TRANSPARENT_8_BIT) {
			SetViewColor(255,255,255);
			SetHighColor(0,127,255);
			FillRect(BRect(0,0,16,16),B_MIXED_COLORS);
		} else {
			SetViewColor(BScreen().ColorForIndex(state));
		}
	}

/*class BIView : public BView {
	public:
		BIView(BBitmap *map) : BView(BRect(0,0,31,31),"icon",B_FOLLOW_NONE,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE) {bits = map;}
		void Draw(BRect updateRect) {
			if (Window() != NULL) {
				Window()->Lock();
				DrawBitmap(bits);
				Window()->Unlock();
			}
		}
	private:
		BBitmap *bits;
	};*/

void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	//BBitmap *bits = new BBitmap(BRect(0,0,31,31),B_CMAP8,false,true);
	//memcpy(bits->Bits(),data + sizeof(TranslatorBitmap),length);
	//BIView *v = new BIView(bits);
	bkgview->Window()->ResizeTo(270,360);
	if (length == 0) {
		data = new unsigned char[16*16];
		for (int i = 0; i < (16*16); i++) {
			data[i] = B_TRANSPARENT_8_BIT;
		}
	}
	icon *v = new icon(data);
	bkgview->AddChild(v);
	bkgview->AddChild(new BCheckBox(BRect(4,340,150,360),"Tp","Transparent",new BMessage(-100)));
	bkgview->AddChild(new BColorControl(BPoint(4,270),B_CELLS_32x8,8,"iconcolors"));
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	BView *icon = bkgview->FindView("icon");
	unsigned char *data = new unsigned char[16*16];
	for (int i = 0; i < (16*16); i++) {
		data[i] = ((pixel *)(icon->ChildAt(i)))->state;
	}
	*length = 16*16;
	return data;
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}