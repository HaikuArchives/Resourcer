
//------------Includes----------------------
#include <image.h>
#include <interface/Window.h>
#include <interface/CheckBox.h>
#include <interface/Box.h>
#include <interface/PopUpMenu.h>
#include <interface/Slider.h>
#include <interface/RadioButton.h>
#include <interface/Button.h>
#include <interface/TextControl.h>
#include <interface/StringView.h>
#include <translation/TranslationUtils.h>
#include <support/ClassInfo.h>
#include <storage/Resources.h>
#include <interface/Bitmap.h>
#include <interface/OutlineListView.h>
#include <interface/ListItem.h>
#include <interface/MenuBar.h>
#include <interface/MenuItem.h>
#include <interface/MenuField.h>
#include <string.h>
#include <interface/Alert.h>
#include <app/Message.h>
#include <app/MessageFilter.h>
#include <support/Beep.h>
#include <interface/TabView.h>
#include <interface/View.h>
#include <interface/ScrollView.h>
#include <stdio.h>
#include <interface/TextControl.h>
#include <app/Application.h>
//-------------------------------------------

extern "C" _EXPORT void loaddata(unsigned char *,size_t,BView *);
extern "C" _EXPORT unsigned char* savedata(size_t *,BView *);
extern "C" _EXPORT void messaging(BMessage *,BView *);
extern "C" _EXPORT const char description[] = "Window";
extern "C" _EXPORT BResources *file;
extern "C" _EXPORT image_id me;
#if __INTEL__
	int32 flipcode (int32 original);
#endif
#if __POWERPC__
	#define flipcode(o) o
