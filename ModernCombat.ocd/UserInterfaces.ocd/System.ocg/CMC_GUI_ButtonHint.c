
static const CMC_GUI_ButtonHint = new GUI_Element
{
	
	Assemble = func (id icon, string graphics_name)
	{
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		
		this.button_symbol = {};
		this.button_symbol.Margin = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)->ToString();
		this.button_symbol.Symbol = icon ?? CMC_Icon_Button;
		this.button_symbol.GraphicsName = graphics_name;
		return this;
	},
};
