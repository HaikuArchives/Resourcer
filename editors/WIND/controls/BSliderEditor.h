//------------------------------------------------------------------------------
//	BSliderEditor.h
//
//------------------------------------------------------------------------------

#ifndef BSLIDEREDITOR_H
#define BSLIDEREDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <Slider.h>

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
		void SetThumbStyle(int32 style);
		void SetHashStyle(int32 style);
		void SetHashLabel(int32 style);
		void SetOrientation(int32 orient);

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
		BTextControl*	fBarThickness;
		BCheckBox*		fUseFillCheck;
		BMenuField*		fOrientation;
		BMenuField*		fHashStyle;
		BMenuField*		fThumbStyle;
};

#endif	// BSLIDEREDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

