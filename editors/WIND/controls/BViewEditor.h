//------------------------------------------------------------------------------
//	BViewEditor.h
//
//------------------------------------------------------------------------------

#ifndef BVIEWEDITOR_H
#define BVIEWEDITOR_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Messenger.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BStringView;
class BTextControl;
class BView;
class BWindow;

class ControlKeeper;

class BViewEditor
{
	public:
		BViewEditor();
		virtual ~BViewEditor();

		void SetParent(BView* parent);
		void SetTarget(BView* target);

		virtual void		SetTo(BMessage* archive);
		virtual bool		MessageReceived(BMessage* msg);
		virtual const char*	Supports();

		BWindow* Window();

		BMessenger&	Messenger();

	protected:

		int	Bottom();
		void SetBottom(int bottom);
		void AddControl(BView* control);
		BView* Parent();
		BView* Target();

	private:
		void Init();

		BMessenger		fControl;
		BStringView*	fClassName;
		BTextControl*	fViewName;
		BView*			fParent;
		BView*			fTarget;
		int				fBottom;
		ControlKeeper*	fControlKeeper;
};

#endif	// BVIEWEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

