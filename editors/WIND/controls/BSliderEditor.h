//------------------------------------------------------------------------------
//	BSliderEditor.h
//
//------------------------------------------------------------------------------

#ifndef BSLIDEREDITOR_H
#define BSLIDEREDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BMenuField;

class BSliderEditor : public BControlEditor
{
	public:
		BSliderEditor();
		~BSliderEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BSlider* Target();
		void SetStyle(int32 style);

		BTextControl*	fModMsg;
		BTextControl*	fSnoozeAmount;
		BTextControl*	fBarColor;
		BTextControl*	fFillColor;
		BTextControl*	fMinLabel;
		BTextControl*	fMaxLabel;
		BTextControl*	fMinValue;
		BTextControl*	fMaxValue;
		BTextControl*	fIncrValue;
		BTextControl*	fHashCount;
		BTextControl*	fHashLocation;
		BMenuField*		fThumbStyle;
};

#endif	// BSLIDEREDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

