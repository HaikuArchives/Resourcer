//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <stdio.h>
#include <interface/PictureButton.h>
#include <interface/Button.h>
#include <interface/Polygon.h>
#include <interface/Box.h>
#include <media/MediaDefs.h>
#include <media/Sound.h>
#include <media/SoundPlayer.h>
#include <storage/FilePanel.h>
#include <translation/TranslationDefs.h>
#include <translation/TranslatorFormats.h>
//-------------------------------------------

size_t encodeformat(unsigned char *buffer,media_raw_audio_format format,size_t totalSize);
void decodeformat(unsigned char *buffer,media_raw_audio_format *format,size_t size,bool *is_old = NULL);
void encode4(unsigned char *buffer,void* toop);
void decode4(unsigned char *buffer,void* toop);
int32 playsound(void *box);
void changesound(BView *box,BMessage *message);

// export functions
extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Translator Sound";

void encode4(unsigned char *buffer,void* toop) {
	char *one = (char *)(toop);
	char *two = one + 1;
	char *three = two + 1;
	char *four = three + 1;
	*buffer = *one;
	*(buffer + 1) = *two;
	*(buffer + 2) = *three;
	*(buffer + 3) = *four;
}

void decode4(unsigned char *buffer,void* toop) {
	char *one = (char *)(toop);
	char *two = one + 1;
	char *three = two + 1;
	char *four = three + 1;
	*one = *buffer;
	*two = *(buffer + 1);
	*three = *(buffer + 2);
	*four = *(buffer + 3);
}


size_t encodeformat(unsigned char *buffer,media_raw_audio_format format,size_t totalSize) {
	TranslatorSound header;
	header.magic = B_TRANSLATOR_SOUND;
	header.channels = format.channel_count;
	header.sampleFreq = format.frame_rate;
	header.numFrames = totalSize / (2 * format.channel_count);
	memcpy(buffer,&header,sizeof(TranslatorSound));
	return sizeof(TranslatorSound);
}

/*Notes----------
	format = B_AUDIO_SHORT;
	byte_order = B_MEDIA_BIG_ENDIAN;
*/

void decodeformat(unsigned char *buffer,media_raw_audio_format *format,size_t size,bool *is_old) {
	unsigned char *worker;
	worker = buffer;
	TranslatorSound *sound = (TranslatorSound *)(buffer);
	bool old;
	if (sound->magic == B_TRANSLATOR_SOUND)
		old = false;
	else
		old = true;
	if (old) {
		decode4(worker,&(format->frame_rate));
		worker = worker + 4;
		decode4(worker,&(format->channel_count));
		worker = worker + 4;
		decode4(worker,&(format->format));
		worker = worker + 4;
		decode4(worker,&(format->byte_order));
		worker = worker + 4;
		decode4(worker,&(format->buffer_size));
		worker = worker + 4;
	} else {
		format->format = media_raw_audio_format::B_AUDIO_SHORT;
		format->byte_order = B_MEDIA_BIG_ENDIAN;
		format->frame_rate = sound->sampleFreq;
		format->channel_count = sound->channels;
		format->buffer_size = size - sizeof(TranslatorSound);
	}
	if (is_old != NULL)
		*is_old = old;
}

class AIFFilter : public BRefFilter {
	bool Filter(const entry_ref *ref, BNode *node, struct stat *st, const char *filetype) {
		if ((strcmp(filetype,"audio/x-aiff") == 0) || (BEntry(ref,true).IsDirectory()))
			return true;
		else
			return false;
	}
};

class BNBox : public BBox {
	public:
		rgb_color getrgb(uint8 r,uint8 g,uint8 b) {rgb_color temp;temp.red = r;temp.green = g;temp.blue = b;return temp;}
		BNBox(BView *parent) : BBox(BRect(20,20,280,80)) {
			parent->AddChild(this);
			BPicture *pict;
			rgb_color black;
			black.red = 0;
			black.green = 0;
			black.blue = 0;
			rgb_color white;
			white.red = 255;
			white.green = 255;
			white.blue = 255;
			BPoint points[3];
			points[0].Set(5,5);
			points[1].Set(5,45);
			points[2].Set(45,25);
			BPolygon triangle(points,3);
			BeginPicture(new BPicture);
			SetHighColor(getrgb(0,255,0));
			FillPolygon(&triangle);
			pict = EndPicture();
			BPicture *pict2;
			BeginPicture(new BPicture);
			SetHighColor(getrgb(150,255,0));
			FillPolygon(&triangle);
			SetViewColor(240,240,240);
			pict2 = EndPicture();
			AddChild(new BPictureButton(BRect(5,5,55,55),"play",pict,pict2,new BMessage(-500)));
			BPicture *picts;
			BRect square(5,5,45,45);
			BeginPicture(new BPicture);
			SetHighColor(getrgb(255,0,0));
			FillEllipse(square);
			picts = EndPicture();
			BPicture *picts2;
			BeginPicture(new BPicture);
			SetHighColor(getrgb(255,0,255));
			FillEllipse(square);
			SetViewColor(240,240,240);
			picts2 = EndPicture();
			//AddChild(new BPictureButton(BRect(65,5,115,55),"stop",picts,picts2,new BMessage(-600)));
			AddChild(new BButton(BRect(125,20,235,40),"openfile","Open AIFF File...",new BMessage(-700)));
			panel = new BFilePanel(B_OPEN_PANEL,new BMessenger(parent),NULL,B_FILE_NODE,false,NULL,new AIFFilter,false,true);
		}
		entry_ref *sound;
		int32 id;
		unsigned char *data;
		size_t length;
		BFilePanel *panel;
};

