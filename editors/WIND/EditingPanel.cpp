//------------------------------------------------------------------------------
//	EditingPanel.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdio.h>
#include <string.h>

// System Includes -------------------------------------------------------------
#include <interface/Box.h>
#include <interface/Button.h>
#include <interface/ColorControl.h>
#include <interface/MenuBar.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/Slider.h>
#include <interface/StringView.h>
#include <interface/TextControl.h>
#include <interface/Window.h>

#include <support/Autolock.h>
#include <support/Beep.h>
#include <support/ClassInfo.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "EditingPanel.h"
#include "MenuEditor.h"
//#include "ResImageView.h"
//#include "ResourceStringView.h"
#include "BViewEditor.h"
#include "BControlEditor.h"
#include "BButtonEditor.h"
#include "BBoxEditor.h"
#include "BStringViewEditor.h"
#include "BSliderEditor.h"
#include "BMenuFieldEditor.h"
#include "BMenuBarEditor.h"
#include "BCheckBoxEditor.h"
#include "BRadioButtonEditor.h"
#include "BColorControlEditor.h"
#include "BTextViewEditor.h"
#include "BTextControlEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
EditingPanel::EditingPanel(void)
	:	BView(BRect(0, 0, 300, 350), "editpanel", B_FOLLOW_ALL_SIDES, 0),
		fCurrentEditor(NULL),
		iscontrol(false),
		pointer(NULL)
{
	SetViewColor(216,216,216);
	AddChild(new BStringView(BRect(25,100,175,125),"noactive","No Focus View"));

	InitEditors();
}
//------------------------------------------------------------------------------
EditingPanel::~EditingPanel()
{
	CleanupEditors();
}
//------------------------------------------------------------------------------
void EditingPanel::AttachedToWindow()
{
	BView::AttachedToWindow();

	BControl* ctrl;
	for (int32 i = 0; i < CountChildren(); ++i)
	{
		ctrl = dynamic_cast<BControl*>(ChildAt(i));
		if (ctrl)
		{
			ctrl->SetTarget(this);
		}
	}
}
//------------------------------------------------------------------------------
void EditingPanel::MessageReceived(BMessage *msg)
{
	if (fCurrentEditor)
	{
		if (fCurrentEditor->Window())
		{
			BAutolock locker(fCurrentEditor->Window());
			if (fCurrentEditor->MessageReceived(msg))
			{
				return;
			}
		}
	}

	switch(msg->what)
	{
		default:
			BView::MessageReceived(msg);
			break;
	}
}
//------------------------------------------------------------------------------
void EditingPanel::SetTo(BMessage* active)
{
	BAutolock locker(Window());
	for (BView* dead = ChildAt(0); dead != NULL; dead = ChildAt(0))
	{
		RemoveChild(dead);
	}

	BView* view = NULL;
	if (active)
	{
		if (active->FindPointer("view", (void**)&view) == B_OK)
		{
			BMessage ctrl;
			if (active->FindMessage("ctrl", &ctrl) == B_OK)
			{
				const char* type;
				if (ctrl.FindString("class", &type) == B_OK)
				{
					EditorMap::iterator i = fEditorMap.find(type);
					if (i != fEditorMap.end())
					{
						fCurrentEditor = i->second;
						fCurrentEditor->SetParent(this);
						fCurrentEditor->SetTarget(view);
						fCurrentEditor->SetTo(&ctrl);
					}
				}
			}
			else
			{
				// Houston, we have a problem ...
				debugger("Archive of currently selected control not found");
			}
		}
	}

	if (!active || !view)
	{
		AddChild(new BStringView(BRect(25, 100, 175, 125), "noactive",
								 "No Focus View"));
		fCurrentEditor = NULL;
		return;
	}
}
//------------------------------------------------------------------------------
void EditingPanel::InitEditors()
{
	RegisterEditor(new BViewEditor);
	RegisterEditor(new BControlEditor);
	RegisterEditor(new BButtonEditor);
	RegisterEditor(new BBoxEditor);
	RegisterEditor(new BStringViewEditor);
	RegisterEditor(new BSliderEditor);
	RegisterEditor(new BMenuFieldEditor);
	RegisterEditor(new BMenuBarEditor);
	RegisterEditor(new BCheckBoxEditor);
	RegisterEditor(new BRadioButtonEditor);
	RegisterEditor(new BColorControlEditor);
	RegisterEditor(new BTextViewEditor);
	RegisterEditor(new BTextControlEditor);
}
//------------------------------------------------------------------------------
void EditingPanel::RegisterEditor(BViewEditor* editor)
{
	if (editor)
	{
		fEditorMap[editor->Supports()] = editor;
	}
}
//------------------------------------------------------------------------------
void EditingPanel::CleanupEditors()
{
	for (EditorMap::iterator i = fEditorMap.begin(); i != fEditorMap.end(); ++i)
	{
		delete i->second;
		i->second = NULL;
	}
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

