#appendto Library_CMC_Scenario

func RelaunchLocations()
{
	var possible_locations = [];

	// TODO - this is all temporary stuff, it is possible that the 
	// arena library has to be adjusted, so at the moment let's
	// just see if you can code comfortably for the needs of CMC
	// with the current implementation;
	// Further notes: Usually I would add only the necessary
	// changes, if I implemented something new, and adapted
	// the implementation later if we had more requirements;
	// However, here we arleady know what is required (sort of)
	// and I want to design the code in such a way that you
	// can programm comfortably, so I already keep in mind future
	// possibilities for this specific feature.
	
	// Design notes: Proplist prototypes, similar to firemode
	// provide a better interface here. I do not like accessing
	// properties directly, because you have 
	
	var is_goal_hold_the_flag = true;
	if (is_goal_hold_the_flag)
	{
		// Note: Originally this returned just the array with all possibly
		// locations... adding entries manually seems to make more sense
		if (GetTeamCount() == 2)
	    {
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition(420, 400)->SetTeam(1));
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition( 420,630)->SetTeam(1));
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition( 450,340)->SetTeam(1));
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition(1465,285)->SetTeam(2));
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition(1495,320)->SetTeam(2));
			PushBack(possible_locations, Arena_RelaunchLocation->AtPosition(1530,550)->SetTeam(2));
		}
		else
		{
			PushBack(possible_locations, {x =  540, y = 420});
			PushBack(possible_locations, {x =  540, y = 540});
			PushBack(possible_locations, {x =  540, y = 630});
			PushBack(possible_locations, {x = 1410, y = 340});
			PushBack(possible_locations, {x = 1410, y = 460});
			PushBack(possible_locations, {x = 1410, y = 550});
		}
	}
	else // Money run, last man standing, team deathmatch
	{
		// Note: I notice, that several locations appear multiple times
		// We should name those by location, and add them via variable names,
		// and move those that are shared out of the if-else-blocks
		if (GetTeamCount() == 2)
	    {
			PushBack(possible_locations, {x =   420, y = 400, team = 1}); 
			PushBack(possible_locations, {x =   420, y = 630, team = 1}); 
			PushBack(possible_locations, {x =   450, y = 340, team = 1}); 
			PushBack(possible_locations, {x =  1465, y = 285, team = 2});
			PushBack(possible_locations, {x =  1495, y = 320, team = 2});
			PushBack(possible_locations, {x =  1530, y = 550, team = 2});
		}
		else
		{
			PushBack(possible_locations, {x =   540, y = 540});
			PushBack(possible_locations, {x =   550, y = 630});
			PushBack(possible_locations, {x =   730, y = 610});
			PushBack(possible_locations, {x =  1220, y = 530});
			PushBack(possible_locations, {x =  1420, y = 460});
			PushBack(possible_locations, {x =  1400, y = 550});
		}
	}
	
	return possible_locations;
}
