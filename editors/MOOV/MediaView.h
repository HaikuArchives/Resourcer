#ifndef _MEDIA_VIEW_H
#define _MEDIA_VIEW_H

#include <View.h>
#include <MediaDefs.h>


enum media_action {
	MEDIA_PLAY,
	MEDIA_STOP,
};


class BMediaFile;
class BMediaTrack;
class AudioOutput;
class _MediaBar_;
class BBitmap;


class MediaView : public BView {
public:
					MediaView(BRect			frame, 
							  const char	*name,
							  uint32		resizeMask,
							  uint32		flags = B_WILL_DRAW | B_FRAME_EVENTS);
	virtual			~MediaView();

	status_t		SetMediaSource(BPositionIO *data);

	status_t		SetColorSpace(color_space depth);
	color_space		ColorSpace() const;

	status_t		Control(media_action action);
	bool			IsPlaying() const;

	bool			HasVideoTrack() const;
	bool			HasAudioTrack() const;

	virtual void	GetPreferredSize(float *width, float *height);
	virtual void	Draw(BRect updateRect);
	virtual void	DetachedFromWindow();
	virtual void	FrameResized(float width, float height);
	size_t GetMovie(void **buffer);

private:
	void			InitObject();

	status_t		SetVideoTrack(BDataIO *data, BMediaTrack *track, media_format *format);
	status_t		SetAudioTrack(BDataIO *data, BMediaTrack *track, media_format *format);

	status_t		Play();
	status_t		Stop();

	void			Reset();

	BRect			VideoBounds() const;

	static void		BuildMediaFormat(BBitmap *bitmap, media_format *format);
	static int32	MediaPlayer(void *arg);
	BPositionIO *file;

private:
	friend class _MediaSlider_;

	BMediaFile*		fMediaFile;
	BMediaTrack*	fVideoTrack;
	BMediaTrack*	fAudioTrack;
	AudioOutput*	fAudioOutput;
	_MediaBar_*		fMediaBar;
	BBitmap*		fBitmap;
	color_space		fBitmapDepth;
	bigtime_t		fCurTime;
	bigtime_t		fScrubTime;
	thread_id		fPlayerThread;
	sem_id			fPlaySem;
	sem_id			fScrubSem;
	bool			fPlaying;
	bool			fSnoozing;
	bool			fUsingOverlay;
	void*			fAudioDumpingBuffer;
};


#endif
