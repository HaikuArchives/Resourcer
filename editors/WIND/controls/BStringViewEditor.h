//------------------------------------------------------------------------------
//	BStringViewEditor.h
//
//------------------------------------------------------------------------------

#ifndef BSTRINGVIEWEDITOR_H
#define BSTRINGVIEWEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BRadioButton;

class BStringViewEditor : public BViewEditor
{
	public:
		BStringViewEditor();
		~BStringViewEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BStringView* Target();
		void SetButtons(int32 align);

		BTextControl*	fLabelText;
		BRadioButton*	fLeftAlign;
		BRadioButton*	fCenterAlign;
		BRadioButton*	fRightAlign;
};

#endif	// BSTRINGVIEWEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

