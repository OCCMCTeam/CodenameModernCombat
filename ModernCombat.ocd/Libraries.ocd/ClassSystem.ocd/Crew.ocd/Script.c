/**
	Interface for Clonks
	
	Clonks that want to use the class system should include this library.
	
	@author Marky
*/

/* --- Properties --- */

local cmc_crew_class = nil;

/* --- Class system interface --- */

public func GetCrewClass()
{
	return cmc_crew_class;
}

public func SetCrewClass(id class)
{
	cmc_crew_class = class;
	this->~OnSetCrewClass(class);
}

public func GetAvailableClasses()
{
	return [CMC_Class_Assault, CMC_Class_Medic, CMC_Class_Support, CMC_Class_AntiSkill, CMC_Class_Artillery];
}

/* --- Callbacks from other systems --- */

public func OnOpenRespawnMenu(proplist menu)
{
	_inherited(menu, ...);

	// Add class selection tabs
	for (var class in this->GetAvailableClasses())
	{
		menu->GetTabs()->AddTab(class,                                               // identifier
		                        class->~GetName(),                                   // label text
		                        DefineCallback(this.OnSelectClassTab, menu, class)); // called on button click
	}
	menu->GetTabs()->SelectTab();
}

public func OnSelectClassTab(proplist menu, id class)
{
	// Update the class
	SetCrewClass(class);
	
	// Update the contents box
	menu->ResetContentBox();
	
	if (class.Description)
	{
		var description = new GUI_Element
		{
			Priority = 1,
			Style = GUI_TextHCenter | GUI_TextVCenter,
			
			Text = class.Description,
			Symbol = class,
		};
		description->SetHeight(200)->AddTo(menu->GetContentBox());
	}
}
