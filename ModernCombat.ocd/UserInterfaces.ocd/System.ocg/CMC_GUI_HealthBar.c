
static const CMC_GUI_HealthBar = new CMC_GUI_ProgressBar
{
	Assemble = func ()
	{
		SetBackgroundColor(GUI_CMC_Color_HealthBar_Transparent);
	    SetBarColor({
	    	Health_Full = GUI_CMC_Color_HealthBar_White,
	        Health_Warn = GUI_CMC_Color_HealthBar_Opaque,
	    });
	    SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_ProgressBar_Height));
	    SetValue(1000); // Full in the beginning
		return this;
	},
	
	IssueWarning = func (bool warn, object target)
	{
		var tag = "Health_Full";
		if (warn)
		{
			tag = "Health_Warn";
		}
		// Has to be updated after changing the progress values
		// and after the GuiUpdate to take proper effect
		GuiUpdateTag(tag, GetRootID(), GetChildID());
	},
	
	SetHealth = func (object crew)
	{
		// Update the values
		var health_max = crew->~GetMaxEnergy();
		if (health_max)
		{
			var health = crew->GetEnergy();
			
			// Set values for the update, this does not yet apply the changes to the menu
			Show();
			SetValue(1000 * health / health_max);
			
			// Apply changes
			Update();
			// Change tag for incomplete health bar
			IssueWarning(health < health_max);
		}
		else
		{
			SetValue(1000);
			Hide();
			// Apply changes
			Update();
		}
	},
};
