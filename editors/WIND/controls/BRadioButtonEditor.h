//------------------------------------------------------------------------------
//	BRadioButtonEditor.h
//
//------------------------------------------------------------------------------

#ifndef BRadioButtonEditor_H
#define BRadioButtonEditor_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BRadioButtonEditor : public BControlEditor
{
	public:
		BRadioButtonEditor();
		~BRadioButtonEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BRadioButton* Target();

		BCheckBox*	fChecked;
};

#endif	// BRadioButtonEditor_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

