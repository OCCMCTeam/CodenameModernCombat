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
			var tabs = new CMC_GUI_RespawnMenu_TabRow {};
			tabs->Assemble();
			
			header->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_V))
			      ->SetHeight(tabs->GetHeight())
			      ->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_H))
			      ->SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H))
			      ->AddTo(this);
			tabs->AddTo(header);
	
			var box_left = new GUI_Element
			{
				BackgroundColor = GUI_CMC_Background_Color_Default
			};
			box_left->SetTop(header->GetBottom()->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
			        ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_V))
			        ->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_H))
			        ->SetRight(250)
			        ->AddTo(this);
	
			var box_right = new GUI_Element
			{
				BackgroundColor = GUI_CMC_Background_Color_Default
			};
			box_right->SetTop(header->GetBottom()->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
			         ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_V))
			         ->SetLeft(750)
			         ->SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_H))
			         ->AddTo(this);
	
			GUI_Respawn_Components[0] = tabs;
			GUI_Respawn_Components[1] = box_left;
			GUI_Respawn_Components[2] = box_right;
		}
		return this;
	},
	
	/* --- Access functions --- */
	
	GetTabs = func ()
	{
		return GUI_Respawn_Components[0]; 
	},
	
	GetContents = func ()
	{
		return GUI_Respawn_Components[1]; 
	},
	
	GetSpawnPoints = func ()
	{
		return GUI_Respawn_Components[2]; 
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

	AddTab = func (identifier, proplist style)
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
			tab->Assemble(style)->Update();
		}
		else
		{
			// Add additional tab
			tab = new CMC_GUI_RespawnMenu_TabButton { Priority = this.Tab_Count, };
			tab->Assemble(style)->AddTo(this);

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
};

/* --- Tab button --- */

static const CMC_GUI_RespawnMenu_TabButton = new GUI_Element
{
	Style = GUI_TextHCenter | GUI_TextVCenter,

	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	Assemble = func (proplist style, desired_width)
	{
		if (style)
		{
			AddProperties(this, style);
		}
		SetWidth(desired_width ?? 100);
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	},
	
	SetSelected = func (bool selected)
	{
		this.Tab_Selected = selected;
		return this;
	},
};
