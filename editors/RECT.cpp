//------------------------------------------------------------------------------
//	RECT.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdlib.h>

// System Includes -------------------------------------------------------------
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/TextControl.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char*, size_t, BView*);
extern "C" _EXPORT unsigned char* savedata(size_t*, BView*);
extern "C" _EXPORT void messaging(BMessage*, BView*);
extern "C" _EXPORT const char description[] = "BRect";


//------------------------------------------------------------------------------
//	Runs when add-on starts, window is initialized, you are expected
//	to write to it: rect is 300 x 300
void loaddata(unsigned char* data, size_t length, BView* bkgview)
{
	BRect rect;
	if (length == sizeof (rect.left) * 4)
	{
		float* rdata = (float*)data;
		rect.left	= rdata[0];
		rect.top	= rdata[1];
		rect.right	= rdata[2];
		rect.bottom	= rdata[3];
	}

	BString sdata;
	int top = 5;
	BTextControl* v;
	BRect frame(10, 5, 280, 25);
	sdata << rect.left;
	v = new BTextControl(frame, "BRect::left", "Left: ", sdata.String(), NULL);
	v->SetDivider(be_plain_font->StringWidth("Bottom: "));
	bkgview->AddChild(v);

	frame.OffsetBy(0, 20);
	sdata = "";
	sdata << rect.top;
	v = new BTextControl(frame, "BRect::top", "Top: ", sdata.String(), NULL);
	v->SetDivider(be_plain_font->StringWidth("Bottom: "));
	bkgview->AddChild(v);

	frame.OffsetBy(0, 20);
	sdata = "";
	sdata << rect.right;
	v = new BTextControl(frame, "BRect::right", "Right: ", sdata.String(), NULL);
	v->SetDivider(be_plain_font->StringWidth("Bottom: "));
	bkgview->AddChild(v);

	frame.OffsetBy(0, 20);
	sdata = "";
	sdata << rect.bottom;
	v = new BTextControl(frame, "BRect::bottom",
						 "Bottom: ", sdata.String(), NULL);
	v->SetDivider(be_plain_font->StringWidth("Bottom: "));
	bkgview->AddChild(v);

	bkgview->Window()->ResizeTo(300, frame.bottom + 5);
}
//------------------------------------------------------------------------------
//	Return data, clean up, and set length to the size of data
unsigned char* savedata(size_t* length, BView* bkgview)
{
	BRect rect;
	BTextControl* text;

	text = (BTextControl*)(bkgview->ChildAt(0));
	rect.left = atof(text->Text());

	text = (BTextControl*)(bkgview->ChildAt(1));
	rect.top = atof(text->Text());

	text = (BTextControl*)(bkgview->ChildAt(2));
	rect.right = atof(text->Text());

	text = (BTextControl*)(bkgview->ChildAt(3));
	rect.bottom = atof(text->Text());

	*length = sizeof (rect.left) * 4;
	float* data = new float[4];
	data[0] = rect.left;
	data[1] = rect.top;
	data[2] = rect.right;
	data[3] = rect.bottom;

	return (unsigned char*)data;
}
//------------------------------------------------------------------------------
//	Receives messages from window with negative 'what' members or
//	B_REFS_RECEIVED messages
void messaging(BMessage* message, BView* bkgview)
{
}

/*
 * $Log $
 *
 * $Id  $
 *
 */

