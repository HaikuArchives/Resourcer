//------------------------------------------------------------------------------
//	WindowEditor.h
//
//------------------------------------------------------------------------------

#ifndef WINDOWEDITOR_H
#define WINDOWEDITOR_H

// Standard Includes -----------------------------------------------------------
#include <string>
#include <vector>

// System Includes -------------------------------------------------------------
#include <app/Messenger.h>
#include <interface/View.h>
#include <interface/Window.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "WindowEditor.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
class WindowEditor : public BView
{
	public:
		WindowEditor(BRect frame, const char* name,
					 const BMessenger& MainWindow,
					 const BMessenger& EditPanel);
		WindowEditor(BRect frame, const char* name,
					 const BMessenger& MainWindow,
					 const BMessenger& EditPanel,
					 unsigned char* data);
		~WindowEditor();


		void AttachedToWindow();
		void MessageReceived(BMessage* msg);

		status_t ArchiveWindow(BMessage* archive);

	private:
		void	Init();
		void	InitLookMenu();
		void	InitFeelMenu();
		void	InitFlagsBoxes();
		uint32	MakeBitmask();

		typedef std::vector<BCheckBox*> CheckBoxVect;

		BMenuField*		fLookField;
		BMenuField*		fFeelField;
		BTextControl*	fTitleText;
		CheckBoxVect	fFlagBoxes;

		BMessenger		fEditWindow;
		bool			fInit;

		struct WindowInfo
		{
			void SetTo(BWindow* Window);

			std::string	title;
			window_look	look;
			window_feel	feel;
			uint32		flags;
			bool		has_menu;
		} fWindowInfo;
};
//------------------------------------------------------------------------------

#endif	// WINDOWEDITOR_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