void changesound(BView *box,BMessage *message) {
	BNBox *boxa = (BNBox *)(box);
	entry_ref ref;
	message->FindRef("refs",&ref);
	BSound *sound = new BSound(&ref,true);
	boxa->length = size_t(sound->Size()) + sizeof(TranslatorSound);
	unsigned char *toreturn = new unsigned char[boxa->length];
	media_raw_audio_format format = sound->Format();
	encodeformat(toreturn,format,sound->Size());
	size_t stupid;
	sound->GetDataAt(0,toreturn + sizeof(TranslatorSound),sound->Size(),&stupid);
	boxa->data = toreturn;
	}

int32 playsound(void *box) {
	BNBox *boxa = (BNBox *)(box);
	media_raw_audio_format format;
	bool is_old;
	decodeformat(boxa->data,&format,boxa->length,&is_old);
	if (boxa->length == 0) {
		return 0;
		}
	BSound *sound;
	if (is_old) {
		unsigned char *worker = boxa->data + 20;
		sound = new BSound(worker,boxa->length - 20,format);
	} else {
		unsigned char *worker = boxa->data + sizeof(TranslatorSound);
		sound = new BSound(worker,boxa->length - sizeof(TranslatorSound),format);
	}
	if (sound->InitCheck() == B_OK) {
		BSoundPlayer *soundp = new BSoundPlayer;
		soundp->Start();
		int32 id = soundp->StartPlaying(sound);
		boxa->id = id;
		soundp->WaitForSound(id);
		soundp->Stop();
		delete soundp;
	}
	return 0;
}

void loaddata(unsigned char *data,size_t length,BView *bkgview) { //runs when editor launched, window rect is 300 x 300 pixels
	BNBox *box = new BNBox(bkgview);
	if (length != 0) {
		box->data = new unsigned char[length];
		memcpy(box->data,data,length);
	} else
		box->data = NULL;
	box->length = length;
	box->SetViewColor(240,240,240);
	bkgview->Window()->ResizeTo(300,100);
}

unsigned char* savedata(size_t *length,BView *bkgview) { //runs when window closes, return value is what should be written to the resource, window is window address given in loaddata()
	BNBox *box = (BNBox *)(bkgview->ChildAt(0));
	if (box->data == NULL) {
		*length = 0;
		return new unsigned char(0);
	}
	if (find_thread("playsound") >= B_OK)
		kill_thread(find_thread("playsound"));
	unsigned char *data = new unsigned char[box->length + sizeof(TranslatorSound)];
	bool is_old;
	media_raw_audio_format form;
	decodeformat(box->data,&form,box->length,&is_old);
	if (is_old) {
		encodeformat(box->data,form,box->length - 20);
		memcpy(data,box->data,sizeof(TranslatorSound));
		memcpy(data + sizeof(TranslatorSound),box->data + 20,box->length - 20);
		*length = box->length - 20 + sizeof(TranslatorSound);
	} else {
		encodeformat(box->data,form,box->length - sizeof(TranslatorSound));
		memcpy(data,box->data,sizeof(TranslatorSound));
		memcpy(data + sizeof(TranslatorSound),box->data + sizeof(TranslatorSound),box->length - sizeof(TranslatorSound));
		*length = box->length;
	}
	box->panel->Window()->PostMessage(B_QUIT_REQUESTED);
	delete box->panel;
	swap_data(uint32(B_INT16_TYPE),(void *)(data + sizeof(TranslatorSound)),box->length - sizeof(TranslatorSound),B_SWAP_HOST_TO_BENDIAN);
	return data;
}

void messaging(BMessage *message,BView *bkgview) { //BMessages from window, to be passed here 'what' field must be a negative integer or B_REFS_RECEIVED
	switch(message->what) {
		case -500:
			{
				if (find_thread("playsound") >= B_OK)
					break;
				thread_id thread = spawn_thread(&playsound,"playsound",10,bkgview->ChildAt(0));
				if (thread < B_OK)
					playsound(bkgview->ChildAt(0));
				else {
					if (resume_thread(thread) != B_OK) {
						playsound(bkgview->ChildAt(0));
					}
				}
			}
			break;
		case -700:
			((BNBox *)(bkgview->ChildAt(0)))->panel->Show();
			break;
		case B_REFS_RECEIVED:
			changesound(bkgview->ChildAt(0),message);
	}
}