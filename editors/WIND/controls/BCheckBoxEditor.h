//------------------------------------------------------------------------------
//	BCheckBoxEditor.h
//
//------------------------------------------------------------------------------

#ifndef BCHECKBOXEDITOR_H
#define BCHECKBOXEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BCheckBoxEditor : public BControlEditor
{
	public:
		BCheckBoxEditor();
		~BCheckBoxEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BCheckBox* Target();

		BCheckBox*	fChecked;
};

#endif	// BCHECKBOXEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

