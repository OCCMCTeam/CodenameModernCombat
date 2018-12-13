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

global func Death_Timeout() {return 16 * 35;}
global func Death_Expected() {return 15 * 35;}
global func Death_MinDelay() {return 5 * 35;}

//--------------------------------------------------------

global func Test1_OnStart(int player){ CreateObject(CMC_Rule_MortalWounds); return InitTest();}
global func Test1_OnFinished()
{
	if (CMC_Rule_MortalWounds->GetInstance()) 
		CMC_Rule_MortalWounds->GetInstance()->RemoveObject(); 
	return;
}
global func Test1_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test1_killed)
	{
		doTest("Victim should be removed upon death. Object is %v, expected %v", victim, nil);
		return Evaluate();
	}
	else
	{
		Log("Test that clonks die normally if the rule is active");
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

global func Test2_OnStart(int player){ return InitTest();}
global func Test2_OnFinished(){ return; }
global func Test2_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test2_killed)
	{
		doTest("Victim should be removed upon death. Object is %v, expected %v", victim, nil);
		return Evaluate();
	}
	else
	{
		Log("Test that clonks do not die immediately if the rule is active");
		if (victim)
		{
			Log("Full energy");

			doTest("Energy is %d, should be %d", victim->GetEnergy(), victim.MaxEnergy / 1000);
			doTest("GetAlive returns %v, should be %v", victim->GetAlive(), true);
			doTest("GetOCF & OCF_Alive returns %d, should be %d", victim->GetOCF() & OCF_Alive, OCF_Alive);
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), false);


			Log("Half energy");
			var diff = victim.MaxEnergy / 2000;
			victim->DoEnergy(-diff);

			doTest("Energy is %d, should be %d", victim->GetEnergy(), victim.MaxEnergy / 1000 - diff);
			doTest("GetAlive returns %v, should be %v", victim->GetAlive(), true);
			doTest("GetOCF & OCF_Alive returns %d, should be %d", victim->GetOCF() & OCF_Alive, OCF_Alive);
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), false);

			Log("Dead");
			victim->DoEnergy(-victim.MaxEnergy / 1000);

			if (!victim)
			{
				Log("Apparently the victim got killed, this should not happen with the rule");
			}

			doTest("Energy is %d, should be %d", victim->GetEnergy(), 0);
			doTest("Energy is actually %d, should be %d", victim->Call(Global.GetEnergy), 1);
			doTest("GetAlive returns %v, should be %v", victim->GetAlive(), false);
			doTest("GetOCF & OCF_Alive returns %d, should be %d", victim->GetOCF() & OCF_Alive, 0);
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			Log("Heal a little, healing has no effect, clonk still counts as incapacitated");

			victim->DoEnergy(4);

			doTest("Energy is %d, should be %d", victim->GetEnergy(), 0);
			doTest("Energy is actually %d, should be %d", victim->Call(Global.GetEnergy), 1);
			doTest("GetAlive returns %v, should be %v", victim->GetAlive(), false);
			doTest("GetOCF & OCF_Alive returns %d, should be %d", victim->GetOCF() & OCF_Alive, 0);
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			Log("Kill() call still kills the victim");

			CurrentTest().test2_killed = true;
			victim->Kill();

			return Wait(30);
		}
		else
		{
			return FailTest(); // Test not implemented
		}
	}
}
global func Test2_OnClonkDeath(object clonk, int killer)
{
	if (CurrentTest().test2_killed)
	{
		ScheduleCall(clonk, Global.RemoveObject, 25, 1);
	}
	else // Unexpected, remove immediately.
	{
		clonk->RemoveObject();
	}
}


//--------------------------------------------------------

global func Test3_OnStart(int player){ return InitTest();}
global func Test3_OnFinished(){ return; }
global func Test3_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test3_incapacitated)
	{
		if (CurrentTest().test3_killed)
		{
			doTest("Victim died at frame %d, expected %d", CurrentTest().test3_killed, CurrentTest().test3_expected);
			return Evaluate();
		}
		else if (FrameCounter() > CurrentTest().test3_timeout)
		{
			return FailTest();
		}
		return Wait(10);
	}
	else
	{
		Log("Test that clonks die after 15 seconds");
		if (victim)
		{	
			victim->DoEnergy(-victim.MaxEnergy / 1000);

			if (!victim)
			{
				Log("Apparently the victim got killed, this should not happen with the rule");
			}
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			CurrentTest().test3_incapacitated = FrameCounter();
			CurrentTest().test3_timeout = Death_Timeout() + CurrentTest().test3_incapacitated;
			CurrentTest().test3_expected = Death_Expected() + CurrentTest().test3_incapacitated;

			return Wait(10);
		}
		else
		{
			return FailTest(); // Test not implemented
		}
	}
}
global func Test3_OnClonkDeath(object clonk, int killer)
{
	CurrentTest().test3_killed = FrameCounter();
}


//--------------------------------------------------------

