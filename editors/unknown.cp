//------------Includes----------------------
#include <image.h>
#include <interface/Alert.h>
#include <stdio.h>
#include <interface/TextView.h>
#include <interface/ScrollView.h>
#include <interface/Button.h>
#include <interface/StatusBar.h>
#include <interface/StringView.h>
#include <interface/TextControl.h>
#include <storage/FilePanel.h>
#include <storage/File.h>
#include <support/String.h>
#include <support/Beep.h>
//-------------------------------------------

#define PAGE 256

char hexd[16];

BFilePanel *panel;
typedef struct {
	BView *bkgview;
	entry_ref *ref;
} winmessage;

char* hexbyte(unsigned char byte);
void dehex(const char *data,size_t length,unsigned char *toreturn);
void seek0s(char *seekin,size_t length);
long openfile(void *m);
class HEXView;
class ASCIIView;
class AddressView;

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);

class ContainerView : public BView {
	public:
		ContainerView(unsigned char *data, size_t length);
		void GoToPage(int32 page,bool refresh = true);
		size_t CopyTo(void **into);
		HEXView *HEX;
		ASCIIView *ASCII;
		AddressView *Address;
		unsigned char *res;
		size_t size;
		int32 curPage;
};

class HEXView : public BTextView {
	public:
		HEXView(BRect frame, const char *name, BRect textRect,uint32 resizingMode,uint32 flags) : BTextView(frame,name,textRect,resizingMode,flags) {prcsed =false;wasdown = false;}
		void InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs);
		void DeleteText(int32 start,int32 finish);
		bool prcsed;
		bool wasdown;
		int32 tempos;
};

class ASCIIView : public BTextView {
	public:
		ASCIIView(BRect frame, const char *name, BRect textRect,uint32 resizingMode,uint32 flags) : BTextView(frame,name,textRect,resizingMode,flags) {prcsed =false;}
		void InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs) {
			for (int i = 0;i <length;i++) {
				if ((text[i] == 0) || (text[i] == '\n'))
					BTextView::InsertText(new char(1),1,offset+i,runs);
				else
					BTextView::InsertText(text+i,1,offset+i,runs);
			}
			if (prcsed == false) {
			if (length > 1) {
				char *hex;
				char *hexed = new char[length*2+1];
				for (int i = 0; i < length; i++) {
					hex = hexbyte(text[i]);
					hexed[i*2] = hex[0];
					hexed[(i*2)+1] = hex[1];
				}
				hexed[length*2] = 0;
				((ContainerView *)(Parent()))->HEX->prcsed = true;
				((ContainerView *)(Parent()))->HEX->Insert(offset*2,hexed,length*2,runs);
				((ContainerView *)(Parent()))->HEX->prcsed = false;
				delete hexed;
			} else {
				((ContainerView *)(Parent()))->HEX->prcsed = true;
				((ContainerView *)(Parent()))->HEX->Insert(offset*2,hexbyte(uint8(*text)),length*2,runs);
				((ContainerView *)(Parent()))->HEX->prcsed = false;
			}
			}
		}
		void DeleteText(int32 start,int32 finish) {
			BTextView::DeleteText(start,finish);
			if (prcsed == false) {
			((ContainerView *)(Parent()))->HEX->prcsed = true;
			((ContainerView *)(Parent()))->HEX->Delete(start*2,finish*2);
			((ContainerView *)(Parent()))->HEX->prcsed = false;
			}
		}
		bool prcsed;
};



/*inline void HEXView::Highlight(int32 start,int32 finish) {
	if (prcsed == false) {
		((ContainerView *)(Parent()))->ASCII->prcsed = true;
		int32 a,b;
		//((ContainerView *)(Parent()))->ASCII->SelectAll();
		((ContainerView *)(Parent()))->ASCII->Select(start/2,finish/2);
		//((ContainerView *)(Parent()))->ASCII->Highlight(start/2,finish/2);
		((ContainerView *)(Parent()))->ASCII->prcsed = false;
	} else {
		BTextView::Highlight(start,finish);
	}
}*/

