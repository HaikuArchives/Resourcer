//------------------------------------------------------------------------------
//	BColorControlEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdlib.h>

// System Includes -------------------------------------------------------------
#include <interface/ColorControl.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TextControl.h>

#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BColorControlEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BColorControlEditor::BColorControlEditor()
	:	BControlEditor(false),
		fCellSize(NULL),
		fLayoutField(NULL)
{
}
//------------------------------------------------------------------------------
BColorControlEditor::~BColorControlEditor()
{
}
//------------------------------------------------------------------------------
void BColorControlEditor::SetTo(BMessage* archive)
{
	BControlEditor::SetTo(archive);
	Init();

	float cellsize;
	if (archive->FindFloat("_csize", &cellsize))
	{
		DEBUG_ARCHIVE("_csize");
	}

	BString text;
	text << cellsize;
	fCellSize->SetText(text.String());
	fCellSize->SetTarget(Parent());
	Parent()->AddChild(fCellSize);

	color_control_layout layout;
	if (archive->FindInt32("_layout", (int32*)&layout))
	{
		DEBUG_ARCHIVE("_layout");
	}

	SetLayout(layout);

	fLayoutField->Menu()->SetTargetForItems(Parent());
	Parent()->AddChild(fLayoutField);
}
//------------------------------------------------------------------------------
bool BColorControlEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_COLOR_CONTROL_SET_ALIGN:
			SetLayout(Target());
			return true;

		case MSG_COLOR_CONTROL_SET_CELL_SIZE:
		{
			float cellsize = atof(fCellSize->Text());
			Target()->SetCellSize(cellsize);
			return true;
		}

		default:
			return BControlEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BColorControlEditor::Supports()
{
	return "BColorControl";
}
//------------------------------------------------------------------------------
void BColorControlEditor::Init()
{
	int bottom;
	if (!fCellSize)
	{
		bottom = Bottom();
		fCellSize = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "cellsize", "Cell Size: ", "",
									 new BMessage(MSG_COLOR_CONTROL_SET_CELL_SIZE));
		fCellSize->SetDivider(be_plain_font->StringWidth(fCellSize->Label()));
		AddControl(fCellSize);
		SetBottom(bottom + 20);
	}

	if (!fLayoutField)
	{
		bottom = Bottom();
		fLayoutField = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									  "alignpicker", "Cell Alignment: ",
									  new BPopUpMenu("Item Alignment"));

		fLayoutField->Menu()->AddItem(new BMenuItem("4 by 64", new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		fLayoutField->Menu()->AddItem(new BMenuItem("8 by 32", new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		fLayoutField->Menu()->AddItem(new BMenuItem("16 by 16", new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		fLayoutField->Menu()->AddItem(new BMenuItem("64 by 4", new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		fLayoutField->Menu()->AddItem(new BMenuItem("32 by 8", new BMessage(MSG_COLOR_CONTROL_SET_ALIGN)));
		fLayoutField->SetDivider(be_plain_font->StringWidth(fLayoutField->Label()));

		AddControl(fLayoutField);
		SetBottom(bottom + 20);
	}

	if (Target())
	{
		SetLayout(Target()->Layout());
	}
}
//------------------------------------------------------------------------------
BColorControl* BColorControlEditor::Target()
{
	return dynamic_cast<BColorControl*>(BControlEditor::Target());
}
//------------------------------------------------------------------------------
void BColorControlEditor::SetLayout(int32 layout)
{
	switch (layout)
	{
		case B_CELLS_4x64:
			fLayoutField->Menu()->ItemAt(0)->SetMarked(true);
			break;
		case B_CELLS_8x32:
			fLayoutField->Menu()->ItemAt(1)->SetMarked(true);
			break;
		case B_CELLS_16x16:
			fLayoutField->Menu()->ItemAt(2)->SetMarked(true);
			break;	
		case B_CELLS_64x4:
			fLayoutField->Menu()->ItemAt(3)->SetMarked(true);
			break;	
		case B_CELLS_32x8:
			fLayoutField->Menu()->ItemAt(4)->SetMarked(true);
			break;
	}
}
//------------------------------------------------------------------------------
void BColorControlEditor::SetLayout(BColorControl* Control)
{
	if (!Control)
	{
		debugger("Control is NULL");
	}

	if (fLayoutField->Menu()->ItemAt(0)->IsMarked())
		Control->SetLayout(B_CELLS_4x64);
	if (fLayoutField->Menu()->ItemAt(1)->IsMarked())
		Control->SetLayout(B_CELLS_8x32);
	if (fLayoutField->Menu()->ItemAt(2)->IsMarked())
		Control->SetLayout(B_CELLS_16x16);
	if (fLayoutField->Menu()->ItemAt(3)->IsMarked())
		Control->SetLayout(B_CELLS_64x4);
	if (fLayoutField->Menu()->ItemAt(4)->IsMarked())
		Control->SetLayout(B_CELLS_32x8);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

