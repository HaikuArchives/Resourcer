//------------------------------------------------------------------------------
//	ResImageView.h
//
//------------------------------------------------------------------------------

#ifndef RESIMAGEVIEW_H
#define RESIMAGEVIEW_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Application.h>
#include <interface/View.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
class _EXPORT ResImageView : public BView {
	public:
		ResImageView(BResources *res = be_app->AppResources());
		ResImageView(BRect rect,int32 id,char *viewname,BResources *res = be_app->AppResources());
		ResImageView(BRect rect,char *name,BResources *res = be_app->AppResources());
		ResImageView(BMessage *archive);

		virtual	status_t	Archive(BMessage *into, bool deep = true) const;
		static	BArchivable	*Instantiate(BMessage *from);
		void AttachedToWindow(void);

		void SetTo(int32 id,bool changesize = true);
		void SetTo(char *name,bool changesize = true);
		void Draw(BRect area);
		int32 ResourceID(void);

	private:
		BBitmap *bitmap;
		int32 resid;
		BResources *res;
};
//------------------------------------------------------------------------------

#endif	// RESIMAGEVIEW_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

