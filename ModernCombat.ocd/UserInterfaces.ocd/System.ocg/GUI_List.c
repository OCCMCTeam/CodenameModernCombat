/**
	List menu prototype
	
	@author Zapper
 */

static const GUI_List = new GUI_Element
{
	entries = nil, 
	on_mouse_over_callback = nil, 
	on_mouse_out_callback = nil,
	on_close_callback = nil,
	permanent = false,

	Assemble = func ()
	{
		entries = [];
		this.Style = GUI_VerticalLayout;
		
		this.OnClose = GuiAction_Call(this, GetFunctionName(this.OnCloseCallback));
		return this;
	},
	
	SetPermanent = func (bool perm)
	{
		permanent = perm ?? true;
		return this;
	},

	OnCloseCallback = func ()
	{
		var self = this;
		if (on_close_callback && on_close_callback[0])
		{
			on_close_callback[0]->Call(on_close_callback[1], on_close_callback[2]);
		}
	},

	SetCloseCallback = func (proplist target, callback, parameter)
	{
		on_close_callback = [target, callback, parameter];
		return this;
	},

	SetMouseOverCallback = func (proplist target, callback)
	{
		on_mouse_over_callback = [target, callback];
		return this;
	},

	SetMouseOutCallback = func (proplist target, callback)
	{
		on_mouse_out_callback = [target, callback];
		return this;
	},

	// can be overloaded for custom menu styles
	MakeEntryProplist = func(symbol, text)
	{
		var custom_entry = {Bottom = "+2em", sym = {Right = "+2em", Bottom = "+2em"}, desc = {Left = "+2em"}};
		custom_entry.sym.Symbol = symbol;
		custom_entry.desc.Text = text;
		custom_entry.desc.Style = GUI_TextVCenter;
		custom_entry.Style = GUI_FitChildren;
		custom_entry.BackgroundColor = {Std = 0, OnHover = 0x50ff0000};
		return custom_entry;
	},

	// custom_menu_id should be passed if the menu was manually opened and not via Open()
	AddItem = func(symbol, string text, user_ID, proplist target, command, parameter, custom_entry, custom_menu_id, bool onlyUpdate)
	{
		onlyUpdate = onlyUpdate ?? false;
		custom_menu_id = custom_menu_id ?? GetRootID();
		
		var on_hover = GuiAction_SetTag("OnHover", 0, nil);
		if (on_mouse_over_callback)
		{
			on_hover = [on_hover, GuiAction_Call(this, "DoCallback", on_mouse_over_callback)];
		}
		var on_hover_stop = GuiAction_SetTag("Std", 0, nil);
		if (on_mouse_out_callback)
		{
			on_hover_stop = [on_hover_stop, GuiAction_Call(this, "DoCallback", on_mouse_out_callback)];
		}
		
		// in case of a new entry, append to array
		var ID = GetLength(entries) + 1;
		// otherwise, replace the old entry
		if (onlyUpdate)
		{
			for (var i = 0; i < GetLength(entries); ++i)
			{
				if (!entries[i]) continue;
				if (entries[i][3] != user_ID) continue;
				ID = i + 1;
				break;
			}
		}
		
		if (!custom_entry)
		{
			custom_entry = this->MakeEntryProplist(symbol, text);
		}
		
		// Always add some properties later. This is done so that real custom entries do not need to care about target etc.
		custom_entry.ID = ID; // A fixed ID is obligatory for now. Might be possible to omit that, but would need to check if updating etc works.
		custom_entry.Target = this; // Same as above.
		
		// These properties can in theory be set/customized by the user without breaking functionality. But they are (probably) required anway.
		custom_entry.Priority = custom_entry.Priority ?? ID;
		custom_entry.OnClick = custom_entry.OnClick ?? GuiAction_Call(this, "OnClick");
		custom_entry.OnMouseIn = custom_entry.OnMouseIn ?? on_hover;
		custom_entry.OnMouseOut = custom_entry.OnMouseOut ?? on_hover_stop;
		
		// Save entry to list and prepare call information.
		entries[ID - 1] = [target, command, parameter, user_ID];
		this[Format("_menuChild%d", ID)] = custom_entry;
		
		// need to add to existing menu?
		if (custom_menu_id)
		{
			if (onlyUpdate)
			{
				// need to close the old entry first
				// this is done so a full refresh is guaranteed
				GuiClose(custom_menu_id, ID, this);
			}
			
			var temp = {_child = custom_entry};
			GuiUpdate(temp, custom_menu_id, this.ID, this);
		}
		
		return custom_entry;
	},

	// updates an existing entry with the given user_ID
	UpdateItem = func(symbol, string text, user_ID, proplist target, command, parameter, custom_entry, custom_menu_id)
	{
		return AddItem(symbol, text, user_ID, target, command, parameter, custom_entry, custom_menu_id, true);
	},

	// can be used when the menu has already been opened
	// needs to be passed the menu ID if the menu was not opened using Open()
	RemoveItem = func(user_ID, int custom_menu_id)
	{
		custom_menu_id = custom_menu_id ?? GetRootID();
		for (var i = 0; i < GetLength(entries); ++i)
		{
			var ID = i+1;
			if (!entries[i]) continue;
			if (entries[i][3] != user_ID) continue;
			GuiClose(custom_menu_id, ID, this);
			entries[i] = nil;
			return true;
		}
		return false;
	},

	DoCall = func(int ID, command, proplist target, bool noclose, int player)
	{
		var self = this; // safety
		var entry = entries[ID - 1];
		target = target ?? entry[0];
		// target removed? safety first!
		if (target)
		{
			if (target->Call(command ?? entry[1], entry[2], entry[3], player) == -1) return;
		}
		if (self)
		if (!noclose && !permanent)
		{
			Close();
		}
	},

	OnClick = func(data, int player, int ID, int subwindowID, object target)
	{
		DoCall(subwindowID, nil, nil, nil, player);	
	},
	
	DoCallback = func(data, int player, int ID, int subwindowID, object target)
	{
		DoCall(subwindowID, data[1], data[0], true, player);
	},
};
