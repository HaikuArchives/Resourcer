int32 copytoattr(void *y);
int32 copy(void *y);
int32 getdata(void *y);
int32 copytoattr(void *y);
int sortmenu(const void *first,const void *second);
type_code code_from_MIME(char *MIME);
char *MIME_from_code(type_code code);
	
class FillerView : public BView {
	public:
		FillerView(void) : BView(BRect(0,400-B_H_SCROLL_BAR_HEIGHT,350-B_V_SCROLL_BAR_WIDTH,400),"BottomFiller",B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM,B_WILL_DRAW | B_FRAME_EVENTS) {
			Draw(Bounds());
		}
		void Draw(BRect toUpdate) {
			BRect temp = toUpdate;
			temp.top = 2;
			SetHighColor(216,216,216);
			FillRect(temp);
			SetHighColor(255,255,255);
			temp.top = 2;
			temp.bottom = 2;
			FillRect(temp);
			SetHighColor(152,152,152);
			temp.top = 1;
			temp.bottom = 1;
			FillRect(temp);
		}
};

class reswindow : public BWindow {
	public:
	BRect gettiledrect(void) {
		BRect work = find_center(350,400);
		work.top += openmwindows * 30;
		work.left += openmwindows * 30;
		work.right += openmwindows * 30;
		work.bottom += openmwindows * 30;
		return work;
	}
	reswindow(const char *title,entry_ref ref) : BWindow(gettiledrect(),title,B_DOCUMENT_WINDOW,0/*B_ASYNCHRONOUS_CONTROLS*/) {
		istempfile = false;
		isquitting = false;
		changes = false;
		fileref = ref;
		file = new BFile(&ref,B_READ_WRITE);
		openres = new BResources;
		if (openres->SetTo(file) != B_NO_ERROR) {
			return;
		}
		openmwindows++;
		Show(); 
		menus = new BMenuBar(BRect(0,0,600,5),"Menus");
		AddChild(menus);
		Lock();
		mbheight = (int32)(menus->Bounds().bottom);
		Unlock();
		AddChild(new FillerView);
		BMenu *file = new BMenu("File");
		menus->AddItem(file);
		file->AddItem(new BMenuItem("About Resourcer...",new BMessage(B_ABOUT_REQUESTED)));
		file->AddItem(new BSeparatorItem);
		file->AddItem(new BMenuItem("New",new BMessage('newf'),'N'));
		file->AddItem(new BMenuItem("Open...",new BMessage('open'),'O'));
		file->AddItem(new BSeparatorItem);
		file->AddItem(new BMenuItem("Save",new BMessage('save'),'S'));
		file->AddItem(new BMenuItem("Save As...",new BMessage('svas'),'S',B_SHIFT_KEY));
		file->AddItem(new BSeparatorItem);
		file->AddItem(new BMenuItem("Revert Resources",new BMessage('rvrt'),'R'));
		file->AddItem(new BMenuItem("Edit File Type...",new BMessage('mime'),'M'));
		file->AddItem(new BSeparatorItem);
		BMenuItem *quit = new BMenuItem("Quit",new BMessage(B_QUIT_REQUESTED),'Q');
		quit->SetTarget(be_app);
		file->AddItem(quit);
		BMenu *edit = new BMenu("Edit");
		menus->AddItem(edit);
		edit->AddItem(new BMenuItem("Cut",new BMessage(-400),'X'));
		edit->AddItem(new BMenuItem("Copy",new BMessage(-500),'C'));
		edit->AddItem(new BMenuItem("Paste",new BMessage(B_PASTE),'V'));
		BMenu *res = new BMenu("Resource");
		menus->AddItem(res);
		res->AddItem(new BMenuItem("Add Resource...",new BMessage(-100),'A'));
		res->AddItem(new BMenuItem("Delete Resource",new BMessage(-200),'D'));
		res->AddItem(new BMenuItem("Resource Info...",new BMessage(-300),'I'));
		res->AddItem(new BSeparatorItem);
		res->AddItem(new BMenuItem("Copy to Attribute/Resource",new BMessage(-900)));
		res->AddItem(new BSeparatorItem);
		res->AddItem(new BMenuItem("Open with Generic Editor",new BMessage(-800)));
		this->res = new restypeview(openres);
		BScrollView *resa = new BScrollView("resa",this->res,B_FOLLOW_ALL_SIDES,0,false,true,B_NO_BORDER);
		AddChild(resa);
		this->res->SelectionChanged();
		Lock();
		this->res->MakeFocus(true);
		if (IsLocked())
			Unlock();
	}
	void CloseWindows(void);
	void WindowActivated (bool active) {
		if (active == true)
			be_app->SetCursor(B_HAND_CURSOR);
	}
	bool QuitRequested(void) {
		char text[800];
		sprintf(text,"Save changes to \"%s\"?\n\nChanges to attributes are saved automatically.",Title());
		CloseWindows();
		isquitting = false;
		if (changes == true) {
			BAlert *alert = new BAlert("save",text,"Cancel","Don't Save","Save",B_WIDTH_AS_USUAL,B_OFFSET_SPACING,B_WARNING_ALERT);
			alert->SetShortcut(0,B_ESCAPE);
			switch(alert->Go()) {
				case 0:
					return false;
					break;
				case 1:
					if (istempfile)
						BEntry(&fileref).Remove();
					break;
				case 2:
					openres->Sync();
					file->Sync();
					changes = false;
					if (istempfile) {
						MessageReceived(new BMessage('svas'));
						isquitting = true;
						return false;
					}
					break;
			}
		} else {
			delete openres;
			delete file;
		}
		openmwindows--;
		if (openmwindows <= 0) {
			BNode node("/boot");
			list_state rescol = collapsed;
			if (res->resources->IsExpanded())
				rescol = expanded;
			node.WriteAttr("rescol",'STAT',0,&rescol,sizeof(list_state));
			rescol = collapsed;
			if (res->attributes->IsExpanded())
				rescol = expanded;
			node.WriteAttr("attrcol",'STAT',0,&rescol,sizeof(list_state));
			be_app->PostMessage(B_QUIT_REQUESTED);
		}
		return true;
	}
	void MessageReceived(BMessage *message);
	restypeview *res;
	bool changes;
	bool istempfile;
	bool isquitting;
	BResources *openres;
	BFile *file;
	entry_ref fileref;
	BMenuBar *menus;
};

