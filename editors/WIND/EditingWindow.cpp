//------------------------------------------------------------------------------
//	EditingWindow.cpp
//
//------------------------------------------------------------------------------

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Message.h>
#include <interface/MenuBar.h>
#include <interface/MenuItem.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------
#include "EditingControl.h"
#include "EditingWindow.h"
#include "Filters.h"
#include "WIND.h"

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

//------------------------------------------------------------------------------
EditingWindow::EditingWindow(void)
	:	BWindow(BRect(100, 100, 350, 450), "New Window", B_TITLED_WINDOW,
				B_ASYNCHRONOUS_CONTROLS)
{
	Init();
}
//------------------------------------------------------------------------------
EditingWindow::EditingWindow(BMessage* archive)
	:	BWindow(archive)
{
	Init();
}
//------------------------------------------------------------------------------
status_t EditingWindow::Archive(BMessage* archive, bool deep) const
{
	EditingWindow* self = const_cast<EditingWindow*>(this);
	if (!self)
		debugger("Couldn't const_cast \"this\"");

	self->Deinit();

	status_t err = BWindow::Archive(archive, deep);
	if (!err)
	{
		err = archive->ReplaceInt32("_wfeel", danger_feel);
	}

	self->Init(false);

	return err;
}
//------------------------------------------------------------------------------
void EditingWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case MSG_WINDOW_SET_TITLE:
		{
			const char* title;
			msg->FindString("title", &title);
			SetTitle(title);
			break;
		}

		case MSG_WINDOW_SET_LOOK:
		{
			int32 look = Look();
			msg->FindInt32("newlook", &look);
			SetLook(window_look(look));
			break;
		}
		case MSG_WINDOW_SET_FLAG:
		{
			int32 flags = Flags();
			msg->FindInt32("flags", &flags);
			SetFlags(flags);
			break;
		}
		case MSG_WINDOW_ADD_MENU:
		{
			bool addmenu = false;
			msg->FindBool("addmenu", &addmenu);
			if (addmenu)
			{
				BMenuBar* mbar;
				mbar = new BMenuBar(BRect(0,0,Frame().right,30),"menubar");
				mbar->AddItem(new BMenuItem("New Menu Bar",new BMessage('none')));
				AddChild(mbar);
			}
			else
			{
				if (cntrl->controlled == KeyMenuBar())
					cntrl->KeyDown(new char(B_DELETE),1);
				else
					KeyMenuBar()->RemoveSelf();
			}
			break;
		}

		case MSG_WINDOW_SET_FEEL:
		{
			int32 feel = Feel();
			msg->FindInt32("newfeel", &feel);
			danger_feel = window_feel(feel);
			if ((danger_feel == B_MODAL_APP_WINDOW_FEEL) ||
				(danger_feel == B_MODAL_ALL_WINDOW_FEEL))
			{
				SetFeel(B_NORMAL_WINDOW_FEEL);
			}
			else
			{
				SetFeel(danger_feel);
			}
			break;
		}

		case MSG_WINDOW_ARCHIVE:
		{
			BMessage* archive;
			if (msg->FindPointer("archive", (void**)&archive) == B_OK)
			{
				status_t err = Archive(archive);
				BMessage reply;
				reply.AddInt32("err", err);
				reply.AddPointer("archive", archive);
				msg->SendReply(&reply);
			}
		}

		case MSG_WINDOW_QUIT:
			Quit();
			break;

		case MSG_VIEW_SET_NAME:
		{
			const char* name;
			BView* View;
			if (msg->FindString("name", &name) == B_OK)
			{
				if (msg->FindPointer("view", (void**)&View) == B_OK)
				{
					View->SetName(name);
				}
			}
			break;
		}

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}
//------------------------------------------------------------------------------
bool EditingWindow::QuitRequested(void)
{
	fMainWindow.SendMessage(B_QUIT_REQUESTED);
	return false;
}
//------------------------------------------------------------------------------
void EditingWindow::SetMainWindow(const BMessenger& target)
{
	fMainWindow = target;
}
//------------------------------------------------------------------------------
void EditingWindow::SetEditPanel(const BMessenger& target)
{
	fEditWindow = target;
}
//------------------------------------------------------------------------------
void EditingWindow::Init(bool fullInit)
{
	cntrl = new EditingControl;
	AddChild(cntrl);

	if (fullInit)
	{
		SetPulseRate(100000);
		delfilt = new DeleteFilter(this);
		actfilt = new ActivateFilter(this);
		dropfilt = new DropFilter;
	}

	AddCommonFilter(delfilt);
	AddCommonFilter(actfilt);
	AddCommonFilter(dropfilt);
}
//------------------------------------------------------------------------------
void EditingWindow::Deinit()
{
	cntrl->RemoveSelf();
	delete cntrl;

	RemoveCommonFilter(delfilt);
	RemoveCommonFilter(actfilt);
	RemoveCommonFilter(dropfilt);
}
//------------------------------------------------------------------------------

/*
 * $Log $
 *
 * $Id  $
 *
 */

