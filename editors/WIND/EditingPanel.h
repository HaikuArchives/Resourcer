//------------------------------------------------------------------------------
//	EditingPanel.h
//
//------------------------------------------------------------------------------

#ifndef EDITINGPANEL_H
#define EDITINGPANEL_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Messenger.h>
#include <interface/View.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
class EditingPanel : public BView
{
	public:
		EditingPanel(void);

		void AttachedToWindow();
		void MessageReceived(BMessage *msg);

		void SetTo(BView *active);
		void SetPart2(BView *active);

		void SetTo(BMessage* active);
		void SetPart2(BMessage* active);

		BResources *reso;

	private:
		bool		iscontrol;
		BView*		pointer;
		BMessenger	fControl;
};
//------------------------------------------------------------------------------

#endif	// EDITINGPANEL_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