bool restypeview::InitiateDrag(BPoint point,int32 index, bool selected) {
	if (!selected)
		return false;
	if (ItemAt(index)->OutlineLevel() < 2)
		return false;
	DoubleItem *y = (DoubleItem *)(ItemAt(index));
	BMessage *todrag = new BMessage(B_SIMPLE_DATA);
	size_t size;
	type_code code = ((TypeItem *)(Superitem(y)))->type;
	unsigned char *data;
	if (y->idstring != NULL) {
		const void *dat = ((reswindow *)(Window()))->openres->LoadResource(code,y->id,&size);
		data = new unsigned char[size];
		memcpy(data,dat,size);
	} else {
		attr_info inf;
		((reswindow *)(Window()))->file->GetAttrInfo(y->name,&inf);
		size = inf.size;
		data = new unsigned char[size];
		((reswindow *)(Window()))->file->ReadAttr(y->name,code,0,data,size);
	}
	char m_type[B_MIME_TYPE_LENGTH];
	strcpy(m_type,MIME_from_code(code));
	todrag->AddString("be:types", B_FILE_MIME_TYPE);
	todrag->AddString("be:types", m_type);
	todrag->AddString("be:filetypes", m_type);
	todrag->AddInt32("be:actions", B_COPY_TARGET);
	todrag->AddInt32("be:actions", B_TRASH_TARGET);
	char filename[B_FILE_NAME_LENGTH];
	if (y->name[0] == 0)
		sprintf(filename,"Resource ID: %ld",y->id);
	else
		strcpy(filename,y->name);
	todrag->AddString("be:clip_name", filename);
	if (y->idstring != NULL)
		todrag->AddBool("isattr",false);
	else
		todrag->AddBool("isattr",true);
	todrag->AddData("type",B_UINT32_TYPE,&code,4);
	if (code == 'CSTR')
		todrag->AddData(m_type,'MIME',data,ssize_t(size));
	delete  [] data;
	todrag->AddString("name",y->name);
	todrag->AddPointer("this",this);
	if (y->idstring == NULL)
		todrag->AddInt32("id",0);
	else
		todrag->AddInt32("id",y->id);
	DragMessage(todrag,ItemFrame(IndexOf(y)));
	return true;
}

type_code code_from_MIME(char *MIME) {
	type_code type = 'RAWT';
	BMimeType mime(MIME);
	if (mime == BMimeType("text/plain"))
		type = 'CSTR';
	if (BMimeType("image").Contains(&mime))
		type = 'bits';
	if (BMimeType("video").Contains(&mime))
		type = 'MOOV';
	if (BMimeType("resource").Contains(&mime)) {
		TypeItem xyz('none');
		xyz.SetTypeCode(MIME + 9);
		type = xyz.type;
	}
	return type;
}

char *MIME_from_code(type_code code) {
	char *MIME;
	switch (code) {
		case 'CSTR':
			MIME = "text/plain";
			break;
		case 'bits':
			MIME = "image/x-portable-pixmap";
			break;
		case 'MOOV':
			MIME = "video";
			break;
		default:
			MIME = new char[B_MIME_TYPE_LENGTH];
			sprintf(MIME,"resource/%s",TypeItem(code).TypeCode());
			break;
	}
	return MIME;
}

