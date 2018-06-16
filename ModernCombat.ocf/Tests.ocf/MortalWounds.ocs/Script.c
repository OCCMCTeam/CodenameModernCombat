/**
	Unit test for the mortal wounds mechanic
 */

static player_victim;


func Initialize()
{
	// Create script players for these tests.
	CreateScriptPlayer("Victim",     RGB(0, 0, 255), nil, CSPF_NoEliminationCheck);
}


func InitializePlayer(int player)
{
	// Initialize script player.
	if (GetPlayerType(player) == C4PT_Script)
	{
		// Store the player numbers.
		// Make crew of killers invincible and set position.	
		if (GetPlayerName(player) == "Victim")
		{
			player_victim = player;
		}
		return;
	}
	
	// Set zoom to full map size.
	SetPlayerZoomByViewRange(player, LandscapeWidth(), nil, PLRZOOM_Direct);
	
	// No FoW to see everything happening.
	SetFoW(false, player);
	
	// Move normal players into a relaunch container.
	var relaunch = CreateObject(RelaunchContainer, LandscapeWidth() / 2, LandscapeHeight() / 2);
	GetCrew(player)->Enter(relaunch);

	LaunchTest(1);
	return true;
}

/* --- Some helper things --- */

global func GetPlayerName(int player)
{
	if (player == NO_OWNER)
		return "NO_OWNER";
	return _inherited(player, ...);
}


public func OnClonkDeath(object clonk, int killer)
{
	if (clonk->GetOwner() != player_victim || clonk.no_kill_tracing)
	{
		return;
	}
	var test = CurrentTest();
	if (test)
	{
		Call(Format("Test%d_OnClonkDeath", test.testnr), clonk, killer);
	}
	return FX_OK;
}



global func InitTest()
{
	// Remove all objects except the player crew members and relaunch container they are in.
	for (var obj in FindObjects(Find_Not(Find_Or(Find_ID(RelaunchContainer), Find_Category(C4D_Rule)))))
		if (!((obj->GetOCF() & OCF_CrewMember) && (GetPlayerType(obj->GetOwner()) == C4PT_User || obj->GetOwner() == player_victim)))
			obj->RemoveObject();

	// Remove all landscape changes.
	DrawMaterialQuad("Brick", 0, 160, LandscapeWidth(), 160, LandscapeWidth(), LandscapeHeight(), 0, LandscapeHeight());
	ClearFreeRect(0, 0, LandscapeWidth(), 160);
	
	// Give script players new crew.
	var victim_crew = GetCrew(player_victim);
	if (victim_crew)
	{
		victim_crew.no_kill_tracing = true;
		victim_crew->RemoveObject();	
	}
	for (var player in [player_victim])
	{	
		if (!GetCrew(player))
		{
			var clonk = CreateObjectAbove(Peacemaker, 100, 150, player);
			clonk->MakeCrewMember(player);
			clonk->SetDir(DIR_Right);
			SetCursor(player, clonk);
			clonk->DoEnergy(clonk.MaxEnergy / 1000);
		}
	}
	GetCrew(player_victim)->SetPosition(100, 150);
	return true;
}

/* --- Tests --- */

//--------------------------------------------------------

global func Test1_OnStart(int player){ return InitTest();}
global func Test1_OnFinished(){ return; }
global func Test1_Execute()
{
	Log("Test that clonks die normally if the rule is not active");
	
	var victim = GetCrew(player_victim);
	if (CurrentTest().test1_killed)
	{
		doTest("Victim should be removed upon death. Object is %v, expected %v", victim, nil);
		return Evaluate();
	}
	else
	{
		if (victim)
		{
			victim->Kill();
			CurrentTest().test1_killed = true;
			return Wait(30);
		}
		else
		{
			return FailTest(); // Victim was not created
		}
	}
}
global func Test1_OnClonkDeath(object clonk, int killer)
{
	ScheduleCall(clonk, Global.RemoveObject, 25, 1);
}


//--------------------------------------------------------

global func Test2_OnStart(int player){ CreateObject(CMC_Rule_MortalWounds); return InitTest();}
global func Test2_OnFinished(){ return; }
global func Test2_Execute()
{
	Log("Test that clonks do not die immediately if the rule is active");
	
	var victim = GetCrew(player_victim);
	
	if (victim)
	{
		victim->DoEnergy(-12);
		victim->DoEnergy(+5);
	
		Log("GetAlive: %v", victim->GetAlive());
		
		victim->DoEnergy(-1000);
		Log("GetAlive: %v", victim->GetAlive());
	}
	
	return FailTest(); // Test not implemented
}
global func Test2_OnClonkDeath(object clonk, int killer)
{
	ScheduleCall(clonk, Global.RemoveObject, 25, 1);
}
