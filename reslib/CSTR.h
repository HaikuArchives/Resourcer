#include <interface/StringView.h>

class _EXPORT ResourceStringView : public BStringView {
	public:
		ResourceStringView(void) : BStringView(BRect(0,0,100,20),"newStringView","No String Set") {
			res = be_app->AppResources();
			id = 0;
		}
		ResourceStringView(BMessage *msg) : BStringView(msg) {
			res = be_app->AppResources();
			msg->FindInt32("resid",&id);
			SetTo(id);
		}
		char *GetResString(void) {
			size_t len;
			char *tm;
			tm = (char *)res->LoadResource('CSTR',id,&len);
			char *tm2 = new char[strlen(tm) + 1];
			strcpy(tm2,tm);
			return tm2;
		}
		void SetTo(int32 id) {
			this->id = id;
			if (res->HasResource('CSTR',id)) {
				char *temp = GetResString();
				SetText(temp);
				delete temp;
			} else
				SetText("No String Set");
		}
		void Draw(BRect rect) {
			BStringView::Draw(rect);
		}
		int32 ResourceID(void) {
			return int32(id);
		}
		virtual	status_t Archive(BMessage *into, bool deep = true) const;
		static	BArchivable	*Instantiate(BMessage *from);
	private:
		BResources *res;
		int32 id;
};

BArchivable	*ResourceStringView::Instantiate(BMessage *from) {
	if ((validate_instantiation(from,"ResourceStringView")))
		return (new ResourceStringView(from));
	return NULL;
}

status_t ResourceStringView::Archive(BMessage *into, bool deep) const {
	status_t stat = BStringView::Archive(into,deep);
	into->AddString("class", "ResourceStringView");
	into->AddInt32("resid",id);
	return stat;
}