//------------------------------------------------------------------------------
//	BRadioButtonEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/CheckBox.h>
#include <interface/RadioButton.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BRadioButtonEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BRadioButtonEditor::BRadioButtonEditor()
	:	fChecked(NULL)
{
}
//------------------------------------------------------------------------------
BRadioButtonEditor::~BRadioButtonEditor()
{
}
//------------------------------------------------------------------------------
void BRadioButtonEditor::SetTo(BMessage* archive)
{
	BControlEditor::SetTo(archive);
	Init();

	int32 value;
	if (archive->FindInt32("_val", &value))
	{
		fChecked->SetValue(B_CONTROL_OFF);
	}
	else
	{
		fChecked->SetValue(value);
	}

	fChecked->SetTarget(Parent());
	Parent()->AddChild(fChecked);
}
//------------------------------------------------------------------------------
bool BRadioButtonEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_VIEW_SET_CHECKED:
			Target()->SetValue(fChecked->Value());
			return true;

		default:
			return BControlEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BRadioButtonEditor::Supports()
{
	return "BRadioButton";
}
//------------------------------------------------------------------------------
void BRadioButtonEditor::Init()
{
	if (!fChecked)
	{
		int bottom = Bottom();
		fChecked = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
								 "selected", "Selected",
								 new BMessage(MSG_VIEW_SET_CHECKED));
		AddControl(fChecked);
	}
}
//------------------------------------------------------------------------------
BRadioButton* BRadioButtonEditor::Target()
{
	return dynamic_cast<BRadioButton*>(BControlEditor::Target());
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

