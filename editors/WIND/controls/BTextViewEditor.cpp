//------------------------------------------------------------------------------
//	BTextViewEditor.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/CheckBox.h>
#include <interface/MenuField.h>
#include <interface/MenuItem.h>
#include <interface/PopUpMenu.h>
#include <interface/TextControl.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BTextViewEditor.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------
#define MSG_TEXT_VIEW_AUTOINDENT		-200
#define MSG_TEXT_VIEW_EDITABLE			-205
#define MSG_TEXT_VIEW_SELECTABLE		-210
#define MSG_TEXT_VIEW_STYLABLE			-215
#define MSG_TEXT_VIEW_WORDWRAP			-220
#define MSG_TEXT_VIEW_ALIGN_LEFT		-225
#define MSG_TEXT_VIEW_ALIGN_RIGHT		-230
#define MSG_TEXT_VIEW_ALIGN_CENTER		-235
#define MSG_TEXT_VIEW_SET_COLOR_SPACE	-240
#define MSG_TEXT_VIEW_DISALLOWED_CHARS	-245
#define MSG_TEXT_VIEW_MAX_BYTES			-250
#define MSG_TEXT_VIEW_TAB_WIDTH			-255
#define MSG_TEXT_VIEW_TEXT_RECT			-260

// Globals ---------------------------------------------------------------------
struct ColorSpaceInfo
{
	int32		code;
	const char*	name;
};

ColorSpaceInfo ColorSpaces[] =
{
	{ B_RGB32,		"B_RGB32"		},	{ B_RGBA32,		"B_RGBA32"		},
	{ B_RGB24,		"B_RGB24"		},	{ B_RGB16,		"B_RGB16"		},
	{ B_RGB15,		"B_RGB15"		},	{ B_RGBA15,		"B_RGBA15"		},
	{ B_CMAP8,		"B_CMAP8"		},	{ B_GRAY8,		"B_GRAY8"		},
	{ B_GRAY1,		"B_GRAY1"		},	{ B_RGB32_BIG,	"B_RGB32_BIG"	},
	{ B_RGBA32_BIG,	"B_RGBA32_BIG"	},	{ B_RGB24_BIG,	"B_RGB24_BIG"	},
	{ B_RGB16_BIG,	"B_RGB16_BIG"	},	{ B_RGB15_BIG,	"B_RGB15_BIG"	},
	{ B_RGBA15_BIG,	"B_RGBA15_BIG"	},	{ B_YCbCr422,	"B_YCbCr422"	},
	{ B_YCbCr411,	"B_YCbCr411"	},	{ B_YCbCr444,	"B_YCbCr444"	},
	{ B_YCbCr420,	"B_YCbCr420"	},	{ B_YUV422,		"B_YUV422"		},
	{ B_YUV411,		"B_YUV411"		},	{ B_YUV444,		"B_YUV444"		},
	{ B_YUV420,		"B_YUV420"		},	{ B_YUV9,		"B_YUV9"		},
	{ B_YUV12,		"B_YUV12"		},	{ B_UVL24,		"B_UVL24"		},
	{ B_UVL32,		"B_UVL32"		},	{ B_UVLA32,		"B_UVLA32"		},
	{ B_LAB24,		"B_LAB24"		},	{ B_LAB32,		"B_LAB32"		},
	{ B_LABA32,		"B_LABA32"		},	{ B_HSI24,		"B_HSI24"		},
	{ B_HSI32,		"B_HSI32"		},	{ B_HSIA32,		"B_HSIA32"		},
	{ B_HSV24,		"B_HSV24"		},	{ B_HSV32,		"B_HSV32"		},
	{ B_HSVA32,		"B_HSVA32"		},	{ B_HLS24,		"B_HLS24"		},
	{ B_HLS32,		"B_HLS32"		},	{ B_HLSA32,		"B_HLSA32"		},
	{ B_CMY24,		"B_CMY24"		},	{ B_CMY32,		"B_CMY32"		},
	{ B_CMYA32,		"B_CMYA32"		},	{ B_CMYK32,		"B_CMYK32"		},
	{ 0, 0 }
};

