//Note: The type code is the four letter code displayed by Resourcer
//      with single quotation marks around it.
//      For instance, CSTR would be written as 'CSTR'


#include <storage/Resources.h>
#include <support/DataIO.h>

const void *AppResource(type_code type,int32 id,size_t *lengthFound);
const void *AppResource(type_code type,const char *name,size_t *lengthFound);
status_t get_app_resource(type_code type,const char *name,void** buffer,size_t *lengthFound);
status_t get_app_resource(type_code type,long id,void** buffer,size_t *lengthFound);

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


const void *AppResource(type_code type,int32 id,size_t *lengthFound) {
	return be_app->AppResources()->LoadResource(type,id,lengthFound);
}

const void *AppResource(type_code type,const char *name,size_t *lengthFound) {
	return be_app->AppResources()->LoadResource(type,name,lengthFound);
}

ResourceIO::ResourceIO(type_code type,int32 id,BResources *res) : BPositionIO() {
	this->id = id;
	this->type = type;
	this->res = res;
	name = NULL;
	curPos = 0;
}

ResourceIO::ResourceIO(type_code type,const char *name,BResources *res) : BPositionIO() {
	this->type = type;
	this->res = res;
	this->name = new char[strlen(name) + 1];
	strcpy(this->name,name);
	int32 id2;
	size_t useless;
	res->GetResourceInfo(type,name,&id2,&useless);
	id = id2;
	curPos = 0;
}

ssize_t ResourceIO::Read(void *buffer,size_t numBytes) {
	ssize_t temp = ReadAt(curPos,buffer,numBytes);
	curPos += temp;
	return temp;
}

ssize_t ResourceIO::ReadAt(off_t position,void *buffer,size_t numBytes) {
	size_t bytesRead;
	const void *buf = res->LoadResource(type,id,&bytesRead);
	if (buf == NULL)
		return 0;
	if (position > bytesRead)
		return B_ERROR;
	if ((bytesRead - position) > numBytes)
		bytesRead = numBytes;
	else
		bytesRead -= position;
	memcpy(buffer,(((unsigned char *)(buf)) + position),bytesRead);
	return (ssize_t)(bytesRead);
}

off_t ResourceIO::Seek(off_t position,uint32 mode) {
	size_t length = Size();
	switch (mode) {
		case SEEK_SET:
			curPos = position;
			break;
		case SEEK_CUR:
			curPos += position;
			break;
		case SEEK_END:
			curPos = length + position;
			break;
	}
	return (off_t)curPos;
}

off_t ResourceIO::Position(void) const{
	return (off_t)curPos;
}

ssize_t ResourceIO::Write(const void *buffer,size_t numBytes) {
	ssize_t temp = WriteAt(curPos,buffer,numBytes);
	curPos += temp;
	return temp;
}

ssize_t ResourceIO::WriteAt(off_t position,const void *buffer,size_t numBytes) {
	res->WriteResource(type,id,buffer,position,numBytes);
	return numBytes;
}

size_t ResourceIO::Size(void) {
	const char *name;
	size_t length;
	res->GetResourceInfo(type,id,&name,&length);
	return length;
}

status_t ResourceIO::SetSize(off_t size) {
	unsigned char *buffer = new unsigned char[size];
	for (off_t i = 0; i < size; i++) {
		buffer[i] = 0;				//Zero the data
	}
	ReadAt(0,buffer,size);
	res->RemoveResource(type,id);
	res->AddResource(type,id,buffer,size,name);
	return B_OK;
}

int32 ResourceIO::ID(void) {
	return ((int32)(id));
}

//----------------FOR COMPATIBILITY ONLY----------------------

status_t get_app_resource(type_code type,long id,void** buffer/*<-- do not initialize*/,size_t *lengthFound) {
	char *fiddle;
	if (be_app->AppResources()->GetResourceInfo(type,id,&fiddle,lengthFound) == false)
		return B_ERROR;
	*buffer = new unsigned char[*lengthFound];
	if (be_app->AppResources()->ReadResource(type,id,*buffer,0,*lengthFound) != B_OK)
		return B_ERROR;
	return B_OK;
}

status_t get_app_resource(type_code type,const char *name,void** buffer/*<-- do not initialize*/,size_t *lengthFound) {
	long id;
	if (be_app->AppResources()->GetResourceInfo(type,name,&id,lengthFound) == false)
		return B_ERROR;
	*buffer = new unsigned char[*lengthFound];
	if (be_app->AppResources()->ReadResource(type,id,*buffer,0,*lengthFound) != B_OK)
		return B_ERROR;
	return B_OK;
}