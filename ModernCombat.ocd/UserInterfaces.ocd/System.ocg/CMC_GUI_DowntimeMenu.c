/**
	Shared menu body for resapwn menu and incapacitated menu
	
	@author Marky
 */

/* --- The main menu --- */

static const CMC_GUI_DowntimeMenu = new GUI_Element
{
	Player = NO_OWNER, // will be shown once a gui update occurs
	Style = GUI_Multiple | GUI_NoCrop,
	Priority = GUI_CMC_Priority_HUD,
	
	GUI_Components = nil,
	
	ComponentIndex_Info = 0,

	Assemble = func (object target)
	{
		if (target)
		{
			this.Target = target;
			
			GUI_Components = [];
			
			// Button for settings
			var button_settings = new CMC_GUI_TextButton {};
			button_settings->Assemble()
			               ->AssignPlayerControl(target->GetOwner(), CON_CMC_GameSettings)
			               ->AlignRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_H))
			               ->AlignBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_V))
			               ->SetData("$ButtonLabelSettings$")
			               ->AddTo(this);
			               
			// Button for scoreboard
			var button_settings = new CMC_GUI_TextButton {};
			button_settings->Assemble()
			               ->AssignButtonHint(target->GetOwner(), "Tab")
			               ->AlignLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_Large_H))
			               ->AlignBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_V))
			               ->SetData("$ButtonLabelScoreboard$")
			               ->AddTo(this);
	
			// The actual box
			var infobox = new CMC_GUI_Element_StatusBox {};
			GUI_Components[ComponentIndex_Info] = infobox;
	
			infobox->Assemble()
			       ->AlignBottom(button_settings->GetTop()->Subtract(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
			       ->AddTo(this);
			         
			this->~AssembleInfoBox(infobox);
		}
		return this;
	},

	
	/* --- Access functions --- */
	
	GetInfoBox = func ()
	{
		return GUI_Components[ComponentIndex_Info]; 
	},
};
