//------------------------------------------------------------------------------
//	BSliderEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
#include <interface/Slider.h>
#include <interface/TextControl.h>

#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BSliderEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
BSliderEditor::BSliderEditor()
	:	fModMsg(NULL),
		fSnoozeAmount(NULL),
		fBarColor(NULL),
		fFillColor(NULL),
		fMinLabel(NULL),
		fMaxLabel(NULL),
		fMinValue(NULL),
		fMaxValue(NULL),
		fIncrValue(NULL),
		fHashCount(NULL),
		fHashLocation(NULL),
		fThumbStyle(NULL)
{
}
//------------------------------------------------------------------------------
BSliderEditor::~BSliderEditor()
{
}
//------------------------------------------------------------------------------
void BSliderEditor::SetTo(BMessage* archive)
{
	BControlEditor::SetTo(archive);
	Init();

	BString Label;
	archive->FindString("_minlbl", &Label);
	fMinLabel->SetText(Label.String());

	Label = "";
	archive->FindString("_maxlbl", &Label);
	fMinLabel->SetText(Label.String());

	thumb_style style;
	if (archive->FindInt32("_sstyle", (int32*)&style) == B_OK)
	{
		SetStyle(style);
	}


//	fModMsg->SetTarget(Parent());
//	fSnoozeAmount->SetTarget(Parent());
//	fBarColor->SetTarget(Parent());
//	fFillColor->SetTarget(Parent());
	fMinLabel->SetTarget(Parent());
	fMaxLabel->SetTarget(Parent());
//	fMinValue->SetTarget(Parent());
//	fMaxValue->SetTarget(Parent());
//	fIncrValue->SetTarget(Parent());
//	fHashCount->SetTarget(Parent());
//	fHashLocation->SetTarget(Parent());
	fThumbStyle->Menu()->SetTargetForItems(Parent());

//	Parent()->AddChild(fModMsg);
//	Parent()->AddChild(fSnoozeAmount);
//	Parent()->AddChild(fBarColor);
//	Parent()->AddChild(fFillColor);
	Parent()->AddChild(fMinLabel);
	Parent()->AddChild(fMaxLabel);
//	Parent()->AddChild(fMinValue);
//	Parent()->AddChild(fMaxValue);
//	Parent()->AddChild(fIncrValue);
//	Parent()->AddChild(fHashCount);
//	Parent()->AddChild(fHashLocation);
	Parent()->AddChild(fThumbStyle);
}
//------------------------------------------------------------------------------
bool BSliderEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_SLIDER_SET_BLOCK_THUMB:
			Target()->SetStyle(B_BLOCK_THUMB);
			Target()->Invalidate();
			return true;

		case MSG_SLIDER_SET_TRI_THUMB:
			Target()->SetStyle(B_TRIANGLE_THUMB);
			Target()->Invalidate();
			return true;

		case MSG_SLIDER_SET_LIMIT_LABELS:
			Target()->SetLimitLabels(fMinLabel->Text(), fMaxLabel->Text());
			return true;

		default:
			return BControlEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BSliderEditor::Supports()
{
	return "BSlider";
}
//------------------------------------------------------------------------------
void BSliderEditor::Init()
{
	int bottom = Bottom();
	if (!fModMsg)
	{
	}

	if (!fSnoozeAmount)
	{
	}

	if (!fBarColor)
	{
	}

	if (!fFillColor)
	{
	}

	if (!fMinLabel)
	{
	}

	if (!fMaxLabel)
	{
	}

	if (!fMinLabel)
	{
		bottom = Bottom();
		fMinLabel = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "minlabel", "Left Label: ", "",
									 new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		fMinLabel->SetDivider(be_plain_font->StringWidth("Left Label: "));
		AddControl(fMinLabel);
		SetBottom(bottom + 20);
	}

	if (!fMaxLabel)
	{
		bottom = Bottom();
		fMaxLabel = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "maxlabel", "Right Label: ", "",
									 new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		fMaxLabel->SetDivider(be_plain_font->StringWidth("Right Label: "));
		AddControl(fMaxLabel);
		SetBottom(bottom + 20);
	}

	if (!fMinValue)
	{
	}

	if (!fMaxValue)
	{
	}

	if (!fIncrValue)
	{
	}

	if (!fHashCount)
	{
	}

	if (!fHashLocation)
	{
	}

	if (!fThumbStyle)
	{
		bottom = Bottom();
		fThumbStyle = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									 "style", "Thumb Style: ",
									 new BPopUpMenu("menu"));
		fThumbStyle->Menu()->AddItem(new BMenuItem("Rectangle",
									 new BMessage(MSG_SLIDER_SET_BLOCK_THUMB)));
		fThumbStyle->Menu()->AddItem(new BMenuItem("Triangle",
									 new BMessage(MSG_SLIDER_SET_TRI_THUMB)));
		AddControl(fThumbStyle);
		SetBottom(bottom + 20);
	}

	if (Target())
	{
		SetStyle(Target()->Style());
	}
}
//------------------------------------------------------------------------------
BSlider* BSliderEditor::Target()
{
	return dynamic_cast<BSlider*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------
void BSliderEditor::SetStyle(int32 style)
{
	fThumbStyle->Menu()->
		ItemAt(B_BLOCK_THUMB)->SetMarked(style == B_BLOCK_THUMB);
	fThumbStyle->Menu()->
		ItemAt(B_TRIANGLE_THUMB)->SetMarked(style == B_TRIANGLE_THUMB);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

