//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/RadioButton.h>
#include <interface/Picture.h>
//-------------------------------------------

uint8 bitat(uint8 byte,char address);
uint8 deb2(uint8 *byte);

uint8 deb2(uint8 *y) {
	uint8 x;
	x = y[0]*128 + y[1]*64 + y[2]*32 + y[3]*16 + y[4]*8 + y[5]*4 + y[6]*2 + y[7];
	return x;
	}
	
uint8 bitat(uint8 byte,char address) {
		uint8 x;
		uint8 *y = new uint8[8]; 
		x = byte;
		y[0]=x/128;
		y[1]=(x-y[0]*128)/64;
		y[2]=(x-(y[0]*128 + y[1]*64))/32;
		y[3]=(x-(y[0]*128 + y[1]*64 + y[2]*32))/16;
		y[4]=(x-(y[0]*128 + y[1]*64 + y[2]*32 + y[3]*16))/8;
		y[5]=(x-(y[0]*128 + y[1]*64 + y[2]*32 + y[3]*16 + y[4]*8))/4;
		y[6]=(x-(y[0]*128 + y[1]*64 + y[2]*32 + y[3]*16 + y[4]*8 + y[5]*4))/2;
		y[7]=(x-(y[0]*128 + y[1]*64 + y[2]*32 + y[3]*16 + y[4]*8 + y[5]*4 + y[6]*2))/1;
		return y[address-1];
	}

class pixel : public BView {
	public:
	pixel(BRect rect, char *name, char color, char x, char y) : BView(rect,name,B_FOLLOW_NONE,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
		black.red = 0;
		black.blue = 0;
		black.green = 0;
		white.red = 255;
		white.blue = 255;
		white.green = 255;
		mask.red = 0;
		mask.green = 200;
		mask.blue = 255;
		if (color == 0) {
			SetViewColor(white);
		} else {
			if (color == 1)
				SetViewColor(black);
			if (color == 2)
				SetViewColor(mask);
		}
		state = color;
		locx = x;
		locy = y;
		Invalidate();
	}
	void MouseDown(BPoint point);
	void Draw(BRect updateRect);
	char state;	
	char locx;
	char locy;
	rgb_color black;
	rgb_color white;
	rgb_color mask;
};

class cursor : public BView {
	public:
	cursor(unsigned char *data, char x, char y) : BView(BRect(4,22,260,278),"cursor",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {
		int row = 0;
		X = new BPicture;
		int byte = 0;
		int col = 0;
		for (int i = 0;!((i == 16) && (row == 15)); byte++) {
			if (i == 16) {
				row++;
				i = 0;
			}
			col = bitat(*(data + byte), 1);
			if (bitat(*(data + byte + 32), 1) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 2);
			if (bitat(*(data + byte + 32), 2) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 3);
			if (bitat(*(data + byte + 32), 3) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 4);
			if (bitat(*(data + byte + 32), 4) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 5);
			if (bitat(*(data + byte + 32), 5) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 6);
			if (bitat(*(data + byte + 32), 6) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 7);
			if (bitat(*(data + byte + 32), 7) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 8);
			if (bitat(*(data + byte + 32), 8) == 0)
				col = 2;
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*16) + i)->Invalidate();
			i++;
		}
		//Invalidate();
		hoty = y;
		hotx = x;
	}
	~cursor() {
		int32 x = CountChildren();
		BView *view;
		for (int i = 0; i < x; i++) {
			view = ChildAt(i);
			if (view == NULL)
				break;
			view->RemoveSelf();
			delete view;
		}
	}
	char hoty;
	char hotx;
	BPicture *X;
};

void pixel::MouseDown(BPoint point) {
		if (((BRadioButton *)(Window()->FindView("Hot")))->Value() == true) {
			((cursor *)(Parent()))->hotx = locx;
			((cursor *)(Parent()))->hoty = locy;
			for (int i = 0; i < 256; i++) {
				Parent()->ChildAt(i)->Invalidate();
			}
			DrawPicture(((cursor *)(Parent()))->X);
		} else {
		if (((BRadioButton *)(Window()->FindView("Mask")))->Value() == true) {
			if (state != 2) {
				SetViewColor(mask);
				state = 2;
			} else {
				SetViewColor(white);
				state = 0;
			}
		} else {
		if (state != 1) {
			SetViewColor(black);
			state = 1;
		} else {
			SetViewColor(white);
			state = 0;
		}
		}
		}
		Invalidate();
	}