void restypeview::MessageReceived(BMessage *msg) {
	if (msg->WasDropped())
		StrokeRect(Bounds(),B_SOLID_LOW);
	BOutlineListView::MessageReceived(msg);
	reswindow *x = (reswindow *)(Window());
	char *name;
	type_code type;
	void *dat;
	ssize_t size;
	int32 id = 0;
	BMessage *message = msg;
	BOutlineListView *old;
	unsigned char *data;
	if (msg->WasDropped() == false) {
		switch (msg->what) {
			case B_COPY_TARGET:
				{
				const BMessage *prev = msg->Previous();
				if (prev->HasInt32("id"))
					prev->FindInt32("id",&id);
				else
					id = 0;
				size_t size;
				type_code *temp;
				prev->FindData("type",(uint32)(B_UINT32_TYPE),(const void **)&temp,new ssize_t);
				type = *temp;
				entry_ref ref;
				msg->FindRef("directory",&ref);
				msg->FindString("name",(const char **)(&name));
				BDirectory dir(&ref);
				BFile file(&dir,name,B_READ_WRITE | B_CREATE_FILE);
				void *data;
				bool isattr;
				prev->FindBool("isattr",&isattr);
				if (!isattr) {
					const void *dat = ((reswindow *)(Window()))->openres->LoadResource(type,id,&size);
					data = new unsigned char[size];
					memcpy(data,dat,size);
				} else {
					char *name;
					prev->FindString("be:clip_name",(const char **)(&name));
					attr_info inf;
					((reswindow *)(Window()))->file->GetAttrInfo(name,&inf);
					size = inf.size;
					data = new unsigned char[size];
					((reswindow *)(Window()))->file->ReadAttr(name,type,0,data,size);
				}
				file.Write(data,size);
				delete [] data;
				BNodeInfo ni(&file);
				/*if (type == 'bits')
					ni.SetType("image/x-portable-pixmap");
				else */{
					char *mime;
					msg->FindString("be:filetypes",(const char **)(&mime));
					ni.SetType(mime);
				}
				return;
				}
				break;
			case B_TRASH_TARGET:
				DeleteSelection();
				break;
			case 'REQU':
				{
				const BMessage *prev = msg->Previous();
				if (prev->HasInt32("id"))
					prev->FindInt32("id",&id);
				else
					id = 0;
				size_t size;
				type_code *temp;
				prev->FindData("type",(uint32)(B_UINT32_TYPE),(const void **)&temp,new ssize_t);
				type = *temp;
				void *data;
				bool isattr;
				prev->FindBool("isattr",&isattr);
				if (!isattr) {
					const void *dat = ((reswindow *)(Window()))->openres->LoadResource(type,id,&size);
					data = new unsigned char[size];
					memcpy(data,dat,size);
				} else {
					char *name;
					prev->FindString("be:clip_name",(const char **)(&name));
					attr_info inf;
					((reswindow *)(Window()))->file->GetAttrInfo(name,&inf);
					size = inf.size;
					data = new unsigned char[size];
					((reswindow *)(Window()))->file->ReadAttr(name,type,0,data,size);
				}
				BMessage *msg1 = new BMessage('REQ1');
				msg1->AddData("data",type,data,size);
				msg->SendReply(msg1);
				}
				break;
			case B_MIME_DATA:
				goto app_reply;
				break;
		}
		return;
	}
	if (msg->HasPointer("this")) {
		message->FindPointer("this",(void **)(&old));
		if (old == this)
			return;
		BMessage *reply = new BMessage;
		message->SendReply('REQU',reply);
		message->FindString("name",(const char **)(&name));
		if (message->HasInt32("id"))
			message->FindInt32("id",&id);
		else
			id = 0;
		message = reply;
		message->GetInfo("data",&type);
		message->FindData("data",type,(const void **)&dat,&size);
		data = new unsigned char[size];
		memcpy(data,dat,size);
		if (data == NULL) {
			return;
		}
	} else {
		if (msg->GetInfo('MIME',0,&name,&type) != B_OK) {
			char adname[B_MIME_TYPE_LENGTH];
			entry_ref ref;
			if (msg->FindRef("refs",&ref) != B_OK) {
				BMessage *reply = new BMessage(B_COPY_TARGET);
				reply->AddString("be:types","image/x-bmp");
				msg->SendReply(reply,this);
				return;
			}
			name = new char[B_FILE_NAME_LENGTH];
			BEntry(&ref).GetName(name);
			BFile file(&ref, B_READ_ONLY);
			BNodeInfo(&file).GetType(adname); 
			if ((strcmp(adname,"application/x-be-resource") == 0)) {
				((reswindow *)(Window()))->CloseWindows();
				Window()->Lock();
				MakeEmpty();
				if (((reswindow *)(Window()))->openres->MergeFrom(&file) != B_OK) {
					beep();
					return;
				}
				FillRect(Bounds(),B_SOLID_LOW);
				Window()->Unlock();
				Init(((reswindow *)(Window()))->openres);
				((reswindow *)(Window()))->changes = true;
				return;
			} else {
				{
					BMessage *q = new BMessage;
					char *temp;
					type_code fool;
					if (message->GetInfo('MSGG',0,&temp,&fool) == B_OK) {
						message->FindMessage(temp,q);
						if (validate_instantiation(q,"BBitmap"))
							message = q;
					}
				}
				if (validate_instantiation(message,"BBitmap")) {
					BBitmap *temp = new BBitmap(message);
					BBitmapStream map(temp);
					size = map.Size();
					data = new unsigned char[size];
					map.Read(data,size);
					size_t tempSize = temp->BitsLength();
					memcpy((void *)(uint32(data) + (size - tempSize)),temp->Bits(),tempSize);
					map.DetachBitmap(&temp);
					delete temp;
					type = 'bits';
					name = "Clipping";
				} else {
					type = code_from_MIME(adname);
					if (type == 'bits') {
						BMallocIO io;
						BTranslatorRoster::Default()->Translate(&file,NULL,NULL,&io,'bits');
						size = io.BufferLength();
						data = new unsigned char[size];
						memcpy(data,io.Buffer(),size);
					} else {
						off_t temporar;
						file.GetSize(&temporar);
						size = temporar;
						data = new unsigned char[size];
						file.Read(data,size);
					}
				}
				id = 0;
			}
		} else {
	app_reply:
			unsigned char *data2;
			char *name2;
			msg->GetInfo(B_MIME_TYPE,0,&name2,&type);
			if (msg->FindData(name2,B_MIME_TYPE,0,(const void **)&data2,&size) != B_OK)
				return;
			type = code_from_MIME(name2);
			if (type == 'CSTR')
				data = new unsigned char[size + 1];
			else
				data = new unsigned char[size];
			memcpy(data,data2,size);
			if (type == 'CSTR') {
				data[size] = 0;
				size++;
			}
			if (msg->FindString("be:clip_name",(const char **)(&name2)) == B_OK) {
				name = new char[strlen(name2) + 1];
				strcpy(name,name2);
			} else
				name = new char(0);
		}
	}
	for(;x->openres->HasResource(type,id);id++) {}
	AddResource(type,id,name,size_t(size),data,false,false);
	delete [] data;
	return;
}

DoubleItem *restypeview::AddResource(type_code type,int32 id,char *name,size_t length,void *data,bool isattr,bool invoke) {
	if (isattr)
		((reswindow *)(Window()))->file->WriteAttr(name,type,0,data,length);
	else
		((reswindow *)(Window()))->openres->AddResource(type,id,data,length,name);
	DoubleItem *item = new DoubleItem(id,name,this,isattr);
	Window()->Lock();
	AddUnder(item,FindType(type,isattr));
	Sort();
	if (invoke)
		item->Invoke();
	((reswindow *)(Window()))->changes = true;
	Invalidate(Bounds());
	if (Window()->IsLocked())
		Window()->Unlock();
	return item;
}
		
int32 copy(void *y) {
	reswindow *x = (reswindow *)(y);
	char *name;
	type_code type;
	unsigned char *data;
	size_t size;
	int32 id;
	TypeItem *cur;
	DoubleItem *cura;
	if (x->res->CurrentSelection() < 0)
		return 0;
	if (x->res->ItemAt(x->res->CurrentSelection())->OutlineLevel() < 2)
		return 0;
	cura = ((DoubleItem *)(x->res->ItemAt(x->res->CurrentSelection())));
	cur = (TypeItem *)x->res->Superitem(cura);
	type = cur->type;
	id = cura->id;
	if (cura->idstring != NULL)
		data = (unsigned char *)x->openres->LoadResource(type,id,&size);
	else {
		attr_info info;
		x->file->GetAttrInfo(cura->name,&info);
		data = new unsigned char[info.size];
		size = info.size;
		x->file->ReadAttr(cura->name,type,0,data,info.size);
	}	
	be_clipboard->Lock();
	be_clipboard->Clear();
	name = cura->name;
	if (name == NULL) {
		name = new char;
		*name = 0;
	}
	if (type == 'CSTR') {
		type = 'MIME';
		name = "text/plain";
	}
	bool isattr;
	if (cura->idstring == NULL)
		isattr = true;
	else
		isattr = false;
	be_clipboard->Data()->AddData(name,type,data,size,isattr);
	be_clipboard->Commit();
	be_clipboard->Unlock();
	delete [] data;
	return 0;
}
	