void HEXView::InsertText(const char *text, int32 length, int32 offset, const text_run_array *runs) {
	BString str(text);
	str.ToUpper();
	strcpy((char *)(text),str.String());
	bool isvalid = true;
	char *x = new char[length];
	char *y = new char[length+1];
	for (int i = 0; i < length; i++) {
		switch (text[i]) {
			case '0':
				break;
			case '1':
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			case '5':
				break;
			case '6':
				break;
			case '7':
				break;
			case '8':
				break;
			case '9':
				break;
			case 'A':
				break;
			case 'B':
				break;
			case 'C':
				break;
			case 'D':
				break;
			case 'E':
				break;
			case 'F':
				break;
			default:
				isvalid = false;
				break;
		}
	}
	if (isvalid == true) {
		//if ((length % 2) != 0)
		//	BTextView::InsertText("0",1,length + offset,runs);
		/*if ((offset % 2) == 0) 
			isend = true;*/
		BTextView::InsertText(text,length,offset,runs);
		/*if (isend) {
			BTextView::InsertText("0",1,length + offset,runs);
			strncpy(y,text,length);
			dehex(y,length,(unsigned char *)(x));
		} else {
			dehex(text,length,(unsigned char *)(x));
		}*/
		if (prcsed == false) {
			if (((ContainerView *)(Parent()))->ASCII != NULL) {
				((ContainerView *)(Parent()))->ASCII->prcsed = true;
				char *dehexed = new char[strlen(Text()) + 1];
				dehex(Text(),strlen(Text()),(unsigned char *)(dehexed));
				seek0s(dehexed,strlen(Text())/2);
				((ContainerView *)(Parent()))->ASCII->SetText(dehexed);
				((ContainerView *)(Parent()))->ASCII->prcsed = false;
			}
		}
	} else {
		Select(offset-1,offset-1);
		beep();
	}
	delete x;
	delete y;
}
void HEXView::DeleteText(int32 start,int32 finish) {
	//if ((((finish-start) % 2) != 0) && ((finish % 2) == 0))
	//	Insert(finish,"0",1);
	//if ((finish % 2) == 0)
	//	finish++;
	BTextView::DeleteText(start,finish);
	if (prcsed == false) {
			if (((ContainerView *)(Parent()))->ASCII != NULL) {
				((ContainerView *)(Parent()))->ASCII->prcsed = true;
				char *dehexed = new char[strlen(Text()) + 1];
				dehex(Text(),strlen(Text()),(unsigned char *)(dehexed));
				seek0s(dehexed,strlen(Text()) /2);
				((ContainerView *)(Parent()))->ASCII->SetText(dehexed);
				((ContainerView *)(Parent()))->ASCII->prcsed = false;
			}
		}
}

void seek0s(char *seekin,size_t length) {
	for (size_t i = 0;i < length;i++) {
		if (seekin[i] == 0)
			seekin[i] = 1;
	}
	seekin[length] = 0;
}

class AddressView : public BTextControl {
  public:
	AddressView(ContainerView *parent) : BTextControl(BRect(70,10,170,50),"address","Current Page: ","0",new BMessage(-200)) {
		SetDivider(be_plain_font->StringWidth(Label()));
	}
};

ContainerView::ContainerView(unsigned char *data, size_t length) : BView(BRect(10,10,290,260),"hex",B_FOLLOW_ALL_SIDES,B_WILL_DRAW) {
		SetViewColor(216,216,216);
		HEX = new HEXView(BRect(0,50,180,250),"HEXView",BRect(5,5,165,195),B_FOLLOW_ALL_SIDES,B_FRAME_EVENTS | B_WILL_DRAW | B_PULSE_NEEDED | B_NAVIGABLE);
		ASCII = new ASCIIView(BRect(190,50,280,250),"ASCIIView",BRect(0,5,75,195),B_FOLLOW_ALL_SIDES,B_FRAME_EVENTS | B_WILL_DRAW | B_PULSE_NEEDED | B_NAVIGABLE);
		AddChild(HEX);
		AddChild(ASCII);
		if (length < PAGE) {
		/*	char *dvf = new char[PAGE];
			memcpy(dvf,data,length);
			for (int i = length; i < PAGE; i++) {
				dvf[i] = 0;
			}*/
			ASCII->SetText((char *)(data),length);
			ASCII->SetMaxBytes(PAGE);
		} else {
			ASCII->SetMaxBytes(PAGE);
			ASCII->SetText((char *)(data),PAGE);
		}
		HEX->SetMaxBytes(PAGE * 2);
		res = data;
		size = length;
		curPage = 0;
		Address = new AddressView(this);
		AddChild(Address);
		char string[20];
		sprintf(string," / %ld",length/PAGE);
		AddChild(new BStringView(BRect(170,10,230,25),"total_blocks",string));
		AddChild(new BButton(BRect(220,10,280,30),"next","Next",new BMessage(-300)));
		AddChild(new BButton(BRect(5,10,65,30),"back","Previous",new BMessage(-400)));
	}
	
