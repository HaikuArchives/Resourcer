//------------------------------------------------------------------------------
//	EditingControl.h
//
//------------------------------------------------------------------------------

#ifndef EDITINGCONTROL_H
#define EDITINGCONTROL_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <interface/View.h>
// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
class EditingControl : public BView
{
	public:
		EditingControl(void);

		void Draw(BRect area);
		void KeyDown(const char *bytes,int32 numBytes);
		void Pulse(void);
		void MouseDown(BPoint point);

		BRect edge;
		BView *controlled;
		bool alreadyDrawing;
		bool notInit;
};
//------------------------------------------------------------------------------

#endif	// EDITINGCONTROL_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