int32 getdata(void *y) {
		reswindow *x = (reswindow *)(y);
		char *name;
		type_code type;
		void *data;
		ssize_t size;
		int32 id;
		bool construct = false;
		be_clipboard->Lock();
		BMessage *message = new BMessage(*(be_clipboard->Data()));
		message->PrintToStream();
		be_clipboard->Unlock();
		if (message == NULL)
			 return 0;
		{
			BMessage *q = new BMessage;
			char *temp;
			type_code fool;
			if (message->GetInfo('MSGG',0,&temp,&fool) == B_OK) {
				message->FindMessage(temp,q);
				if (validate_instantiation(q,"BBitmap"))
					message = q;
			}
		}
		if (message->GetInfo(B_ANY_TYPE,0,&name,&type) == B_BAD_TYPE)
			 return 0;
		message->FindData(name,type,(const void **)&data,&size);
		if (data == NULL)
			 return 0;
		if (message->HasString("class")) {
			if (validate_instantiation(message,"BBitmap")) {
				BBitmap *temp = new BBitmap(message);
				BBitmapStream map(temp);
				size = map.Size();
				data = new unsigned char[size];
				map.Read(data,size);
				size_t tempSize = temp->BitsLength();
				memcpy((void *)(uint32(data) + (size - tempSize)),temp->Bits(),tempSize);
				map.DetachBitmap(&temp);
				delete temp;
				construct = true;
				type = 'bits';
				name = "Clipping";
			} else
				goto NORMAL;
		} else {
			NORMAL:
			if (type == 'MIME') {
				type = code_from_MIME(name);
			}
			if (type == 'bits') {
				BMallocIO out;
				BMemoryIO in(data,size);
				BTranslatorRoster::Default()->Translate(&in,NULL,NULL,&out,'bits');
				size = out.BufferLength();
				data = new unsigned char[size];
				memcpy(data,out.Buffer(),size);
				construct = true;
			}
		}
		for(id = 0;x->openres->HasResource(type,id);id++) {}
		x->res->AddResource(type,id,name,size_t(size),data,false,false);
		if (construct)
			delete [] data;
		return 0;
	}
	
