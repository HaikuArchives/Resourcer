//------------------------------------------------------------------------------
//	BBoxEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/Box.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TextControl.h>
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BBoxEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BBoxEditor::BBoxEditor()
	:	fLabelText(NULL),
		fBorderStyle(NULL)
{
}
//------------------------------------------------------------------------------
BBoxEditor::~BBoxEditor()
{
}
//------------------------------------------------------------------------------
void BBoxEditor::SetTo(BMessage *archive)
{
	BViewEditor::SetTo(archive);

	Init();

	BString Label;
	archive->FindString("_label", &Label);

	border_style style;
	if (archive->FindInt32("_style", (int32*)&style))
		style = B_FANCY_BORDER;

	fLabelText->SetText(Label.String());
	SetStyle(style);

	fLabelText->SetTarget(Parent());
	fBorderStyle->Menu()->SetTargetForItems(Parent());

	Parent()->AddChild(fLabelText);
	Parent()->AddChild(fBorderStyle);
}
//------------------------------------------------------------------------------
bool BBoxEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case MSG_BOX_SET_LABEL:
			Target()->SetLabel(fLabelText->Text());
			return true;
		case MSG_BOX_FANCY_BORDER:
			Target()->SetBorder(B_FANCY_BORDER);
			return true;
		case MSG_BOX_PLAIN_BORDER:
			Target()->SetBorder(B_PLAIN_BORDER);
			return true;
		case MSG_BOX_NO_BORDER:
			Target()->SetBorder(B_NO_BORDER);
			return true;
		default:
			return BViewEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BBoxEditor::Supports()
{
	return "BBox";
}
//------------------------------------------------------------------------------
void BBoxEditor::Init()
{
	int bottom = Bottom();
	if (!fLabelText)
	{
		fLabelText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "label", "Label: ", "",
									  new BMessage(MSG_BOX_SET_LABEL));
		fLabelText->SetDivider(be_plain_font->StringWidth("Label: "));
		AddControl(fLabelText);
	}

	if (!fBorderStyle)
	{
		bottom = Bottom();
		fBorderStyle = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									  "style", "Border Style: ",
									  new BPopUpMenu("menu"));
		fBorderStyle->Menu()->AddItem(new BMenuItem("Plain",
										new BMessage(MSG_BOX_PLAIN_BORDER)));
		fBorderStyle->Menu()->AddItem(new BMenuItem("Fancy",
										new BMessage(MSG_BOX_FANCY_BORDER)));
		fBorderStyle->Menu()->AddItem(new BMenuItem("None",
									 new BMessage(MSG_BOX_NO_BORDER)));
		AddControl(fBorderStyle);
	}

	if (Target())
	{
		SetStyle(Target()->Border());
	}
}
//------------------------------------------------------------------------------
BBox* BBoxEditor::Target()
{
	return dynamic_cast<BBox*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------
void BBoxEditor::SetStyle(int32 style)
{
	fBorderStyle->Menu()->ItemAt(B_PLAIN_BORDER)->SetMarked(style == B_PLAIN_BORDER);
	fBorderStyle->Menu()->ItemAt(B_FANCY_BORDER)->SetMarked(style == B_FANCY_BORDER);
	fBorderStyle->Menu()->ItemAt(B_NO_BORDER)->SetMarked(style == B_NO_BORDER);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