global func Test4_OnStart(int player){ return InitTest();}
global func Test4_OnFinished(){ return; }
global func Test4_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test4_incapacitated)
	{
		if (CurrentTest().test4_killed)
		{
			return FailTest();
		}
		else if (FrameCounter() > CurrentTest().test4_timeout)
		{
			doTest("Energy is %d, should be %d", victim->GetEnergy(), 1);
			doTest("Energy is actually %d, should be %d", victim->Call(Global.GetEnergy), 1);
			doTest("GetAlive returns %v, should be %v", victim->GetAlive(), true);
			doTest("GetOCF & OCF_Alive returns %d, should be %d", victim->GetOCF() & OCF_Alive, OCF_Alive);
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), false);
			return Evaluate();
		}

		return Wait(10);
	}
	else
	{
		Log("Test that clonks can be reanimated until the timeout is exceeded");
		if (victim)
		{	
			victim->DoEnergy(-victim.MaxEnergy / 1000);

			if (!victim)
			{
				Log("Apparently the victim got killed, this should not happen with the rule");
			}
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			CurrentTest().test4_incapacitated = FrameCounter();
			CurrentTest().test4_timeout = Death_Timeout() + CurrentTest().test4_incapacitated;
			CurrentTest().test4_expected = Death_Expected() + CurrentTest().test4_incapacitated;

			// Resurrect 1 frame before he dies
			ScheduleCall(victim, victim.DoReanimate, Death_Expected() - 1, 1);

			return Wait(10);
		}
		else
		{
			return FailTest(); // Test not implemented
		}
	}
}
global func Test4_OnClonkDeath(object clonk, int killer)
{
	CurrentTest().test4_killed = FrameCounter();
}


//--------------------------------------------------------

global func Test5_OnStart(int player){ return InitTest();}
global func Test5_OnFinished(){ return; }
global func Test5_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test5_incapacitated)
	{
		if (CurrentTest().test5_killed)
		{
			doTest("Victim died at frame %d, expected %d", CurrentTest().test5_killed, CurrentTest().test5_expected);
			return Evaluate();
		}
		else if (FrameCounter() > CurrentTest().test5_timeout)
		{
			return FailTest();
		}
		return Wait(10);
	}
	else
	{
		Log("Test that clonk death delay is regenerated");
		Log("Reanimate after 8 seconds (7s left to live), kill again after 9 seconds (8s left to live)");
		if (victim)
		{	
			victim->DoEnergy(-victim.MaxEnergy / 1000);

			if (!victim)
			{
				Log("Apparently the victim got killed, this should not happen with the rule");
			}
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			CurrentTest().test5_incapacitated = FrameCounter();
			var second_life = 8 * 35;
			var second_death = 9 * 35; // 35 frames to recharge the interval
			CurrentTest().test5_expected =  second_death + (Death_Expected() - second_life + 35) + CurrentTest().test5_incapacitated;
			CurrentTest().test5_timeout = CurrentTest().test5_expected + 35;

			// Resurrect before he dies, kill again
			ScheduleCall(victim, victim.DoReanimate, second_life, 1);
			ScheduleCall(victim, victim.DoEnergy, second_death, 1, - victim.MaxEnergy / 1000);

			return Wait(10);
		}
		else
		{
			return FailTest(); // Test not implemented
		}
	}
}
global func Test5_OnClonkDeath(object clonk, int killer)
{
	CurrentTest().test5_killed = FrameCounter();
}


//--------------------------------------------------------

global func Test6_OnStart(int player){ return InitTest();}
global func Test6_OnFinished(){ return; }
global func Test6_Execute()
{

	var victim = GetCrew(player_victim);
	if (CurrentTest().test6_incapacitated)
	{
		if (CurrentTest().test6_killed)
		{
			doTest("Victim died at frame %d, expected %d", CurrentTest().test6_killed, CurrentTest().test6_expected);
			return Evaluate();
		}
		else if (FrameCounter() > CurrentTest().test6_timeout)
		{
			return FailTest();
		}
		return Wait(10);
	}
	else
	{
		Log("Test that clonk death has a minimum time");
		Log("Reanimate after 14 seconds (1 second left to live), kill again immediately afterwards (5 seconds left to live)");
		if (victim)
		{	
			victim->DoEnergy(-victim.MaxEnergy / 1000);

			if (!victim)
			{
				Log("Apparently the victim got killed, this should not happen with the rule");
			}
			doTest("IsIncapacitated() returns %v, should return %v", victim->IsIncapacitated(), true);

			CurrentTest().test6_incapacitated = FrameCounter();
			var second_life = 14 * 35;
			var second_death = second_life + 1;
			CurrentTest().test6_expected =  second_death + Death_MinDelay() + CurrentTest().test6_incapacitated - 1; // Is one frame less, but that does not matter
			CurrentTest().test6_timeout = CurrentTest().test6_expected + 35;

			// Resurrect before he dies, kill again
			ScheduleCall(victim, victim.DoReanimate, second_life, 1);
			ScheduleCall(victim, victim.DoEnergy, second_death, 1, - victim.MaxEnergy / 1000);

			return Wait(10);
		}
		else
		{
			return FailTest(); // Test not implemented
		}
	}
}
global func Test6_OnClonkDeath(object clonk, int killer)
{
	CurrentTest().test6_killed = FrameCounter();
}

