//------------------------------------------------------------------------------
//	ResourceStringView.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "ResourceStringView.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
ResourceStringView::ResourceStringView(void)
	:	BStringView(BRect(0,0,100,20),"newStringView","No String Set")
{
	res = NULL;
	id = 0;
}
//------------------------------------------------------------------------------
ResourceStringView::ResourceStringView(BMessage *msg)
	:	BStringView(msg)
{
	res = NULL;
	msg->FindInt32("resid",&id);
}
//------------------------------------------------------------------------------
char *ResourceStringView::GetResString(void)
{
	size_t len;
	char *tm;
	tm = (char *)res->LoadResource('CSTR',id,&len);
	char *tm2 = new char[strlen(tm) + 1];
	strcpy(tm2,tm);
	return tm2;
}
//------------------------------------------------------------------------------
void ResourceStringView::SetTo(int32 id)
{
	this->id = id;
	if (res->HasResource('CSTR',id))
		SetText(GetResString());
	else
		SetText("No String Set");
}
//------------------------------------------------------------------------------
void ResourceStringView::Draw(BRect rect)
{
	if (res == NULL)
		AttachedToWindow();
	BStringView::Draw(rect);
}
//------------------------------------------------------------------------------
int32 ResourceStringView::ResourceID(void)
{
	return int32(id);
}
//------------------------------------------------------------------------------
void ResourceStringView::AttachedToWindow(void)
{
	if (is_kind_of(Window(),EditingWindow)) {
		res = ((EditingWindow *)(Window()))->reso;
		SetTo(id);
	} else {
		SetText("No String Set");
	}
	BView::AttachedToWindow();
}
//------------------------------------------------------------------------------
BArchivable	*ResourceStringView::Instantiate(BMessage *from)
{
	if ((validate_instantiation(from,"ResourceStringView")))
		return (new ResourceStringView(from));
	return NULL;
}
//------------------------------------------------------------------------------
status_t ResourceStringView::Archive(BMessage *into, bool deep) const
{
	status_t stat = BStringView::Archive(into,deep);
	into->AddString("class", "ResourceStringView");
	into->AddInt32("resid",id);
	return stat;
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

