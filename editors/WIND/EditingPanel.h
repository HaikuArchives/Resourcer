//------------------------------------------------------------------------------
//	EditingPanel.h
//
//------------------------------------------------------------------------------

#ifndef EDITINGPANEL_H
#define EDITINGPANEL_H

// Standard Includes -----------------------------------------------------------
#include <map>
#include <string>

// System Includes -------------------------------------------------------------
#include <app/Messenger.h>
#include <interface/View.h>
#include <Resources.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class BViewEditor;

//------------------------------------------------------------------------------
class EditingPanel : public BView
{
	public:
		EditingPanel(void);
		~EditingPanel();

		void AttachedToWindow();
		void MessageReceived(BMessage *msg);

		void SetTo(BMessage* active);

		BResources *reso;

	private:
		void InitEditors();
		void RegisterEditor(BViewEditor* editor);
		void CleanupEditors();

		typedef std::map<std::string, BViewEditor*>	EditorMap;

		EditorMap		fEditorMap;
		BViewEditor*	fCurrentEditor;
		bool			iscontrol;
		BView*			pointer;
		BMessenger		fControl;
};
//------------------------------------------------------------------------------

#endif	// EDITINGPANEL_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

