//------------------------------------------------------------------------------
//	BStringViewEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/RadioButton.h>
#include <interface/StringView.h>
#include <interface/TextControl.h>
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BStringViewEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BStringViewEditor::BStringViewEditor()
	:	fLabelText(NULL),
		fLeftAlign(NULL),
		fCenterAlign(NULL),
		fRightAlign(NULL)
{
}
//------------------------------------------------------------------------------
BStringViewEditor::~BStringViewEditor()
{
}
//------------------------------------------------------------------------------
void BStringViewEditor::SetTo(BMessage* archive)
{
	BViewEditor::SetTo(archive);
	Init();

	BString Label;
	archive->FindString("_text", &Label);

	alignment align;
	if (archive->FindInt32("_align", (int32*)&align))
	{
		debugger("Couldn't find _align in archive");
	}

	fLabelText->SetText(Label.String());
	SetButtons(align);

	fLabelText->SetTarget(Parent());
	fLeftAlign->SetTarget(Parent());
	fCenterAlign->SetTarget(Parent());
	fRightAlign->SetTarget(Parent());

	Parent()->AddChild(fLabelText);
	Parent()->AddChild(fLeftAlign);
	Parent()->AddChild(fCenterAlign);
	Parent()->AddChild(fRightAlign);
}
//------------------------------------------------------------------------------
bool BStringViewEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_STRING_VIEW_SET_TEXT:
			Target()->SetText(fLabelText->Text());
			return true;

		case MSG_STRING_VIEW_ALIGN_LEFT:
			Target()->SetAlignment(B_ALIGN_LEFT);
			return true;

		case MSG_STRING_VIEW_ALIGN_CENTER:
			Target()->SetAlignment(B_ALIGN_CENTER);
			return true;

		case MSG_STRING_VIEW_ALIGN_RIGHT:
			Target()->SetAlignment(B_ALIGN_RIGHT);
			return true;

		default:
			return BViewEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BStringViewEditor::Supports()
{
	return "BStringView";
}
//------------------------------------------------------------------------------
void BStringViewEditor::Init()
{
	int bottom = Bottom();
	if (!fLabelText)
	{
		fLabelText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "label", "Label: ", "",
									  new BMessage(MSG_STRING_VIEW_SET_TEXT));
		fLabelText->SetDivider(be_plain_font->StringWidth("Label: "));
		AddControl(fLabelText);
	}

	if (!fLeftAlign)
	{
		bottom = Bottom();
		fLeftAlign = new BRadioButton(BRect(10, bottom, 190, bottom + 20),
									  "left", "Left Alignment",
									  new BMessage(MSG_STRING_VIEW_ALIGN_LEFT));
		AddControl(fLeftAlign);
	}

	if (!fCenterAlign)
	{
		bottom = Bottom();
		fCenterAlign = new BRadioButton(BRect(10, bottom, 190, bottom + 20),
									  "center", "Center Alignment",
									  new BMessage(MSG_STRING_VIEW_ALIGN_CENTER));
		AddControl(fCenterAlign);
	}

	if (!fRightAlign)
	{
		bottom = Bottom();
		fRightAlign = new BRadioButton(BRect(10, bottom, 190, bottom + 20),
									  "right", "Right Alignment",
									  new BMessage(MSG_STRING_VIEW_ALIGN_RIGHT));
		AddControl(fRightAlign);
	}

	if (Target())
	{
		SetButtons(Target()->Alignment());
	}
}
//------------------------------------------------------------------------------
BStringView* BStringViewEditor::Target()
{
	return dynamic_cast<BStringView*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------
void BStringViewEditor::SetButtons(int32 align)
{
	fLeftAlign->SetValue(align == B_ALIGN_LEFT);
	fCenterAlign->SetValue(align == B_ALIGN_CENTER);
	fRightAlign->SetValue(align == B_ALIGN_RIGHT);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

