//------------------------------------------------------------------------------
//	BTextControlEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TextControl.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BTextControlEditor.h"
#include "BTextViewEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------
#define MSG_TEXT_CTRL_SET_MOD_MSG	-600
#define MSG_TEXT_CTRL_SET_TEXT		-610
#define MSG_TEXT_CTRL_SET_ALIGNMENT	-620
#define MSG_TEXT_CTRL_SET_DIVIDER	-630

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BTextControlEditor::BTextControlEditor()
	:	fModMsg(NULL),
		fText(NULL),
		fDivider(NULL),
		fLabelAlignment(NULL),
		fTextAlignment(NULL),
		fTextViewEditor(new BTextViewEditor(true))
{
}
//------------------------------------------------------------------------------
BTextControlEditor::~BTextControlEditor()
{
}
//------------------------------------------------------------------------------
void BTextControlEditor::SetParent(BView* parent)
{
	BControlEditor::SetParent(parent);
	fTextViewEditor->SetParent(parent);
}
//------------------------------------------------------------------------------
void BTextControlEditor::SetTarget(BView* target)
{
	BControlEditor::SetTarget(target);
	fTextViewEditor->SetTarget(Target()->TextView());
}
//------------------------------------------------------------------------------
void BTextControlEditor::SetTo(BMessage* archive)
{
	BControlEditor::SetTo(archive);
	Init();

	int32 labelalign = B_ALIGN_LEFT;
	if (archive->FindInt32("_a_label", &labelalign))
	{
		DEBUG_ARCHIVE("_a_label");
	}
	fLabelAlignment->Menu()->ItemAt(labelalign)->SetMarked(true);

	int32 textalign = B_ALIGN_LEFT;
	if (archive->FindInt32("_a_text", &textalign))
	{
		DEBUG_ARCHIVE("_a_text");
	}
	fTextAlignment->Menu()->ItemAt(textalign)->SetMarked(true);

	float divider;
	if (archive->FindFloat("_divide", &divider))
	{
		DEBUG_ARCHIVE("_divide");
	}
	fDivider->SetText((BString() << divider).String());

	BMessage modmsg;
	if (archive->FindMessage("_mod_msg", &modmsg) == B_OK)
	{
		SetMessageText(modmsg, fModMsg);
	}
	else
	{
		fModMsg->SetText("");
	}

	fModMsg->SetTarget(Parent());
	fText->SetTarget(Parent());
	fDivider->SetTarget(Parent());
	fLabelAlignment->Menu()->SetTargetForItems(Parent());
	fTextAlignment->Menu()->SetTargetForItems(Parent());

	Parent()->AddChild(fModMsg);
	Parent()->AddChild(fText);
	Parent()->AddChild(fDivider);
	Parent()->AddChild(fLabelAlignment);
	Parent()->AddChild(fTextAlignment);

	fTextViewEditor->SetBottom(Bottom());
	fTextViewEditor->SetTo(archive);
}
//------------------------------------------------------------------------------
bool BTextControlEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_TEXT_CTRL_SET_MOD_MSG:
		{
			BMessage* Message = new BMessage(flipcode(*(int32*)fModMsg->Text()));
			Target()->SetModificationMessage(Message);
			return true;
		}

		case MSG_TEXT_CTRL_SET_TEXT:
			Target()->SetText(fText->Text());
			return true;

		case MSG_TEXT_CTRL_SET_ALIGNMENT:
		{
			alignment label;
			alignment text;
			GetAlignments(label, text);
			Target()->SetAlignment(label, text);
			return true;
		}

		case MSG_TEXT_CTRL_SET_DIVIDER:
			Target()->SetDivider(atof(fDivider->Text()));
			return true;

		default:
			if (!fTextViewEditor->MessageReceived(msg))
			{
				return BControlEditor::MessageReceived(msg);
			}
			return true;
	}
}
//------------------------------------------------------------------------------
const char* BTextControlEditor::Supports()
{
	return "BTextControl";
}
//------------------------------------------------------------------------------
void BTextControlEditor::Init()
{
	int bottom;
	if (!fModMsg)
	{
		bottom = Bottom();
		fModMsg = new BTextControl(BRect(10, bottom, 190, bottom + 20),
								   "modmsg", "Modification Message: ", "",
								   new BMessage(MSG_TEXT_CTRL_SET_MOD_MSG));
		fModMsg->SetDivider(be_plain_font->StringWidth("Modification Message: "));
		AddControl(fModMsg);

	}

	if (!fText)
	{
		bottom = Bottom();
		fText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
								 "text", "Text: ", "",
								 new BMessage(MSG_TEXT_CTRL_SET_TEXT));
		fText->SetDivider(be_plain_font->StringWidth("Text: "));
		AddControl(fText);
	}

	if (!fDivider)
	{
		bottom = Bottom();
		fDivider = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									"divider", "Divider: ", "",
									new BMessage(MSG_TEXT_CTRL_SET_DIVIDER));
		AddControl(fDivider);
	}

	if (!fLabelAlignment)
	{
		bottom = Bottom();
		fLabelAlignment = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									"alignment", "Label Alignment: ",
									new BPopUpMenu("menu"));
		fLabelAlignment->Menu()->AddItem(new BMenuItem("Left",
									new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		fLabelAlignment->Menu()->AddItem(new BMenuItem("Right",
									new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		fLabelAlignment->Menu()->AddItem(new BMenuItem("Center",
									new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		AddControl(fLabelAlignment);
	}

	if (!fTextAlignment)
	{
		bottom = Bottom();
		fTextAlignment = new BMenuField(BRect(10, bottom, 190, bottom + 20),
										"alignment", "Text Alignment: ",
										new BPopUpMenu("menu"));
		fTextAlignment->Menu()->AddItem(new BMenuItem("Left",
										new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		fTextAlignment->Menu()->AddItem(new BMenuItem("Right",
										new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		fTextAlignment->Menu()->AddItem(new BMenuItem("Center",
										new BMessage(MSG_TEXT_CTRL_SET_ALIGNMENT)));
		AddControl(fTextAlignment);
	}
}
//------------------------------------------------------------------------------
BTextControl* BTextControlEditor::Target()
{
	return dynamic_cast<BTextControl*>(BControlEditor::Target());
}
//------------------------------------------------------------------------------
void BTextControlEditor::GetAlignments(alignment& label, alignment& text)
{
	if (fLabelAlignment->Menu()->ItemAt(B_ALIGN_LEFT)->IsMarked())
		label = B_ALIGN_LEFT;
	else if (fLabelAlignment->Menu()->ItemAt(B_ALIGN_RIGHT)->IsMarked())
		label = B_ALIGN_RIGHT;
	else if (fLabelAlignment->Menu()->ItemAt(B_ALIGN_CENTER)->IsMarked())
		label = B_ALIGN_CENTER;

	if (fTextAlignment->Menu()->ItemAt(B_ALIGN_LEFT)->IsMarked())
		text = B_ALIGN_LEFT;
	else if (fTextAlignment->Menu()->ItemAt(B_ALIGN_RIGHT)->IsMarked())
		text = B_ALIGN_RIGHT;
	else if (fTextAlignment->Menu()->ItemAt(B_ALIGN_CENTER)->IsMarked())
		text = B_ALIGN_CENTER;
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

