//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/Menu.h>
#include <interface/MenuBar.h>
#include <interface/Alert.h>
#include <interface/MenuItem.h>
#include <interface/StringView.h>
#include <media/MediaTheme.h>
#include <media/MediaFormats.h>
#include <media/MediaDefs.h>
#include <media/MediaFile.h>
#include <media/MediaTrack.h>
#include <storage/FilePanel.h>
#include <storage/Mime.h>
#include "MediaView.h"
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Movie";

MediaView *v;
BFilePanel *peterpan;

class VideoFilter : public BRefFilter {
	bool Filter(const entry_ref *ref,BNode *node, struct stat *st, const char *filetype) {
		BMimeType type(filetype);
		if (BMimeType("video").Contains(&type))
			return true;
		if (node->IsDirectory())
			return true;
		return false;
	}
};

void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	BPositionIO *movie;
	if (length == 0)
		movie = new BMallocIO;
	else {
		BMallocIO *file = new BMallocIO;
		file->Write(data,length);
		movie = file;
	}
	BMenuBar *mbar = new BMenuBar(BRect(0,0,bkgview->Bounds().right,10),"mbar");
	BMenu *file = new BMenu("File");
	file->AddItem(new BMenuItem("About...",new BMessage(-100)));
	file->AddSeparatorItem();
	file->AddItem(new BMenuItem("Open...",new BMessage(-200)));
	mbar->AddItem(file);
	bkgview->AddChild(mbar);
	BRect rect = bkgview->Bounds();
	rect.top += mbar->Bounds().Height();
	rect.bottom += mbar->Bounds().Height();
	//movie = new BFile("/boot/optional/MemorialDayUpper.avi",B_READ_WRITE);
	v = new MediaView(rect,"player",B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
	bkgview->AddChild(v);
	v->SetMediaSource(movie);
	float x,y;
	v->GetPreferredSize(&x,&y);
	bkgview->Window()->ResizeTo(x, v->Frame().bottom);
	v->MakeFocus(true);
	if (length == 0) {
		BView *view = new BView(rect,"wait",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
		view->SetViewColor(216,216,216);
		view->AddChild(new BStringView(BRect((view->Bounds().right / 2) - (be_plain_font->StringWidth("Please Open a New File.") / 2),(view->Bounds().bottom / 2) - 7,(view->Bounds().right / 2) + (be_plain_font->StringWidth("Please Open a New File.") / 2),(view->Bounds().bottom / 2) + 7),"please","Please Open a New File."));
		bkgview->AddChild(view);
	}
	peterpan = NULL;
}

unsigned char* savedata(size_t *length,BView *) {
	if (peterpan != NULL)
		peterpan->Window()->PostMessage(B_QUIT_REQUESTED);
	unsigned char *toreturn;
	*length = v->GetMovie((void **)&toreturn);
	v->Control(MEDIA_STOP);
	return toreturn;
}

void messaging(BMessage *msg,BView *bkgview) {
	entry_ref ref;
	BRect rect;
	float x,y;
	switch (msg->what) {
		case -100:
			(new BAlert("about","MOOV Editor for Resourcer 3.0\nCore Code "B_UTF8_COPYRIGHT"1991-1999, Be, Inc.","OK"))->Go();
			break;
		case -200:
			if (peterpan == NULL)
				peterpan = new BFilePanel(B_OPEN_PANEL,new BMessenger(bkgview->Window()),NULL,0,false,NULL,new VideoFilter);
			peterpan->Show();
			break;
		case B_REFS_RECEIVED:
			if (bkgview->FindView("wait") != NULL)
				bkgview->FindView("wait")->RemoveSelf();
			msg->FindRef("refs",&ref);
			v->Control(MEDIA_STOP);
			v->RemoveSelf();
			delete v;
			rect = bkgview->Bounds();
			rect.top += bkgview->Window()->KeyMenuBar()->Bounds().Height();
			rect.bottom += bkgview->Window()->KeyMenuBar()->Bounds().Height();
			v = new MediaView(rect,"player",B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE);
			bkgview->AddChild(v);
			v->SetMediaSource(new BFile(&ref,B_READ_WRITE));
			v->GetPreferredSize(&x,&y);
			bkgview->Window()->ResizeTo(x, v->Frame().bottom);
			v->MakeFocus(true);
			break;
	}
}