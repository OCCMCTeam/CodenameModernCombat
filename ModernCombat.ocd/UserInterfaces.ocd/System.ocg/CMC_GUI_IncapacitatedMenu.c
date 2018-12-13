/**
	Incapacitated

	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_IncapacitatedMenu = new CMC_GUI_DowntimeMenu
{
	ComponentIndex_Digits = 2,
	ComponentIndex_Text = 3,

	/* --- Creation functions --- */

	AssembleInfoBox = func (object target, proplist infobox)
	{
		// Separator
		var separator = new GUI_Element { Symbol = CMC_Icon_Number, GraphicsName = "Dash", };
		separator->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size))
		         ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size))
		         ->AlignCenterH()
		         ->AlignCenterV()
		         ->AddTo(infobox);

		// Large digits for remaining seconds
		var countdown_digits = new GUI_Counter {};
		GUI_Components[ComponentIndex_Digits] = countdown_digits;

		countdown_digits->SetMaxDigits(3)
		                ->ShowTrailingZeros(false)
		                ->SetDigitProperties({Symbol = CMC_Icon_Number})
		                ->SetDigitWidth(GuiDimensionCmc(nil, 52))
		                ->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_Large_V))
		                ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_V))
		                ->AlignRight(separator->GetLeft())
		                ->AddTo(infobox);

		// Text
		var countdown_text = new GUI_Element {Style = GUI_TextLeft | GUI_TextVCenter, };
		GUI_Components[ComponentIndex_Text] = countdown_text;

		countdown_text->SetWidth(GuiDimensionCmc(400))
		              ->AlignLeft(separator->GetRight())
		              ->AddTo(infobox);

		// Additional buttons
		var button_help = new CMC_GUI_TextButton {};
		button_help->Assemble()
		           ->AssignPlayerControl(target->GetOwner(), CON_CMC_Incapacitated_RequestHelp)
		           ->SetData("$IncapacitatedRequestHelp$")
		           ->ComposeLayout();
		button_help->SetRight(button_help->GetRight()->Add(GuiDimensionCmc(nil, 20)));

		var button_toggle = new CMC_GUI_TextButton {};
		button_toggle->Assemble()
		             ->AssignPlayerControl(target->GetOwner(), CON_CMC_Incapacitated_ToggleReanimation)
		             ->SetData("$IncapacitatedReanimAllowed$")
		             ->ComposeLayout();
		button_toggle->SetRight(button_help->GetRight());

		var margin = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V);
		var shift = button_help->GetWidth()->Add(margin)->Shrink(2);
		var bottom = this->GetInfoBox()->GetTop()->Subtract(margin);

		// Request help
		button_help->SetIndex(0)
		           ->AlignBottom(bottom)
		           ->AlignCenterH()
		           ->ShiftLeft(shift)
		           ->AddTo(GetMainWindow());

		// Button for toggle
		button_toggle->SetIndex(1)
		             ->AlignBottom(bottom)
		             ->AlignCenterH()
		             ->ShiftRight(shift)
		             ->AddTo(GetMainWindow());

		return infobox;
	},

	/* --- Access functions --- */

	GetCountdownDigits = func ()
	{
		return GUI_Components[ComponentIndex_Digits]; 
	},

	GetCountdownText = func ()
	{
		return GUI_Components[ComponentIndex_Text]; 
	},

	/* --- Status functions --- */

	OnTimeRemaining = func (int frames)
	{
		frames = Max(frames, 0);

		// Update the countdown text
		var caption = "$RespawnWaitingIncapacitated$";
		var roundup = RELAUNCH_Factor_Second - frames % RELAUNCH_Factor_Second;
		var seconds = (frames + roundup) / RELAUNCH_Factor_Second; // Round up, so that 1 second is displayed when 35 frames are hit
		if (seconds == 1)
		{
			caption = "$RespawnImminentIncapacitated$";
		}

		GetCountdownDigits()->SetValue(seconds)->Update();
		GetCountdownText()->Update({Text = caption});

		// Claustrophobic zoom effect
		var max_time = RELAUNCH_Factor_Second * 15;
		var view_range = InterpolateLinear(Min(frames, max_time), 0, CMC_ViewRange_GlobalMin, max_time, CMC_ViewRange_Default_Player);
		SetPlayerZoomByViewRange(this.GUI_Owner, view_range, nil, PLRZOOM_Set | PLRZOOM_LimitMax);
	},

	OnClose = func ()
	{
		// Reset zoom
		SetPlayerZoomDefault(this.GUI_Owner);
	},
};

