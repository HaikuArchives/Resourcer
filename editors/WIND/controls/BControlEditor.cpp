//------------------------------------------------------------------------------
//	BControlEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/CheckBox.h>
#include <interface/TextControl.h>
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BControlEditor::BControlEditor()
	:	fLabelText(NULL),
		fMsgText(NULL),
		fEnabled(NULL),
		fShowLabel(false)
{
}
//------------------------------------------------------------------------------
BControlEditor::~BControlEditor()
{
}
//------------------------------------------------------------------------------
void BControlEditor::SetTo(BMessage* archive)
{
	BViewEditor::SetTo(archive);

	Init();

	if (fShowLabel)
	{
		BString Label;
		if (archive->FindString("_label", &Label))
		{
			DEBUG_ARCHIVE("_label");
		}

		fLabelText->SetText(Label.String());
		fLabelText->SetTarget(Parent());
		Parent()->AddChild(fLabelText);
	}

	BMessage Message;
	if (archive->FindMessage("_msg", &Message))
	{
		DEBUG_ARCHIVE("_msg");
	}

	// TODO: expand
	// This is a very simple way to handle messages; it might be nice to
	// have a BMessage editor which would allow for more complex control
	// message definitions.
	char msg[5];
	msg[0] = 0;
	strncpy(msg, (char *)(new int32(flipcode(Message.what))), 4);
	msg[4] = 0;
	fMsgText->SetText(msg);
	fMsgText->SetTarget(Parent());
	Parent()->AddChild(fMsgText);

	bool disabled = false;
	archive->FindBool("_disable", &disabled);

	fEnabled->SetValue(!disabled);
	fEnabled->SetTarget(Parent());
	Parent()->AddChild(fEnabled);
}
//------------------------------------------------------------------------------
bool BControlEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_CONTROL_SET_LABEL:
		{
			Target()->SetLabel(fLabelText->Text());
			return true;
		}

		case MSG_CONTROL_SET_MSG:
		{
			BMessage* Message = new BMessage(flipcode(*(int32*)fMsgText->Text()));
			Target()->SetMessage(Message);
			return true;
		}

		case MSG_CONTROL_SET_ENABLED:
		{
			Target()->SetEnabled(fEnabled->Value());
			return true;
		}

		default:
			return BViewEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BControlEditor::Supports()
{
	return "BControl";
}
//------------------------------------------------------------------------------
void BControlEditor::Init()
{
	int bottom = Bottom();
	if (!fLabelText && fShowLabel)
	{
		fLabelText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "label", "Label: ", "",
									  new BMessage(MSG_CONTROL_SET_LABEL));
		fLabelText->SetDivider(be_plain_font->StringWidth("Label: "));
		AddControl(fLabelText);
		SetBottom(bottom + 20);
	}

	if (!fMsgText)
	{
		bottom = Bottom();
		fMsgText = new BTextControl(BRect(10, bottom, 190, bottom + 20), "msg",
									"Message: ", "",
									new BMessage(MSG_CONTROL_SET_MSG));
		fMsgText->TextView()->SetMaxBytes(4);
		fMsgText->SetDivider(be_plain_font->StringWidth("Message: "));
		AddControl(fMsgText);
		SetBottom(bottom + 20);
	}

	if (!fEnabled)
	{
		bottom = Bottom();
		fEnabled = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
								 "enabled", "Enabled: ",
								 new BMessage(MSG_CONTROL_SET_ENABLED));
		AddControl(fEnabled);
		SetBottom(bottom + 20);
	}
}
//------------------------------------------------------------------------------
BControlEditor::BControlEditor(bool showLabel)
	:	fLabelText(NULL),
		fMsgText(NULL),
		fEnabled(NULL),
		fShowLabel(showLabel)
{
}
//------------------------------------------------------------------------------
BControl* BControlEditor::Target()
{
	return dynamic_cast<BControl*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

