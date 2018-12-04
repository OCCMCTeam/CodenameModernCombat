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

	AssembleInfoBox = func (proplist infobox)
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

		// Callback in the wrong place, lets see
		AssembleTabs(this->GetTabs());
		return infobox;
	},
	
	AssembleTabs = func (proplist tabs)
	{
			// Request help
			var button_help = new CMC_GUI_TextButton {};
			button_help->Assemble()
			           ->AssignPlayerControl(tabs.GUI_Owner, CON_CMC_Incapacitated_RequestHelp)
			           ->SetData("$IncapacitatedRequestHelp$")
			           ->SetIndex(0)
			           ->AddTo(tabs);

			// Button for toggle
			var button_toggle = new CMC_GUI_TextButton {};
			button_toggle->Assemble()
			             ->AssignPlayerControl(tabs.GUI_Owner, CON_CMC_Incapacitated_ToggleReanimation)
			             ->SetData("$IncapacitatedReanimAllowed$")
			             ->SetIndex(1)
			             ->AddTo(tabs);

			button_help->ComposeLayout();
			button_toggle->ComposeLayout();
			tabs->ComposeLayout();

			tabs->SetHeight(button_help->GetHeight())
			    ->ShiftTop(button_help->GetHeight());
			tabs->SetWidth(button_help->GetWidth()->Scale(2)->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))->AlignCenterH();
			tabs->AddTab(0, button_help);
			tabs->AddTab(1, button_toggle);
			tabs->Update();
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

		var caption = "$RespawnWaitingIncapacitated$";
		var roundup = RELAUNCH_Factor_Second - frames % RELAUNCH_Factor_Second;
		var seconds = (frames + roundup) / RELAUNCH_Factor_Second; // Round up, so that 1 second is displayed when 35 frames are hit
		if (seconds == 1)
		{
			caption = "$RespawnImminentIncapacitated$";
		}

		GetCountdownDigits()->SetValue(seconds)->Update();
		GetCountdownText()->Update({Text = caption});
	},
};

