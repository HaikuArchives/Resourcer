//------------------------------------------------------------------------------
//	BBoxEditor.h
//
//------------------------------------------------------------------------------

#ifndef BBOXEDITOR_H
#define BBOXEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BMenuField;
class BTextControl;

class BBoxEditor : public BViewEditor
{
	public:
		BBoxEditor();
		~BBoxEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BBox* Target();
		void SetStyle(int32 style);

		BTextControl*	fLabelText;
		BMenuField*		fBorderStyle;
};

#endif	// BBOXEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

