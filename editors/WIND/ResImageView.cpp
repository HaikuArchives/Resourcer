//------------------------------------------------------------------------------
//	ResImageView.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/Bitmap.h>
#include <interface/Window.h>
#include <support/ClassInfo.h>
#include <translation/TranslationUtils.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "EditingWindow.h"
#include "ResImageView.h"
#include "ResourceIO.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
ResImageView::ResImageView(BResources *res) : BView(BRect(0,0,100,100),"newpictureview",B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	bitmap = NULL;
	resid = 0;
}
//------------------------------------------------------------------------------
ResImageView::ResImageView(BRect rect,int32 id,char *viewname,BResources *res) : BView(rect,viewname,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	resid = id;
	ResourceIO io('bits',id,res);
	bitmap = BTranslationUtils::GetBitmap(&io);
}
//------------------------------------------------------------------------------
ResImageView::ResImageView(BRect rect,char *name,BResources *res) : BView(rect,name,B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS) {
	this->res = res;
	ResourceIO io('bits',name,res);
	resid = io.ID();
	bitmap = BTranslationUtils::GetBitmap(&io);
}
//------------------------------------------------------------------------------
ResImageView::ResImageView(BMessage *archive) : BView(archive) {
	archive->FindInt32("resid",&resid);
	bitmap = NULL;
}
//------------------------------------------------------------------------------
void ResImageView::SetTo(int32 id,bool changesize) {
	resid = id;
	ResourceIO io('bits',id,res);
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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void ResImageView::Draw(BRect /*area*/) {
	if (bitmap == NULL) {
		AttachedToWindow();
	}
	if (bitmap != NULL)
		DrawBitmap(bitmap);
	else {
		MovePenTo(BPoint(10,20));
		DrawString("No Bitmap Set");
	}
}
//------------------------------------------------------------------------------
int32 ResImageView::ResourceID(void) {
	return resid;
} 
//------------------------------------------------------------------------------
status_t ResImageView::Archive(BMessage *archive, bool deep) const{
	status_t stat = BView::Archive(archive,deep);
	if (stat != B_OK)
		return stat;
	archive->AddString("class", "ResImageView");
	archive->AddInt32("resid",resid);
	return B_OK;
}
//------------------------------------------------------------------------------
BArchivable *ResImageView::Instantiate(BMessage *archive) {
	if ((validate_instantiation(archive,"ResImageView")))
		return (new ResImageView(archive));
	return NULL;
}
//------------------------------------------------------------------------------
void ResImageView::AttachedToWindow(void) {
	if (is_kind_of(Window(),EditingWindow)) {
		res = ((EditingWindow *)(Window()))->reso;
		ResourceIO io('bits',resid, res);
		bitmap = BTranslationUtils::GetBitmap(&io);
		if (bitmap != NULL)
			Invalidate(Bounds());
	} else {
		bitmap = NULL;
	}
	BView::AttachedToWindow();
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

