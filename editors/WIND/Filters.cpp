//------------------------------------------------------------------------------
//	Filters.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/AppDefs.h>
#include <app/Message.h>

#include <interface/TextView.h>
#include <interface/View.h>
#include <interface/Window.h>

#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "EditingControl.h"
#include "EditingWindow.h"
#include "Filters.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
DeleteFilter::DeleteFilter(EditingWindow* EditingWin)
	:	BMessageFilter(B_KEY_DOWN),
		fEditingWin(EditingWin)
{
}
//------------------------------------------------------------------------------
filter_result DeleteFilter::Filter(BMessage* message, BHandler** target)
{
	char *name;
	message->FindString("bytes", (const char**)&name);
	if (name == NULL)
		return B_DISPATCH_MESSAGE;

	if ((*name == B_BACKSPACE) &&
		(is_kind_of(fEditingWin->CurrentFocus(), BTextView)))
	{
		return B_DISPATCH_MESSAGE;
	}

	if ((*name == B_DELETE) || (*name == B_BACKSPACE))
	{
		*target = fEditingWin->cntrl;
	}

	return B_DISPATCH_MESSAGE;
}
//------------------------------------------------------------------------------
ActivateFilter::ActivateFilter(EditingWindow* EditingWin)
	:	BMessageFilter(B_MOUSE_DOWN),
		fEditingWin(EditingWin)
{
}
//------------------------------------------------------------------------------
filter_result ActivateFilter::Filter(BMessage* /*message*/, BHandler** target)
{
	if ((*target != fEditingWin->cntrl) &&
		(*target != fEditingWin->cntrl->controlled))
	{
		((BView *)(*target))->MakeFocus();
		fEditingWin->cntrl->Pulse();
		return B_SKIP_MESSAGE;
	}
	return B_DISPATCH_MESSAGE;
}
//------------------------------------------------------------------------------
DropFilter::DropFilter()
	:	BMessageFilter(B_DROPPED_DELIVERY, B_ANY_SOURCE)
{
}
//------------------------------------------------------------------------------
filter_result DropFilter::Filter(BMessage* msg,BHandler** /*target*/)
{
	BMessage *message = new BMessage(*msg);
	BPoint point;
	point = ((BWindow *)(Looper()))->ConvertFromScreen(message->DropPoint());
	if (message == NULL)
		return B_DISPATCH_MESSAGE;

	BView *controlled;
	// TODO: add some real error handling
	image_id me;
	controlled = cast_as(instantiate_object(message, &me), BView);
	if (controlled == NULL)
		return B_DISPATCH_MESSAGE;
	controlled->MoveTo(point);
	((BWindow *)(Looper()))->AddChild(controlled);
	BRect empty = controlled->ConvertToScreen(controlled->Bounds());
	empty = ((BWindow *)(Looper()))->ConvertFromScreen(empty);
	BPoint empt = empty.LeftTop();
	empt.x -= 1;
	empt.y -= 1;
	BView *posparent = ((BWindow *)(Looper()))->FindView(empt);
	if (posparent != NULL)
	{
		if (posparent->ConvertToScreen(posparent->Bounds()).Contains(controlled->ConvertToScreen(controlled->Bounds())))
		{
			BRect wait = controlled->ConvertToScreen(controlled->Bounds());
			wait = posparent->ConvertFromScreen(wait);
			controlled->RemoveSelf();
			controlled->MoveTo(wait.LeftTop());
			posparent->AddChild(controlled);
		}
	}
	return B_SKIP_MESSAGE;
}
//------------------------------------------------------------------------------


/*
 * $Log $
 *
 * $Id  $
 *
 */

