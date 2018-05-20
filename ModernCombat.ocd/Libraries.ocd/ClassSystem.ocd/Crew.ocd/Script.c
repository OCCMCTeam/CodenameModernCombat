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


public func OnCrewRelaunchFinish()
{
	_inherited(...);

	// Create ammo
	var ammo_types = GetProperties(GetCrewClass().Ammo);
	for (var ammo_type in ammo_types)
	{
		var ammo = GetDefinition(ammo_type);
		var amount = GetCrewClass().Ammo[ammo_type];
		this->SetAmmo(ammo, amount);
	}
	
	// Create contents
	var item_types = GetProperties(GetCrewClass().Items);
	for (var item_type in item_types)
	{
		var item = GetDefinition(item_type);
		var amount = GetCrewClass().Items[item_type];
		CreateContents(item, amount);
	}
	
	// Create grenades
	var grenade_types = GetProperties(GetCrewClass().Grenades);
	for (var grenade_type in grenade_types)
	{
		var grenade = GetDefinition(grenade_type);
		var amount = GetCrewClass().Grenades[grenade_type];
		// TODO - no grenades yet
	}
}

/* --- Callbacks from respawn system --- */

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

	// --- Actual contents

	var icon_size = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size);

	// Description box
	if (class.Description)
	{
		var description = new GUI_Element
		{
			Priority = 1,
			
			icon =  // Display icon and then transparent background with text, because text will be easier to read
			{
				Bottom = GuiDimensionCmc(1000, -GUI_CMC_Margin_Element_V)->ToString(),
				Symbol = class,
				
				desc =
				{
					Style = GUI_TextHCenter | GUI_TextVCenter,
					Text = class.Description,
					BackgroundColor = GUI_CMC_Background_Color_Default,
				},
			},
		};
		description->SetHeight(200)->AddTo(menu->GetContentBox());
	}

	// Ammo
	if (class.Ammo)
	{
		var ammo_info = new GUI_Element 
		{
			Priority = 2,
			
			box = 
			{
				BackgroundColor = GUI_CMC_Background_Color_Default,
				Bottom = icon_size->ToString(),
				Style = GUI_GridLayout,
			}
		};
		var ammo_types = GetProperties(class.Ammo);
		var index = 0;
		for (var ammo_type in ammo_types)
		{
			var ammo = GetDefinition(ammo_type);
			ammo_info.box[ammo_type] = 
			{
				Priority = index,
				Right = icon_size->ToString(),
				Bottom = icon_size->ToString(),
				
				Symbol = ammo,
				Text = Format("%dx", class.Ammo[ammo_type]),
				Tooltip = ammo->GetName(),
				Style = GUI_TextBottom | GUI_TextRight,
			};
			index += 1;
		}
		ammo_info->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + GUI_CMC_Margin_Element_V))
		         ->AddTo(menu->GetContentBox());
	}

	// Abilities
	if (class->ClassHasAbilities())
	{
		var ability_info = new GUI_Element 
		{
			Priority = 3,
			
			box = 
			{
				BackgroundColor = GUI_CMC_Background_Color_Default,
				Bottom = icon_size->ToString(),
				Style = GUI_GridLayout,
			}
		};

		var index = 1;

		if (class->ClassImprovesAmmoEquipment())
		{
			ability_info.box.ammo = AssembleClassTabAbilityIcon(index++, LeadBullet, icon_size->ToString());
		}

		if (class->ClassImprovesMedicalEquipment())
		{
			ability_info.box.ammo = AssembleClassTabAbilityIcon(index++, Icon_Heart, icon_size->ToString());
		}

		ability_info->SetHeight(GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size + GUI_CMC_Margin_Element_V))
		            ->AddTo(menu->GetContentBox());
		
	}
	
	// Weapons
	if (class.Items)
	{
		var item_types = GetProperties(class.Items);
		var item_count = GetLength(item_types);
		var size = GuiDimensionCmc(nil, GUI_CMC_Element_Icon_Size * item_count);
		var item_info = new GUI_Element 
		{
			Priority = 4,

			list = 
			{
				BackgroundColor = GUI_CMC_Background_Color_Default,
				Bottom = size->ToString(),
				Style = GUI_VerticalLayout,
			}
		};
		var index = 0;
		for (var item_type in item_types)
		{
			var item = GetDefinition(item_type);
			item_info.list[item_type] = 
			{
				Priority = index,
				Tooltip = item.Description,
				
				icon = 
				{
					Right = icon_size->ToString(),
					Bottom = icon_size->ToString(),
					Symbol = item,
					
					count = 
					{			
						Text = Format("%dx", class.Items[item_type]),
						Style = GUI_TextBottom | GUI_TextRight,
					},
				},
				
				label = 
				{
					Left = icon_size->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_Small_H))->ToString(),
					Style = GUI_TextVCenter,
					Text = item->GetName(),
				},
			};
			index += 1;
		}
		item_info->SetHeight(size->Add(GuiDimensionCmc(nil, GUI_CMC_Margin_Element_V)))
		         ->AddTo(menu->GetContentBox());
	}
}


func AssembleClassTabAbilityIcon(int priority, id symbol, string size)
{
	return
	{
		Priority = priority,
		Symbol = symbol,
		Text = symbol->GetName(),
		Tooltip = symbol.Description,

		Right = size,
		Bottom = size,
		Style = GUI_TextBottom | GUI_TextRight,
	};
}
