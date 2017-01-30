//Extracts sounds from applications resources

//IMPORTANT!
//Requires libmedia.so

#include <media/Sound.h>
#include <translation/TranslatorFormats.h>

//---Prototypes--
BSound *getsound(long id);
BSound *getsound(const char *name);
void decodeformat(unsigned char *buffer,media_raw_audio_format *format,size_t size,bool *is_old = NULL);
void decode4(unsigned char *buffer,void* toop);

//---Useful Functions---
BSound *getsound(long id) {
	void *sound;
	size_t length;
	sound = (void *)AppResource('nois',id,&length);
	media_raw_audio_format format;
	bool is_old;
	decodeformat((unsigned char *)(sound),&format,length,&is_old);
	BSound *sounda;
	if (is_old) {
		sound = (void *)((addr_t)sound + 20);
		sounda = new BSound((void *)sound,length - 20,format);
	} else {
		sound = (void *)((addr_t)sound + sizeof(TranslatorSound));
		sounda = new BSound((void *)sound,length - 20,format);
	}
	return (sounda);
}

BSound *getsound(const char *name) {
	const void *sound;
	size_t length;
	sound = AppResource('nois',name,&length);
	media_raw_audio_format format;
	bool is_old;
	decodeformat((unsigned char *)(sound),&format,length,&is_old);
	BSound *sounda;
	if (is_old) {
		sound = (void *)((addr_t)sound + 20);
		sounda = new BSound((void *)sound,length - 20,format);
	} else {
		sound = (void *)((addr_t)sound + sizeof(TranslatorSound));
		sounda = new BSound((void *)sound,length - 20,format);
	}
	return (sounda);
}


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
