//------------------------------------------------------------------------------
//	BCheckBoxEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/CheckBox.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BCheckBoxEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BCheckBoxEditor::BCheckBoxEditor()
	:	fChecked(NULL)
{
}
//------------------------------------------------------------------------------
BCheckBoxEditor::~BCheckBoxEditor()
{
}
//------------------------------------------------------------------------------
void BCheckBoxEditor::SetTo(BMessage* archive)
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
bool BCheckBoxEditor::MessageReceived(BMessage* msg)
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
const char* BCheckBoxEditor::Supports()
{
	return "BCheckBox";
}
//------------------------------------------------------------------------------
void BCheckBoxEditor::Init()
{
	if (!fChecked)
	{
		int bottom = Bottom();
		fChecked = new BCheckBox(BRect(10, bottom, 190, bottom + 20), "checked",
								 "Checked", new BMessage(MSG_VIEW_SET_CHECKED));
		AddControl(fChecked);
		SetBottom(bottom + 20);
	}
}
//------------------------------------------------------------------------------
BCheckBox* BCheckBoxEditor::Target()
{
	return dynamic_cast<BCheckBox*>(BControlEditor::Target());
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