void ContainerView::GoToPage(int32 page,bool refresh) {
	if (page * PAGE > size) {
		beep();
		return;
	}
	if (refresh) {
		unsigned char *yikes = new unsigned char[strlen(HEX->Text()) /2];
		dehex(HEX->Text(),strlen(HEX->Text()),yikes);
		memcpy(res + (curPage * PAGE),yikes,strlen(HEX->Text()) /2);
		if ((curPage + 1) * PAGE > size)
			size = (curPage * PAGE) + (strlen(HEX->Text())/2);
	}
	if ((page + 1) * PAGE > size)
		ASCII->SetText(((char *)(res) + (page * PAGE)),size - (page * PAGE));
	else
		ASCII->SetText(((char *)(res) + (page * PAGE)),PAGE);
	curPage = page;
}

size_t ContainerView::CopyTo(void **into) {
	GoToPage(0,true);
	*into = new unsigned char[size];
	memcpy(*into,res,size);
	return size;
}

char* hexbyte(unsigned char byte) {
	unsigned char high;
	static char string[3];
	high = byte/16;
	string[0] = hexd[high];
	string[1] = hexd[byte-high*16];
	return string;
}

void dehex(const char *data,size_t length,unsigned char *toreturn) {
	int x;
	char hexd[20];
	int high;
	char left;       // left part of the hex-value
	int low;
	char right;
	char* count;
	int i;
	int j;
	j = 0;
	hexd[1] = '0'; hexd[2] = '1'; hexd[3] = '2'; hexd[4] = '3'; hexd[5] = '4'; hexd[6] = '5'; hexd[7] = '6'; hexd[8] = '7'; hexd[9] = '8'; hexd[10] = '9';hexd[11]='A'; hexd[12]='B'; hexd[13]='C'; hexd[14]='D'; hexd[15]='E'; hexd[16]='F';
	for (count = (char *)(data);count < (data + length);count+=2) {
		left = *count;
		right = *(count + 1);
		for (i = 1;i <= 16;i++) {
			if (hexd[i] == left) {
				left = i-1;
				break;
			}
		}
		high=left*16;              //1
		for (i = 1;i <= 16;i++) {
			if (hexd[i] == right) {
				right = i-1;
				break;
			}
		}
		low = right;
		x = high + low;
		*(toreturn + j) = x;
		j++;
	}
} 
long openfile(void *m) {
	winmessage *ma = (winmessage *)(m);
	BView *bkgview = ma->bkgview;
	BFile file(ma->ref,B_READ_ONLY);
	unsigned char *data;
	if (file.InitCheck() != B_OK) {
		char *text;
		switch (file.InitCheck()) {
			case B_BAD_VALUE:
				text = "The file panel was sent an incorrect value and the file could not be opened.";
				break;
			case B_ENTRY_NOT_FOUND:
				text = "The file does not exist and so the file could not be opened.";
				break;
			case B_PERMISSION_DENIED:
				text = "You do not have permission to open this file.";
				break;
			case B_NO_MEMORY:
				text = "There was not enough memory to open the file.";
				break;
			default:
				text = "The file could not be opened because of an unknown error.";
				break;
			}
		if ((new BAlert("ahh!",text,"Try Again","Cancel",NULL,B_WIDTH_FROM_WIDEST,B_STOP_ALERT))->Go() == 0) {
			bkgview->Window()->PostMessage(-100);
		}
		return 0;
		}
	size_t size;
	off_t size2;
	file.GetSize(&size2);
	size = size2;
	data = new unsigned char[size];
	if (data == NULL) {
		(new BAlert("ahh!","There was not enough memory to open the file.","OK",NULL,NULL,B_WIDTH_FROM_WIDEST,B_STOP_ALERT))->Go();
		return 0;
	}
	file.Read(data,size);
	bkgview->Window()->Lock();
	((ContainerView *)(bkgview->FindView("hex")))->res = data;
	((ContainerView *)(bkgview->FindView("hex")))->size = size;
	((ContainerView *)(bkgview->FindView("hex")))->curPage = 0;
	((ContainerView *)(bkgview->FindView("hex")))->GoToPage(0,false);
	char *string = new char[20];
	sprintf(string," / %ld",(size / PAGE));
	((BStringView *)(bkgview->FindView("hex")->FindView("total_blocks")))->SetText(string);
	delete string;
	bkgview->Window()->Unlock();
	return(0);
	}
	


