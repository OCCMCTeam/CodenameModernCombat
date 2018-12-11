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
	
	ComponentIndex_Main = 0,
	ComponentIndex_Info = 1,

	Assemble = func (object target)
	{
		if (target)
		{
			this.Target = target;
			
			GUI_Components = [];
			
			// Main canvas
			var main = new GUI_Element {};
			
			main->SetTop(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_Large_V))
			    ->SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_Large_H))
			    ->SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_H))
			    ->SetBottom(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_V))
			    ->AddTo(this);
			GUI_Components[ComponentIndex_Main] = main;
			
			// Button for settings
			var button_settings = new CMC_GUI_TextButton {};
			button_settings->Assemble()
			               ->AssignPlayerControl(target->GetOwner(), CON_CMC_GameSettings)
			               ->AlignRight(1000)
			               ->AlignBottom(1000)
			               ->SetData("$ButtonLabelSettings$")
			               ->AddTo(main);
			button_settings.OnClickCall = this.ShowSettings;
			               
			// Button for scoreboard
			var button_scoreboard = new CMC_GUI_TextButton {};
			button_scoreboard->Assemble()
			                 ->AssignButtonHint(target->GetOwner(), "Tab")
			                 ->AlignLeft()
			                 ->AlignBottom(1000)
			                 ->SetData("$ButtonLabelScoreboard$")
			                 ->AddTo(main);
			button_scoreboard.OnClickCall = this.ShowScoreboard;
	
			// The actual box
			var infobox = new CMC_GUI_Element_StatusBox {};
			GUI_Components[ComponentIndex_Info] = infobox;
	
			infobox->Assemble()
			       ->AlignBottom(button_settings->GetTop()->Subtract(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
			       ->AddTo(main);

			// Callbacks
			this->~AssembleInfoBox(target, infobox);
		}
		return this;
	},

	
	/* --- Access functions --- */
	
	GetMainWindow = func ()
	{
		return GUI_Components[ComponentIndex_Main]; 
	},
	
	GetInfoBox = func ()
	{
		return GUI_Components[ComponentIndex_Info]; 
	},

	/* --- Button click functions --- */
	
	ShowScoreboard = func () // Shows the scoreboard for 5 seconds, if it is not already displayed; Toggle is hard to implement properly
	{
		var player = this.GUI_Owner + 1;
		Schedule(nil, Format("DoScoreboardShow(+1, %d)", player), 1);
		Schedule(nil, Format("DoScoreboardShow(-1, %d)", player), RELAUNCH_Factor_Second * 5);
	},
	
	ShowSettings = func () // Does not work yet...
	{
		PlayerControl(this.GUI_Owner, CON_PlayerMenu, nil, nil, nil, false, CONS_Down);
		PlayerControl(this.GUI_Owner, CON_PlayerMenu, nil, nil, nil, false, CONS_Up);
	},
};
