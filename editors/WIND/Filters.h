//------------------------------------------------------------------------------
//	Filters.h
//
//------------------------------------------------------------------------------

#ifndef FILTERS_H
#define FILTERS_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/MessageFilter.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

class EditingWindow;

class DeleteFilter : public BMessageFilter {
	public:
		DeleteFilter(EditingWindow* EditingWin);
		filter_result Filter(BMessage *message,BHandler **target);

	private:
		EditingWindow*	fEditingWin;
};

class ActivateFilter : public BMessageFilter {
	public:
		ActivateFilter(EditingWindow* EditingWin);
		filter_result Filter(BMessage *message,BHandler **target);

	private:
		EditingWindow*	fEditingWin;
}; 

class DropFilter : public BMessageFilter {
	public:
		DropFilter();
		filter_result Filter(BMessage *message,BHandler **target);
};


#endif	// FILTERS_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