void loaddata(unsigned char *data,size_t length,BView *bkgview) {//runs when add-on starts, window is initialized, you are expected to write to it: rect is 300 x 300
	hexd[0] = '0'; hexd[1] = '1'; hexd[2] = '2'; hexd[3] = '3'; hexd[4] = '4'; hexd[5] = '5'; hexd[6] = '6'; hexd[7] = '7'; hexd[8] = '8'; hexd[9] = '9';hexd[10]='A'; hexd[11]='B'; hexd[12]='C'; hexd[13]='D'; hexd[14]='E'; hexd[15]='F';
	panel = new BFilePanel(B_OPEN_PANEL,new BMessenger(bkgview->Window()),NULL,0,false);
	bkgview->AddChild(new ContainerView(data,length));
	/*BTextView *view = new BTextView(BRect(10,10,280,260),"hex",BRect(0,0,270,280),B_FOLLOW_ALL,B_WILL_DRAW);
	BStatusBar *status = new BStatusBar(BRect(30,100,270,150),"readstatus","Processing Data",NULL);
	bkgview->AddChild(status);
	//bkgview->Window()->Show();
	view->SetText(hexit(data,length,status));
	status->RemoveSelf();
	delete status;
	bkgview->AddChild(new BScrollView("hexscroll",view,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true));*/
	bkgview->AddChild(new BButton(BRect(100,270,200,290),"file","Insert File...",new BMessage(-100)));
	//bkgview->Window()->Hide();
}

unsigned char* savedata(size_t *length,BView *bkgview) { //return data, clean up, and set sizeto the size of data
	unsigned char *toreturn;
	/*if (((ContainerView *)(bkgview->FindView("hex")))->size <= 256) {
		*length = strlen(((ContainerView *)(bkgview->FindView("hex")))->HEX->Text()) / 2;
		toreturn = new unsigned char[*length];
		dehex(((ContainerView *)(bkgview->FindView("hex")))->HEX->Text(),strlen(((ContainerView *)(bkgview->FindView("hex")))->HEX->Text()),toreturn);
	} else {*/
		*length = ((ContainerView *)(bkgview->FindView("hex")))->CopyTo((void **)(&toreturn));
	//}
	return toreturn;
}

void messaging(BMessage *message,BView *bkgview) { //receives messages from window with negative 'what' members or B_REFS_RECEIVED messages
	int32 newPage;
	char hoober[35];
	switch(message->what) {
		case -100:
			panel->Show();
			break;
		case -200:
			sscanf(((ContainerView *)(bkgview->FindView("hex")))->Address->Text(),"%ld",&newPage);
			if (newPage < 0)
				break;
			((ContainerView *)(bkgview->FindView("hex")))->GoToPage(newPage);
			break;
		case -300:
			((ContainerView *)(bkgview->FindView("hex")))->GoToPage(((ContainerView *)(bkgview->FindView("hex")))->curPage + 1);
			sprintf(hoober,"%ld",((ContainerView *)(bkgview->FindView("hex")))->curPage);
			((ContainerView *)(bkgview->FindView("hex")))->Address->SetText(hoober);
			break;
		case -400:
			((ContainerView *)(bkgview->FindView("hex")))->GoToPage(((ContainerView *)(bkgview->FindView("hex")))->curPage - 1);
			sprintf(hoober,"%ld",((ContainerView *)(bkgview->FindView("hex")))->curPage);
			((ContainerView *)(bkgview->FindView("hex")))->Address->SetText(hoober);
			break;
		case B_REFS_RECEIVED:
			winmessage *m = new winmessage;
			m->ref = new entry_ref;
			message->FindRef("refs",m->ref);
			m->bkgview = bkgview;
			openfile(m);
			break;
	}
}