//------------------------------------------------------------------------------
BTextViewEditor::BTextViewEditor(bool isTextControlChild)
	:	fIsTextControlChild(isTextControlChild),
		fAutoindent(NULL),
		fEditable(NULL),
		fSelectable(NULL),
		fStylable(NULL),
		fWordWrap(NULL),
		fAlignment(NULL),
		fColorSpace(NULL),
		fDisallowedChars(NULL),
		fMaxBytes(NULL),
		fTabWidth(NULL),
		fTextRect(NULL)
{
}
//------------------------------------------------------------------------------
BTextViewEditor::~BTextViewEditor()
{
}
//------------------------------------------------------------------------------
void BTextViewEditor::SetTo(BMessage* archive)
{
	if (!fIsTextControlChild)
		BViewEditor::SetTo(archive);
	Init();

	if (!fIsTextControlChild)
	{
		int32 align = B_ALIGN_LEFT;
		archive->FindInt32("_align", &align);
		fAlignment->Menu()->ItemAt(align)->SetMarked(true);
	}

	if (!fIsTextControlChild)
	{
		float tabwidth;
		if (archive->FindFloat("_tab", &tabwidth))
		{
			DEBUG_ARCHIVE("_tab");
		}
		fTabWidth->SetText((BString() << tabwidth).String());
	}

	int32 colorspace;
	if (archive->FindInt32("_col_sp", &colorspace))
	{
		if (fIsTextControlChild && !colorspace)
		{
			colorspace = Target()->ColorSpace();
		}
		else
		{
			DEBUG_ARCHIVE("_col_sp");
		}
	}
	SetColorSpace(colorspace);

	int32 maxbytes;
	if (archive->FindInt32("_max", &maxbytes))
	{
		if (fIsTextControlChild)
		{
			maxbytes = Target()->MaxBytes();
		}
		else
		{
			DEBUG_ARCHIVE("_max");
		}
	}
	fMaxBytes->SetText((BString() << maxbytes).String());

	bool stylable = false;
	archive->FindBool("_stylable", &stylable);
	fStylable->SetValue(stylable);

	if (!fIsTextControlChild)
	{
		bool autoindent = false;
		archive->FindBool("_auto_in", &autoindent);
		fAutoindent->SetValue(autoindent);
	}

	if (!fIsTextControlChild)
	{
		bool wordwrap = false;
		archive->FindBool("_wrap", &wordwrap);
		fWordWrap->SetValue(wordwrap);
	}

	bool editable = true;
	if (archive->FindBool("_nedit", &editable))
	{
		editable = true;
	}
	fEditable->SetValue(editable);

	bool selectable = true;
	if (archive->FindBool("_nsel", &selectable))
	{
		selectable = true;
	}
	fSelectable->SetValue(selectable);

	if (!fIsTextControlChild)
		fAutoindent->SetTarget(Parent());
	fEditable->SetTarget(Parent());
	fSelectable->SetTarget(Parent());
	fStylable->SetTarget(Parent());
	if (!fIsTextControlChild)
		fWordWrap->SetTarget(Parent());
	if (!fIsTextControlChild)
		fAlignment->Menu()->SetTargetForItems(Parent());
	fColorSpace->Menu()->SetTargetForItems(Parent());
	fDisallowedChars->SetTarget(Parent());
	fMaxBytes->SetTarget(Parent());
	if (!fIsTextControlChild)
		fTabWidth->SetTarget(Parent());
	if (!fIsTextControlChild)
		fTextRect->SetTarget(Parent());

	if (!fIsTextControlChild)
		Parent()->AddChild(fAutoindent);
	Parent()->AddChild(fEditable);
	Parent()->AddChild(fSelectable);
	Parent()->AddChild(fStylable);
	if (!fIsTextControlChild)
		Parent()->AddChild(fWordWrap);
	if (!fIsTextControlChild)
		Parent()->AddChild(fAlignment);
	Parent()->AddChild(fColorSpace);
	Parent()->AddChild(fDisallowedChars);
	Parent()->AddChild(fMaxBytes);
	if (!fIsTextControlChild)
		Parent()->AddChild(fTabWidth);
	if (!fIsTextControlChild)
		Parent()->AddChild(fTextRect);
}
//------------------------------------------------------------------------------
bool BTextViewEditor::MessageReceived(BMessage* msg)
{
	switch (msg->what)
	{
		case MSG_TEXT_VIEW_AUTOINDENT:
			Target()->SetAutoindent(fAutoindent->Value());
			return true;

		case MSG_TEXT_VIEW_EDITABLE:
			Target()->MakeEditable(fEditable->Value());
			return true;

		case MSG_TEXT_VIEW_SELECTABLE:
			Target()->MakeSelectable(fSelectable->Value());
			return true;

		case MSG_TEXT_VIEW_STYLABLE:
			Target()->SetStylable(fStylable->Value());
			return true;

		case MSG_TEXT_VIEW_WORDWRAP:
			Target()->SetWordWrap(fWordWrap->Value());
			return true;

		case MSG_TEXT_VIEW_ALIGN_LEFT:
			Target()->SetAlignment(B_ALIGN_LEFT);
			return true;

		case MSG_TEXT_VIEW_ALIGN_RIGHT:
			Target()->SetAlignment(B_ALIGN_RIGHT);
			return true;

		case MSG_TEXT_VIEW_ALIGN_CENTER:
			Target()->SetAlignment(B_ALIGN_CENTER);
			return true;

		case MSG_TEXT_VIEW_SET_COLOR_SPACE:
		{
			color_space cspace;
			if (msg->FindInt32("colorspace", (int32*)&cspace) == B_OK)
			{
				Target()->SetColorSpace(cspace);
			}
			return true;
		}

		case MSG_TEXT_VIEW_DISALLOWED_CHARS:
		{
			for (int32 i = 0; i < fDisallowedBuffer.Length(); ++i)
			{
				Target()->AllowChar(fDisallowedBuffer[i]);
			}
			const char* text = fDisallowedChars->Text();
			int32 len = strlen(text);
			for (int32 i = 0; i < len; ++i)
			{
				Target()->DisallowChar(text[i]);
			}
			return true;
		}

		case MSG_TEXT_VIEW_MAX_BYTES:
			Target()->SetMaxBytes(atoi(fMaxBytes->Text()));
			return true;

		case MSG_TEXT_VIEW_TAB_WIDTH:
			Target()->SetTabWidth(atof(fTabWidth->Text()));
			return true;

		case MSG_TEXT_VIEW_TEXT_RECT:
			// TODO: implement
			return true;

		default:
			return BViewEditor::MessageReceived(msg);
	}
}
//------------------------------------------------------------------------------
const char* BTextViewEditor::Supports()
{
	return "BTextView";
}
//------------------------------------------------------------------------------
void BTextViewEditor::Init()
{
	int bottom;
	if (!fAutoindent && !fIsTextControlChild)
	{
		bottom = Bottom();
		fAutoindent = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
									"autoindent", "Autoindent",
									new BMessage(MSG_TEXT_VIEW_AUTOINDENT));
		AddControl(fAutoindent);
	}

	if (!fEditable)
	{
		bottom = Bottom();
		fEditable = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
								  "editable", "Editable",
								  new BMessage(MSG_TEXT_VIEW_EDITABLE));
		AddControl(fEditable);
	}

	if (!fSelectable)
	{
		bottom = Bottom();
		fSelectable = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
									"selectable", "Selectable",
									new BMessage(MSG_TEXT_VIEW_SELECTABLE));
		AddControl(fSelectable);
	}

	if (!fStylable)
	{
		bottom = Bottom();
		fStylable = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
								  "stylable", "Stylable",
								  new BMessage(MSG_TEXT_VIEW_STYLABLE));
		AddControl(fStylable);
	}

	if (!fWordWrap && !fIsTextControlChild)
	{
		bottom = Bottom();
		fWordWrap = new BCheckBox(BRect(10, bottom, 190, bottom + 20),
								  "wordwrap", "Word Wrap",
								  new BMessage(MSG_TEXT_VIEW_WORDWRAP));
		AddControl(fWordWrap);
	}

	if (!fAlignment && !fIsTextControlChild)
	{
		bottom = Bottom();
		fAlignment = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									"alignment", "Alignment: ",
									new BPopUpMenu("menu"));
		fAlignment->Menu()->AddItem(new BMenuItem("Left",
									new BMessage(MSG_TEXT_VIEW_ALIGN_LEFT)));
		fAlignment->Menu()->AddItem(new BMenuItem("Right",
									new BMessage(MSG_TEXT_VIEW_ALIGN_RIGHT)));
		fAlignment->Menu()->AddItem(new BMenuItem("Center",
									new BMessage(MSG_TEXT_VIEW_ALIGN_CENTER)));
		fAlignment->SetDivider(be_plain_font->StringWidth("Alignment: "));
		AddControl(fAlignment);
	}

	if (!fColorSpace)
	{
		bottom = Bottom();
		fColorSpace = new BMenuField(BRect(10, bottom, 190, bottom + 20),
									 "colorspace", "Color Space: ",
									 new BPopUpMenu("menu"));
		BMessage* msg;
		for (int32 i = 0; ColorSpaces[i].name; ++i)
		{
			msg = new BMessage(MSG_TEXT_VIEW_SET_COLOR_SPACE);
			msg->AddInt32("colorspace", ColorSpaces[i].code);
			fColorSpace->Menu()->AddItem(new BMenuItem(ColorSpaces[i].name, msg));
		}
		fColorSpace->SetDivider(be_plain_font->StringWidth("Color Space: "));
		AddControl(fColorSpace);
	}

	if (!fDisallowedChars)
	{
		bottom = Bottom();
		fDisallowedChars = new BTextControl(BRect(10, bottom, 190, bottom + 20),
											"disallowedchars",
											"Disallowed Characters: ", "",
											new BMessage(MSG_TEXT_VIEW_DISALLOWED_CHARS));
		fDisallowedChars->SetDivider(be_plain_font->StringWidth("Disallowed Characters: "));
		AddControl(fDisallowedChars);
	}

	if (!fMaxBytes)
	{
		bottom = Bottom();
		fMaxBytes = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "maxbytes", "Max Bytes: ", "",
									 new BMessage(MSG_TEXT_VIEW_MAX_BYTES));
		fMaxBytes->SetDivider(be_plain_font->StringWidth("Max Bytes: "));
		AddControl(fMaxBytes);
	}

	if (!fTabWidth && !fIsTextControlChild)
	{
		bottom = Bottom();
		fTabWidth = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "tabwidth", "Tab Width: ", "",
									 new BMessage(MSG_TEXT_VIEW_TAB_WIDTH));
		fTabWidth->SetDivider(be_plain_font->StringWidth("Tab Width: "));
		AddControl(fTabWidth);
	}

	if (!fTextRect && !fIsTextControlChild)
	{
		bottom = Bottom();
		fTextRect = new BTextControl(BRect(10, bottom, 190, bottom + 20),
									 "textrect", "Text Rect: ", "",
									 new BMessage(MSG_TEXT_VIEW_TEXT_RECT));
		fTextRect->SetDivider(be_plain_font->StringWidth("Text Rect: "));
		AddControl(fTextRect);
	}
}
//------------------------------------------------------------------------------
BTextView* BTextViewEditor::Target()
{
	return dynamic_cast<BTextView*>(BViewEditor::Target());
}
//------------------------------------------------------------------------------
void BTextViewEditor::BuildColorSpaceMenu()
{
}
//------------------------------------------------------------------------------
void BTextViewEditor::SetColorSpace(int32 colorspace)
{
	for (int32 i = 0; ColorSpaces[i].name; ++i)
	{
		if (colorspace == ColorSpaces[i].code)
		{
			fColorSpace->Menu()->ItemAt(i)->SetMarked(true);
			return;
		}
	}
	debugger("Bad colorspace specification");
}
//------------------------------------------------------------------------------


/*
 * $Log $
 *
 * $Id  $
 *
 */

