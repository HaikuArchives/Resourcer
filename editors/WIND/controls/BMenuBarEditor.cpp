//------------------------------------------------------------------------------
//	BMenuBarEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/MenuBar.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BMenuBarEditor.h"

// Local Defines ---------------------------------------------------------------
#include "MenuEditor.h"

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BMenuBarEditor::BMenuBarEditor()
	:	fMenuEditor(NULL)
{
}
//------------------------------------------------------------------------------
BMenuBarEditor::~BMenuBarEditor()
{
}
//------------------------------------------------------------------------------
void BMenuBarEditor::SetTo(BMessage* archive)
{
	BViewEditor::SetTo(archive);
	Init();

	Parent()->AddChild(fMenuEditor);
}
//------------------------------------------------------------------------------
bool BMenuBarEditor::MessageReceived(BMessage* msg)
{
	return BViewEditor::MessageReceived(msg);
}
//------------------------------------------------------------------------------
const char* BMenuBarEditor::Supports()
{
	return "BMenuBar";
}
//------------------------------------------------------------------------------
void BMenuBarEditor::Init()
{
	int bottom;
	if (!fMenuEditor)
	{
		bottom = Bottom();
		fMenuEditor = new MenuEditor(BRect(20, bottom, 240, bottom + 230));
		SetBottom(bottom + 230);
	}

	if (Target())
	{
		fMenuEditor->SetMenu(Target());
	}
}
//------------------------------------------------------------------------------
BMenuBar* BMenuBarEditor::Target()
{
	return dynamic_cast<BMenuBar*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