int32 copytoattr(void *y) {
		reswindow *x = (reswindow *)(y);
		type_code type;
		int32 id;
		TypeItem *cur;
		DoubleItem *cura;
		if (x->res->CurrentSelection() < 0)
			return 0;
		if (x->res->ItemAt(x->res->CurrentSelection())->OutlineLevel() < 2)
			return 0;
		cura = ((DoubleItem *)(x->res->ItemAt(x->res->CurrentSelection())));
		cur = (TypeItem *)x->res->Superitem(cura);
		type = cur->type;
		id = cura->id;
		if (cura->idstring != NULL) {
				unsigned char *data;
				size_t size;
				char *name;
				data = (unsigned char *)x->openres->LoadResource(type,id,&size);
				attr_info inf;
				if (cura->name == NULL) {
					name = new char[255];
					sprintf(name,"Resource ID: %ld",id);
				} else {
					if (cura->name[0] == 0) {
						name = new char[255];
						sprintf(name,"Resource ID: %ld",id);
					} else {
						name = new char[strlen(cura->name) + 1];
						strcpy(name,cura->name);
					}
				}
				if (x->file->GetAttrInfo(name,&inf) != B_ENTRY_NOT_FOUND) {
					BAlert *a = new BAlert("attrexists","An attribute of the same name or ID already exists. Would you like to overwrite it?","Cancel","OK",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
					if (a->Go() == 0) {
						return 0;
					} else {
						TypeItem *high = x->res->FindType(type,true);
						DoubleItem *ite = NULL;
						DoubleItem *temp;
						for (int32 i = x->res->CountItemsUnder(high,true) - 1;i >= 0;i--) {
							temp = (DoubleItem *)x->res->ItemUnderAt(high,true,i);
							if (strcmp(temp->name,name) == 0) {
								ite = temp;
								x->Lock();
								x->res->RemoveItem(x->res->IndexOf(temp));
								x->Unlock();
							}
					}
				}
			}
			x->res->AddResource(type,0,name,size,data,true,false);
			delete [] name;
			delete [] data;
		} else {
			unsigned char *data;
			char *name = new char[strlen(cura->name) + 1];
			if (cura->name[0] == 0) {
				delete [] name;
				name = NULL;
			} else {
				strcpy(name,cura->name);
			}
			size_t length;
			attr_info inf;
			x->file->GetAttrInfo(cura->name,&inf);
			data = new unsigned char[inf.size];
			length = inf.size;
			x->file->ReadAttr(cura->name,type,0,data,inf.size);
			int32 id;
			if (x->openres->HasResource(type,name)) {
				BAlert *a = new BAlert("resexists","A resource of the same name already exists. Would you like to assign the new data a unique ID or overwrite the existing data?","Overwrite Existing","Unique ID",NULL,B_WIDTH_FROM_LABEL,B_STOP_ALERT);
				if (a->Go() == 0) {
					x->openres->GetResourceInfo(type,name,&id,&length); 
					{
						TypeItem *high = x->res->FindType(type,false);
						DoubleItem *ite = NULL;
						DoubleItem *temp;
						for (int32 i = x->res->CountItemsUnder(high,true) - 1;i >= 0;i--) {
							temp = (DoubleItem *)x->res->ItemUnderAt(high,true,i);
							if (id == temp->id) {
								ite = temp;
								x->Lock();
								x->res->RemoveItem(x->res->IndexOf(temp));
								x->Unlock();
							}
						}
					}
					x->res->AddResource(type,id,name,length,data,false,false);
					return 0;
				}
			}
			for(id = 0;x->openres->HasResource(type,id);id++) {}
			x->res->AddResource(type,id,name,length,data,false,false);
		}
		return 0;
}

int sortmenu(const void *first,const void *second) {
	if (alphabet((*((BMenuItem **)(first)))->Label(),(*((BMenuItem **)(second)))->Label()))
		return -1;
	else
		return 1;
	return 0;
}

class optwindow : public BWindow {
	public:
		optwindow(bool add,DoubleItem* to,type_code typex,reswindow *x) : BWindow(BRect(find_center(400,140).left,find_center(200,140).top - 150,find_center(400,140).right,find_center(200,140).bottom - 150),"Change Resource Properties",B_FLOATING_WINDOW_LOOK,B_MODAL_APP_WINDOW_FEEL,B_NOT_RESIZABLE | B_NOT_ZOOMABLE/* | B_ASYNCHRONOUS_CONTROLS*/) {
				AddShortcut('W',0,new BMessage(B_QUIT_REQUESTED),this);
				AddShortcut('.',0,new BMessage(B_QUIT_REQUESTED),this);
				win = x;
				gray = new BView(Bounds(),"graybkgrd",B_FOLLOW_ALL_SIDES,B_WILL_DRAW | B_FRAME_EVENTS);
				gray->SetViewColor(216,216,216);
				if (add) {
					SetTitle("New Resource Properties");
					BPopUpMenu *attrmenu = new BPopUpMenu("Resource");
					attrmenu->AddItem(new BMenuItem("Resource",new BMessage('resr')));
					attrmenu->AddItem(new BMenuItem("Attribute",new BMessage('attr')));
					BMenuField *attrchoice = new BMenuField(BRect(10,9,390,29),"attrchoice","Data Format: ",attrmenu);
					attrchoice->SetDivider(be_plain_font->StringWidth("Data Format: "));
					gray->AddChild(attrchoice);
					type = new BTextControl(BRect(10,34,be_plain_font->StringWidth("AAAA")+be_plain_font->StringWidth("Data Type: ")+20,54),"Type","Data Type: ","",new BMessage('tyc2'));
					type->SetDivider(be_plain_font->StringWidth("Data Type: "));
					type->TextView()->SetMaxBytes(4);
					if (itemLista == NULL) {
						BPopUpMenu *typemenu = new BPopUpMenu("Available Data Types",false,false);
						init_typemenu(typemenu);
						BMenuField *typechoice = new BMenuField(BRect(be_plain_font->StringWidth("AAAA")+be_plain_font->StringWidth("Data Type: ")+40,34,390,54),"typechoice","",typemenu);
						typechoice->SetDivider(0);
						gray->AddChild(typechoice);
						itemLista = new BMessage;
						typechoice->Archive(itemLista,true);
					} else {
						BMenuField *typechoice = new BMenuField(itemLista);
						gray->AddChild(typechoice);
					}
					gray->AddChild(type);
					id = new BTextControl(BRect(10,56,390,76),"ID","ID: ","",NULL);
					id->SetDivider(be_plain_font->StringWidth("ID: "));
					gray->AddChild(id);
					name = new BTextControl(BRect(10,78,390,98),"Name","Name: ",NULL,NULL);
					name->SetDivider(be_plain_font->StringWidth("Name: "));
					gray->AddChild(name);
					willberes = true;
				} else {
					char ida[255];
					sela = x->res->CurrentSelection();
					if (to->idstring != NULL) {
						sprintf(ida,"%ld",to->id);
						sel = sela; //check
					} else {
						ida[0] = 0;
						sel = sela; //check
					}
					BPopUpMenu *attrmenu = new BPopUpMenu("Resource");
					attrmenu->AddItem(new BMenuItem("Resource",new BMessage('resr')));
					BMenuItem *attritem = new BMenuItem("Attribute",new BMessage('attr'));
					attrmenu->AddItem(attritem);
					if (to->idstring == NULL) {
						attritem->SetMarked(true);
					}
					BMenuField *attrchoice = new BMenuField(BRect(10,9,190,29),"attrchoice","Data Format: ",attrmenu);
					attrchoice->SetDivider(be_plain_font->StringWidth("Data Format: "));
					gray->AddChild(attrchoice);
					size_t len;
					char *bogus;
					attr_info inf;
					if (to->idstring != NULL)
						x->openres->GetResourceInfo(typex,to->id,(const char **)(&bogus),&len);
					else {
						x->file->GetAttrInfo(to->name,&inf);
						len = inf.size;
					}
					gray->AddChild(new BStringView(BRect(210,9,390,29),"Size",parse_size(len),B_FOLLOW_RIGHT | B_FOLLOW_TOP));
					id = new BTextControl(BRect(10,43,390,63),"ID","ID: ",ida,NULL);
					id->SetDivider(be_plain_font->StringWidth("ID: "));
					if (to->idstring == NULL) {
						id->SetEnabled(false);
					}
					gray->AddChild(id);
					name = new BTextControl(BRect(10,77,390,97),"Name","Name: ",to->name,NULL);
					name->SetDivider(be_plain_font->StringWidth("Name: "));
					gray->AddChild(name);
					if (to->idstring == NULL)
						willberes = false;
					else
						willberes = true;
				}
				whattodo = add;
				toop = to;
				typeb = typex;
				gray->AddChild(new BButton(BRect(240,110,300,130),"Cancel","Cancel",new BMessage(B_QUIT_REQUESTED)));
				BButton *OK = new BButton(BRect(320,110,380,130),"OK","OK",new BMessage('bxok'));
				gray->AddChild(OK);
				OK->MakeDefault(true);
				AddChild(gray);
				Show();
			}
			void init_typemenu(BPopUpMenu *menu) {
				BMessage *message;
				char name[B_FILE_NAME_LENGTH];
				//char *descriptstring;
				//---Get desc string
				app_info info;
				be_app->GetAppInfo(&info);
				BEntry entry(&(info.ref));
				BDirectory dir;
				entry.GetParent(&dir);
				entry.SetTo(&dir,"editors");
				dir.SetTo(&entry);
				BPath path;
				BList *itemList = new BList;
				dir.Rewind();
				if (dir.GetNextEntry(&entry,true) == B_ENTRY_NOT_FOUND) {
					menu->SetEnabled(false);
					return;
				}
				dir.Rewind();
				for (;dir.GetNextEntry(&entry,true) != B_ENTRY_NOT_FOUND;) {
					entry.GetPath(&path);
					if (entry.Exists() == false)
						break;
					image_id editor = load_add_on(path.Path());
					char *temp;
					if (get_image_symbol(editor,"description",B_SYMBOL_TYPE_DATA,(void **)(&temp)) != B_NO_ERROR)
						continue;
					//descriptstring = new char[strlen(temp) + 1];
					//strcpy(descriptstring,temp);
					entry.GetName(name);
					message = new BMessage('tycd');
					message->AddString("typecode",name);
					itemList->AddItem(new BMenuItem(temp,message));
					unload_add_on(editor);
				}
				itemList->SortItems(&sortmenu);
				for (int32 i = itemList->CountItems() - 1;i >= 0;i--) {
					menu->AddItem((BMenuItem *)(itemList->ItemAt(i)));
				}
			}
			char *parse_size(size_t sizer) {
				float size = sizer;
				static char textsize[255];
				char temp[20];
				char *suffix;
				if (size >= 1024*1024) {
					size /= 1024*1024;
					suffix = "MB";
				} else {
					if (size >= 1024) {
						size /= 1024;
						suffix = "KB";
					} else {
						suffix = "B";
					}
				}
				sprintf(temp,"%.2f",size);
				int i;
				for (i = 0;(temp[i] != '.') && (temp[i] != 0);i++) {}
				for (;(temp[i] != '0') && (temp[i] != 0);i++) {}
				temp[i] = 0;
				if (temp[i-1] == '.')
					temp[i-1] = 0;
				sprintf(textsize,"Resource Size: %s %s",temp,suffix);
				return textsize;
			}
			void add(void) {
				Lock();
				int32 test = 1;
				char temp[15];
				for (int i = 0; i < 4;i++) {
					temp[i] = ' ';
				}
				sscanf(id->Text(),"%ld",&test);
				strcpy(temp,type->Text());
				for (int i = strlen(temp); i < 4;i++) {
					temp[i] = ' ';
				}
				temp[4] = 0;
				type_code typea;
				strncpy((char *)(&typea),temp,4);
				typea = flipcode(typea);
				char *namea = new char[strlen(name->Text()) + 1];
				strcpy(namea,name->Text());
				if (namea[0] == 0) {
					delete [] namea;
					namea = new char[8];
					strcpy(namea,"Unnamed");
				}
				if (willberes) {
					if (win->openres->HasResource(typea,test) == true) {
						BAlert *alert = new BAlert("alert","This resource already exists. Would you like to overwrite the existing one?","Cancel","OK",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						if (alert->Go() == 0) {
							return;
						}
					}
				} else {
					attr_info inf;
					if (win->file->GetAttrInfo(namea,&inf) != B_ENTRY_NOT_FOUND) {
						BAlert *alert = new BAlert("alert","An attribute with this name already exists. Would you like to overwrite the existing one?","Cancel","OK",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						if (alert->Go() == 0) {
							return;
						}
					}
				}
				DoubleItem *item = win->res->AddResource(typea,test,namea,0,NULL,!(willberes),true);
				win->Lock();
				win->res->Expand(win->res->Superitem(item));
				win->res->Expand(win->res->Superitem(win->res->Superitem(item)));
				win->res->Select(win->res->IndexOf(item));
				win->Unlock();
				Unlock();
				delete [] namea;
				PostMessage(B_QUIT_REQUESTED);
				//name->SetText(NULL);
			}
			void changeinfo(void) {
				int32 test = 1;
				sscanf(id->Text(),"%ld",&test);
				char *namea = new char[strlen(name->Text()) + 1];
				strcpy(namea,name->Text());
				if (*namea == 0)
					namea = NULL;
				if (willberes) {
					if ((win->openres->HasResource(typeb,test) == true) && (test != toop->id)) {
						BAlert *alert = new BAlert("alert","This resource already exists. Would you like to overwrite the existing one?","Cancel","OK",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						if (alert->Go() == 0) {
							return;
						}
					}
				} else {
					if (namea == NULL) {
						(new BAlert("alert","Attributes must have a name.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT))->Go();
						return;
					} 
					attr_info inf;
					if ((win->file->GetAttrInfo(namea,&inf) != B_ENTRY_NOT_FOUND) && (strcmp(namea,toop->name))) {
						BAlert *alert = new BAlert("alert","An attribute with this name already exists. Would you like to overwrite the existing one?","Cancel","OK",NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
						if (alert->Go() == 0) {
							return;
						}
					}
				}
				size_t length;
				unsigned char *data;
				bool isattr;
				if (toop->idstring == NULL)
					isattr = true;
				else
					isattr = false;
				if (isattr) {
					attr_info info;
					win->file->GetAttrInfo(toop->name,&info);
					length = info.size;
					data = new unsigned char[length];
					win->file->ReadAttr(toop->name,typeb,0,data,length);
				} else {
					const void *dat = win->openres->LoadResource(typeb,toop->id,&length);
					data = new unsigned char[length];
					memcpy(data,dat,length);
				}
				if (isattr)
					win->file->RemoveAttr(toop->name);
				else
					win->openres->RemoveResource(typeb,toop->id);
				if (willberes)
					win->openres->AddResource(typeb,test,data,length,namea);
				else
					win->file->WriteAttr(namea,typeb,0,data,length);
				toop->id = test;
				if (namea == NULL)
					toop->name = new char(0);
				else {
					toop->name = new char[strlen(namea) + 1];
					strcpy(toop->name,namea);
				}
				win->Lock();
				if ((willberes) && (toop->idstring == NULL)) {
					if (win->res->CountItemsUnder(win->res->Superitem(toop),true) == 1) {
						BListItem *super = win->res->Superitem(toop);
						if (win->res->CountItemsUnder(win->res->attributes,true) == 1) {
							win->res->attributes->SetText("No Attributes");
							win->res->attributes->SetEnabled(false);
						}
						win->res->RemoveItem(toop);
						win->res->RemoveItem(super);
					} else
						win->res->RemoveItem(toop);
					win->res->AddUnder(toop,win->res->FindType(typeb,false));
				}
				if ((!willberes) && (toop->idstring != NULL)) {
					if (win->res->CountItemsUnder(win->res->Superitem(toop),true) == 1) {
						BListItem *super = win->res->Superitem(toop);
						if (win->res->CountItemsUnder(win->res->resources,true) == 1) {
							win->res->resources->SetText("No Resources");
							win->res->resources->SetEnabled(false);
						}
						win->res->RemoveItem(toop);
						win->res->RemoveItem(super);
					} else
						win->res->RemoveItem(toop);
					win->res->AddUnder(toop,win->res->FindType(typeb,true));
				}
				if (willberes) {
					toop->idstring = new char[20];
					sprintf(toop->idstring,"%ld",toop->id);
				} else
					toop->idstring = NULL;
				win->res->Invalidate(win->res->ItemFrame(win->res->IndexOf(toop)));
				win->res->Sort();
				if (win->IsLocked())
					win->Unlock();
				win->changes = true;
				PostMessage(B_QUIT_REQUESTED);
				}
			void MessageReceived(BMessage *message) {
				char *string;
				switch (message->what) {
					case 'bxok':
						if (whattodo == true) {
							add();
						} else {
							changeinfo();
						}
						win->changes = true;
						break;
					case 'attr':
						willberes = false;
						//Lock();
						id->SetEnabled(false);
						//Unlock();
						break;
					case 'tycd':
						message->FindString("typecode",(const char **)(&string));
						type->SetText(string);
						break;
					case 'resr':
						willberes = true;
						//Lock();
						id->SetEnabled(true);
						//Unlock();
						break;
					default:
						BWindow::MessageReceived(message);
					}
				}
			private:
				BView *gray;
				BTextControl *type;
				BTextControl *id;
				BTextControl *name;
				bool whattodo;
				bool willberes;
				int32 sel;
				int32 sela;
				DoubleItem* toop;
				type_code typeb;
				reswindow *win;
};

class pluginwindow : public BWindow {
	public:
		pluginwindow(pluginwindow **reg,plug_in plugin1,char *name,int32 resid,type_code type,char *xname,bool isattr) : BWindow(BRect(400,300,700,600),name,B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE/* | B_ASYNCHRONOUS_CONTROLS*/) {
			registration = reg;
			*registration = this;
			plugin = plugin1;
			id = resid;
			typea = type;
			yname = new char[strlen(xname) + 1];
			strcpy(yname,xname);
			attr = isattr;
		}
		void RemoveChildren(void) {
			BView *view;
			for (int32 x = 0;;) {
				view = gray->ChildAt(x);
				if (view == NULL) {
					break;
				}
				view->RemoveSelf();
				delete view;
			}
			gray->RemoveSelf();
			delete gray;
		}
		bool QuitRequested(void) {
			be_app->SetCursor(B_HAND_CURSOR);
			size_t give;
			unsigned char *data;
			Hide();
			*registration = NULL;
			bool changes = true;
			data = (*(plugin.savedata))(&give,gray);
			if (data == NULL) {
				BAlert *alert = new BAlert("whoops","The resource could not be written because the editor passed a NULL pointer (i.e. it didn't write any data).","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
				alert->Go();
				RemoveChildren();
				unload_add_on(plugin.plugin);
				delete [] plugin.olddata;
				return true;
			}
			if (give == plugin.oldlength) {
				//(new BAlert("h","Que es?","OK"))->Go();
				//(new BAlert("h","Qu'est-ce que c'est?","OK"))->Go();
				//(new BAlert("h","What is it?","OK"))->Go();
				if (memcmp(data,plugin.olddata,give) == 0) {
					changes = false;
				}
			}
			if (attr == false) {
				plugin.win->openres->RemoveResource(typea,id);
				if (plugin.win->openres->AddResource(typea,id,data,give,yname) != B_NO_ERROR) {
					BAlert *alert = new BAlert("whoops","The resource could not be written.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
					alert->Go();
					RemoveChildren();
					unload_add_on(plugin.plugin);
					delete [] data;
					delete [] plugin.olddata;
					return true;
				}
			} else {
				if (plugin.win->file->WriteAttr(yname,typea,0,data,give) <= 0) {
					BAlert *alert = new BAlert("whoops","The attribute could not be written.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_STOP_ALERT);
					alert->Go();
					RemoveChildren();
					unload_add_on(plugin.plugin);
					delete [] data;
					delete [] plugin.olddata;
					return true;
				}
			}
			if ((attr == false) && changes)
				plugin.win->changes = true;
			RemoveChildren();
			unload_add_on(plugin.plugin);
			delete [] data;
			delete [] plugin.olddata;
			delete [] yname;
			return true;
			}
		void MessageReceived(BMessage *message) {
			int32 y = message->what;
			if ((y < 0) || (y == B_REFS_RECEIVED)) {
				(*(plugin.messaging))(message,gray);
			} else {
				BWindow::MessageReceived(message);
			}
		}
		BView *gray;
		plug_in plugin;
	private:
		int32 id;
		type_code typea;
		char *yname;
		pluginwindow **registration;
		bool attr;
		BView *oldfocus;
};

void reswindow::CloseWindows(void) {
		TypeItem *a;
		DoubleItem *b;
		int32 total = res->CountItemsUnder(res->resources,true);
		for (int32 i = 0;i < total;i++) {
			a = (TypeItem *)res->ItemUnderAt(res->resources,true,i);
			if (a == NULL)
				break;
			for (int32 y = 0;true;y++) {
				b = (DoubleItem *)res->ItemUnderAt(a,true,y);
				if (b == NULL)
					break;
				if (b->win != NULL) {
					b->win->PostMessage(B_QUIT_REQUESTED);
				}
			}
		}
		total = res->CountItemsUnder(res->attributes,true);
		for (int32 i = 0;i < total;i++) {
			a = (TypeItem *)res->ItemUnderAt(res->attributes,true,i);
			if (a == NULL)
				break;
			for (int32 y = 0;true;y++) {
				b = (DoubleItem *)res->ItemUnderAt(a,true,y);
				if (b == NULL)
					break;
				if (b->win != NULL) {
					b->win->PostMessage(B_QUIT_REQUESTED);
				}
			}
		}
	}
	
void restypeview::KeyDown(const char *bytes, int32 numBytes) {
	if (numBytes == 1) {
		if ((*bytes == B_BACKSPACE) || (*bytes == B_DELETE)) {
			if (CurrentSelection() < 0) {
				BOutlineListView::KeyDown(bytes,numBytes);
				return;
			}
			if (ItemAt(CurrentSelection())->OutlineLevel() < 2) {
				BOutlineListView::KeyDown(bytes,numBytes);
				return;
			}
			DeleteSelection();
		}
	}
	BOutlineListView::KeyDown(bytes,numBytes);
}

void restypeview::DeleteSelection(void) {
	DoubleItem *item = (DoubleItem *)(ItemAt(CurrentSelection()));
	TypeItem *type = (TypeItem *)(Superitem(item));
	if (item->idstring != NULL)
		((reswindow *)(Window()))->openres->RemoveResource(type->type,item->id);
	else
		((reswindow *)(Window()))->file->RemoveAttr(item->name);
	if (item->win != NULL) {
		item->win->PostMessage(B_QUIT_REQUESTED);
		item->win = NULL;
	}
	if (CountItemsUnder(type,true) == 1)
		RemoveItem(type);
	else
		RemoveItem(item);
	if (CountItemsUnder(resources,true) == 0) {
		resources->SetText("No Resources");
		resources->SetEnabled(false);
	}
	if (CountItemsUnder(attributes,true) == 0) {
		attributes->SetText("No Attributes");
		attributes->SetEnabled(false);
	}
	((reswindow *)(Window()))->changes = true;
}

void reswindow::MessageReceived(BMessage *message) {
	thread_id thread;
	BMessage *msg;
	switch (message->what) {
		case B_ABOUT_REQUESTED:
			#if __POWERPC__
				(new BAlert("About...","Resourcer 3.0 for PowerPC\n"B_UTF8_COPYRIGHT"1999-2000 Nathan Whitehorn","OK"))->Go();
			#endif
			#if __INTEL__
				(new BAlert("About...","Resourcer 3.0 for Intel\n"B_UTF8_COPYRIGHT"1999-2000 Nathan Whitehorn","OK"))->Go();
			#endif
			break;
		case 'newf':
			((BRApplication *)(be_app))->saveas = false;
			alredopen = false;
			((BRApplication *)(be_app))->ReadyToRun();
			break;
		case 'open':
			openpanel->Show();
			break;
		case 'save':
			openres->Sync();
			file->Sync();
			changes = false;
			if (!istempfile)
				break;
		case 'rsiv':
			if (res->CurrentSelection() < 0)
				break;
			if (res->ItemAt(res->CurrentSelection())->OutlineLevel() < 2) {
				if (res->ItemAt(res->CurrentSelection())->IsExpanded())
					res->Collapse(res->ItemAt(res->CurrentSelection()));
				else
					res->Expand(res->ItemAt(res->CurrentSelection()));
				break;
			}
			((DoubleItem *)(res->ItemAt(res->CurrentSelection())))->Invoke(false); 
			break;
		case 'svas':
			((BRApplication *)(be_app))->saveas = true;
			if (newpanel == NULL)
				newpanel = new BFilePanel(B_SAVE_PANEL,NULL,NULL,B_FILE_NODE,false,NULL,NULL,false,true);
			msg = new BMessage(B_SAVE_REQUESTED);
			msg->AddPointer("reswindow",this);
			newpanel->SetMessage(msg);
			newpanel->Show();
			break;
		case 'mime':
			{
				TypeItem *high = res->FindType('MIMS',true);
				DoubleItem *ite = NULL;
				DoubleItem *temp;
				for (int32 i = res->CountItemsUnder(high,true) - 1;i >= 0;i--) {
					temp = (DoubleItem *)res->ItemUnderAt(high,true,i);
					if (strcmp(temp->name,"BEOS:TYPE") == 0) {
						ite = temp;
						temp->Invoke();
					}
				}
				if (ite == NULL)
					ite = res->AddResource('MIMS',0,"BEOS:TYPE",0,NULL,true);
			}
			break;			
		case -100:
			new optwindow(true,NULL,'blah',this);
			break;
		case -200:
			res->DeleteSelection();
			break;
		case -300:
			{
				DoubleItem *cura;
				TypeItem *cur;
				if (res->CurrentSelection() < 0)
					return;
				if (res->ItemAt(res->CurrentSelection())->OutlineLevel() < 2)
					return;
				cura = (DoubleItem *)(res->ItemAt(res->CurrentSelection()));
				cur = (TypeItem *)(res->Superitem(cura));
				if (cura->win != NULL) {
					alert = new BAlert("alert","This resource is open, and so its properties cannot be changed.","OK",NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
					alert->Go();
					return;
				}
				new optwindow(false,cura,cur->type,this);
			}
			break;
		case -400:
			copy(this);
			Lock();
			res->DeleteSelection();
			if (IsLocked())
				Unlock();
			break;
		case -500:
			thread = spawn_thread(&copy,"copying",10,this);
			if (thread < B_OK)
				copy(this);
			else {
				if (resume_thread(thread) != B_OK) {
					copy(this);
				}
			}
			break;
		case B_PASTE:
			thread = spawn_thread(&getdata,"pasting",10,this);
			if (thread < B_OK)
				getdata(this);
			else {
				if (resume_thread(thread) != B_OK) {
					getdata(this);
				}
			}
			break;
		case -800:
			if (res->ItemAt(res->CurrentSelection())->OutlineLevel() == 2) 
				((DoubleItem *)(res->ItemAt(res->CurrentSelection())))->Invoke(true);
			break;
		case -900:
			thread = spawn_thread(&copytoattr,"copying",10,this);
			if (thread < B_OK)
				copytoattr(this);
			else {
				if (resume_thread(thread) != B_OK) {
					copytoattr(this);
				}
			}
			break;
		case 'rvrt':
			CloseWindows();
			res->MakeEmpty();
			openres = new BResources(file);
			res->FillRect(res->Bounds(),B_SOLID_LOW);
			res->Init(openres);
			break;
		default:
			BWindow::MessageReceived(message);
		}
	}