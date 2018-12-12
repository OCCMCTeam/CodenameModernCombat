/**
	Collection of miscellaneous GUI elements.
	
	@author Marky
 */

/* --- Status box, for Resapwn and Incapacitated menu --- */

static const CMC_GUI_Element_StatusBox = new GUI_Element
{
	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	Assemble = func (object target)
	{
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_PlayerStatus_Height));
		SetLeft(nil);
		SetRight(1000);
		return this;
	},
};

/* --- Functionality-Buttons  --- */


static const CMC_GUI_TextButton = new CMC_GUI_Button
{
	// --- GUI Properties

	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	hover = nil, // Overlay for hover effect
	
	// --- Functions
	
	AssignPlayerControl = func (int player, int control) // Sets the button hint and default with, defines some stuff from the inherited control
	{
		return AssignButtonHint(player, GetPlayerControlAssignment(player, control, true, true));
	},
	
	AssignButtonHint = func (int player, string button_assignment) // Sets the button hint and default with, defines some stuff from the inherited control
	{
		this.hover = { Priority = 1};
		
		// Text
		this.label = new GUI_Element { Priority = 2, Style = GUI_TextLeft | GUI_TextVCenter | GUI_NoCrop};
		this.label->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_TextButton_Width))
		     ->AlignLeft(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		     ->AddTo(this, nil, "label", true);

		// Button hint
		var button_hint = new CMC_GUI_ButtonHint{};
		button_hint->Assemble(nil, button_assignment)
		           ->AddTo(this);

		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + GUI_CMC_Element_TextButton_Width));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	},
};

/* --- Tab container, can hold various CMC_GUI_Buttons in a grid or list --- */

static const CMC_GUI_Element_TabContainer = new GUI_Element
{
	Style = GUI_GridLayout,
};

static const CMC_GUI_Controller_Tab = new Global
{
	// Elements for pseudo-proplist
	// Adding a real proplist would add the elements as a submenu ()
	Tab_Ids = nil,
	Tab_Elements = nil,

	GetTab = func (identifier, int index, bool strict)
	{
		// Establish defaults
		this.Tab_Ids = this.Tab_Ids ?? [];
		this.Tab_Elements = this.Tab_Elements ?? [];
		
		if (identifier)
		{
			index = GetIndexOf(this.Tab_Ids, identifier);
		}
		index = index ?? 0;

		if (index == -1)
		{
			if (strict)
			{
				FatalError("Tab not found");
			}
			return nil;
		}
		return this.Tab_Elements[index];
	},
	
	GetTabCount = func ()
	{
		return GetLength(this.Tab_Ids ?? []);
	},

	AddTab = func (identifier, proplist tab, bool overwrite)
	{
		// Establish defaults
		this.Tab_Ids = this.Tab_Ids ?? [];
		this.Tab_Elements = this.Tab_Elements ?? [];

		var index = GetIndexOf(this.Tab_Ids, identifier);
		if (index >= 0)
		{
			if (overwrite)
			{
				this.Tab_Elements[index] = tab;
			}
			tab = this.Tab_Elements[index];
		}
		else
		{
			PushBack(this.Tab_Ids, identifier);
			PushBack(this.Tab_Elements, tab);
		}
		
		// Done
		return tab;
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
