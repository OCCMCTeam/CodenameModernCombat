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
		SetLeft(GuiDimensionCmc(nil, GUI_CMC_Margin_Screen_Large_H));
		SetRight(GuiDimensionCmc(1000, -GUI_CMC_Margin_Screen_Large_H));
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
		     ->AddTo(this, nil, "label");

		// Button hint
		var button_hint = new CMC_GUI_ButtonHint{};
		button_hint->Assemble(nil, button_assignment)
		           ->AddTo(this);

		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + GUI_CMC_Element_TextButton_Width));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		return this;
	},
};