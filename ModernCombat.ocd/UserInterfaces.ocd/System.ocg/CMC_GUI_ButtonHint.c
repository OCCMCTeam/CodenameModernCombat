/**
	Button hint element that can be added to other GUI elements.
	
	@author Marky
 */

static const CMC_GUI_ButtonHint = new GUI_Element
{
	Assemble = func (id icon, string graphics_name)
	{
		SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size));
		
		var button_symbol = new GUI_Element{};
		button_symbol->SetWidth(GuiDimensionCmc(nil, GUI_CMC_Element_ButtonHint_Size))
		             ->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ButtonHint_Size))
		             ->AlignCenterH()->AlignCenterV();
		//button_symbol.Margin = GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)->ToString();
		button_symbol.Symbol = icon ?? CMC_Icon_Button;
		button_symbol.GraphicsName = graphics_name;
		
		button_symbol->AddTo(this, nil, "button_symbol", true);
		return this;
	},
	
	GetButtonIcon = func ()
	{
		return this.button_symbol;
	}
};
