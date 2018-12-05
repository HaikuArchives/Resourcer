#include <FindDirectory.h>
#include <Path.h>
bool launched;
bool alredopen;
bool panelopen;
bool openpan;

class BRApplication : public BApplication {
	public:
	BRApplication(void) : BApplication("application/x-vnd.NW-resourcer") {openpan = false;retrieve = new char[2];retrieve[0] = 0;}
	void RefsReceived(BMessage *message);
	void getready(BMessage *message,bool istempfile = false);
	void dissectwindow(BMessage *msg);
	void MessageReceived(BMessage *message) {
		switch (message->what) {
			case B_SAVE_REQUESTED:
				getready(message);
				break;
			case B_CANCEL:
				if (openmwindows == 0) {
					be_app->Quit();
				}
				if ((panelopen == true) && (openpan == false)) {
					savepan = true;
				}
				dissectwindow(message);
				break;
			case 'chge':
				delete retrieve;
				BTextControl *text;
				message->FindPointer("source",(void **)(&text));
				retrieve = new char[strlen(text->Text()) + 1];
				strcpy(retrieve,text->Text());
				break;
			default:
				BApplication::MessageReceived(message);
			}
		}
	void ReadyToRun(void) {
		launched = true;
		saveas = false;
		if (alredopen)
			return;
		BMessage *msg = new BMessage(B_SAVE_REQUESTED);
		BPath path;
		find_directory(B_SYSTEM_TEMP_DIRECTORY, &path);
		entry_ref ref;
		get_ref_for_path(path.Path(), &ref);
		//BEntry entry;
		msg->AddRef("directory",&ref);
		char text[B_FILE_NAME_LENGTH];
		sprintf(text,"tmpresfile0");
		BEntry entry;
		BDirectory tmp(path.Path());
		for (int i = 1;tmp.FindEntry(text,&entry) != B_ENTRY_NOT_FOUND; i++) {
			sprintf(text,"tmpresfile%d",i);
		}
		msg->AddString("name",text);
		getready(msg,true);
	}
	bool savepan;
	char *retrieve;
	bool saveas;
};
