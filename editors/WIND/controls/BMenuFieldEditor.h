//------------------------------------------------------------------------------
//	BMenuFieldEditor.h
//
//------------------------------------------------------------------------------

#ifndef BMENUFIELDEDITOR_H
#define BMENUFIELDEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class MenuEditor;

class BMenuFieldEditor : public BViewEditor
{
	public:
		BMenuFieldEditor();
		~BMenuFieldEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BMenuField* Target();

		BTextControl*	fLabelText;
		BTextControl*	fDefaultText;
		MenuEditor*		fMenuEditor;
};

#endif	// BMENUFIELDEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

