class reswindow;
BRect find_center(uint32 width, uint32 height);
BRect find_center(BRect rect,uint32 width, uint32 height);
#if __INTEL__
	type_code flipcode (type_code original);
#endif
#if __POWERPC__
	#define flipcode(o) o
#endif
typedef struct{
	image_id plugin;
	void (*loaddata)(unsigned char *,size_t,BView *);
	unsigned char* (*savedata)(size_t *,BView *);
	void (*messaging)(BMessage *,BView *);
	reswindow *win;
	unsigned char *olddata;
	size_t oldlength;
} plug_in;


#if __INTEL__
	type_code flipcode (type_code original) {
		type_code type = original;
		char *one = (char *)(&original);
		char *two = one + 1;
		char *three = two + 1;
		char *four = three + 1;
		*((char *)(&type)) = *four;
		*((char *)(&type) + 1) = *three;
		*((char *)(&type) + 2) = *two;
		*((char *)(&type) + 3) = *one;
		return type;
	}
#endif

class appFilter : public BRefFilter {
	public:
		bool Filter(const entry_ref *ref, BNode *node, struct stat *st,
					   const char *mimetype) {
			if (BEntry(ref,true).IsDirectory())
				return true;
			BFile type(ref,B_READ_WRITE);
			BResources super;
			if (super.SetTo(&type) == B_OK)
				return true;
			return false;
		}
};

BRect find_center(uint32 width, uint32 height) {
	BRect a = BScreen(B_MAIN_SCREEN_ID).Frame();
	uint32 screen_width = (uint32)(a.right - a.left);
	uint32 screen_height = (uint32)(a.bottom - a.top);
	uint32 c = (uint32)(((screen_width / 2) - (width / 2)) + a.left);
	uint32 d = (uint32)(((screen_height / 2) - (height / 2)) + a.top);
	BRect toreturn;
	toreturn.left = c;
	toreturn.top = d;
	toreturn.right = c + width;
	toreturn.bottom = d + height;
	return toreturn;
}

BRect find_center(BRect rect,uint32 width, uint32 height) {
	BRect a = rect;
	uint32 screen_width = (uint32)(a.right - a.left);
	uint32 screen_height = (uint32)(a.bottom - a.top);
	uint32 c = (uint32)(((screen_width / 2) - (width / 2)) + a.left);
	uint32 d = (uint32)(((screen_height / 2) - (height / 2)) + a.top);
	BRect toreturn;
	toreturn.left = c;
	toreturn.top = d;
	toreturn.right = c + width;
	toreturn.bottom = d + height;
	return toreturn;
}
BFilePanel *openpanel;
BFilePanel *newpanel;
BAlert *alert;
int32 mbheight;
int32 openmwindows;
BMessage *itemLista;
class pluginwindow;