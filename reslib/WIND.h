#include <interface/Window.h>
#include <app/Message.h>

BWindow *GetResWindow(char *name);
BWindow *GetResWindow(int32 id);
BMessage *GetResWindowArchive(char *name);
BMessage *GetResWindowArchive(int32 id);

BWindow *GetResWindow(char *name) {
	BMessage *archive = GetResWindowArchive(name);
	BWindow *toret = new BWindow(archive);
	delete archive;
	return (toret);
}

BWindow *GetResWindow(int32 id) {
	BMessage *archive = GetResWindowArchive(id);
	BWindow *toret = new BWindow(archive);
	delete archive;
	return (toret);
}

BMessage *GetResWindowArchive(char *name) {
	BMessage *archive = new BMessage;
	ResourceIO res('WIND',name);
	archive->Unflatten(&res);
	return archive;
}

BMessage *GetResWindowArchive(int32 id) {
	BMessage *archive = new BMessage;
	ResourceIO res('WIND',id);
	archive->Unflatten(&res);
	return archive;
}
