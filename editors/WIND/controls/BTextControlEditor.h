//------------------------------------------------------------------------------
//	BTextControlEditor.h
//
//------------------------------------------------------------------------------

#ifndef BTEXTCONTROLEDITOR_H
#define BTEXTCONTROLEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BTextViewEditor;

class BTextControlEditor : public BControlEditor
{
	public:
		BTextControlEditor();
		~BTextControlEditor();

		virtual void SetParent(BView* parent);
		virtual void SetTarget(BView* target);

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void			Init();
		BTextControl*	Target();
		void			GetAlignments(alignment& label, alignment& text);

		BTextControl*		fModMsg;
		BTextControl*		fText;
		BTextControl*		fDivider;
		BMenuField*			fLabelAlignment;
		BMenuField*			fTextAlignment;

		BTextViewEditor*	fTextViewEditor;
};

#endif	// BTEXTCONTROLEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

