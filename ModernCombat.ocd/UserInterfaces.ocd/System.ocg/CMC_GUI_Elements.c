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
