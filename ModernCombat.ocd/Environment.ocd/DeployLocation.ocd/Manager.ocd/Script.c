/**
	Deploy location manager
	
	Handles deploy locations.
 */

/* --- Properties --- */

local Visibility = VIS_Editor;

local locations_static;
local locations_temporary;

/* --- Engine callbacks --- */

func Initialize()
{
	if (ObjectCount(Find_ID(GetID())) > 1)
	{
		RemoveObject();
	}
	locations_static = [];
	locations_temporary = [];
}

/* --- Handle deploy locations --- */

public func ScanDeployLocations()
{
	if (GetType(this) == C4V_Def)
	{
		return GetManager()->ScanDeployLocations();
	}
	else
	{
		var locations = FindObjects(Find_Func("IsDeployLocation"));
		locations_temporary = [];
		
		for (var location in locations)
		{
			if (location->~IsTemporary())
			{
				PushBack(locations_temporary, location);
			}
			else if (!IsValueInArray(locations_static, location))
			{
				PushBack(locations_static, location);
			}
		}
	}
}

public func GetDeployLocations(int player)
{
	if (GetType(this) == C4V_Def)
	{
		return GetManager()->GetDeployLocations(player);
	}
	else
	{
		var specific = [];
		for (var location in Concatenate(locations_static, locations_temporary))
		{
			if (location && location->IsDisplayed(player))
			{
				PushBack(specific, location);
			}
		}
		return specific;
	}
}


func GetManager()
{
	AssertDefinitionContext();
	var manager = FindObject(Find_ID(this));
	if (manager)
	{
		return manager;
	}
	else
	{
		return CreateObject(this);
	}
}
