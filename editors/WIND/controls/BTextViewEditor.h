//------------------------------------------------------------------------------
//	BTextViewEditor.h
//
//------------------------------------------------------------------------------

#ifndef BTEXTVIEWEDITOR_H
#define BTEXTVIEWEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <support/String.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BTextViewEditor : public BViewEditor
{
	public:
		BTextViewEditor(bool isTextControlChild = false);
		~BTextViewEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void		Init();
		BTextView*	Target();
		void		BuildColorSpaceMenu();
		void		SetColorSpace(int32 colorspace);

		bool			fIsTextControlChild;

		BCheckBox*		fAutoindent;
		BCheckBox*		fEditable;
		BCheckBox*		fSelectable;
		BCheckBox*		fStylable;
		BCheckBox*		fWordWrap;
		BMenuField*		fAlignment;
		BMenuField*		fColorSpace;
		BTextControl*	fDisallowedChars;
		BTextControl*	fMaxBytes;
		BTextControl*	fTabWidth;
		BTextControl*	fTextRect;

		BString			fDisallowedBuffer;
};

#endif	// BTEXTVIEWEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

