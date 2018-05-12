/**
	Respawn menu
	
	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_RespawnMenu = new GUI_Element
{
	Player = NO_OWNER, // will be shown once a gui update occurs
	Style = GUI_Multiple | GUI_NoCrop,
	
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

		// Spawn points list
		var list_container = new GUI_Element
		{
			Priority = 2,
		};
		list_container->SetHeight(GuiDimensionCmc(1000)->Subtract(icon_size->Scale(2)))
		              ->AddTo(box_right);
		              
		var spawnpoint_list = new GUI_Element
		{
			ID = 4,
			BackgroundColor = GUI_CMC_Background_Color_Default,
			Style = GUI_VerticalLayout,
		};
		spawnpoint_list->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V))
		               ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Element_V))
		               ->AddTo(list_container);
		GUI_Respawn_Components[4] = spawnpoint_list;
		
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
	
	GetRightBox = func ()
	{
		return GUI_Respawn_Components[3]; 
	},
	
	GetSpawnPoints = func ()
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
		return !user_ready;
	},
	
	/* --- Reset functions --- */
	
	ResetContentBox = func ()
	{
		GetContentBox()->Close();
		AssembleContentBox()->Update();
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
			tab->Assemble()->AddTo(this);

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

static const CMC_GUI_RespawnMenu_TabButton = new GUI_Element
{
	// --- Properties

	Tab_Selected = nil,
	Tab_Hovered = nil,
	Tab_Callback = nil,

	// --- GUI Properties

	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	hover = nil, // Overlay for hover effect
	label = nil, // Overlay for text, should be over the hover effect
	
	// --- Functions
	
	Assemble = func (desired_width)
	{
		this.OnClick = GuiAction_Call(this, GetFunctionName(this.OnClickCall));
		this.OnMouseIn = GuiAction_Call(this, GetFunctionName(this.OnMouseInCall));
		this.OnMouseOut = GuiAction_Call(this, GetFunctionName(this.OnMouseOutCall));
		
		this.hover = { Priority = 1};
		this.label = { Priority = 2, Style = GUI_TextHCenter | GUI_TextVCenter};
		
		SetWidth(desired_width ?? 100);
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	},
	
	SetData = func (string caption, array callback, proplist style)
	{
		this.label.Text = caption;
		if (style)
		{
			AddProperties(this, style);
		}
		this.Tab_Callback = callback;
		return this;
	},
	
	OnMouseInCall = func ()
	{
		Update({ hover = {BackgroundColor = GUI_CMC_Background_Color_Hover}});
	},
	
	OnMouseOutCall = func ()
	{
		Update({ hover = {BackgroundColor = nil}});
	},
	
	OnClickCall = func ()
	{
		GetParent()->SelectTab(nil, this.Priority);
	},
	
	SetSelected = func (bool selected)
	{
		// Update the display
		this.Tab_Selected = selected;
		UpdateBackground();
		
		// Issue a callback?
		if (this.Tab_Callback && selected)
		{
			DoCallback(this.Tab_Callback);
		}
		return this;
	},
	
	UpdateBackground = func (int color)
	{
		if (color == nil)
		{
			if (this.Tab_Selected)
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
