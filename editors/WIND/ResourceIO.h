//------------------------------------------------------------------------------
//	ResourceIO.h
//
//Note: The type code is the four letter code displayed by Resourcer
//      with single quotation marks around it.
//      For instance, CSTR would be written as 'CSTR'
//------------------------------------------------------------------------------

#ifndef RESOURCEIO_H
#define RESOURCEIO_H

// Standard Includes -----------------------------------------------------------

// System Includes -------------------------------------------------------------
#include <app/Application.h>
#include <storage/Resources.h>
#include <support/DataIO.h>

// Project Includes ------------------------------------------------------------

// Local Includes --------------------------------------------------------------

// Local Defines ---------------------------------------------------------------

// Globals ---------------------------------------------------------------------

const void *AppResource(type_code type,int32 id,size_t *lengthFound);
const void *AppResource(type_code type,const char *name,size_t *lengthFound);
status_t get_app_resource(type_code type,const char *name,void** buffer,size_t *lengthFound);
status_t get_app_resource(type_code type,long id,void** buffer,size_t *lengthFound);

class BResources;

//------------------------------------------------------------------------------
class ResourceIO : public BPositionIO {
	public:
		ResourceIO(type_code type,int32 id,BResources *res = be_app->AppResources());
		ResourceIO(type_code type,const char *name,BResources *res = be_app->AppResources());
		ssize_t Read(void *buffer,size_t numBytes);
		ssize_t ReadAt(off_t position,void *buffer,size_t numBytes);
		off_t Seek(off_t position,uint32 mode);
		off_t Position(void) const;
		ssize_t Write(const void *buffer,size_t numBytes);
		ssize_t WriteAt(off_t position,const void *buffer,size_t numBytes);
		size_t Size(void);
		status_t SetSize(off_t size);
		int32 ID(void);
	private:
		int32 id;
		type_code type;
		off_t curPos;
		char *name;
		BResources *res;
};
//------------------------------------------------------------------------------

#endif	// RESOURCEIO_H

/*
 * $Log $
 *
 * $Id  $
 *
 */

