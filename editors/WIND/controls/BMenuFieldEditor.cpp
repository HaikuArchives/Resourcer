//------------------------------------------------------------------------------
//	BMenuFieldEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/TextControl.h>
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BMenuFieldEditor.h"
#include "MenuEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BMenuFieldEditor::BMenuFieldEditor()
	:	fLabelText(NULL),
		fDefaultText(NULL),
		fMenuEditor(NULL)
{
}
//------------------------------------------------------------------------------
BMenuFieldEditor::~BMenuFieldEditor()
{
}
//------------------------------------------------------------------------------
void BMenuFieldEditor::SetTo(BMessage *archive)
{
	BViewEditor::SetTo(archive);
	Init();

	BString Text;
	if (archive->FindString("_label", &Text))
	{
		debugger("Couldn't find _label in archive");
	}
	
	fLabelText->SetText(Text.String());

	if (Target() && Target()->Menu())
	{
		fDefaultText->SetText(Target()->Menu()->Name());
	}

	fLabelText->SetTarget(Parent());
	fDefaultText->SetTarget(Parent());

	Parent()->AddChild(fLabelText);
	Parent()->AddChild(fDefaultText);
	Parent()->AddChild(fMenuEditor);
}
//------------------------------------------------------------------------------
bool BMenuFieldEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case MSG_MENU_FIELD_SET_LABEL:
			Target()->SetLabel(fLabelText->Text());
			Target()->SetDivider(be_plain_font->StringWidth(Target()->Label()));
			return true; 

		case MSG_MENU_SET_DEFAULT_TEXT:
			Target()->Menu()->SetName(fDefaultText->Text());
			Target()->Menu()->Superitem()->SetLabel(fDefaultText->Text());
			return true;

		default:
			return BViewEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BMenuFieldEditor::Supports()
{
	return "BMenuField";
}
//------------------------------------------------------------------------------
void BMenuFieldEditor::Init()
{
	int bottom;
	if (!fLabelText)
	{
		bottom = Bottom();
		fLabelText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "label", "Label: ", "",
									  new BMessage(MSG_MENU_FIELD_SET_LABEL));
		fLabelText->SetDivider(be_plain_font->StringWidth("Label: "));
		AddControl(fLabelText);
		SetBottom(bottom + 20);
	}

	if (!fDefaultText)
	{
		bottom = Bottom();
		fDefaultText = new BTextControl(BRect(10, bottom, 190, bottom + 20),
										"deftext", "Default Menu Text: ", "",
										new BMessage(MSG_MENU_SET_DEFAULT_TEXT));
		fDefaultText->SetDivider(be_plain_font->StringWidth(fDefaultText->Label()));
		AddControl(fDefaultText);
		SetBottom(bottom + 20);
	}

	if (!fMenuEditor)
	{
		bottom = Bottom();
		fMenuEditor = new MenuEditor(BRect(20, bottom, 240, bottom + 230));
		SetBottom(bottom + 230);
	}

	if (Target())
	{
		fMenuEditor->SetMenu(Target()->Menu());
	}
}
//------------------------------------------------------------------------------
BMenuField* BMenuFieldEditor::Target()
{
	return dynamic_cast<BMenuField*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

