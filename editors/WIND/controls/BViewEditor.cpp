//------------------------------------------------------------------------------
//	BViewEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <algorithm>
#include <vector>

// System Includes -------------------------------------------------------------
#include <interface/StringView.h>
#include <interface/TextControl.h>
#include <interface/View.h>
#include <support/ClassInfo.h>
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"
#include "../EditingPanel.h"
#include "../WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class ControlKeeper
{
	public:
		ControlKeeper();
		~ControlKeeper();

		void AddControl(BView* control);

	private:
		std::vector<BView*>	fControls;
};

//------------------------------------------------------------------------------
BViewEditor::BViewEditor()
	:	fClassName(NULL),
		fViewName(NULL),
		fBottom(10),
		fControlKeeper(new ControlKeeper)
{
}
//------------------------------------------------------------------------------
BViewEditor::~BViewEditor()
{
	if (fControlKeeper)
		delete fControlKeeper;
}
//------------------------------------------------------------------------------
void BViewEditor::SetParent(BView* parent)
{
	fParent = parent;
}
//------------------------------------------------------------------------------
void BViewEditor::SetTarget(BView* target)
{
	fTarget = target;
}
//------------------------------------------------------------------------------
void BViewEditor::SetTo(BMessage* archive)
{
	Init();

	if (archive->FindMessenger("Resourcer:messenger", &fControl))
		debugger("Couldn't find messenger in archive");

	BString ClassName;
	if (archive->FindString("class", &ClassName))
		debugger("Couldn't find class name in archive");

	BString ViewName;
	if (archive->FindString("_name", &ViewName))
		debugger("Couldn't find view name in archive");

	ClassName.Prepend("Element Type: ");
	fClassName->SetText(ClassName.String());

	fViewName->SetText(ViewName.String());

	fViewName->SetTarget(Parent());

	Parent()->AddChild(fClassName);
	Parent()->AddChild(fViewName);
}
//------------------------------------------------------------------------------
bool BViewEditor::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case MSG_VIEW_SET_NAME:
		{
//			BMessage notify(MSG_VIEW_SET_NAME);
//			notify.AddString("name", fViewName->Text());
//			notify.AddPointer("view", fControl.Target(NULL));
//			
//			fControl.SendMessage(&notify);
			Target()->SetName(fViewName->Text());
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------
const char* BViewEditor::Supports()
{
	return "BView";
}
//------------------------------------------------------------------------------
BWindow* BViewEditor::Window()
{
	if (Target())
	{
		return Target()->Window();
	}

	return NULL;
}
//------------------------------------------------------------------------------
BMessenger& BViewEditor::Messenger()
{
	return fControl;
}
//------------------------------------------------------------------------------
int BViewEditor::Bottom()
{
	return fBottom;
}
//------------------------------------------------------------------------------
void BViewEditor::SetBottom(int bottom)
{
	fBottom = bottom;
}
//------------------------------------------------------------------------------
void BViewEditor::AddControl(BView *control)
{
	fControlKeeper->AddControl(control);
}
//------------------------------------------------------------------------------
BView* BViewEditor::Parent()
{
	return fParent;
}
//------------------------------------------------------------------------------
BView* BViewEditor::Target()
{
	return fTarget;
}
//------------------------------------------------------------------------------
void BViewEditor::Init()
{
	SetBottom(10);
	int bottom;
	if (!fClassName)
	{
		bottom = Bottom();
		fClassName = new BStringView(BRect(10, bottom, 190, bottom + 20),
									 "classname", "Element Type: ");
		AddControl(fClassName);
		SetBottom(bottom + 20);
	}

	if (!fViewName)
	{
		bottom = Bottom();
		fViewName = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "name", "View Name: ", "",
									 new BMessage(MSG_VIEW_SET_NAME));
		fViewName->SetDivider(be_plain_font->StringWidth("View Name: "));
		AddControl(fViewName);
		SetBottom(bottom + 20);
	}
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	#pragma mark -
//------------------------------------------------------------------------------
ControlKeeper::ControlKeeper()
{
	;
}
//------------------------------------------------------------------------------
ControlKeeper::~ControlKeeper()
{
	for (uint32 i = 0; i < fControls.size(); ++i)
	{
		if (fControls[i]->Parent() == NULL)
		{
			delete fControls[i];
			fControls[i] = NULL;
		}
	}
}
//------------------------------------------------------------------------------
void ControlKeeper::AddControl(BView* control)
{
	std::vector<BView*>::iterator i = std::find(fControls.begin(),
												fControls.end(),
												control);
	if (i != fControls.end())
	{
		// control hasn't been previously added
		fControls.push_back(control);
	}
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