void pixel::Draw(BRect updateRect) {
		if (((cursor *)(Parent()))->hotx == locx) {
		if (((cursor *)(Parent()))->hoty == locy) {
			DrawPicture(((cursor *)(Parent()))->X);
		}}
		if (state == 0) {
			SetViewColor(white);
		} else {
			if (state == 1)
				SetViewColor(black);
			if (state == 2)
				SetViewColor(mask);
		}
	}

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Cursor";

void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	bkgview->Window()->ResizeBy(65,0);
	if (length == 0) {
		unsigned char *bla = new unsigned char[70];
		for (int i = 0;i < 32; i++) {
			bla[i] = 0;
		}
		for (int i = 32;i < 64; i++) {
			bla[i] = 0;
		}
		bkgview->AddChild(new cursor(bla,0,0));
	} else {
		bkgview->AddChild(new cursor(data + 4,*(data + 3),*(data + 2)));
	}
	cursor *cur = ((cursor *)(bkgview->FindView("cursor")));
	bkgview->AddChild(new BRadioButton(BRect(265,20,365,40), "Color", "Color Change", new BMessage(-200)));
	((BRadioButton *)(bkgview->FindView("Color")))->SetValue(true);
	bkgview->AddChild(new BRadioButton(BRect(265,40,365,60), "Hot", "Choose Hot Spot", new BMessage(-200)));
	bkgview->AddChild(new BRadioButton(BRect(265,60,365,80), "Mask", "Define Mask", new BMessage(-200)));
	BPicture *X = cur->X;
	cur->BeginPicture(X);
	cur->SetHighColor(127,127,127);
	cur->SetPenSize(4);
	cur->StrokeLine(BPoint(0,0),BPoint(16,16));
	cur->StrokeLine(BPoint(0,16),BPoint(16,0));
	cur->EndPicture();
	cur->Invalidate();
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	unsigned char *bla = new unsigned char[68];
	cursor *c = (cursor *)(bkgview->FindView("cursor"));
	bla[0] = 16;
	bla[1] = B_GRAY1;
	bla[2] = c->hoty;
	bla[3] = c->hotx;
	uint8 byte[8];
	int spot = 0;
	for (int i = 0;i < 256;spot++) {
		byte[0] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[0] == 2)
			byte[0] = 0;
		i++;
		byte[1] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[1] == 2)
			byte[1] = 0;
		i++;
		byte[2] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[2] == 2)
			byte[2] = 0;
		i++;
		byte[3] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[3] == 2)
			byte[3] = 0;
		i++;
		byte[4] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[4] == 2)
			byte[4] = 0;
		i++;
		byte[5] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[5] == 2)
			byte[5] = 0;
		i++;
		byte[6] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[6] == 2)
			byte[6] = 0;
		i++;
		byte[7] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[7] == 2)
			byte[7] = 0;
		i++;
		bla[spot + 4] = deb2(byte);
	}
		for (int i = 0;i < 256;spot++) {
		byte[0] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[0] == 2)
			byte[0] = 0;
		else
			byte[0] = 1;
		i++;
		byte[1] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[1] == 2)
			byte[1] = 0;
		else
			byte[1] = 1;
		i++;
		byte[2] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[2] == 2)
			byte[2] = 0;
		else
			byte[2] = 1;
		i++;
		byte[3] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[3] == 2)
			byte[3] = 0;
		else
			byte[3] = 1;
		i++;
		byte[4] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[4] == 2)
			byte[4] = 0;
		else
			byte[4] = 1;
		i++;
		byte[5] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[5] == 2)
			byte[5] = 0;
		else
			byte[5] = 1;
		i++;
		byte[6] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[6] == 2)
			byte[6] = 0;
		else
			byte[6] = 1;
		i++;
		byte[7] = ((pixel *)(c->ChildAt(i)))->state;
		if (byte[7] == 2)
			byte[7] = 0;
		else
			byte[7] = 1;
		i++;
		bla[spot + 4] = deb2(byte);
	}
	*length = 68;
	return bla;
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}