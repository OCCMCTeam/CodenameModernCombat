/**
	Incapacitated
	
	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_IncapacitatedMenu = new GUI_Element
{
	Player = NO_OWNER, // will be shown once a gui update occurs
	Style = GUI_Multiple | GUI_NoCrop,
	
	GUI_Components = nil,
	
	ComponentIndex_Status = 0,
	ComponentIndex_Digits = 1,
	ComponentIndex_Text = 2,

	Assemble = func (object target)
	{
		if (target)
		{
			this.Target = target;
			
			GUI_Components = [];
			AssembleStatusBox();
		}
		return this;
	},
	
	/* --- Creation functions --- */
	
	AssembleStatusBox = func ()
	{
		// The actual box
		var statusbox = new CMC_GUI_Element_StatusBox {};
		GUI_Components[ComponentIndex_Status] = statusbox;

		statusbox->Assemble()
		         ->AlignBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Element_V))
		         ->AddTo(this);

		// Separator
		var separator = new GUI_Element { Symbol = CMC_Icon_Number, GraphicsName = "Dash", };
		separator->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size))
		         ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ListIcon_Size))
		         ->AlignCenterH()
		         ->AlignCenterV()
		         ->AddTo(statusbox);

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
		                ->AddTo(statusbox);
		                
		// Text
		var countdown_text = new GUI_Element {Style = GUI_TextLeft | GUI_TextVCenter, };
		GUI_Components[ComponentIndex_Text] = countdown_text;
		
		countdown_text->SetWidth(GuiDimensionCmc(400))
		              ->AlignLeft(separator->GetRight())
		              ->AddTo(statusbox);

		return statusbox;
	},
	
	/* --- Access functions --- */
	
	GetStatusBox = func ()
	{
		return GUI_Components[ComponentIndex_Status]; 
	},
	
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
		var seconds = frames / RELAUNCH_Factor_Second;
		if (seconds == 1)
		{
			caption = "$RespawnImminentIncapacitated$";
		}

		GetCountdownDigits()->SetValue(seconds)->Update();
		GetCountdownText()->Update({Text = caption});
	},
};

