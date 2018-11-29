/**
	Respawn menu
	
	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_RespawnMenu = new GUI_Element
{
	Player = NO_OWNER, // will be shown once a gui update occurs
	Style = GUI_Multiple | GUI_NoCrop,
	Priority = GUI_CMC_Priority_HUD,
	
	GUI_Respawn_Components = nil,

	Assemble = func (object target)
	{
		if (target)
		{
			this.Target = target;
			
			// Basic layout
			GUI_Respawn_Components = [];
	
			var header = new GUI_Element {};
			var tabs = new CMC_GUI_RespawnMenu_TabRow {ID = 1};
			tabs->Assemble();
			
			header->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_V))
			      ->SetHeight(tabs->GetHeight())
			      ->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_H))
			      ->SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H))
			      ->AddTo(this);
			tabs->AddTo(header);
			GUI_Respawn_Components[0] = header;
			GUI_Respawn_Components[1] = tabs;
	
			AssembleContentBox();
			AssembleRespawnBox();
		}
		return this;
	},
	
	/* --- Creation functions --- */
	
	AssembleContentBox = func ()
	{
		var child_id = 2;
		var box_left = new GUI_Element
		{
			ID = child_id, 
			Style = GUI_VerticalLayout,
		};
		box_left->SetTop(GetHeader()->GetBottom()->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
		        ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_V))
		        ->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_H))
		        ->SetRight(250)
		        ->AddTo(this);
		        
		GUI_Respawn_Components[child_id] = box_left;
		return box_left;
	},
	
	AssembleRespawnBox = func ()
	{
		var child_id = 3;
		var box_right = new GUI_Element
		{
			ID = child_id,
			Style = GUI_VerticalLayout,
		};
		box_right->SetTop(GetHeader()->GetBottom()->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
		         ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_V))
		         ->SetLeft(750)
		         ->SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H))
		         ->AddTo(this);

		GUI_Respawn_Components[child_id] = box_right;
		
		// Fill directly, because this has less different contents
		
		var icon_size = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);
		
		// Overview button
		var button_overview = new CMC_GUI_RespawnMenu_OverviewButton
		{
			Priority = 1,
			Tooltip = "$OverviewButtonTooltip$",
		};
		button_overview->Assemble()
		               ->SetWidth(1000)
		               ->SetHeight(icon_size)
		               ->SetData("$OverviewButtonLabel$", DefineCallback(Global.SetPlayerZoomLandscape, this.Target->GetOwner()))
		               ->AddTo(box_right);

		// Deploy location list
		var list_container = new GUI_Element
		{
			Priority = 2,
		};
		list_container->SetHeight(GuiDimensionCmc(1000)->Subtract(icon_size->Scale(2)))
		              ->AddTo(box_right);
		              
		var deploy_location_list = new CMC_GUI_RespawnMenu_DeployList
		{
			ID = 4,
			//BackgroundColor = GUI_CMC_Background_Color_Default,
			Style = GUI_VerticalLayout,
		};
		deploy_location_list->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V))
		                    ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Element_V))
		                    ->AddTo(list_container);
		GUI_Respawn_Components[4] = deploy_location_list;
		
		// Respawn button
		var button_respawn = new CMC_GUI_RespawnMenu_RespawnButton
		{
			Priority = 3,
			Tooltip = "$RespawnButtonTooltip$",
		};
		button_respawn->Assemble()
		              ->SetWidth(1000)
		              ->SetHeight(icon_size)
		              ->SetData(Format("$RespawnButtonLabelWaiting$", CMC_RELAUNCH_TIME))
		              ->AddTo(box_right);
		GUI_Respawn_Components[5] = button_respawn;
	},
	
	/* --- Access functions --- */
	
	GetHeader = func ()
	{
		return GUI_Respawn_Components[0]; 
	},
	
	GetTabs = func ()
	{
		return GUI_Respawn_Components[1]; 
	},
	
	GetContentBox = func ()
	{
		return GUI_Respawn_Components[2]; 
	},
	
	GetRespawnBox = func ()
	{
		return GUI_Respawn_Components[3]; 
	},
	
	GetDeployLocations = func ()
	{
		return GUI_Respawn_Components[4]; 
	},
	
	GetRespawnButton = func ()
	{
		return GUI_Respawn_Components[5]; 
	},
	
	/* --- Status functions --- */
	
	IsRespawnBlocked = func ()
	{
		var user_ready = GetRespawnButton()->IsUserReady();
		var no_class_selected = this.Target.GetCrewClass && this.Target->GetCrewClass() == nil;
		
		var deploy_location_available = false;
		var deploy_location = SelectedDeployLocation();
		if (deploy_location)
		{
			deploy_location_available = deploy_location->IsAvailable(this.Target->GetOwner());
		}
		return !user_ready
		    ||  no_class_selected
		    || !deploy_location_available;
	},
	
	SelectedDeployLocation = func ()
	{
		var deploy_location = GetDeployLocations()->GetSelectedTab();
		if (deploy_location && deploy_location->GetLocation())
		{
			return deploy_location->GetLocation();
		}
		return nil;
	},
	
	/* --- Reset functions --- */
	
	ResetContentBox = func ()
	{
		GetContentBox()->Close();
		AssembleContentBox()->Update();
	},
	
	ResetRespawnBox = func ()
	{
		GetRespawnBox()->Close();
		AssembleRespawnBox()->Update();
	},
};

