//------------------------------------------------------------------------------
//	BButtonEditor.h
//
//------------------------------------------------------------------------------

#ifndef BBUTTONEDITOR_H
#define BBUTTONEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "BControlEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BButtonEditor : public BControlEditor
{
	public:
		BButtonEditor() {;}
		~BButtonEditor() {;}

		virtual const char*	Supports();
};

#endif	// BBUTTONEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

