//------------------------------------------------------------------------------
//	BControlEditor.h
//
//------------------------------------------------------------------------------

#ifndef BCONTROLEDITOR_H
#define BCONTROLEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BCheckBox;

class BControlEditor : public BViewEditor
{
	public:
		BControlEditor();
		~BControlEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	protected:
		BControlEditor(bool showLabel);

		BControl* Target();

	private:
		void Init();

		BTextControl*	fLabelText;
		BTextControl*	fMsgText;
		BCheckBox*		fEnabled;
		bool			fShowLabel;
};

#endif	// BCONTROLEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

