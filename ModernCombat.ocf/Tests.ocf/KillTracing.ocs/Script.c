/**
	Unit test for the kill tracing mechanic
 */

static player_victim;
static player_killer;
static player_killer_fake;

func Initialize()
{
	// Create script players for these tests.
	CreateScriptPlayer("Victim",     RGB(0, 0, 255), nil, CSPF_NoEliminationCheck);
	CreateScriptPlayer("Killer",     RGB(0, 255, 0), nil, CSPF_NoEliminationCheck);
	CreateScriptPlayer("KillerFake", RGB(255, 0, 0), nil, CSPF_NoEliminationCheck);
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
		else if (GetPlayerName(player) == "Killer")
		{
			player_killer = player;
			GetCrew(player)->MakeInvincible(true);
			GetCrew(player)->SetPosition(50, 150);
		}
	 	else if (GetPlayerName(player) == "KillerFake")
	 	{
			player_killer_fake = player;
			GetCrew(player)->MakeInvincible(true);
			GetCrew(player)->SetPosition(20, 150);
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

// Aborts the current test and launches the specified test instead.
global func LaunchTest(int nr)
{
	// Get the control test.
	var test = CurrentTest();
	if (test)
	{
		// Finish the currently running test.
		Call(Format("~OnTestFinished", test.testnr));
		// Start the requested test by just setting the test number and setting 
		// test.launched to false, effect will handle the rest.
	}
	else
	{
		// Create a new control effect and launch the test.
		test = Scenario->CreateEffect(IntKillTraceTestControl, 100, 2);
		test.player = GetPlayerByIndex(0, C4PT_User);
		test.global_result = true;
		test.count_total = 0;
		test.count_failed = 0;
		test.count_skipped = 0;
	}

	test.testnr = nr;
	test.launched = false;
}

static const IntKillTraceTestControl = new IntTestControl
{
	HasNextTest = func ()
	{
		return Global[Format("Test%d_Execute", this.testnr)];
	},
	
	ExecuteTest = func ()
	{
		if (!this.setup)
		{
			InitTest();
			Call(Format("Test%d_Log", this.testnr));
		}
		var victim = GetCrew(player_victim);
		var killer = GetCrew(player_killer);
		var fake_killer = GetCrew(player_killer_fake);
		return Call(Format("Test%d_Execute", this.testnr), victim, killer, fake_killer);
	},
	

	CleanupTest = func ()
	{
		this.setup = false;
	},
	
	OnDeath = func (int killer, object clonk)
	{
		// Store the result.
		doTest(Format("Kill trace determined %s, expected %s", Format("%s (%s)", "%d", GetPlayerName(killer)), 
		                                                       Format("%s (%s)", "%d", GetPlayerName(player_killer))), 
		                                                       killer, player_killer);
		return FX_OK;
	}
};

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
	for (var player in [player_victim, player_killer, player_killer_fake])
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
	GetCrew(player_killer)->SetPosition(50, 150);
	GetCrew(player_killer_fake)->SetPosition(20, 150);
	GetCrew(player_victim)->CreateEffect(IntLogEnergyChange, 1, 1);
	return;
}

static const IntLogEnergyChange = new Effect 
{
	Timer = func ()
	{
		var now = this.Target->GetEnergy();
		if (this.then == nil)
		{
			this.then = now;
		}
		if (now != this.then)
		{
			var change = now - this.then;
			this.then = now;
			Log("V energy change: %d", change);
		}
		return FX_OK;
	},
};

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
	EffectCall(nil, test, "OnDeath", killer, clonk);
	return FX_OK;
}


global func CurrentTest()
{
	return GetEffect("IntKillTraceTestControl", Scenario);
}

/* --- Tests --- */

global func Test1_Log() { return "K throws an activated iron bomb at V (reference test case)"; }
global func Test1_Execute(object victim, object killer, object fake_killer)
{
	var test = CurrentTest();
	if (test.setup)
	{
		if (victim == nil)
		{
			return Evaluate();
		}
	}
	else
	{
		victim->SetPosition(145, 150);
		victim->DoEnergy(10 - victim->GetMaxEnergy());
			
		var bomb = killer->CreateContents(IronBomb);
		bomb->ControlUse(killer);
		killer->SetHandAction(0);
		killer->ControlThrow(bomb, 20, -20);
		
		test.setup = true;
	}
	return Wait();
}

global func Test2_Log() { return "K throws an activated grenade at V"; }
global func Test2_Execute(object victim, object killer, object fake_killer)
{
	var test = CurrentTest();
	if (test.setup)
	{
		if (victim == nil)
		{
			return Evaluate();
		}
	}
	else
	{
		victim->SetPosition(125, 150);
		victim->DoEnergy(-35);
	
		var grenade = killer->CreateContents(CMC_Grenade_Field);
		grenade->ControlUseStart(killer, +50, -50);
		grenade->ControlUseStop(killer, +50, -50);
		
		test.setup = true;
	}
	return Wait();
}
