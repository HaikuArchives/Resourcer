//------------------------------------------------------------------------------
//	BSliderEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

// System Includes -------------------------------------------------------------
#include <interface/CheckBox.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/RadioButton.h>
//#include <interface/Slider.h>
#include <interface/TextControl.h>

#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BSliderEditor.h"
#include "ModSlider.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------
#define MSG_SLIDER_SET_BLOCK_THUMB			-700
#define MSG_SLIDER_SET_TRI_THUMB			-701
#define MSG_SLIDER_SET_HASH_NONE			-710
#define MSG_SLIDER_SET_HASH_TOP_LEFT		-711
#define MSG_SLIDER_SET_HASH_BOTTOM_RIGHT	-712
#define MSG_SLIDER_SET_HASH_COUNT			-713
#define MSG_SLIDER_SET_LIMIT_LABELS			-720
#define MSG_SLIDER_SET_ORIENT_HORZ			-730
#define MSG_SLIDER_SET_ORIENT_VERT			-731
#define MSG_SLIDER_SET_MIN_VALUE			-740
#define MSG_SLIDER_SET_MAX_VALUE			-741
#define MSG_SLIDER_SET_INCREMENT			-742
#define MSG_SLIDER_SET_MOD_MSG				-750
#define MSG_SLIDER_SET_BAR_COLOR			-760
#define MSG_SLIDER_SET_FILL_COLOR			-761
#define MSG_SLIDER_USE_FILL_COLOR			-762
#define MSG_SLIDER_SET_THICKNESS			-763
#define MSG_SLIDER_SET_SNOOZE				-770

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
		fBarThickness(NULL),
		fUseFillCheck(NULL),
		fOrientation(NULL),
		fHashStyle(NULL),
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

	thumb_style style = B_BLOCK_THUMB;
	if (archive->FindInt16("_sstyle", (int16*)&style))
	{
		DEBUG_ARCHIVE("_sstyle");
	}
	SetThumbStyle(style);

	int32 orient = B_HORIZONTAL;
	if (archive->FindInt32("_orient", &orient))
	{
		DEBUG_ARCHIVE("_orient");
	}
	SetOrientation(orient);
	fOrientation->Menu()->ItemAt(orient)->SetMarked(true);

	int16 hashmarks = B_HASH_MARKS_NONE;
	if (archive->FindInt16("_hashloc", &hashmarks))
	{
		DEBUG_ARCHIVE("_hashloc");
	}
	SetHashLabel(hashmarks);

	int32 hashcount = 0;
	if (archive->FindInt32("_hashcount", &hashcount))
	{
		DEBUG_ARCHIVE("_hashcount");
	}
	fHashCount->SetText((BString() << hashcount).String());

	int32 min = 0;
	if (archive->FindInt32("_min", &min))
	{
		DEBUG_ARCHIVE("_min");
	}
	fMinValue->SetText((BString() << min).String());

	int32 max = 0;
	if (archive->FindInt32("_max", &max))
	{
		DEBUG_ARCHIVE("_max");
	}
	fMaxValue->SetText((BString() << max).String());

	BMessage modmsg;
	if (archive->FindMessage("_mod_msg", &modmsg) == B_OK)
	{
		// TODO: expand
		// This is a very simple way to handle messages; it might be nice to
		// have a BMessage editor which would allow for more complex control
		// message definitions.
		char msg[5];
		msg[0] = 0;
		strncpy(msg, (char *)(new int32(flipcode(modmsg.what))), 4);
		msg[4] = 0;
		fModMsg->SetText(msg);
	}
	else
	{
		fModMsg->SetText("");
	}

	int32 increment = 1;
	if (archive->FindInt32("_incrementvalue", &increment))
	{
		DEBUG_ARCHIVE("_incrementvalue");
	}
	fIncrValue->SetText((BString() << increment).String());

	rgb_color barcolor;
	if (archive->FindInt32("_bcolor", (int32*)&barcolor))
	{
		DEBUG_ARCHIVE("_bcolor");
	}
	else
	{
		char colortext[12];
		sprintf(colortext, "%lX", *(int32*)&barcolor);
		fBarColor->SetText(colortext);
	}

	rgb_color fillcolor;
	if (archive->FindInt32("_fcolor", (int32*)&barcolor))
	{
		fUseFillCheck->SetValue(B_CONTROL_OFF);
		fFillColor->SetEnabled(false);
	}
	else
	{
		fUseFillCheck->SetValue(B_CONTROL_ON);
		fFillColor->SetEnabled(true);
		char colortext[12];
		sprintf(colortext, "%lX", *(int32*)&fillcolor);
		fFillColor->SetText(colortext);
	}

	float thickness = 1.0;
	if (archive->FindFloat("_bthickness", &thickness))
	{
		DEBUG_ARCHIVE("_bthickness");
	}
	fBarThickness->SetText((BString() << thickness).String());

	int32 sdelay = 20000;
	if (archive->FindInt32("_sdelay", &sdelay))
	{
		DEBUG_ARCHIVE("_sdelay");
	}
	fSnoozeAmount->SetText((BString() << sdelay).String());

	fModMsg->SetTarget(Parent());
	fMinLabel->SetTarget(Parent());
	fMaxLabel->SetTarget(Parent());
	fMinValue->SetTarget(Parent());
	fMaxValue->SetTarget(Parent());
	fIncrValue->SetTarget(Parent());
	fOrientation->Menu()->SetTargetForItems(Parent());
	fHashStyle->Menu()->SetTargetForItems(Parent());
	fHashCount->SetTarget(Parent());
	fThumbStyle->Menu()->SetTargetForItems(Parent());
	fBarColor->SetTarget(Parent());
	fUseFillCheck->SetTarget(Parent());
	fFillColor->SetTarget(Parent());
	fBarThickness->SetTarget(Parent());
	fSnoozeAmount->SetTarget(Parent());

	Parent()->AddChild(fModMsg);
	Parent()->AddChild(fMinLabel);
	Parent()->AddChild(fMaxLabel);
	Parent()->AddChild(fMinValue);
	Parent()->AddChild(fMaxValue);
	Parent()->AddChild(fIncrValue);
	Parent()->AddChild(fOrientation);
	Parent()->AddChild(fHashStyle);
	Parent()->AddChild(fHashCount);
	Parent()->AddChild(fThumbStyle);
	Parent()->AddChild(fBarColor);
	Parent()->AddChild(fUseFillCheck);
	Parent()->AddChild(fFillColor);
	Parent()->AddChild(fBarThickness);
	Parent()->AddChild(fSnoozeAmount);
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

		case MSG_SLIDER_SET_HASH_TOP_LEFT:
		{
			SetHashStyle(B_HASH_MARKS_TOP);
			return true;
		}

		case MSG_SLIDER_SET_HASH_BOTTOM_RIGHT:
		{
			SetHashStyle(B_HASH_MARKS_BOTTOM);
			return true;
		}

		case MSG_SLIDER_SET_ORIENT_HORZ:
			Target()->SetOrientation(B_HORIZONTAL);
			SetOrientation(B_HORIZONTAL);
			return true;

		case MSG_SLIDER_SET_ORIENT_VERT:
			Target()->SetOrientation(B_VERTICAL);
			SetOrientation(B_VERTICAL);
			return true;

		case MSG_SLIDER_SET_HASH_COUNT:
			Target()->SetHashMarkCount(atoi(fHashCount->Text()));
			return true;

		case MSG_SLIDER_SET_MIN_VALUE:
		case MSG_SLIDER_SET_MAX_VALUE:
			Target()->SetLimits(atoi(fMinValue->Text()),
								atoi(fMaxValue->Text()));
			return true;

		case MSG_SLIDER_SET_INCREMENT:
			Target()->SetKeyIncrementValue(atoi(fIncrValue->Text()));
			return true;

		case MSG_SLIDER_SET_MOD_MSG:
		{
			BMessage* Message = new BMessage(flipcode(*(int32*)fModMsg->Text()));
			Target()->SetModificationMessage(Message);
			return true;
		}

		case MSG_SLIDER_SET_BAR_COLOR:
		{
			int32 color = atoi(fBarColor->Text());
			Target()->SetBarColor(*(rgb_color*)&color);
			Target()->Invalidate();
			return true;
		}

		case MSG_SLIDER_USE_FILL_COLOR:
		case MSG_SLIDER_SET_FILL_COLOR:
		{
			bool use = fUseFillCheck->Value();
			int32 color = atoi(fFillColor->Text());
			Target()->UseFillColor(use, (rgb_color*)&color);
			fFillColor->SetEnabled(use);
			Target()->Invalidate();
			return true;
		}

		case MSG_SLIDER_SET_THICKNESS:
			Target()->SetBarThickness(atof(fBarThickness->Text()));
			Target()->Invalidate();
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
		bottom = Bottom();
		fModMsg = new BTextControl(BRect(10, bottom, 190, bottom + 20),
								   "modmsg", "Modification Message: ", "",
								   new BMessage(MSG_SLIDER_SET_MOD_MSG));
		fModMsg->SetDivider(be_plain_font->StringWidth("Modification Message: "));
		AddControl(fModMsg);
	}

	if (!fMinLabel)
	{
		bottom = Bottom();
		fMinLabel = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "minlabel", "Left Label: ", "",
									 new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		fMinLabel->SetDivider(be_plain_font->StringWidth("Left Label: "));
		AddControl(fMinLabel);
	}

	if (!fMaxLabel)
	{
		bottom = Bottom();
		fMaxLabel = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "maxlabel", "Right Label: ", "",
									 new BMessage(MSG_SLIDER_SET_LIMIT_LABELS));
		fMaxLabel->SetDivider(be_plain_font->StringWidth("Right Label: "));
		AddControl(fMaxLabel);
	}

	if (!fMinValue)
	{
		bottom = Bottom();
		fMinValue = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "minvalue", "Min Value: ", "",
									 new BMessage(MSG_SLIDER_SET_MIN_VALUE));
		fMinValue->SetDivider(be_plain_font->StringWidth("Min Value: "));
		AddControl(fMinValue);
	}

	if (!fMaxValue)
	{
		bottom = Bottom();
		fMaxValue = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "maxvalue", "Max Value: ", "",
									 new BMessage(MSG_SLIDER_SET_MAX_VALUE));
		fMaxValue->SetDivider(be_plain_font->StringWidth("Max Value: "));
		AddControl(fMaxValue);
	}

	if (!fIncrValue)
	{
		bottom = Bottom();
		fIncrValue = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "incrvalue", "Increment: ", "",
									  new BMessage(MSG_SLIDER_SET_INCREMENT));
		fIncrValue->SetDivider(be_plain_font->StringWidth("Increment: "));
		AddControl(fMaxValue);
	}

	if (!fOrientation)
	{
		bottom = Bottom();
		fOrientation = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									  "orientation", "Orientation: ",
									  new BPopUpMenu("menu"));
		fOrientation->Menu()->AddItem(new BMenuItem("Horizontal",
									  new BMessage(MSG_SLIDER_SET_ORIENT_HORZ)));
		fOrientation->Menu()->AddItem(new BMenuItem("Vertical",
									  new BMessage(MSG_SLIDER_SET_ORIENT_VERT)));
		AddControl(fOrientation);
	}

	if (!fHashStyle)
	{
		bottom = Bottom();
		fHashStyle = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									"hashstyle", "Hash Marks: ",
									new BMenu("menu"));
		fHashStyle->Menu()->AddItem(new BMenuItem("Top",
									new BMessage(MSG_SLIDER_SET_HASH_TOP_LEFT)));
		fHashStyle->Menu()->AddItem(new BMenuItem("Bottom",
									new BMessage(MSG_SLIDER_SET_HASH_BOTTOM_RIGHT)));
		AddControl(fHashStyle);
	}

	if (!fHashCount)
	{
		bottom = Bottom();
		fHashCount = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "hashcount", "Hash Mark Count: ", "",
									  new BMessage(MSG_SLIDER_SET_HASH_COUNT));
		fHashCount->SetDivider(be_plain_font->StringWidth("Hash Mark Count: "));
		AddControl(fHashCount);
	}

	if (!fThumbStyle)
	{
		bottom = Bottom();
		fThumbStyle = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									 "thumbstyle", "Thumb Style: ",
									 new BPopUpMenu("menu"));
		fThumbStyle->Menu()->AddItem(new BMenuItem("Rectangle",
									 new BMessage(MSG_SLIDER_SET_BLOCK_THUMB)));
		fThumbStyle->Menu()->AddItem(new BMenuItem("Triangle",
									 new BMessage(MSG_SLIDER_SET_TRI_THUMB)));
		AddControl(fThumbStyle);
	}

	if (!fBarColor)
	{
		bottom = Bottom();
		fBarColor = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "barcolor", "Bar Color: ", "",
									 new BMessage(MSG_SLIDER_SET_BAR_COLOR));
		fBarColor->SetDivider(be_plain_font->StringWidth("Bar Color: "));
		AddControl(fBarColor);
	}

	if (!fUseFillCheck)
	{
		bottom = Bottom();
		fUseFillCheck = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
									  "usefillcolor", "Use Fill Color",
									  new BMessage(MSG_SLIDER_USE_FILL_COLOR));
		AddControl(fUseFillCheck);
	}

	if (!fFillColor)
	{
		bottom = Bottom();
		fFillColor = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									  "fillcolor", "Fill Color: ", "",
									  new BMessage(MSG_SLIDER_SET_FILL_COLOR));
		fFillColor->SetDivider(be_plain_font->StringWidth("Bar Thickness: "));
		AddControl(fFillColor);
	}

	if (!fBarThickness)
	{
		bottom = Bottom();
		fBarThickness = new BTextControl(BRect(10, bottom, 190, bottom + 20),
										 "barthickness", "Bar Thickness: ", "",
										 new BMessage(MSG_SLIDER_SET_THICKNESS));
		fBarThickness->SetDivider(be_plain_font->StringWidth("Bar Thickness: "));
		AddControl(fBarThickness);
	}

	if (!fSnoozeAmount)
	{
		bottom = Bottom();
		fSnoozeAmount = new BTextControl(BRect(10, bottom, 190, bottom + 20),
										 "snoozeamount", "Snooze: ", "",
										 new BMessage(MSG_SLIDER_SET_SNOOZE));
		fSnoozeAmount->SetDivider(be_plain_font->StringWidth("Snooze: "));
		AddControl(fSnoozeAmount);
	}

	if (Target())
	{
		SetThumbStyle(Target()->Style());
	}
}
//------------------------------------------------------------------------------
BSlider* BSliderEditor::Target()
{
	return dynamic_cast<BSlider*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------
void BSliderEditor::SetThumbStyle(int32 style)
{
	fThumbStyle->Menu()->
		ItemAt(B_BLOCK_THUMB)->SetMarked(style == B_BLOCK_THUMB);
	fThumbStyle->Menu()->
		ItemAt(B_TRIANGLE_THUMB)->SetMarked(style == B_TRIANGLE_THUMB);

	Target()->Invalidate();
}
//------------------------------------------------------------------------------
void BSliderEditor::SetHashStyle(int32 style)
{
	int32 oldStyle = Target()->HashMarks();
	if (oldStyle & style)
	{
		oldStyle &= ~style;
	}
	else
	{
		oldStyle |= style;
	}
	Target()->SetHashMarks(hash_mark_location(oldStyle));
	SetHashLabel(oldStyle);
}
//------------------------------------------------------------------------------
void BSliderEditor::SetHashLabel(int32 style)
{
	fHashStyle->Menu()->ItemAt(0)->SetMarked(style & B_HASH_MARKS_TOP);
	fHashStyle->Menu()->ItemAt(1)->SetMarked(style & B_HASH_MARKS_BOTTOM);

	switch (style)
	{
		case B_HASH_MARKS_NONE:
			fHashStyle->Menu()->Superitem()->SetLabel("None");
			break;
		
		case B_HASH_MARKS_TOP:
			fHashStyle->Menu()->Superitem()->
				SetLabel(fHashStyle->Menu()->ItemAt(0)->Label());
			break;

		case B_HASH_MARKS_BOTTOM:
			fHashStyle->Menu()->Superitem()->
				SetLabel(fHashStyle->Menu()->ItemAt(1)->Label());
			break;

		case B_HASH_MARKS_BOTH:
			fHashStyle->Menu()->Superitem()->SetLabel("Both");
			break;
	}

	fHashCount->SetEnabled(style != B_HASH_MARKS_NONE);
}
//------------------------------------------------------------------------------
void BSliderEditor::SetOrientation(int32 orient)
{
	switch (orient)
	{
		case B_HORIZONTAL:
			fHashStyle->Menu()->ItemAt(0)->SetLabel("Top");
			fHashStyle->Menu()->ItemAt(1)->SetLabel("Bottom");
			break;

		case B_VERTICAL:
			fHashStyle->Menu()->ItemAt(0)->SetLabel("Left");
			fHashStyle->Menu()->ItemAt(1)->SetLabel("Right");
			break;
	}

	Target()->Invalidate();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void BSlider::SetLimits(int32 minimum, int32 maximum)
{
	fMinValue = minimum;
	fMaxValue = maximum;
	Invalidate();
}
//------------------------------------------------------------------------------


/*
 * $Log $
 *
 * $Id  $
 *
 */

