//------------------------------------------------------------------------------
//	EditingWindow.h
//
//------------------------------------------------------------------------------

#ifndef EDITINGWINDOW_H
#define EDITINGWINDOW_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Messenger.h>
#include <interface/Window.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class ActivateFilter;
class DeleteFilter;
class DropFilter;
class EditingControl;

//------------------------------------------------------------------------------
class EditingWindow : public BWindow
{
	public:
		EditingWindow(void);
		EditingWindow(BMessage *archive);

		status_t Archive(BMessage* archive, bool deep = true) const;
		void MessageReceived(BMessage* msg);
		bool QuitRequested(void);

		void SetMainWindow(const BMessenger& target);
		void SetEditPanel(const BMessenger& target);

		BView*			curfocus;
		EditingControl*	cntrl;
		DeleteFilter*	delfilt;
		ActivateFilter*	actfilt;
		DropFilter*		dropfilt;
		window_feel		danger_feel;

	private:
		void Init(bool fullInit = true);
		void Deinit();

		BMessenger		fMainWindow;
		BMessenger		fEditWindow;
};
//------------------------------------------------------------------------------

#endif	// EDITINGWINDOW_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

