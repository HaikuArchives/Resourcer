// extracts a BBitmap from a Resource

//IMPORTANT!
//Requires libtranslation.so

#include <translation/TranslationUtils.h>
#include <interface/Bitmap.h>
#include <interface/View.h>
#include <support/DataIO.h>
#include <interface/Window.h>

BBitmap *get_bitmap(long id);
BBitmap *get_bitmap(const char *name);

BBitmap *get_bitmap(long id) {
	return BTranslationUtils::GetBitmap(B_TRANSLATOR_BITMAP,id);
}

BBitmap *get_bitmap(const char *name) {
	return BTranslationUtils::GetBitmap(B_TRANSLATOR_BITMAP,name);
}

class _EXPORT ResImageView : public BView {
	public:
		ResImageView(BResources *res = be_app->AppResources());
		ResImageView(BRect rect,int32 id,char *viewname,BResources *res = be_app->AppResources());
		ResImageView(BRect rect,char *name,BResources *res = be_app->AppResources());
		ResImageView(BMessage *archive);
		void SetTo(int32 id,bool changesize = true);
		void SetTo(char *name,bool changesize = true);
		void Draw(BRect area);
		void InitBitmap(void);
		virtual	status_t	Archive(BMessage *into, bool deep = true) const;
		static	BArchivable	*Instantiate(BMessage *from);
		int32 ResourceID(void);
		~ResImageView(void);
	private:
		BBitmap *bitmap;
		int32 resid;
		BResources *res;
};

ResImageView::ResImageView(BResources *res) : BView(BRect(0,0,100,100),"newpictureview",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	bitmap = NULL;
	resid = 0;
}

ResImageView::ResImageView(BRect rect,int32 id,char *viewname,BResources *res) : BView(rect,viewname,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	resid = id;
	ResourceIO io('bits',id,res);
	bitmap = BTranslationUtils::GetBitmap(&io);
}

ResImageView::ResImageView(BRect rect,char *name,BResources *res) : BView(rect,name,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	ResourceIO io('bits',name,res);
	resid = io.ID();
	bitmap = BTranslationUtils::GetBitmap(&io);
}

ResImageView::ResImageView(BMessage *archive) : BView(archive) {
	archive->FindInt32("resid",&resid);
	res = be_app->AppResources();
	InitBitmap();
}

ResImageView::~ResImageView(void) {
	if (bitmap != NULL)
		delete bitmap;
}

void ResImageView::SetTo(int32 id,bool changesize) {
	resid = id;
	InitBitmap();
	if (changesize) {
		if (Window() != NULL)
			Window()->Lock();
		BPoint newsize = bitmap->Bounds().RightBottom();
		ResizeTo(newsize.x,newsize.y);
		if (Window() != NULL) {
			if (Window()->IsLocked())
				Window()->Unlock();
		}
	}
	Invalidate(Bounds());
}

void ResImageView::InitBitmap(void) {
	ResourceIO io('bits',resid, res);
	bitmap = BTranslationUtils::GetBitmap(&io);
	Invalidate(Bounds());
}

void ResImageView::SetTo(char *name,bool changesize) {
	ResourceIO io('bits',name,res);
	resid = io.ID();
	bitmap = BTranslationUtils::GetBitmap(&io);
	if (changesize) {
		if (Window() != NULL)
			Window()->Lock();
		BPoint newsize = bitmap->Bounds().RightBottom();
		ResizeTo(newsize.x,newsize.y);
		if (Window() != NULL) {
			if (Window()->IsLocked())
				Window()->Unlock();
		}
	}
	Invalidate(Bounds());
}

void ResImageView::Draw(BRect area) {
	if (bitmap == NULL) {
		AttachedToWindow();
	}
	if (bitmap != NULL)
		DrawBitmap(bitmap);
	else {
		MovePenTo(BPoint(10,20));
		DrawString("No Bitmap Set");
	}
	BView::Draw(area);
}

int32 ResImageView::ResourceID(void) {
	return resid;
} 

status_t ResImageView::Archive(BMessage *archive, bool deep) const{
	status_t stat = BView::Archive(archive,deep);
	if (stat != B_OK)
		return stat;
	archive->AddString("class", "ResImageView");
	archive->AddInt32("resid",resid);
	return B_OK;
}

BArchivable *ResImageView::Instantiate(BMessage *archive) {
	if ((validate_instantiation(archive,"ResImageView")))
		return (new ResImageView(archive));
	return NULL;
}