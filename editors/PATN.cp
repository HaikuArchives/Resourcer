//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <interface/View.h>
#include <stdio.h>
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
		uint8 y[8]; 
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
		if (color == 0) {
			SetViewColor(white);
		} else {
			SetViewColor(black);
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
};

class cursor : public BView {
	public:
	cursor(unsigned char *data, char x, char y) : BView(BRect(4,4,132,132),"cursor",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {
		int row = 0;
		int byte = 0;
		int col = 0;
		for (int i = 0;row < 8; byte++) {
			col = bitat(*(data + byte), 1);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 2);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 3);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 4);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 5);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 6);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 7);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i++;
			col = bitat(*(data + byte), 8);
			AddChild(new pixel(BRect(i*16,row*16,(i*16)+16,(row*16)+16),NULL,col,i,row));
			ChildAt((row*8) + i)->Invalidate();
			i = 0;
			row++;
		}
		//Invalidate();
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
};

void pixel::MouseDown(BPoint point) {
		if (state != 1) {
			SetViewColor(black);
			state = 1;
		} else {
			SetViewColor(white);
			state = 0;
		}
		Invalidate();
	}

void pixel::Draw(BRect updateRect) {
		if (state == 0) {
			SetViewColor(white);
		} else {
			SetViewColor(black);
		}
	}

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "BView Pattern";

void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	bkgview->Window()->ResizeTo(136,136);
	if (length == 0) {
		pattern *bla = new pattern;
		for (int i = 0;i < 8; i++) {
			bla->data[i] = 0;
		}
		bkgview->AddChild(new cursor(bla->data,0,0));
	} else {
		bkgview->AddChild(new cursor(((pattern *)(data))->data,0,0));
	}
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	pattern *toreturn = new pattern;
	cursor *c = (cursor *)(bkgview->FindView("cursor"));
	int spot = 0;
	uchar byte[8];
	for (int i = 0;i < 64;spot++) {
		byte[0] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[1] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[2] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[3] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[4] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[5] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[6] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		byte[7] = ((pixel *)(c->ChildAt(i)))->state;
		i++;
		toreturn->data[spot] = deb2(byte);
	}
	*length = sizeof(pattern);
	return (unsigned char *)(toreturn);
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
}