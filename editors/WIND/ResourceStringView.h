//------------------------------------------------------------------------------
//	ResourceStringView.h
//
//------------------------------------------------------------------------------

#ifndef RESOURCESTRINGVIEW_H
#define RESOURCESTRINGVIEW_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/StringView.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
class _EXPORT ResourceStringView : public BStringView
{
	public:
		ResourceStringView(void);
		ResourceStringView(BMessage *msg);
		char *GetResString(void);
		void SetTo(int32 id);
		void Draw(BRect rect);
		int32 ResourceID(void);
		void AttachedToWindow(void);
		virtual	status_t Archive(BMessage *into, bool deep = true) const;
		static	BArchivable	*Instantiate(BMessage *from);

	private:
		BResources *res;
		int32 id;
};
//------------------------------------------------------------------------------

#endif	// RESOURCESTRINGVIEW_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

