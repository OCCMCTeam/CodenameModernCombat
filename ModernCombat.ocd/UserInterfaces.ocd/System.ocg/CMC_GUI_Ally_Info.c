
static const CMC_GUI_Ally_Info = new GUI_Element
{
	BackgroundColor = GUI_CMC_Background_Color_Default,
	
	Assemble = func ()
	{
		// Element data
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Player_Width));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));

		var dash_offset = 3;

		// Rank symbol		      
		var rank = new GUI_Element
		{
			Margin = ToEmString(5),
		};
		rank->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		    ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		    ->ShiftLeft(GuiDimensionCmc(nil, 2))
		    ->AddTo(this, nil, "player_rank");
		
		// Separator
		var dash = new GUI_Element
		{
			Symbol = CMC_Icon_Number,
			GraphicsName = "Dash",
			Margin = ToEmString(1),
		};
		dash->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		    ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size))
		    ->ShiftRight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size / 2 - dash_offset))
		    ->AddTo(this);
		
		// Player name and status icons
		var info_field = new GUI_Element { Text = "Player Name", Style = GUI_TextVCenter };
		info_field->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + dash_offset))
		          ->SetRight(GuiDimensionCmc(nil, GUI_CMC_Element_Player_Width - GUI_CMC_Margin_Element_Small_V))
		          ->SetBottom(GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height))
		          ->AddTo(this, nil, "player_name");
		
		// Health bar
		var bar_field = new GUI_Element {};
		bar_field->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + dash_offset))
		         ->SetRight(GuiDimensionCmc(nil, GUI_CMC_Element_Player_Width - GUI_CMC_Margin_Element_Small_V))
		         ->SetTop(GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height))
		         ->SetBottom(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size - GUI_CMC_Margin_Element_Small_V))
		         ->AddTo(this, nil, "bars");

		var health_bar = new CMC_GUI_HealthBar {};
		health_bar->Assemble()->AddTo(bar_field, this.ID, "health_bar");
		return this;
	},

	// Setter functions
	
	SetNameLabel = func (string name, int color)
	{
		if (color != nil)
		{
			name = Format("<c %x>%s</c>", color, name);
		}
		this.player_name.Text = name;
		return this;
	},

	SetRankSymbol = func (id symbol)
	{
		this.player_rank.Symbol = symbol;
		return this;
	},
	
	GetHealthBar = func ()
	{
		return this.bars.health_bar;
	},
};