/* --- Tab row --- */

static const CMC_GUI_RespawnMenu_TabRow = new GUI_Element
{
	Style = GUI_GridLayout,

	// Elements for pseudo-proplist
	// Adding a real proplist would add the elements as a submenu ()
	Tab_Ids = nil,
	Tab_Elements = nil,
	Tab_Width = nil,

	Assemble = func ()
	{
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	},

	AddTab = func (identifier, string caption, array callback, proplist style)
	{
		// Establish defaults
		this.Tab_Ids = this.Tab_Ids ?? [];
		this.Tab_Elements = this.Tab_Elements ?? [];
		this.Tab_Width = this.Tab_Width ?? GuiDimensionCmc();
		
		var tab;
		var index = GetIndexOf(this.Tab_Ids, identifier);
		if (index >= 0)
		{
			tab = this.Tab_Elements[index];
		}
		else
		{
			var tab_count = GetLength(this.Tab_Elements);
			
			// Add additional tab
			tab = new CMC_GUI_RespawnMenu_TabButton { Priority = tab_count, Tab_Index = tab_count };
			tab->Assemble()->WithDefaultDimensions()->SetIndex(tab_count)->AddTo(this);

			PushBack(this.Tab_Ids, identifier);
			PushBack(this.Tab_Elements, tab);
	
			// Update tab width
			this.Tab_Width = this.Tab_Width->Add(tab->GetWidth());
			var percent = this.Tab_Width->GetPercent() * 10 / (this.Tab_Width->GetPercentFactor() ?? 10);
			if (percent >= 1000)
			{
				this.Tab_Width->SetPercent(1000)->SetPercentFactor(10);
				this.Tab_Width->SetEm(0);
			}
		}
		tab->SetData(caption, callback, style)->Update();
		
		// Update the individual tabs for uniform width
		var tab_width = GuiDimensionCmc(1000)->Shrink(GetLength(this.Tab_Elements));
		for (var element in this.Tab_Elements)
		{
			element->SetWidth(tab_width)->Update();
		}

		SetWidth(this.Tab_Width);
		AlignCenterH();
		Update(ComposeLayout());
		
		// Done
		return tab;
	},
	
	SelectTab = func (identifier, int index)
	{
		if (identifier)
		{
			index = GetIndexOf(this.Tab_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			FatalError("Tab not found");
		}
		
		for (var i = 0; i < GetLength(this.Tab_Elements); ++i)
		{
			this.Tab_Elements[i]->SetSelected(i == index);
		}
	},
};

/* --- Tab button --- */

static const CMC_GUI_RespawnMenu_TabButton = new CMC_GUI_Button
{
	WithDefaultDimensions = func (desired_width)
	{
		SetWidth(desired_width ?? 100);
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	}
};

static const CMC_GUI_RespawnMenu_OverviewButton = new CMC_GUI_RespawnMenu_TabButton // this is actually misuse, no? A separate list would be better
{
	OnClickCall = func ()
	{
		if (this.Tab_Callback)
		{
			DoCallback(this.Tab_Callback);
		}
	},
};

static const CMC_GUI_RespawnMenu_RespawnButton = new CMC_GUI_RespawnMenu_TabButton // this is actually misuse, no? A separate list would be better
{
	user_ready = false,   // bool - Did the player click the button, i.e. is he done with configuration?
	time_remaining = nil, // int remaining time

	OnClickCall = func ()
	{
		// Toggle readiness
		this.user_ready = !this.user_ready;
		UpdateBackground();
		OnTimeRemaining(this.time_remaining);
	},
	
	OnTimeRemaining = func (int frames)
	{
		frames = Max(frames, 0);
		this.time_remaining = frames;
		
		var caption;
		var seconds = frames / RELAUNCH_Factor_Second;
		if (frames == 0)
		{
			caption = "$RespawnButtonLabelReady$";
		}
		else if (user_ready)
		{
			caption = Format("$RespawnButtonLabelWaiting$", seconds);
		}
		else
		{
			caption = Format("$RespawnButtonLabelBlocked$", seconds);
		}
		
		this.label.Text = caption;
		Update({label = {Text = caption}});
	},
	
	
	UpdateBackground = func (int color)
	{
		if (color == nil)
		{
			if (this.user_ready)
			{
				UpdateBackground(GUI_CMC_Background_Color_Highlight);
			}
			else
			{
				UpdateBackground(GUI_CMC_Background_Color_Default);
			}
		}
		else
		{
			this.BackgroundColor = color;
			Update({BackgroundColor = color});
		}
	},
	
	IsUserReady = func ()
	{
		return this.user_ready;
	},
};

/* --- Deploy locations --- */

static const CMC_GUI_RespawnMenu_DeployList = new GUI_Element
{
	Style = GUI_GridLayout,

	// Elements for pseudo-proplist
	// Adding a real proplist would add the elements as a submenu ()
	Tab_Ids = nil,
	Tab_Elements = nil,
	Tab_Width = nil,

	AddTab = func (object location)
	{
		// Establish defaults
		this.Tab_Ids = this.Tab_Ids ?? [];
		this.Tab_Elements = this.Tab_Elements ?? [];
		this.Tab_Width = this.Tab_Width ?? GuiDimensionCmc();
		
		var identifier = location->ObjectNumber();
		
		var tab;
		var index = GetIndexOf(this.Tab_Ids, identifier);
		if (index >= 0)
		{
			tab = this.Tab_Elements[index];
		}
		else
		{
			var tab_count = GetLength(this.Tab_Elements);
		
			// Add additional tab
			tab = new CMC_GUI_RespawnMenu_LocationButton { Priority = location->GetPriority(), Tab_Index = tab_count };
			tab->Assemble()->WithDefaultDimensions(1000)->AddTo(this);

			PushBack(this.Tab_Ids, identifier);
			PushBack(this.Tab_Elements, tab);
		}
		tab->SetLocation(location)->Update();
		
		// Done
		return tab;
	},
	
	GetTab = func (identifier, int index)
	{
		if (identifier)
		{
			index = GetIndexOf(this.Tab_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			FatalError("Tab not found");
		}
		return this.Tab_Elements[index];
	},
	
	SelectTab = func (identifier, int index, bool skip_callback)
	{
		if (identifier)
		{
			index = GetIndexOf(this.Tab_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			FatalError("Tab not found");
		}
		
		for (var i = 0; i < GetLength(this.Tab_Elements); ++i)
		{
			this.Tab_Elements[i]->SetSelected(i == index, skip_callback);
		}
	},
	
	SelectBestTab = func (bool skip_callback)
	{
		var best_index = 0;
		for (var i = 1; i < GetLength(this.Tab_Elements); ++i)
		{
			if (this.Tab_Elements[i].Priority < this.Tab_Elements[best_index].Priority)
			{
				best_index = i;
			}
		}
		
		SelectTab(nil, best_index, skip_callback);
	},
	
	GetSelectedTab = func ()
	{
		for (var tab in this.Tab_Elements)
		{
			if (tab->IsSelected())
			{
				return tab;
			}
		}
		return nil;
	},
};

static const CMC_GUI_RespawnMenu_LocationButton = new CMC_GUI_RespawnMenu_TabButton
{
	TextColor = 0xffffffff,

	SetLocation = func (object location)
	{
		this.RespawnLocation = location->ObjectNumber();
		this.label.Text = location->GetName();
		this.ToolTip = location.Description;
		Update({ label = {Text = this.label.Text}, ToolTip = this.ToolTip});
		this.Tab_Callback = this->DefineCallback(this.ZoomTo);
		return this;
	},
	
	ZoomTo = func ()
	{
		var location = GetLocation();
		if (location)
		{
			var target = GetRoot().Target;
			var player = target->GetOwner();
			SetPlrView(player, location);
			SetPlayerZoomDefault(player);
		}
	},
	
	GetLocation = func ()
	{
		if (this.RespawnLocation)
		{
			return Object(this.RespawnLocation);
		}
		return nil;		
	},
	
	UpdateLocationStatus = func (int color)
	{
		var location = GetLocation();
		if (location)
		{
			// Update availability
			var target = GetRoot().Target;
			var player = target->GetOwner();
			SetEnabled(location->IsAvailable());

			// Support new color, or keep the same color if nil is passed
			this.TextColor = color ?? this.TextColor;
			if (this.Tab_Enabled)
			{
				color = this.TextColor;
			}
			else
			{
				color = SetRGBaValue(this.TextColor, 128, RGBA_ALPHA);
			}

			// Update the text
			this.label.Text = Format("<c %x>%s</c>", color, location->GetName());
			Update({ label = {Text = this.label.Text}});
		}
		return this;
	},
};

