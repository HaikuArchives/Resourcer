typedef enum list_state{
	collapsed,
	expanded
};

int sortList(const BListItem *one,const BListItem *two);

class DoubleItem;

bool alphabet(const char *string1, const char *string2);
	
bool alphabet(const char *string1, const char *string2) {
	size_t length1 = strlen(string1);
	size_t length2 = strlen(string2);
	size_t length;
	bool xgreater = false;
	if (length1 > length2)
		length = length2;
	else
		length = length1;
	size_t i;
	BString s1(string1);
	BString s2(string2);
	s1.ToLower();
	s2.ToLower();
	for (i = 0;i < length;i++) {
		if (s1[i] != s2[i]) {
			if (s1[i] > s2[i]) {
				xgreater = true;
				break;
			} else {
				xgreater = false;
				break;
			}
		}
	}
	return xgreater;
}

class TypeItem : public BStringItem {
	public:
		TypeItem(type_code code) : BStringItem("type") {
			type = code;
			app_info info;
			be_app->GetAppInfo(&info);
			BEntry entry(&(info.ref));
			BDirectory dir;
			entry.GetParent(&dir);
			entry.SetTo(&dir,"editors");
			BPath path;
			entry.GetPath(&path);
			path.Append(TypeCode());
			if (entry.Exists() == false) {
				description = "Unknown";
			} else {
				image_id editor = load_add_on(path.Path());
				char *temp;
				if (get_image_symbol(editor,"description",B_SYMBOL_TYPE_DATA,(void **)(&temp)) != B_NO_ERROR)
					description = "Unknown";
				else {
					description = new char[strlen(temp) + 1];
					strcpy(description,temp);
				}
			}
			char *tempy = new char[10 + strlen(description)];
			sprintf(tempy,"%s (%s)",TypeCode(),description);
			SetText(tempy);
		}
		char *TypeCode(void) {
			type_code code = flipcode(type);
			char *typea = new char[5];
			strncpy(typea,(char *)(&code),4);
			typea[4] = 0;
			return typea;
		}
		void SetTypeCode(char *code) {
			 strncpy((char *)(&type),code,4);
			 type = flipcode(type);
		}
		type_code type;
		char *description;
};

class DoubleItem : public BListItem {
	public:
		DoubleItem(int32 resid,char *resname,BOutlineListView *owner,bool isattr) : BListItem(0,true) {
			if (isattr) {
				idstring = NULL;
			} else {
				idstring = new char[20];
				sprintf(idstring,"%ld",resid);
			}
			name = new char[strlen(resname) + 1];
			strcpy(name,resname);
			id = resid;
			win = NULL;
			parent = owner;
		}
		void DrawItem(BView *owner, BRect frame, bool complete = false) {
			if (IsSelected()) {
				owner->SetHighColor(150,150,150);
				owner->SetLowColor(150,150,150);
			} else
				owner->SetHighColor(255,255,255);
			owner->FillRect(frame);
			owner->SetHighColor(0,0,0);
			if (idstring != NULL) {
				owner->DrawString(idstring,BPoint(frame.left + 5,frame.top + 10));
				owner->DrawString(name,BPoint(frame.left + 50,frame.top + 10));
			} else {
				owner->DrawString(name,BPoint(frame.left + 5,frame.top + 10));
			}
			owner->SetLowColor(255,255,255);
		}
		void Invoke(bool generic = false);
		~DoubleItem(void) {
			delete name;
			if (idstring != NULL)
				delete idstring;
		}
		BOutlineListView *parent;
		int32 id;
		char *idstring;
		char *name;
		pluginwindow *win;
};

int sortList(const BListItem *one,const BListItem *two) {
	if (is_instance_of((BListItem *)one,BStringItem)) {
		return 0;
	}
	/*if (one->OutlineLevel() != two->OutlineLevel())
		(new BAlert("ck","Yikes!","OK"))->Go();*/
	if (is_kind_of((BListItem *)one,TypeItem)) {
		if (alphabet(((TypeItem *)(one))->TypeCode(),((TypeItem *)(two))->TypeCode()))
			return 1;
		else
			return -1;
	}
	if (is_kind_of((BListItem *)one,DoubleItem)) {
		if (((DoubleItem *)(one))->idstring != NULL) {
			if (((DoubleItem *)(one))->id > ((DoubleItem *)(two))->id)
				return 1;
			else
				return -1;
		} else {	
			if (alphabet(((DoubleItem *)(one))->name,((DoubleItem *)(two))->name))
				return 1;
			else
				return -1;
		}
	}
	return 0;
}

