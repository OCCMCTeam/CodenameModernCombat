
static const CMC_GUI_Ally_Info = new GUI_Element
{
	BackgroundColor = GUI_CMC_Background_Color_Default,

	GUI_Element_StatusIcons_Max = 4,       // this many are allowed at most
	GUI_Element_StatusIcons_Current = nil, // this many are currently displayed

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
			grade = 
			{
				Right = ToPercentString(500),
				Bottom = ToPercentString(500),
			},
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
		var child_id = 100 + this.ID;
		var info_field = new GUI_Element { ID = child_id, Style = GUI_TextVCenter };
		info_field->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + dash_offset))
		          ->SetRight(GuiDimensionCmc(nil, GUI_CMC_Element_Player_Width - GUI_CMC_Margin_Element_Small_V))
		          ->SetBottom(GuiDimensionCmc(nil, GUI_CMC_Element_Default_Height))
		          ->AddTo(this, child_id, "player_name");

		var status_icon_offset = nil;          
		for (var i = 0; i < this.GUI_Element_StatusIcons_Max; ++i)
		{
			var status_icon = new GUI_Element {};

			status_icon->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_StatusIcon_Size))
			           ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_StatusIcon_Size))
			           ->AlignRight(status_icon_offset)
			           ->AlignTop(GuiDimensionCmc(nil, 2))
			           ->AddTo(info_field, child_id, Format("status_icon_%d", i));

		    status_icon_offset = status_icon->GetLeft()->Subtract(GuiDimensionCmc(nil, GUI_Margin_StatusIcon_H));
		}

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

	SetRankSymbol = func (id symbol, int rank, int graphics_amount)
	{
		// Default graphics amount
		graphics_amount = graphics_amount ?? 24;

		// Rank itself
		this.player_rank.Symbol = symbol;
		this.player_rank.GraphicsName = Format("%d", rank % graphics_amount);

		// Upgrade
		this.player_rank.grade.Symbol = symbol;
		this.player_rank.grade.GraphicsName = Format("Upgrade%d", rank / graphics_amount);
		return this;
	},

	GetHealthBar = func ()
	{
		return this.bars.health_bar;
	},

	AddStatusIcon = func (symbol, string identifier)
	{
		AssertNotNil(identifier);

		// Only add if there are enough items, otherwise simply ignore it
		if (GUI_Element_StatusIcons_Current < GUI_Element_StatusIcons_Max)
		{
			var existing = GetStatusIcon(identifier);
			if (!existing)
			{
				existing = GetStatusIcon(this.GUI_Element_StatusIcons_Current ?? 0);
				existing.Priority = this.GUI_Element_StatusIcons_Current; // Save the position / index for removal

				// Increase counter
				this.GUI_Element_StatusIcons_Current += 1;
			}
			existing.Symbol = symbol;
			existing.Status_Identifier = identifier;
			existing->Update();
		}
		return this;
	},

	RemoveStatusIcon = func (identifier)
	{
		var icon = GetStatusIcon(identifier);

		if (icon)
		{
			var removed_index = icon.Priority ?? 0;
			for (var index = removed_index; index < this.GUI_Element_StatusIcons_Max; ++index)
			{
				var current = GetStatusIcon(index);
				if (current)
				{
					// Transfer info from the current icon
					icon.Symbol = current.Symbol;
					icon.Status_Identifier = current.Status_Identifier;
					icon->Update();

					// Delete info in the current icon
					current.Symbol = nil;
					current.Status_Identifier = nil;
					current->Update();
					icon = current;
				}
			}
		}
		return this;
	},

	GetStatusIcon = func (identifier)
	{
		if (GetType(identifier) == C4V_String)
		{
			if (this.player_name[identifier])
			{
				return this.player_name[identifier];
			}
			else
			{
				for (var i = 0; i < this.GUI_Element_StatusIcons_Max; ++i)
				{
					var icon = GetStatusIcon(i);
					if (icon.Status_Identifier == identifier)
					{
						return icon;
					}
				}
				return nil;
			}
		}
		else if (GetType(identifier) == C4V_Int)
		{
			return GetStatusIcon(Format("status_icon_%d", identifier));
		}
		else
		{
			FatalError("Has to be string or int, got %v", GetType(identifier));
		}
	}
};
