//------------------------------------------------------------------------------
//	BMenuBarEditor.h
//
//------------------------------------------------------------------------------

#ifndef BMENUBAREDITOR_H
#define BMENUBAREDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BViewEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class MenuEditor;

class BMenuBarEditor : public BViewEditor
{
	public:
		BMenuBarEditor();
		~BMenuBarEditor();

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

	private:
		void Init();
		BMenuBar* Target();

		MenuEditor*	fMenuEditor;
};

#endif	// BMENUBAREDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

