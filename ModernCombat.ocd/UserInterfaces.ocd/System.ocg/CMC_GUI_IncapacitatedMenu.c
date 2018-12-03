/**
	Incapacitated
	
	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_IncapacitatedMenu = new CMC_GUI_DowntimeMenu
{
	ComponentIndex_Digits = 1,
	ComponentIndex_Text = 2,

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