#endif
#if __INTEL__
	int32 flipcode (int32 original) {
		int32 type = original;
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
BResources *file;
image_id me;
BResources *resources;
BMessage *widemsg;

class EditingWindow;

BMessage *current;
EditingWindow *editingwin;
BView *wineditor;
BWindow *main;
window_feel danger_feel = B_NORMAL_WINDOW_FEEL;
int32 track_over(void *view);

const pattern STIPPLED = {240,240,240,240,15,15,15,15};

#include "bits.h"

class _EXPORT ResourceStringView : public BStringView {
	public:
		ResourceStringView(void) : BStringView(BRect(0,0,100,20),"newStringView","No String Set") {
			res = NULL;
			id = 0;
		}
		ResourceStringView(BMessage *msg) : BStringView(msg) {
			res = NULL;
			msg->FindInt32("resid",&id);
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
			if (res->HasResource('CSTR',id))
				SetText(GetResString());
			else
				SetText("No String Set");
		}
		void Draw(BRect rect) {
			if (res == NULL)
				AttachedToWindow();
			BStringView::Draw(rect);
		}
		int32 ResourceID(void) {
			return int32(id);
		}
		void AttachedToWindow(void);
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

class InterfaceItem : public BStringItem {
	public:
		InterfaceItem(char *label,BView *tocreate) : BStringItem(label) {
			what_a_drag = new BMessage;
			tocreate->ResizeToPreferred();
			tocreate->Archive(what_a_drag,true);
			drag_size = tocreate->Bounds();
			delete tocreate;	
		}
		BMessage *what_a_drag;
		BRect drag_size;
};

class InterfaceListView : public BOutlineListView {
	public:
		InterfaceListView(void) : BOutlineListView(BRect(20,20,230,200),"elements") {}
		bool InitiateDrag(BPoint point, int32 index, bool wasSelected) {
			if (ItemAt(index)->OutlineLevel() != 1)
				return false;
			BRect temp = ((InterfaceItem *)(ItemAt(index)))->drag_size;
			BMessage *msg = ((InterfaceItem *)(ItemAt(index)))->what_a_drag;
			BBitmap *bits = new BBitmap(temp,BScreen(Window()).ColorSpace(),true);
			BView *view = (BView *)(instantiate_object(msg));
			bits->AddChild(view);
			view->Window()->Lock();
			view->Draw(view->Bounds());
			view->Sync();
			if (view->Window()->IsLocked())
				view->Window()->Unlock();
			DragMessage(msg,bits,BPoint(0,0));
			return true;
		}
};

class MenuListItem : public BStringItem {
	public:
		MenuListItem(BMenuItem *ite,BOutlineListView *list) : BStringItem(ite->Label()) {
			if (is_kind_of(ite,BSeparatorItem))
				SetText("-------------");
			item = ite;
			lview = list;
		}
		void CheckSub(void) {
			if (item->Submenu() != NULL) {
				BMenu *menu = item->Submenu();
				BMenuItem *cur = NULL;
				MenuListItem *toadd;
				for (int32 i = menu->CountItems()-1; menu->ItemAt(i) != NULL; i--) {
					cur = menu->ItemAt(i);
					toadd = new MenuListItem(cur,lview);
					lview->AddUnder(toadd,this);
					toadd->CheckSub();
				}
			}
		}
		void MarkAsOver(bool state,BOutlineListView *owner) {
			if (is_kind_of(item,BSeparatorItem))
				return;
			asover = state;
			owner->Invalidate(owner->ItemFrame(owner->IndexOf(this)));
		}
		void DrawItem(BView *owner, BRect itemRect, bool drawEverything = false) {
			if (asover)
				owner->SetLowColor(51,152,255);
			owner->FillRect(itemRect,B_SOLID_LOW);
			BStringItem::DrawItem(owner,itemRect,drawEverything);
			owner->SetLowColor(255,255,255);
		}
		BMenuItem *item;
	private:
		BOutlineListView *lview;
		bool asover;
};


class DragOutlineListView : public BOutlineListView {
	public:
		DragOutlineListView(BRect rect,BMenu *men) : BOutlineListView(rect,"itemeditor") {
			menu = men;
			BMenuItem *cur = NULL;
			for (int32 i = 0; menu->ItemAt(i) != NULL; i++) {
				cur = menu->ItemAt(i);
				MenuListItem *toadd = new MenuListItem(cur,this);
				AddItem(toadd);
				toadd->CheckSub();
			}
		}
		void Refresh(bool noItemsAdded) {
			int32 old;
			if (noItemsAdded)
				old = FullListCurrentSelection();
			MakeEmpty();
			SelectionChanged();
			BMenuItem *cur = NULL;
			for (int32 i = 0; menu->ItemAt(i) != NULL; i++) {
				cur = menu->ItemAt(i);
				MenuListItem *toadd = new MenuListItem(cur,this);
				AddItem(toadd);
				toadd->CheckSub();
			}
			if (noItemsAdded)
				Select(old);
		}
		
		void SelectionChanged(void) {
			Window()->Lock();
			if ((CurrentSelection() < 0) || is_kind_of(((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item,BSeparatorItem)) {
				((BTextControl *)(Parent()->Parent()->FindView("label")))->SetText("");
				((BTextControl *)(Parent()->Parent()->FindView("label")))->SetEnabled(false);
				((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetText("none");
				((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetEnabled(false);
				((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetText("");
				((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(false);
				((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetEnabled(false);
				((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_OFF);
				if (Window()->IsLocked())
					Window()->Unlock();
				return;
			} else {
				((BTextControl *)(Parent()->Parent()->FindView("label")))->SetEnabled(true);
				((BTextControl *)(Parent()->Parent()->FindView("msg")))->SetEnabled(true);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(false);
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(false);
				((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetEnabled(true);
				if (((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->IsEnabled())
					((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_ON);
				else
					((BCheckBox *)(Parent()->Parent()->FindView("enabled")))->SetValue(B_CONTROL_OFF);
				if (((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->Submenu() == NULL) {
					((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(true);
					((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(true);
				} else {
					((BTextControl *)(Parent()->Parent()->FindView("shrtct")))->SetEnabled(false);
					((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->SetEnabled(false);
				}
			}
			((BTextControl *)(Parent()->Parent()->FindView("label")))->SetText(((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item->Label());
			char msg[5];
			BMenuItem *item = ((MenuListItem *)(FullListItemAt(FullListCurrentSelection())))->item;
			if (item->Message() != NULL)
				strncpy(msg,(char *)(new int32(flipcode(item->Message()->what))),4);
			else
				strcpy(msg,"none");
			msg[4] = 0;
			BTextControl *tcont = (BTextControl *)(Parent()->Parent()->FindView("msg"));
			tcont->SetText(msg);
			uint32 mod;
			msg[1] = 0;
			msg[0] = item->Shortcut(&mod);
			tcont = (BTextControl *)(Parent()->Parent()->FindView("shrtct"));
			tcont->SetText(msg);
			if (mod & B_OPTION_KEY)
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(0)->SetMarked(true);
			if (mod & B_SHIFT_KEY)
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(1)->SetMarked(true);
			if (mod & B_CONTROL_KEY)
				((BMenuField *)(Parent()->Parent()->FindView("modchoice")))->Menu()->ItemAt(2)->SetMarked(true);
			if (Window()->IsLocked())
				Window()->Unlock();
		}
		bool InitiateDrag(BPoint point, int32 index, bool wasSelected) {
			if (!wasSelected)
				return false;
			BMessage *msg = new BMessage('dolv');
			msg->AddPointer("item",ItemAt(index));
			DragMessage(msg, ItemFrame(index));
			thread_id thread = spawn_thread(track_over,"watch_mouse_over",B_NORMAL_PRIORITY,this);
			if ((thread) < B_OK) 
				(new BAlert("ck","Could not spawn watch_mouse_over! You are probably out of memory.","OK"))->Go();
			else 
				resume_thread(thread);
			return true;
		}
		void KeyDown(const char *bytes,int32 numBytes) {
			MenuListItem *litem;
			if ((*bytes == B_DELETE) || (*bytes == B_BACKSPACE)) {
				litem = ((MenuListItem *)(ItemAt(CurrentSelection())));
				litem->item->Menu()->RemoveItem(litem->item);
				Refresh(false);
			}
		}
		void WatchMouse(void) {
			BPoint newloc;
			BRect hello;
			float issue;
			int32 ind = -1024;
			uint32 buttons;
			float y = -20;
			MenuListItem *temp = NULL;
			for (;true;) {
				Window()->Lock();
				GetMouse(&newloc,&buttons,false);
				if (buttons != B_PRIMARY_MOUSE_BUTTON) {
					if (temp != NULL)
						temp->MarkAsOver(false,this);
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
					if (Window()->IsLocked())
						Window()->Unlock();
					return;
				}
				ind = IndexOf(newloc);
				if (ind < 0) {
					if (y == ItemFrame(IndexOf(LastItem())).bottom + 1) {
						if (Window()->IsLocked())
							Window()->Unlock();
						continue;
					}
					if (temp != NULL)
						temp->MarkAsOver(false,this);
					temp = NULL;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
					y = ItemFrame(IndexOf(LastItem())).bottom + 1;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS); 
					if (Window()->IsLocked())
						Window()->Unlock();
					continue;
				}
				hello = ItemFrame(ind);
				issue = (hello.Height() / 3);
				if (newloc.y > (hello.top + (issue * 2))) {
					if (y == hello.bottom) {
						if (Window()->IsLocked())
							Window()->Unlock();
						continue;
					}
					if (temp != NULL)
						temp->MarkAsOver(false,this);
					temp = NULL;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
					y = hello.bottom;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS); 
					if (Window()->IsLocked())
						Window()->Unlock();
					continue;
				}
				if (newloc.y > (hello.top + issue)) {
					if (y == -283 + ind) {
						if (Window()->IsLocked())
							Window()->Unlock();
						continue;
					}
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
					y = -283 + ind;
					if (temp != NULL)
						temp->MarkAsOver(false,this);
					if (is_kind_of(((MenuListItem *)(ItemAt(ind)))->item,BSeparatorItem)) {
						temp = NULL;
						if (Window()->IsLocked())
							Window()->Unlock();
						continue;
					}
					temp = ((MenuListItem *)(ItemAt(ind)));
					temp->MarkAsOver(true,this);
					if (Window()->IsLocked())
						Window()->Unlock();
					continue;
				}
				if (newloc.y > hello.top) {
					if (y == hello.top) {
						if (Window()->IsLocked())
							Window()->Unlock();
						continue;
					}
					if (temp != NULL)
						temp->MarkAsOver(false,this);
					temp = NULL;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_SOLID_LOW);
					y = hello.top;
					StrokeLine(BPoint(Bounds().left,y),BPoint(Bounds().right,y),B_MIXED_COLORS);
					if (Window()->IsLocked())
						Window()->Unlock();
					continue;
				}
				if (Window()->IsLocked())
						Window()->Unlock();
			}
		}
		void MessageReceived(BMessage *msg) {
			if (!(msg->WasDropped()))
				return;
			if (msg->what != 'dolv')
				return;
			MenuListItem *item;
			msg->FindPointer("item",(void **)(&item));
			BPoint newloc = msg->DropPoint();
			newloc = ConvertFromScreen(newloc);
			int32 ind = IndexOf(newloc);
			BMenu *men;
			if (ind < 0) {
				BMenuItem *menite = item->item->Menu()->Superitem();
				men = item->item->Menu();
				men->RemoveItem(item->item);
				menu->AddItem(item->item);
				if (menite != NULL) {
					if (menite->Submenu() != NULL) {
						if (menite->Submenu()->CountItems() == 0)
							RemoveSubMenu(menite);
					}
				}
				Refresh(false);
				return;
			}
			if (ItemAt(ind) == item)
				return;
			BRect hello = ItemFrame(ind);
			float issue = (hello.Height() / 3);
			if (newloc.y > (hello.top + (issue * 2))) {
				MenuListItem *lite = (MenuListItem *)(ItemAt(ind));
				BMenuItem *menite = item->item->Menu()->Superitem();
				men = item->item->Menu();
				men->RemoveItem(item->item);
				lite->item->Menu()->AddItem(item->item,lite->item->Menu()->IndexOf(lite->item) + 1);
				if (menite != NULL) {
					if (menite->Submenu() != NULL) {
						if (menite->Submenu()->CountItems() == 0)
							RemoveSubMenu(menite);
					}
				}
				Refresh(false);
				return;
			}
			if (newloc.y > (hello.top + issue)) {
				if (is_kind_of(((MenuListItem *)(ItemAt(ind)))->item,BSeparatorItem))
					return;
				BMenuItem *menite = item->item->Menu()->Superitem();
				men = item->item->Menu();
				AddSubItem(item,(MenuListItem *)(ItemAt(ind)));
				if (menite != NULL) {
					if (menite->Submenu() != NULL) {
						if (menite->Submenu()->CountItems() == 0)
							RemoveSubMenu(menite);
					}
				}
				Refresh(false);
				return;
			}
			if (newloc.y > hello.top) {
				BMenuItem *menite = item->item->Menu()->Superitem();
				MenuListItem *lite = (MenuListItem *)(ItemAt(ind));
				item->item->Menu()->RemoveItem(item->item);
				lite->item->Menu()->AddItem(item->item,lite->item->Menu()->IndexOf(lite->item));
				if (menite != NULL) {
					if (menite->Submenu() != NULL) {
						if (menite->Submenu()->CountItems() == 0)
							RemoveSubMenu(menite);
					}
				}
				Refresh(false);
				return;
			}
			if (Window()->IsLocked())
				Window()->Unlock();
		}
		void AddSubItem(MenuListItem *add, MenuListItem *tothis) {
			BMenuItem *toadd = add->item;
			BMenuItem *addunder = tothis->item;
			BMenuItem *temp;
			BMenu *men;
			int32 ind;
			if (addunder->Submenu() == NULL) {
				temp = new BMenuItem(new BMenu(addunder->Label()),new BMessage(addunder->Message()));
				men = addunder->Menu();
				ind = men->IndexOf(addunder);
				men->RemoveItem(addunder);
				delete addunder;
				addunder = temp;
				men->AddItem(addunder,ind);
			}
			toadd->Menu()->RemoveItem(toadd);
			addunder->Submenu()->AddItem(toadd);
			tothis->item = addunder;
		}
		void RemoveSubMenu(BMenuItem *toremfrom) {
			BMenuItem *temp;
			int32 ind;
			BMenu *men;
			BMenuItem *addunder = toremfrom;
			if (addunder->Submenu() == NULL)
				return;
			temp = new BMenuItem(addunder->Label(),new BMessage(addunder->Message()));
			men = addunder->Menu();
			ind = men->IndexOf(addunder);
			men->RemoveItem(addunder);
			delete addunder;
			addunder = temp;
			men->AddItem(addunder,ind);
		}
	private:
		BMenu *menu;
};

int32 track_over(void *view) {
	DragOutlineListView *lview = (DragOutlineListView *)(view);
	lview->WatchMouse();
	return B_OK;
}

class MenuEditor : public BBox {
	public:
		MenuEditor(BMenu *men,BRect rect) : BBox(rect,"menueditor") {
			menu = men;
			SetViewColor(216,216,216);
			lview = new DragOutlineListView(BRect(10,10,rect.Width() - 10 - B_V_SCROLL_BAR_WIDTH,100),menu);
			AddChild(new BScrollView("itemscroll",lview,B_FOLLOW_LEFT | B_FOLLOW_TOP,0,false,true));
			BTextControl *tcont = new BTextControl(BRect(10,110,rect.Width() - 10,130),"label","Label: ","",new BMessage(-4001));
			tcont->SetDivider(be_plain_font->StringWidth(tcont->Label()));
			tcont->SetEnabled(false);
			AddChild(tcont);
			char msg[5];
			msg[0] = 0;
			strcpy(msg,"none");
			/*if (((BControl *)(pointer))->Message() != NULL)
				strncpy(msg,(char *)(new int32(((BControl *)(pointer))->Message()->what)),4);*/
			BTextControl *name = new BTextControl(BRect(10,130,rect.Width() - 10,150),"msg","Message: ",msg,new BMessage(-4002));
			name->TextView()->SetMaxBytes(4);
			name->SetDivider(be_plain_font->StringWidth("Message: "));
			name->SetEnabled(false);
			AddChild(name);
			tcont = new BTextControl(BRect(10,150,be_plain_font->StringWidth("Shortcut: ") + 40,170),"shrtct","Shortcut: ","",new BMessage(-4003));
			tcont->SetDivider(be_plain_font->StringWidth(tcont->Label()));
			tcont->SetEnabled(false);
			tcont->TextView()->SetMaxBytes(1);
			AddChild(tcont);
			BMenu *meniu = new BMenu("Modifier Keys");
			meniu->AddItem(new BMenuItem("Option",new BMessage(-4004)));
			meniu->AddItem(new BMenuItem("Shift",new BMessage(-4004)));
			meniu->AddItem(new BMenuItem("Control",new BMessage(-4004)));
			AddChild(new BMenuField(BRect(be_plain_font->StringWidth("Shortcut: ") + 50,150,rect.Width() - 10,170),"modchoice",NULL,meniu));
			((BMenuField *)(FindView("modchoice")))->SetEnabled(false);
			meniu->SetRadioMode(false);
			meniu->SetLabelFromMarked(false);
			AddChild(new BCheckBox(BRect(10,175,rect.Width() - 10,195),"enabled","Enabled",new BMessage(-4005)));
			((BCheckBox *)(FindView("enabled")))->SetEnabled(false);
			AddChild(new BButton(BRect(10,200,(rect.Width()/2)-10,220),"addiantem","Add Item",new BMessage(-4000)));
			AddChild(new BButton(BRect((rect.Width()/2)+10,200,rect.Width() - 10,220),"addsepitem","Add Separator",new BMessage(-4006)));
		}
		void MessageReceived(BMessage *msg) {
			BMenuItem *item;
			switch(msg->what) {
				case -4000:
					item = new BMenuItem("New Item",new BMessage('none'));
					menu->AddItem(item);
					lview->Refresh(false);
					break;
				case -4001:
					item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
					item->SetLabel(((BTextControl *)(FindView("label")))->Text());
					lview->Refresh(true);
					break;
				case -4002:
					item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
					item->SetMessage(new BMessage(flipcode(*((int32 *)(((BTextControl *)(FindView("msg")))->Text())))));
					break;
				case -4004:
					msg->FindPointer("source",(void **)(&item));
					if (item->IsMarked())
						item->SetMarked(false);
					else
						item->SetMarked(true);
				case -4003:
					item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
					item->SetShortcut(*(((BTextControl *)(FindView("shrtct")))->Text()),make_mod_mask());
					break;
				case -4005:
					item = ((MenuListItem *)(lview->FullListItemAt(lview->FullListCurrentSelection())))->item;
					if (((BCheckBox *)(FindView("enabled")))->Value() == B_CONTROL_ON)
						item->SetEnabled(true);
					else
						item->SetEnabled(false);
					break;
				case -4006:
					item = new BSeparatorItem;
					menu->AddItem(item);
					lview->Refresh(false);
					break;
			}
		}
		uint32 make_mod_mask(void) {
			BMenu *men = ((BMenuField *)(FindView("modchoice")))->Menu();
			uint32 modifiers = 0;
			if (men->ItemAt(0)->IsMarked())
				modifiers = modifiers | B_OPTION_KEY;
			if (men->ItemAt(1)->IsMarked())
				modifiers = modifiers | B_SHIFT_KEY;
			if (men->ItemAt(2)->IsMarked())
				modifiers = modifiers | B_CONTROL_KEY;
			return modifiers;
		}	
	private:
		DragOutlineListView *lview;
		BMenu *menu;
};

BCheckBox *menubox;

class EditingPanel : public BView {
	public:
		EditingPanel(void) : BView(BRect(0,0,300,350),"editpanel",B_FOLLOW_ALL_SIDES,0) {
			pointer = NULL;
			SetViewColor(216,216,216);
			iscontrol = false;
			AddChild(new BStringView(BRect(25,100,175,125),"noactive","No Focus View"));
		}
		void SetTo(BView *active) {
			Window()->Lock();
			SetPart2(active);
			if (Window()->IsLocked())
				Window()->Unlock();
		}
		void SetPart2(BView *active) {
			for (BView *dead = ChildAt(0); dead != NULL; dead = ChildAt(0)) {
				RemoveChild(dead);
			}
			pointer = active;
			if (active == NULL) {
				AddChild(new BStringView(BRect(25,100,175,125),"noactive","No Focus View"));
				return;
			}
			//-----------General BView------------
			char classname[512];
			strcpy(classname,class_name(pointer));
			char *temp = new char[strlen("Element Type: ")+strlen(classname)+1];
			sprintf(temp,"Element Type: %s",classname);
			AddChild(new BStringView(BRect(10,10,190,30),"classname",temp));
			delete temp;
			BTextControl *name = new BTextControl(BRect(10,30,190,50),"name","View Name: ",((BButton *)(pointer))->Name(),new BMessage(-400));
			name->SetDivider(be_plain_font->StringWidth("View Name: "));
			AddChild(name);
			//-----------Base Classes----------------
			//-----------BControl------------------
			if (is_kind_of(pointer,BControl)) {
				name = new BTextControl(BRect(10,50,190,70),"label","Label: ",((BControl *)(pointer))->Label(),new BMessage(-500));
				name->SetDivider(be_plain_font->StringWidth("Label: "));
				AddChild(name);
				char msg[5];
				msg[0] = 0;
				if (((BControl *)(pointer))->Message() != NULL)
					strncpy(msg,(char *)(new int32(flipcode(((BControl *)(pointer))->Message()->what))),4);
				name = new BTextControl(BRect(10,70,190,90),"msg","Message: ",msg,new BMessage(-600));
				name->TextView()->SetMaxBytes(4);
				name->SetDivider(be_plain_font->StringWidth("Message: "));
				AddChild(name);
				iscontrol = true;
			}
			//----------Individuals--------------
			//----------BSlider------------------
			if (is_kind_of(pointer,BSlider)) {
				BSlider *slider = (BSlider *)(pointer);
				name = new BTextControl(BRect(10,90,190,110),"minlabel","Left Label: ",slider->MinLimitLabel(),new BMessage(-750));
				name->SetDivider(be_plain_font->StringWidth("Left Label: "));
				AddChild(name);
				name = new BTextControl(BRect(10,110,190,130),"maxlabel","Right Label: ",slider->MaxLimitLabel(),new BMessage(-750));
				name->SetDivider(be_plain_font->StringWidth("Right Label: "));
				AddChild(name);
				BRadioButton *but = new BRadioButton(BRect(10,130,190,150),"rect","Rectangular Thumb",new BMessage(-700));
				BRadioButton *but2 = new BRadioButton(BRect(10,150,190,170),"rect","Triangular Thumb",new BMessage(-701));
				if (slider->Style() == B_BLOCK_THUMB)
					but->SetValue(B_CONTROL_ON);
				else
					but2->SetValue(B_CONTROL_ON);
				AddChild(but);
				AddChild(but2);
			}
			//----------BStringView-------------
			if ((is_kind_of(pointer,BStringView)) && (!(is_kind_of(pointer,ResourceStringView)))) {
				BStringView *strview = (BStringView *)(pointer);
				name = new BTextControl(BRect(10,50,190,70),"text","Text: ",strview->Text(),new BMessage(-800));
				name->SetDivider(be_plain_font->StringWidth("Text: "));
				AddChild(name);
			}
			//---------ResourceStringView-------
			if (is_kind_of(pointer,ResourceStringView)) {
				ResourceStringView *img = (ResourceStringView *)pointer;
				int32 id = img->ResourceID();
				char idtext[15];
				sprintf(idtext,"%ld",id);
				name = new BTextControl(BRect(10,50,190,70),"idnum","Resource ID: ",idtext,new BMessage(-1300));
				name->SetDivider(be_plain_font->StringWidth("Resource ID: "));
				AddChild(name);
			}
			//---------BBox---------------------
			if (is_kind_of(pointer,BBox)) {
				BBox *box = (BBox *)(pointer);
				name = new BTextControl(BRect(10,50,190,70),"label","Label: ",box->Label(),new BMessage(-900));
				name->SetDivider(be_plain_font->StringWidth("Label: "));
				BRadioButton *but1 = new BRadioButton(BRect(10,70,190,90),"fancy","Fancy Border",new BMessage(-901));
				BRadioButton *but2 = new BRadioButton(BRect(10,90,190,110),"plain","Plain Border",new BMessage(-902));
				BRadioButton *but3 = new BRadioButton(BRect(10,110,190,130),"none","No Border",new BMessage(-903));
				AddChild(name);
				AddChild(but1);
				AddChild(but2);
				AddChild(but3);
				Window()->Lock();
				switch (box->Border()) {
					case B_NO_BORDER:
						but3->SetValue(B_CONTROL_ON);
						break;
					case B_PLAIN_BORDER:
						but2->SetValue(B_CONTROL_ON);
						break;
					case B_FANCY_BORDER:
						but1->SetValue(B_CONTROL_ON);
				}
				if (Window()->IsLocked())
					Window()->Unlock();
			}
			//---------ResImageView---------------
			if (is_kind_of(pointer,ResImageView)) {
				ResImageView *img = (ResImageView *)(pointer);
				int32 id = img->ResourceID();
				char idtext[15];
				sprintf(idtext,"%ld",id);
				name = new BTextControl(BRect(10,50,190,70),"idnum","Resource ID: ",idtext,new BMessage(-1000));
				name->SetDivider(be_plain_font->StringWidth("Resource ID: "));
				AddChild(name);
			}
			//----------BMenuField----------------
			if (is_kind_of(pointer,BMenuField)) {
				BMenuField *mfield = (BMenuField *)(pointer);
				name = new BTextControl(BRect(10,50,190,70),"label","Label: ",mfield->Label(),new BMessage(-1100));
				name->SetDivider(be_plain_font->StringWidth("Label: "));
				AddChild(name);
				name = new BTextControl(BRect(10,70,190,90),"deftext","Default Menu Text: ",mfield->Menu()->Name(),new BMessage(-1101));
				name->SetDivider(be_plain_font->StringWidth(name->Label()));
				AddChild(name);
				MenuEditor *items = new MenuEditor(mfield->Menu(),BRect(20,90,240,320));
				AddChild(items);
			}
			//----------BMenuBar------------------
			if (is_kind_of(pointer,BMenuBar)) {
				BMenuBar *mbar = (BMenuBar *)(pointer);
				MenuEditor *items = new MenuEditor(mbar,BRect(20,50,240,280));
				AddChild(items);
			}
			//-----------BColorControl------------
			if (is_kind_of(pointer,BColorControl)) {
				FindView("label")->RemoveSelf();
				FindView("msg")->MoveTo(10,50);
				BColorControl *colpicker = (BColorControl *)(pointer);
				BPopUpMenu *alignment = new BPopUpMenu("Item Alignment");
				alignment->AddItem(new BMenuItem("4 by 64",new BMessage(-1200)));
				alignment->AddItem(new BMenuItem("8 by 32",new BMessage(-1200)));
				alignment->AddItem(new BMenuItem("16 by 16",new BMessage(-1200)));
				alignment->AddItem(new BMenuItem("64 by 4",new BMessage(-1200)));
				alignment->AddItem(new BMenuItem("32 by 8",new BMessage(-1200)));
				BMenuField *items = new BMenuField(BRect(10,80,190,100),"alignpicker","Cell Alignment: ",alignment);
				items->SetDivider(be_plain_font->StringWidth(items->Label()));
				switch (colpicker->Layout()) {
					case B_CELLS_4x64:
						alignment->ItemAt(0)->SetMarked(true);
						break;
					case B_CELLS_8x32:
						alignment->ItemAt(1)->SetMarked(true);
						break;
					case B_CELLS_16x16:
						alignment->ItemAt(2)->SetMarked(true);
						break;	
					case B_CELLS_64x4:
						alignment->ItemAt(3)->SetMarked(true);
						break;	
					case B_CELLS_32x8:
						alignment->ItemAt(4)->SetMarked(true);
						break;
				}
				AddChild(items);
			}
		}
		void messages(BMessage *msg);
		BResources *reso;
	private:
		bool iscontrol;
		BView *pointer;
};

EditingPanel *editpanel;

class DeleteFilter : public BMessageFilter {
	public:
		DeleteFilter(void) : BMessageFilter(B_KEY_DOWN) {}
		filter_result Filter(BMessage *message,BHandler **target);
};

class ActivateFilter : public BMessageFilter {
	public:
		ActivateFilter(void) : BMessageFilter(B_MOUSE_DOWN) {}
		filter_result Filter(BMessage *message,BHandler **target);
}; 

class DropFilter : public BMessageFilter {
	public:
		DropFilter(void) : BMessageFilter(B_DROPPED_DELIVERY,B_ANY_SOURCE) {}
		filter_result Filter(BMessage *message,BHandler **target);
};
			

class EditingControl : public BView {
	public:
		EditingControl(void) : BView(BRect(0,0,1,1),"edithandle",B_FOLLOW_TOP | B_FOLLOW_LEFT,B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE | B_PULSE_NEEDED) {
			SetViewColor(255,255,255);
			//SetLowColor(B_TRANSPARENT_32_BIT);
			controlled = NULL;
			alreadyDrawing = false;
			notInit = true;
			/*Window()->Lock();
			Window()->Unlock();*/
		}
		void Draw(BRect area) {
			edge = Bounds();
			if (alreadyDrawing == true) {
				FillRect(BRect(edge.left,edge.top,controlled->Bounds().left,edge.bottom),B_SOLID_LOW);
				FillRect(BRect(edge.left,controlled->Bounds().bottom,edge.right,edge.bottom),B_SOLID_LOW);
				FillRect(BRect(controlled->Bounds().right,edge.top,edge.right,edge.bottom),B_SOLID_LOW);
				FillRect(BRect(edge.left,edge.top,edge.right,controlled->Bounds().top),B_SOLID_LOW);
				StrokeRect(Bounds(),B_MIXED_COLORS);
				FillRect(BRect(edge.right-5,edge.bottom-5,edge.right,edge.bottom));
				alreadyDrawing = false;
				return;
			}
			alreadyDrawing = true;
			if (controlled != NULL) {
				controlled->Draw(controlled->Bounds());
				Draw(area);
			} else
				alreadyDrawing = false;
		}
		void KeyDown(const char *bytes,int32 numBytes) {
			if (controlled == NULL)
				return;
			if (numBytes != 1)
				return;
			if ((*bytes == B_DELETE) || (*bytes == B_BACKSPACE)) {
				Window()->Lock();
				controlled->RemoveSelf();
				if (is_kind_of(controlled,BMenuBar)) {
					main->Lock();
					menubox->SetValue(B_CONTROL_OFF);
					main->Unlock();
				}
				delete controlled;
				controlled = NULL;
				notInit = true;
				editpanel->SetTo(NULL);
				ResizeTo(1,1);
				MoveTo(0,0);
				Invalidate(Bounds());
				Window()->Unlock();
			}
		}
		void Pulse(void) {
			if (Window() != NULL) {
			if ((Window()->CurrentFocus() != controlled) && (Window()->CurrentFocus() != this)) {
				BView *cur = Window()->CurrentFocus();
				if (cur == NULL) {
					return;
				}
				if (is_kind_of(cur,BTextView)) {
					if (is_kind_of(cur->Parent(),BTextControl))
						cur = cur->Parent();
				}
				if (is_kind_of(cur,BMenu)) {
					if (is_kind_of(cur->Parent(),BMenuField))
						cur = cur->Parent();
				}
				if (is_kind_of(cur,BTextControl)) {
					if (is_kind_of(cur->Parent(),BColorControl))
						cur = cur->Parent();
				}
				if (cur == controlled)
					return;
				//if (Parent() != NULL) {
					//MakeFocus();
					BRect tempy = cur->ConvertToScreen(cur->Bounds());
					tempy = Window()->ConvertFromScreen(tempy);
					cur->RemoveSelf();
					cur->MoveTo(tempy.LeftTop());
					Window()->AddChild(cur);
					//cur->MakeFocus();
				//}
				if (controlled != NULL) {
					if (!(is_kind_of(controlled,BBox))) {
						if (controlled->Parent() == NULL)
							controlled->SetViewColor(255,255,255);
					}
				}
				BRect edge = cur->Frame();
				edge.left -= 5;
				edge.top -= 5;
				MoveTo(edge.left,edge.top);
				edge = cur->Bounds();
				edge.bottom += 10;
				edge.right += 10;
				ResizeTo(edge.right,edge.bottom);
				if (controlled != NULL) {
					BRect empty = controlled->ConvertToScreen(controlled->Bounds());
					empty = Window()->ConvertFromScreen(empty);
					BPoint empt = empty.LeftTop();
					empt.x -= 1;empt.y -= 1;
					BView *posparent = Window()->FindView(empt);
					if (posparent != NULL) {
						if (posparent->ConvertToScreen(posparent->Bounds()).Contains(controlled->ConvertToScreen(controlled->Bounds()))) {
							BRect wait = controlled->ConvertToScreen(controlled->Bounds());
							wait = posparent->ConvertFromScreen(wait);
							controlled->RemoveSelf();
							controlled->MoveTo(wait.LeftTop());
							posparent->AddChild(controlled);
						}
					}
				}
				if (controlled != NULL)
					//controlled->Invalidate(controlled->Bounds());
					controlled->SetViewColor(controlled->ViewColor());
				controlled = cur;
				if (controlled != NULL)
					notInit = false;
				SetResizingMode(controlled->ResizingMode());
				Invalidate(Bounds());
				editpanel->SetTo(cur);
			}
			}
		}
		void MouseDown(BPoint point) {
			if (controlled != NULL) {
				BPoint tempit = ConvertToScreen(point);
				tempit = controlled->ConvertFromScreen(tempit);
				if (controlled->Bounds().Contains(tempit)) {
					controlled->MouseDown(tempit);
					return;
				}
			}
			if (is_kind_of(controlled,BMenuBar))
				return;
			uint32 buttons;
			BPoint temp;
			uint32 tmpbtn;
			Window()->Lock();
			GetMouse(&point,&buttons,true);
			int32 xdiff = -32767;
			int32 ydiff = -32767;
			signed char resizing = -2;
			for(;true;snooze(20000)) {
				GetMouse(&temp,&tmpbtn,true);
				if (tmpbtn != B_PRIMARY_MOUSE_BUTTON)
					break;
				if (temp == point)
					continue;
				if (resizing == -2) {
					if ((point.x > Bounds().right - 5) && (point.y > Bounds().bottom - 5))
						resizing = true;
					else
						resizing = false;
				} else {
					if (resizing) {
						ResizeTo(point.x + 5,point.y + 5);
						controlled->ResizeTo(Bounds().right - 15,Bounds().bottom - 15);
					} else {
						if (xdiff == -32767) {
							xdiff = temp.x - Bounds().left;
							ydiff = temp.y - Bounds().top;
						} else {
							MoveBy(temp.x - xdiff,temp.y - ydiff);
							controlled->MoveTo(Frame().left + 5,Frame().top + 5);
						}
					}
				}
				point = temp;
			}
			Window()->Unlock();
			Invalidate(Bounds());
		}
		BRect edge;
		BView *controlled;
		bool alreadyDrawing;
		bool notInit;
};


class EditingWindow : public BWindow {
	public:
		EditingWindow(void) : BWindow(BRect(100,100,350,450),"New Window",B_TITLED_WINDOW,B_ASYNCHRONOUS_CONTROLS) {
			SetPulseRate(100000);
			cntrl = new EditingControl;
			AddChild(cntrl);
			delfilt = new DeleteFilter;
			AddCommonFilter(delfilt);
			actfilt = new ActivateFilter;
			AddCommonFilter(actfilt);
			dropfilt = new DropFilter;
			AddCommonFilter(dropfilt);
			reso = file;
		}
		EditingWindow(BMessage *archive) : BWindow(archive) {
			SetPulseRate(100000);
			cntrl = new EditingControl;
			AddChild(cntrl);
			delfilt = new DeleteFilter;
			AddCommonFilter(delfilt);
			actfilt = new ActivateFilter;
			AddCommonFilter(actfilt);
			dropfilt = new DropFilter;
			AddCommonFilter(dropfilt);
			reso = file;
		}
		bool QuitRequested(void) {
			main->PostMessage(B_QUIT_REQUESTED);
			return false;
		}
		BView *curfocus;
		EditingControl *cntrl;
		DeleteFilter *delfilt;
		ActivateFilter *actfilt;
		DropFilter *dropfilt;
		BResources *reso;
};

void EditingPanel::messages(BMessage *msg) {
	pointer->Window()->Lock();
	int32 idnum;
	switch(msg->what) {
		case -400:
			pointer->SetName(((BTextControl *)(FindView("name")))->Text());
			break;
		case -500:
			((BControl *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			if (is_kind_of(pointer,BTextControl))
				((BTextControl *)(pointer))->SetDivider(be_plain_font->StringWidth(((BTextControl *)(pointer))->Label())); 
			break;
		case -600:
			((BControl *)(pointer))->SetMessage(new BMessage(flipcode(*(int32 *)(((BTextControl *)(FindView("msg")))->Text()))));
			break;
		case -700:
			((BSlider *)(pointer))->SetStyle(B_BLOCK_THUMB);
			((BSlider *)(pointer))->Invalidate(pointer->Bounds());
			break;
		case -701:
			((BSlider *)(pointer))->SetStyle(B_TRIANGLE_THUMB);
			((BSlider *)(pointer))->Invalidate(pointer->Bounds());
			break;
		case -750:
			((BSlider *)(pointer))->SetLimitLabels(((BTextControl *)(FindView("minlabel")))->Text(),((BTextControl *)(FindView("maxlabel")))->Text());
			break;
		case -800:
			((BStringView *)(pointer))->SetText(((BTextControl *)(FindView("text")))->Text());
			break;
		case -900:
			((BBox *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			break;
		case -901:
			((BBox *)(pointer))->SetBorder(B_FANCY_BORDER);
			break;
		case -902:
			((BBox *)(pointer))->SetBorder(B_PLAIN_BORDER);
			break;
		case -903:
			((BBox *)(pointer))->SetBorder(B_NO_BORDER);
			break;
		case -1000:
			sscanf(((BTextControl *)(FindView("idnum")))->Text(),"%ld",&idnum);
			if (resources->HasResource('bits',idnum)) {
				((ResImageView *)(pointer))->SetTo(idnum,true); 
				//editingwin->cntrl->ResizeTo(pointer->Bounds().right+10,pointer->Bounds().bottom+10);
				editingwin->cntrl->controlled = NULL;
				editingwin->cntrl->notInit = true;
				pointer->MakeFocus();
				editingwin->cntrl->Pulse();
				//editingwin->cntrl->Draw(editingwin->cntrl->Bounds());
			} else
				beep();
			break;
		case -1100:
			((BMenuField *)(pointer))->SetLabel(((BTextControl *)(FindView("label")))->Text());
			((BMenuField *)(pointer))->SetDivider(be_plain_font->StringWidth(((BMenuField *)(pointer))->Label()));
			break; 
		case -1101:
			((BMenuField *)(pointer))->Menu()->SetName(((BTextControl *)(FindView("deftext")))->Text());
			((BMenuField *)(pointer))->Menu()->Superitem()->SetLabel(((BTextControl *)(FindView("deftext")))->Text());
			break;
		case -1300:
			sscanf(((BTextControl *)(FindView("idnum")))->Text(),"%ld",&idnum);
			if (resources->HasResource('CSTR',idnum)) {
				((ResourceStringView *)(pointer))->SetTo(idnum); 
				//editingwin->cntrl->ResizeTo(pointer->Bounds().right+10,pointer->Bounds().bottom+10);
				editingwin->cntrl->controlled = NULL;
				editingwin->cntrl->notInit = true;
				pointer->MakeFocus();
				editingwin->cntrl->Pulse();
				//editingwin->cntrl->Draw(editingwin->cntrl->Bounds());
			} else
				beep();
			break;
	}
	pointer->Window()->Unlock();
}

filter_result DeleteFilter::Filter(BMessage *message,BHandler **target) {
	char *name;
	message->FindString("bytes",&name);
	if (name == NULL)
		return B_DISPATCH_MESSAGE;
	if ((*name == B_BACKSPACE) && (is_kind_of(editingwin->CurrentFocus(),BTextView)))
		return B_DISPATCH_MESSAGE;
	if ((*name == B_DELETE) || (*name == B_BACKSPACE))
		*target = editingwin->cntrl;
	return B_DISPATCH_MESSAGE;
}

filter_result ActivateFilter::Filter(BMessage *message,BHandler **target) {
	if ((*target != editingwin->cntrl) && (*target != editingwin->cntrl->controlled)) {
		((BView *)(*target))->MakeFocus();
		editingwin->cntrl->Pulse();
		return B_SKIP_MESSAGE;
	}
	return B_DISPATCH_MESSAGE;
}

filter_result DropFilter::Filter(BMessage *msg,BHandler **target) {
	BMessage *message = new BMessage(*msg);
	BPoint point;
	point = ((BWindow *)(Looper()))->ConvertFromScreen(message->DropPoint());
	if (message == NULL)
		return B_DISPATCH_MESSAGE;
	BView *controlled;
	/*if (validate_instantiation(message,"ResImageView"))
		controlled = new ResImageView(message);
	else*/
	widemsg = message;
		controlled = cast_as(instantiate_object(message,&me),BView);
	if (controlled == NULL)
		return B_DISPATCH_MESSAGE;
	controlled->MoveTo(point);
	((BWindow *)(Looper()))->AddChild(controlled);
	BRect empty = controlled->ConvertToScreen(controlled->Bounds());
	empty = ((BWindow *)(Looper()))->ConvertFromScreen(empty);
	BPoint empt = empty.LeftTop();
	empt.x -= 1;empt.y -= 1;
	BView *posparent = ((BWindow *)(Looper()))->FindView(empt);
	if (posparent != NULL) {
		if (posparent->ConvertToScreen(posparent->Bounds()).Contains(controlled->ConvertToScreen(controlled->Bounds()))) {
			BRect wait = controlled->ConvertToScreen(controlled->Bounds());
			wait = posparent->ConvertFromScreen(wait);
			controlled->RemoveSelf();
			controlled->MoveTo(wait.LeftTop());
			posparent->AddChild(controlled);
		}
	}
	return B_SKIP_MESSAGE;
}

void setwindoweditor(BView *x);
uint32 make_bitmask(void);
bool startingup;

void setwindoweditor(BView *x) {
	startingup = true;
	x->SetViewColor(216,216,216);
	BTextControl *name = new BTextControl(BRect(10,10,190,30),"title","Title: ",editingwin->Title(),new BMessage(-100));
	name->SetDivider(be_plain_font->StringWidth("Title: "));
	x->AddChild(name);
	//----------Window Look-------------
		BPopUpMenu *menu = new BPopUpMenu("Document Window");
		BMessage *msg = new BMessage(-200);
		msg->AddInt32("newlook",B_DOCUMENT_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Document Window",msg));
		msg = new BMessage(-200);
		msg->AddInt32("newlook",B_TITLED_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Titled Window",msg));
		msg = new BMessage(-200);
		msg->AddInt32("newlook",B_FLOATING_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Floating Window",msg));
		msg = new BMessage(-200);
		msg->AddInt32("newlook",B_MODAL_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Modal Window",msg));
		msg = new BMessage(-200);
		msg->AddInt32("newlook",B_BORDERED_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Bordered Window",msg));
		msg = new BMessage(-200);
		msg->AddInt32("newlook",B_NO_BORDER_WINDOW_LOOK);
		menu->AddItem(new BMenuItem("Borderless Window",msg));
		switch (editingwin->Look()) {
			case B_DOCUMENT_WINDOW_LOOK:
				menu->ItemAt(0)->SetMarked(true);
				break;
			case B_TITLED_WINDOW_LOOK:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_FLOATING_WINDOW_LOOK:
				menu->ItemAt(2)->SetMarked(true);
				break;
			case B_MODAL_WINDOW_LOOK:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_BORDERED_WINDOW_LOOK:
				menu->ItemAt(4)->SetMarked(true);
				break;
			case B_NO_BORDER_WINDOW_LOOK:
				menu->ItemAt(5)->SetMarked(true);
				break;
		}
		BMenuField *field = new BMenuField(BRect(10,40,240,60),"look","Window Look: ",menu);
		field->SetDivider(be_plain_font->StringWidth("Window Look: "));
		x->AddChild(field);
	//----------Window Feel-------------
		menu = new BPopUpMenu("Normal");
		msg = new BMessage(-300);
		msg->AddInt32("newfeel",B_NORMAL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Normal",msg));
		msg = new BMessage(-300);
		msg->AddInt32("newfeel",B_MODAL_APP_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Modal Across Application",msg));
		msg = new BMessage(-300);
		msg->AddInt32("newfeel",B_MODAL_ALL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Modal Across System",msg));
		msg = new BMessage(-300);
		msg->AddInt32("newfeel",B_FLOATING_APP_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Floating Across Application",msg));
		msg = new BMessage(-300);
		msg->AddInt32("newfeel",B_FLOATING_ALL_WINDOW_FEEL);
		menu->AddItem(new BMenuItem("Floating Across System",msg));
		switch (editingwin->Feel()) {
			case B_NORMAL_WINDOW_FEEL:
				menu->ItemAt(0)->SetMarked(true);
				break;
			case B_MODAL_APP_WINDOW_FEEL:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_MODAL_SUBSET_WINDOW_FEEL:
				menu->ItemAt(1)->SetMarked(true);
				break;
			case B_MODAL_ALL_WINDOW_FEEL:
				menu->ItemAt(2)->SetMarked(true);
				break;
			case B_FLOATING_APP_WINDOW_FEEL:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_FLOATING_SUBSET_WINDOW_FEEL:
				menu->ItemAt(3)->SetMarked(true);
				break;
			case B_FLOATING_ALL_WINDOW_FEEL:
				menu->ItemAt(4)->SetMarked(true);
				break;
		}
		field = new BMenuField(BRect(10,70,240,90),"look","Window Behavior: ",menu);
		field->SetDivider(be_plain_font->StringWidth("Window Behavior: "));
		x->AddChild(field);
	//----------Window Flags------------
		BRect work = x->Bounds();
		work.left = 10;
		work.top = 100;
		work.bottom -= 5;
		work.right -= 10;
		work.right -= 10;
		int i = 20;
		int inc = 10;
		BBox *box = new BBox(work,"flags");
		box->SetLabel("Window Flags");
		msg = new BMessage(-250);
		msg->AddInt32("flags",B_NOT_MOVABLE);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"nmov","Movable",msg));
		msg = new BMessage(-250);
		msg->AddInt32("flags",B_NOT_CLOSABLE);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"clos","Closable",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_ZOOMABLE);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"zoom","Zoomable",msg));
		msg = new BMessage(-250);
		msg->AddInt32("flags",B_NOT_MINIMIZABLE);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"mini","Minimizable",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_H_RESIZABLE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"hres","Horizontally Resizable",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_V_RESIZABLE);
		box->AddChild(new BCheckBox(BRect(10,i,125,i+inc),"vres","Vertically Resizable",msg));
		menubox = new BCheckBox(BRect(130,i,210,i+inc),"menus","Menu Bar",new BMessage(-258));
		box->AddChild(menubox);
		if (editingwin->KeyMenuBar() != NULL)
			menubox->SetValue(B_CONTROL_ON);
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_OUTLINE_RESIZE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"roiw","Resize with Outline Instead of Window",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_WILL_ACCEPT_FIRST_CLICK);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"wafc","Will Accept First Click",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_AVOID_FRONT);
		box->AddChild(new BCheckBox(BRect(10,i,120,i+inc),"avfr","Avoid Front",msg));
		msg = new BMessage(-250);
		msg->AddInt32("flags",B_AVOID_FOCUS);
		box->AddChild(new BCheckBox(BRect(130,i,210,i+inc),"avfo","Avoid Focus",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_NO_WORKSPACE_ACTIVATION);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"nwoa","Do Not Activate Workspace",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_NOT_ANCHORED_ON_ACTIVATE);
		box->AddChild(new BCheckBox(BRect(10,i,210,i+inc),"brcu","Bring Window To Current Workspace",msg));
		msg = new BMessage(-250);
		i+= inc*2;
		msg->AddInt32("flags",B_ASYNCHRONOUS_CONTROLS);
		box->AddChild(new BCheckBox(BRect(10,i,210,+inc),"async","Asynchronous Controls (Should Be On)",msg));
		uint32 flags = editingwin->Flags();
		if (!(flags & B_NOT_MOVABLE))
			((BCheckBox *)(box->ChildAt(0)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_CLOSABLE))
			((BCheckBox *)(box->ChildAt(1)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_ZOOMABLE))
			((BCheckBox *)(box->ChildAt(2)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_MINIMIZABLE))
			((BCheckBox *)(box->ChildAt(3)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_H_RESIZABLE))
			((BCheckBox *)(box->ChildAt(4)))->SetValue(B_CONTROL_ON);
		if (!(flags & B_NOT_V_RESIZABLE))
			((BCheckBox *)(box->ChildAt(5)))->SetValue(B_CONTROL_ON);
		if (flags & B_OUTLINE_RESIZE)
			((BCheckBox *)(box->ChildAt(7)))->SetValue(B_CONTROL_ON);
		if (flags & B_WILL_ACCEPT_FIRST_CLICK)
			((BCheckBox *)(box->ChildAt(8)))->SetValue(B_CONTROL_ON);
		if (flags & B_AVOID_FRONT)
			((BCheckBox *)(box->ChildAt(9)))->SetValue(B_CONTROL_ON);
		if (flags & B_AVOID_FOCUS)
			((BCheckBox *)(box->ChildAt(10)))->SetValue(B_CONTROL_ON);
		if (flags & B_NO_WORKSPACE_ACTIVATION)
			((BCheckBox *)(box->ChildAt(11)))->SetValue(B_CONTROL_ON);
		if (flags & B_NOT_ANCHORED_ON_ACTIVATE)
			((BCheckBox *)(box->ChildAt(12)))->SetValue(B_CONTROL_ON);
		if (flags & B_ASYNCHRONOUS_CONTROLS)
			((BCheckBox *)(box->ChildAt(13)))->SetValue(B_CONTROL_ON);
		x->AddChild(box);
		startingup = false;
}

uint32 make_bitmask(void) {
	BView *box = ((BTabView *)(main->FindView("tabs")))->TabAt(1)->View()->FindView("flags");
	uint32 bitmask = 0;
	if (((BCheckBox *)(box->ChildAt(0)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_MOVABLE;
	if (((BCheckBox *)(box->ChildAt(1)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_CLOSABLE;
	if (((BCheckBox *)(box->ChildAt(2)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_ZOOMABLE;
	if (((BCheckBox *)(box->ChildAt(3)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_MINIMIZABLE;
	if (((BCheckBox *)(box->ChildAt(4)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_H_RESIZABLE;
	if (((BCheckBox *)(box->ChildAt(5)))->Value() == B_CONTROL_OFF)
		bitmask = bitmask | B_NOT_V_RESIZABLE;
	if (((BCheckBox *)(box->ChildAt(7)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_OUTLINE_RESIZE;
	if (((BCheckBox *)(box->ChildAt(8)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_WILL_ACCEPT_FIRST_CLICK;
	if (((BCheckBox *)(box->ChildAt(9)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_AVOID_FRONT;
	if (((BCheckBox *)(box->ChildAt(10)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_AVOID_FOCUS;
	if (((BCheckBox *)(box->ChildAt(11)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_NO_WORKSPACE_ACTIVATION;
	if (((BCheckBox *)(box->ChildAt(12)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_NOT_ANCHORED_ON_ACTIVATE;
	if (((BCheckBox *)(box->ChildAt(13)))->Value() == B_CONTROL_ON)
		bitmask = bitmask | B_ASYNCHRONOUS_CONTROLS;
	return bitmask;
}
		

void loaddata(unsigned char *data,size_t length,BView *bkgview) {
	resources = file;
	startingup = false;
	main = bkgview->Window();
	//-------------Init Editable Window------------------
	if (length == 0) {
		editingwin = new EditingWindow;
	} else {
		current = new BMessage;
		current->Unflatten((char *)data);
		editingwin = new EditingWindow(current);
	}
	danger_feel = editingwin->Feel();
	editingwin->Show();
	//-------------Init Focus View & Window Options------
	bkgview->Window()->SetFlags(B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_WILL_ACCEPT_FIRST_CLICK);
	bkgview->Window()->SetLook(B_FLOATING_WINDOW_LOOK);
	bkgview->Window()->ResizeTo(250,350);
	BTabView *tabs = new BTabView(BRect(0,0,350,350),"tabs",B_WIDTH_FROM_WIDEST);
	editpanel = new EditingPanel;
	tabs->AddTab(editpanel);
	tabs->TabAt(0)->SetLabel("Focus View");
	BView *x = new BView(BRect(0,0,250,330),"wineditpanel",B_FOLLOW_ALL_SIDES,0);
	setwindoweditor(x);
	editingwin->Lock();
	if ((danger_feel == B_MODAL_APP_WINDOW_FEEL) || (danger_feel == B_MODAL_ALL_WINDOW_FEEL))
		editingwin->SetFeel(B_NORMAL_WINDOW_FEEL);
	editingwin->Unlock();
	tabs->AddTab(x);
	tabs->TabAt(1)->SetLabel("Window");
	wineditor = x;
	//-------------Init Toolbar---------------------------
	BView *y = new BView(BRect(0,0,250,300),"viewtoolbar",B_FOLLOW_ALL_SIDES,0);
	y->SetViewColor(216,216,216);
	y->AddChild(new BStringView(BRect(20,210,20+be_plain_font->StringWidth("Drag a List Item to the Window to Add It"),230),"instructions","Drag a List Item to the Window to Add It",B_FOLLOW_TOP | B_FOLLOW_H_CENTER));
	BOutlineListView *list = new InterfaceListView;
	BStringItem *controls = new BStringItem("Controls");
	BStringItem *visuals = new BStringItem("Visual Organizers");
	BStringItem *editable = new BStringItem("Editable Items");
		list->AddItem(controls);
			list->AddUnder(new InterfaceItem("Button",new BButton(BRect(0,0,100,50),"newbutton","New Button",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Checkbox",new BCheckBox(BRect(0,0,100,50),"newcheckbox","New Checkbox",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Radio Button",new BRadioButton(BRect(0,0,100,50),"newrbutton","New Radio Button",new BMessage('none'))),controls);
			list->AddUnder(new InterfaceItem("Slider",new BSlider(BRect(0,0,100,50),"newslider","New Slider",new BMessage('none'),0,100)),controls);
			BMenuField *tempx = new BMenuField(BRect(0,0,100,50),"newpopupmenu","New Pop-Up Menu: ",new BPopUpMenu("newpopupmenu"));
			tempx->SetDivider(be_plain_font->StringWidth(tempx->Label()));
			list->AddUnder(new InterfaceItem("Pop-Up Menu",tempx),controls);
			list->AddUnder(new InterfaceItem("Color Picker",new BColorControl(BPoint(0,0),B_CELLS_32x8,8,"newcolpicker",new BMessage('none'))),controls);
		list->AddItem(visuals);
			list->AddUnder(new InterfaceItem("Static Text",new BStringView(BRect(0,0,100,50),"newstatictext","Static Text")),visuals);
			list->AddUnder(new InterfaceItem("Resource Text",new ResourceStringView()),visuals);
			list->AddUnder(new InterfaceItem("Box",new BBox(BRect(0,0,100,50),"newbox")),visuals);
			list->AddUnder(new InterfaceItem("Bitmap",new ResImageView(resources)),visuals);
		list->AddItem(editable);
			list->AddUnder(new InterfaceItem("Text Box",new BTextView(BRect(0,0,100,50),"newtextbox",BRect(5,5,95,45),B_FOLLOW_LEFT | B_FOLLOW_TOP,B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED)),editable);
			BTextControl *tempy = new BTextControl(BRect(0,0,100,50),"newtextfield","Text Field ",NULL,new BMessage('none'));
			tempy->SetDivider(be_plain_font->StringWidth(tempy->Label()));
			list->AddUnder(new InterfaceItem("Text Field",tempy),editable);
	y->AddChild(new BScrollView("scroller",list,B_FOLLOW_ALL_SIDES,0,false,true));
	tabs->AddTab(y);
	tabs->TabAt(2)->SetLabel("Elements");
	bkgview->AddChild(tabs);
}
	
	
unsigned char* savedata(size_t *length,BView *bkgview) {
	BMessage *archive = new BMessage;
	editingwin->SetFeel(danger_feel);
	editingwin->Lock();
	editingwin->cntrl->RemoveSelf();
	editingwin->RemoveCommonFilter(editingwin->delfilt);
	editingwin->RemoveCommonFilter(editingwin->actfilt);
	editingwin->RemoveCommonFilter(editingwin->dropfilt);
	delete editingwin->cntrl;
	if (editingwin->Archive(archive,true) != B_OK)
		(new BAlert("yikes","Archive is failing...","OK"))->Go();
	//archive = current;
	/**length = archive->FlattenedSize();
	unsigned char *data = new unsigned char[*length];
	if (archive->Flatten((char *)(data),NULL) != B_OK)
		(new BAlert("whoa","Flatten is failing...","OK"))->Go();*/
	editingwin->Unlock();
	BMallocIO buffer;
	ssize_t xyz;
	archive->Flatten(&buffer,&xyz);
	*length = xyz;
	unsigned char *data = new unsigned char[*length];
	memcpy(data,buffer.Buffer(),*length);
	delete archive;
	editingwin->Quit();
	//(new BAlert("helo","savedata executing properly.","OK"))->Go();
	return data;
}

void messaging(BMessage *msg,BView *bkgview){
	if (msg->what <= -4000) {
		BView *x = bkgview->FindView("menueditor");
		if (x != NULL) {
			x->MessageReceived(msg);
			return;
		}
	}
	if (msg->what <= -400) {
		editpanel->messages(msg);
		return;
	}
	//(new BAlert("helo","Code executing properly.","OK"))->Go();
	int32 temp;
	BCheckBox *cbox;
	BMenuBar *mbar;
	switch (msg->what) {
		case -100:
			editingwin->SetTitle(((BTextControl *)(wineditor->FindView("title")))->Text());
			break;
		//case -200:
		//	editingwin->AddChild(new BButton(BRect(50,50,150,100),"blank","Blank Button",new BMessage(-200)));
		//	break;
		case -200:
			msg->FindInt32("newlook",&temp);
			editingwin->SetLook(window_look(temp));
			break;
		case -250:
			editingwin->Lock();
			editingwin->SetFlags(make_bitmask());
			editingwin->Unlock();
			break;
		case -258:
			if (startingup)
				break;
			msg->FindPointer("source",(void **)(&cbox));
			if (cbox->Value() == B_CONTROL_ON) {
				mbar = new BMenuBar(BRect(0,0,editingwin->Frame().right,30),"menubar");
				mbar->AddItem(new BMenuItem("New Menu Bar",new BMessage('none')));
				editingwin->AddChild(mbar);
			} else {
				editingwin->Lock();
				if (editingwin->cntrl->controlled == editingwin->KeyMenuBar())
					editingwin->cntrl->KeyDown(new char(B_DELETE),1);
				else
					editingwin->KeyMenuBar()->RemoveSelf();
				editingwin->Unlock();
			}
			break;
		case -300:
			msg->FindInt32("newfeel",&temp);
			danger_feel = window_feel(temp);
			if (!((temp == B_MODAL_APP_WINDOW_FEEL) || (temp == B_MODAL_ALL_WINDOW_FEEL)))
				editingwin->SetFeel(window_feel(temp));
			else
				editingwin->SetFeel(B_NORMAL_WINDOW_FEEL);
			break;
	}
}



void ResImageView::AttachedToWindow(void) {
	if (is_kind_of(Window(),EditingWindow)) {
		res = ((EditingWindow *)(Window()))->reso;
		ResourceIO io('bits',resid, res);
		bitmap = BTranslationUtils::GetBitmap(&io);
		if (bitmap != NULL)
			Invalidate(Bounds());
	} else {
		bitmap = NULL;
	}
	BView::AttachedToWindow();
}

void ResourceStringView::AttachedToWindow(void) {
	if (is_kind_of(Window(),EditingWindow)) {
		res = ((EditingWindow *)(Window()))->reso;
		SetTo(id);
	} else {
		SetText("No String Set");
	}
	BView::AttachedToWindow();
}