

class _EXPORT ResImageView : public BView {
	public:
		ResImageView(void) : BView(BRect(0,0,100,100),"newpictureview",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
			bitmap = NULL;
			resid = 112;
		}
		ResImageView(BRect rect,int32 id,char *viewname) : BView(rect,viewname,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
			BResources *res = file;
			resid = id;
			char *namefound;
			size_t length;
			res->GetResourceInfo('bits',resid,&namefound,&length);
			BMemoryIO io(res->LoadResource('bits',resid,&length),length);
			bitmap = BTranslationUtils::GetBitmap(&io);
		}
		ResImageView(BRect rect,char *name) : BView(rect,name,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
			BResources *res = file;
			size_t length;
			res->GetResourceInfo('bits',name,&resid,&length);
			BMemoryIO io(res->LoadResource('bits',resid,&length),length);
			bitmap = BTranslationUtils::GetBitmap(&io);
		}
		ResImageView(BMessage *archive) : BView(archive) {
			BResources *res = file;
			archive->FindInt32("resid",&resid);
			char *namefound;
			size_t length;
			if (res->HasResource('bits',resid) == false) {
				bitmap = NULL;
				return;
			}
			res->GetResourceInfo('bits',resid,&namefound,&length);
			BMemoryIO io(res->LoadResource('bits',resid,&length),length);
			bitmap = BTranslationUtils::GetBitmap(&io);
			Draw(Bounds());
		}
		_EXPORT BArchivable *Instantiate(BMessage *archive);
		void SetTo(int32 id,bool changesize = true) {
			BResources *res = file;
			resid = id;
			char *namefound;
			size_t length;
			res->GetResourceInfo('bits',resid,&namefound,&length);
			BMemoryIO io(res->LoadResource('bits',resid,&length),length);
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
		void SetTo(char *name,bool changesize = true) {
			BResources *res = file;
			size_t length;
			res->GetResourceInfo('bits',name,&resid,&length);
			BMemoryIO io(res->LoadResource('bits',resid,&length),length);
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
		void Draw(BRect area) {
			if (bitmap != NULL)
				DrawBitmap(bitmap);
			else {
				MovePenTo(BPoint(10,20));
				DrawString("No Bitmap Set");
			}
			BView::Draw(area);
		}
		_EXPORT virtual status_t Archive(BMessage *archive, bool deep = true) const;
		int32 ResourceID(void) {
			return resid;
		} 
	private:
		BBitmap *bitmap;
		int32 resid;
};

status_t ResImageView::Archive(BMessage *archive, bool deep) const{
	status_t stat = BView::Archive(archive,deep);
	if (stat != B_OK)
		return stat;
	archive->AddString("class", "ResImageView");
	archive->AddInt32("resid",resid);
	archive->AddString("add-on","application/x-vnd.NW-resourcer-WIND");
	return B_OK;
}

BArchivable *ResImageView::Instantiate(BMessage *archive) {
	if ((validate_instantiation(archive,"ResImageView")))
		return (new ResImageView(archive));
	return NULL;
}
