//------------------------------------------------------------------------------
//	WIND.h
//
//------------------------------------------------------------------------------

#ifndef WIND_H
#define WIND_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <support/SupportDefs.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------
#define MSG_MENU_FIELD_SET_LABEL	-1100
#define MSG_MENU_SET_DEFAULT_TEXT	-1101
#define	MSG_MENU_NEW_ITEM			-4000
#define MSG_MENU_SET_ITEM_LABEL		-4001
#define MSG_MENU_SET_ITEM_MSG		-4002
#define MSG_MENU_SET_ITEM_SHORTCUT	-4003
#define MSG_MENU_SET_ITEM_MARKED	-4004
#define MSG_MENU_SET_ITEM_ENABLED	-4005
#define MSG_MENU_ADD_SEPARATOR		-4006

#define MSG_VIEW_SET_NAME			-400

#define MSG_CONTROL_SET_LABEL		-500
#define MSG_CONTROL_SET_MSG			-600

#define MSG_SLIDER_SET_BLOCK_THUMB	-700
#define MSG_SLIDER_SET_TRI_THUMB	-701
#define MSG_SLIDER_SET_LIMIT_LABELS	-750
#define MSG_STRING_VIEW_SET_TEXT	-800

#define MSG_BOX_SET_LABEL			-900
#define MSG_BOX_FANCY_BORDER		-901
#define MSG_BOX_PLAIN_BORDER		-902
#define MSG_BOX_NO_BORDER			-903

#define MSG_RES_IMAGE_VIEW_SET_ID	-1000
#define MSG_RES_STRING_VIEW_SET_ID	-1300

#define MSG_WINDOW_SET_TITLE		-100
#define MSG_WINDOW_SET_LOOK			-200
#define MSG_WINDOW_SET_FLAG			-250
#define MSG_WINDOW_ADD_MENU			-258
#define MSG_WINDOW_SET_FEEL			-300
#define MSG_WINDOW_ARCHIVE			-350
#define MSG_WINDOW_QUIT				-375

#define MSG_COLOR_CONTROL_SET_ALIGN	-1200

// Globals ---------------------------------------------------------------------
#if __POWERPC__
	#define flipcode(o) o
#endif
#if __INTEL__
	int32 flipcode(int32 original);
#endif

int32 track_over(void *view);

#endif	// WIND_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