class restypeview : public BOutlineListView {
	public:
		restypeview(BResources *openres) : BOutlineListView(BRect(0,1 + mbheight,350 - B_V_SCROLL_BAR_WIDTH,400 - B_H_SCROLL_BAR_HEIGHT),"restype",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES) {
			Init(openres);
			SetInvocationMessage(new BMessage('rsiv'));
		}
		//void MoveToAttr(DoubleItem *item);
		//void MoveToRes(DoubleItem *item);
		void Init(BResources *openres) {
			attr_info inf;
			list_state rescol;
			list_state attrcol;
			BNode node("/boot");
			if (node.GetAttrInfo("rescol",&inf) == B_OK) {
				node.ReadAttr("rescol",'STAT',0,&rescol,sizeof(list_state));
				node.ReadAttr("attrcol",'STAT',0,&attrcol,sizeof(list_state));
			} else {
				rescol = expanded;
				attrcol = expanded;
			}
			resources = new BStringItem("Resources");
			AddItem(resources);
			if (rescol == collapsed)
				Collapse(resources);
			attributes = new BStringItem("Attributes");
			AddItem(attributes);
			if (attrcol == collapsed)
				Collapse(attributes);
			BPoint point = find_center(Bounds(),be_plain_font->StringWidth("Loading Resources..."),20).LeftTop();
			DrawString("Loading Resources...",point);
			AddResources(openres);
			Invalidate(Bounds());
			if (CountItemsUnder(resources,true) == 0) {
				resources->SetText("No Resources");
				resources->SetEnabled(false);
			}
			if (CountItemsUnder(attributes,true) == 0) {
				attributes->SetText("No Attributes");
				attributes->SetEnabled(false);
			}
		}
		void AddResources(BResources *res) {
			int32 index;
			type_code type_found;
			int32 id;
			char *name;
			size_t length;
			for (index = 0;res->GetResourceInfo(index, &type_found, &id,(const char **)(&name), &length);index++) {
				AddUnder(new DoubleItem(id,name,this,false),FindType(type_found,false));
			}
			BFile file(res->File());
			file.RewindAttrs();
			attr_info attrinfo;
			name = new char[B_ATTR_NAME_LENGTH];
			for (index = 0;file.GetNextAttrName(name) == B_OK;index++) {
				file.GetAttrInfo(name,&attrinfo);
				type_found = attrinfo.type;
				AddUnder(new DoubleItem(id,name,this,true),FindType(type_found,true));
			}
			Sort();
		}
		void Sort(void) {
			SortItemsUnder(resources,false,&sortList);
			SortItemsUnder(attributes,false,&sortList);
			BListItem *cur;
			for (int32 i = CountItemsUnder(resources,true); i >= 0; i--) {
				cur = ItemUnderAt(resources,true,i);
				SortItemsUnder(cur,false,&sortList);
			}
			for (int32 i = CountItemsUnder(attributes,true); i >= 0; i--) {
				cur = ItemUnderAt(attributes,true,i);
				SortItemsUnder(cur,false,&sortList);
			}
		}
		DoubleItem *AddResource(type_code type,int32 id,char *name,size_t length,void *data,bool isattr,bool invoke = true);
		TypeItem *FindType(type_code type,bool attr) {
			TypeItem *cur;
			if (attr) {
				attributes->SetText("Attributes");
				attributes->SetEnabled(true);
				for (int32 i = 0;true;i++) {
					cur = (TypeItem *)(ItemUnderAt(attributes,true,i));
					if (cur == NULL) {
						cur = new TypeItem(type);
						AddUnder(cur,attributes);
						Collapse(cur);
						break;
					}
					if (cur->type == type) {
						break;
					}
				}
				return cur;
			} else {
				resources->SetText("Resources");
				resources->SetEnabled(true);
				for (int32 i = 0;true;i++) {
					cur = (TypeItem *)(ItemUnderAt(resources,true,i));
					if (cur == NULL) {
						cur = new TypeItem(type);
						AddUnder(cur,resources);
						Collapse(cur);
						break;
					}
					if (cur->type == type) {
						break;
					}
				}
				return cur;
			}
		}
		void MouseMoved(BPoint point,uint32 transit,const BMessage *msg) {
			rgb_color oldcolor;
			switch (transit) {
				case B_ENTERED_VIEW:
					if (msg != NULL) {
						if (msg->HasPointer("this")) {
							restypeview *t;
							msg->FindPointer("this",(void **)&t);
							if (t == this)
								break;
						}
						oldcolor = HighColor();
						SetHighColor(0,152,255);
						StrokeRect(Bounds());
						SetHighColor(oldcolor);
					}
					break;
				case B_INSIDE_VIEW:
					break;
				case B_EXITED_VIEW:
					StrokeRect(Bounds(),B_SOLID_LOW);
					break;
			}
		}
		void SelectionChanged(void) {
			if (CurrentSelection() >= 0) {
				if (ItemAt(CurrentSelection())->OutlineLevel() != 2)
					goto DISABLEITEMS;
				BMenuBar *mbar = Window()->KeyMenuBar();
				mbar->FindItem(-200)->SetEnabled(true);
				mbar->FindItem(-300)->SetEnabled(true);
				mbar->FindItem(-400)->SetEnabled(true);
				mbar->FindItem(-500)->SetEnabled(true);
				mbar->FindItem(-800)->SetEnabled(true);
				mbar->FindItem(-900)->SetEnabled(true);
			} else {
				DISABLEITEMS:
				BMenuBar *mbar = Window()->KeyMenuBar();
				mbar->FindItem(-200)->SetEnabled(false);
				mbar->FindItem(-300)->SetEnabled(false);
				mbar->FindItem(-400)->SetEnabled(false);
				mbar->FindItem(-500)->SetEnabled(false);
				mbar->FindItem(-800)->SetEnabled(false);
				mbar->FindItem(-900)->SetEnabled(false);
			}
		}
		void MessageReceived(BMessage *msg);
		bool InitiateDrag(BPoint point,int32 index, bool selected);
		void KeyDown(const char *bytes, int32 numBytes);
		void DeleteSelection(void);
		BStringItem *resources;
		BStringItem *attributes;
};