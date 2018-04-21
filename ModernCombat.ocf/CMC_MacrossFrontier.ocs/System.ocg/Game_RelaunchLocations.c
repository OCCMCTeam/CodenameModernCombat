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
	
	// Team positions
	var pos_team1_1 = {x =   420, y = 400, team = 1};
	var pos_team1_2 = {x =   420, y = 630, team = 1};
	var pos_team1_3 = {x =   450, y = 340, team = 1};
	
	var pos_team2_1 = {x =  1465, y = 285, team = 2};
	var pos_team2_2 = {x =  1495, y = 320, team = 2};
	var pos_team2_3 = {x =  1530, y = 550, team = 2};
	
	// Generic positions - TODO: a descriptive name would be useful for these
	var pos_all_1 = {x = 540, y = 420};
	var pos_all_2 = {x = 540, y = 540};
	// FIXME: Why do these two differ by 10 pixels only? Should be one position
	var pos_all_3_htf =   {x = 540, y = 630};
	var pos_all_3_other = {x = 550, y = 630};
	var pos_all_4 = {x = 1410, y = 340};
	var pos_all_5 = {x = 730, y = 610};
	var pos_all_6 = {x =  1220, y = 530};
	// FIXME: Why do these two differ by 10 pixels only? Should be one position
	var pos_all_7_htf =   {x = 1410, y = 460};
	var pos_all_7_other = {x = 1420, y = 460};
	// FIXME: Why do these two differ by 10 pixels only? Should be one position
	var pos_all_8_htf =   {x = 1410, y = 550};
	var pos_all_8_other = {x =  1400, y = 550};
	
	var is_goal_hold_the_flag = true;
	if (is_goal_hold_the_flag)
	{
		// Note: Originally this returned just the array with all possibly
		// locations... adding entries manually seems to make more sense
		if (GetTeamCount() == 2)
	    {
			PushBack(possible_locations, pos_team1_1);
			PushBack(possible_locations, pos_team1_2);
			PushBack(possible_locations, pos_team1_3);
			PushBack(possible_locations, pos_team2_1);
			PushBack(possible_locations, pos_team2_2);
			PushBack(possible_locations, pos_team2_3);
		}
		else
		{
			PushBack(possible_locations, pos_all_1);
			PushBack(possible_locations, pos_all_2);
			PushBack(possible_locations, pos_all_3_htf);
			PushBack(possible_locations, pos_all_4);
			PushBack(possible_locations, pos_all_7_htf);
			PushBack(possible_locations, pos_all_8_htf);
		}
	}
	else // Money run, last man standing, team deathmatch
	{
		// Note: I notice, that several locations appear multiple times
		// We should name those by location, and add them via variable names,
		// and move those that are shared out of the if-else-blocks
		if (GetTeamCount() == 2)
	    {
			PushBack(possible_locations, pos_team1_1);
			PushBack(possible_locations, pos_team1_2);
			PushBack(possible_locations, pos_team1_3);
			PushBack(possible_locations, pos_team2_1);
			PushBack(possible_locations, pos_team2_2);
			PushBack(possible_locations, pos_team2_3);
		}
		else
		{
			PushBack(possible_locations, pos_all_2);
			PushBack(possible_locations, pos_all_3_other);
			PushBack(possible_locations, pos_all_5);
			PushBack(possible_locations, pos_all_6);
			PushBack(possible_locations, pos_all_7_other);
			PushBack(possible_locations, pos_all_8_other);
		}
	}
	
	return possible_locations;
}
