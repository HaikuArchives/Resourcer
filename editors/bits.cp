//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <string.h>
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <interface/Button.h>
#include <storage/FilePanel.h>
#include <storage/Entry.h>
#include <storage/File.h>
#include <support/DataIO.h>
#include <translation/TranslatorRoster.h>
#include <translation/TranslationUtils.h>
#include <translation/BitmapStream.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Translator Bitmap";

BRect find_center(uint32 width, uint32 height, BRect a);
class BPView;
void processTIFF(BMessage *TIFF, BPView *view);
void loadopenbox(BPView *view);

BRect find_center(uint32 width, uint32 height, BRect a) {
	uint32 screen_width = a.right - a.left;
	uint32 screen_height = a.bottom - a.top;
	uint32 c = ((screen_width / 2) - (width / 2)) + a.left;
	uint32 d = ((screen_height / 2) - (height / 2)) + a.top;
	BRect toreturn;
	toreturn.left = c;
	toreturn.top = d;
	toreturn.right = c + width;
	toreturn.bottom = d + height;
	return toreturn;
	}

class TIFFilter : public BRefFilter {
	bool Filter(const entry_ref *ref, BNode *node, struct stat *st, const char *filetype) {
		if (BEntry(ref,true).IsDirectory())
			return true;
		if (BEntry(ref,true).IsFile()) {
			BFile file(ref,B_READ_ONLY);
			BTranslatorRoster *roster = BTranslatorRoster::Default();
			translator_info check;
			if ((((((filetype[0] == 'i') && (filetype[1] == 'm')) && (filetype[2] == 'a')) && (filetype[3] == 'g')) && (filetype[4] == 'e'))) {
			if (roster->Identify(&file,NULL,&check) == B_OK) {
				if (check.capability != 0) {
					return true;
				}
			}
			}
		}
		return false;
	}
};

class BPView : public BView {
	public:
	BPView (BRect frame, const char *name, uint32 resizingMode, uint32 flags,BView *parenta,BBitmap *imagea) : BView(frame, name,resizingMode,flags) {
		image = imagea;
		parent = parenta;
		opbut = new BButton(BRect(0,0,80,25),"opbut","Open...",new BMessage(-200));
		 if (image == NULL) {
		 	BRect a = find_center(80,25,BRect(frame.left,frame.bottom,frame.right,frame.bottom + 50));
		 	opbut->MoveTo(a.left,a.top);
		 }
		parent->AddChild(this);
		parent->AddChild(opbut);
		change_image(imagea);
		openpanel = new BFilePanel(B_OPEN_PANEL,new BMessenger(parent),NULL,B_FILE_NODE,false,NULL,new TIFFilter,false,true);
	}
	
	void Draw(BRect updateRect) {
		Window()->Lock();
		if (image != NULL) {
			DrawBitmap(image);
		}
		Window()->Unlock();
	}
	
	void change_image(BBitmap *bitmap) {
		image = bitmap;
		if (Window() != NULL)
			Window()->Lock();
		if (image != NULL) {
			DrawBitmap(image);
			Window()->ResizeTo(bitmap->Bounds().Width(),bitmap->Bounds().Height() + 50);
			ResizeTo(bitmap->Bounds().Width(),bitmap->Bounds().Height());
			BRect area = find_center(80,25,BRect(0,bitmap->Bounds().Height(),bitmap->Bounds().Width(),bitmap->Bounds().Height() + 50));
			opbut->MoveTo(area.left,area.top);
		}
		if (Window() != NULL)
			Window()->Unlock();
	}
	//---------------------Data------------------------
	BBitmap *image;
	BView *parent;
	BButton *opbut;
	BFilePanel *openpanel;
	};

void processTIFF(BMessage *TIFF, BPView *view) {
	entry_ref ref;
	TIFF->FindRef("refs",&ref);
	BFile file(&ref,B_READ_ONLY);
	BBitmap *bitmap = BTranslationUtils::GetBitmap(&file);
	view->change_image(bitmap);
}

void loadopenbox(BPView *view) {
	view->openpanel->Show();
}

void loaddata(unsigned char *data,size_t length,BView *bkgview) { //runs when editor launched, window rect is 300 x 300 pixels
	BMemoryIO *io = new BMemoryIO(data,length);
	BBitmap *bitmap = BTranslationUtils::GetBitmap(io);
	delete io;
	BPView *view = new BPView(BRect(0,0,300,250),"view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS,bkgview,bitmap);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //runs when window closes, passes back window address given in loaddata()
	BPView *x = (BPView *)(bkgview->ChildAt(0));
	unsigned char *data;
	if (x->image == NULL) {
		*length = 0;
		data = new unsigned char(0);
		return data;
	}
	BBitmapStream *stream = new BBitmapStream(x->image);
	stream->Seek(0,SEEK_END);
	*length = stream->Position();
	data = new unsigned char[*length];
	stream->Seek(0,SEEK_SET);
	stream->Read(data,*length);
	stream->DetachBitmap(&(x->image));
	size_t head_length = *length - x->image->BitsLength();
	memcpy(data + head_length,x->image->Bits(),*length - head_length);
	((BPView *)(bkgview->ChildAt(0)))->openpanel->Window()->PostMessage(B_QUIT_REQUESTED);
	delete ((BPView *)(bkgview->ChildAt(0)))->openpanel;
	/*BBitmapStream *s2 = new BBitmapStream;
	s2->Write(data,*length);
	s2->DetachBitmap(&x->image);
	bkgview->Window()->Show();
	x->Invalidate(x->Bounds());
	(new BAlert("ck","Check...","OK"))->Go();*/
	delete stream;
	return data;
}

void messaging(BMessage *message,BView *bkgview) { //BMessages from window, to be passed here 'what' field must be a negative number
	switch (message->what) {
		case -200:
			loadopenbox((BPView *)(bkgview->ChildAt(0)));
			break;
		case B_REFS_RECEIVED:
			processTIFF(message,(BPView *)(bkgview->ChildAt(0)));
			break;
	}
}