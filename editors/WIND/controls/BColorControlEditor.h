//------------------------------------------------------------------------------
//	BColorControlEditor.h
//
//------------------------------------------------------------------------------

#ifndef BCOLORCONTROLEDITOR_H
#define BCOLORCONTROLEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BMenuField;

class BColorControlEditor : public BControlEditor
{
	public:
		BColorControlEditor();
		~BColorControlEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BColorControl* Target();
		void SetLayout(int32 layout);
		void SetLayout(BColorControl* Control);

		BTextControl*	fCellSize;
		BMenuField*		fLayoutField;
};

#endif	// BCOLORCONTROLEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

