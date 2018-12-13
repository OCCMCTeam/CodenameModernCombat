/**
	Unit test for the kill tracing mechanic
 */

static player_victim;
static player_killer;
static player_killer_fake;

func Initialize()
{
	// Disable delayed death for now
	CreateObject(CMC_Rule_MortalWounds);

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
		return Global[Format("Test%d_Setup", this.testnr)];
	},

	ExecuteTest = func ()
	{		
		if (this.setup > 0)
		{
			// Timeout? in seconds
			var runtime = FrameCounter() - this.setup;
			if (runtime > ((this.timeout ?? 10) * 35))
			{
				return FailTest();
			}

			// Victim died
			if (GetCrew(player_victim) == nil)
			{
				return Evaluate();
			}
		}
		else
		{
			InitTest();

			var output = Call(Format("~Test%d_Log", this.testnr));
			if (output)
			{
				Log(output);
			}

			var victim = GetCrew(player_victim);
			var killer = GetCrew(player_killer);
			var fake_killer = GetCrew(player_killer_fake);
			Call(Format("Test%d_Setup", this.testnr), victim, killer, fake_killer);

			this.setup = FrameCounter();
		}
		return Wait();
	},


	CleanupTest = func ()
	{
		this.setup = 0;
		this.timeout = nil;
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

	ResetHostility();
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


global func Test_WeaponSingleCall(string call, object weapon, int delay, object user, int x, int y)
{
	ScheduleCall(weapon, Format("Control%sStart", call), delay + 0, 1, user, x, y);
	ScheduleCall(weapon, Format("Control%sHolding", call), delay + 1, 1, user, x, y);
	ScheduleCall(weapon, Format("Control%sStop", call), delay + 2, 1, user, x, y);
}

global func ResetHostility()
{
	SetPlayersAllied(player_victim, player_killer);
	SetPlayersAllied(player_victim, player_killer_fake);
	SetPlayersAllied(player_killer, player_killer_fake);
}

global func SetPlayersAllied(int a, int b)
{
	SetHostility(a, b, false, true);
	SetHostility(b, a, false, true);
}

global func SetPlayersHostile(int a, int b)
{
	SetHostility(a, b, true, true);
	SetHostility(b, a, true, true);
}

/* --- Tests --- */

global func Test1_Log() { return "K throws an activated iron bomb at V (reference test case)"; }
global func Test1_Setup(object victim, object killer, object fake_killer)
{
	victim->SetPosition(145, 150);
	victim->DoEnergy(10 - victim->GetMaxEnergy());

	var bomb = killer->CreateContents(IronBomb);
	bomb->ControlUse(killer);
	killer->SetHandAction(0);
	killer->ControlThrow(bomb, 20, -20);
}


global func Test2_Log() { return "K throws an activated field grenade at V"; }
global func Test2_Setup(object victim, object killer, object fake_killer)
{
	victim->SetPosition(240, 150);
	victim->DoEnergy(35 - victim->GetEnergy());

	var grenade = killer->CreateContents(CMC_Grenade_Field);
	grenade->ControlUseStart(killer, +50, -50);
	grenade->ControlUseStop(killer, +50, -50);
}


global func Test3_Log() { return "K throws an activated frag grenade at V"; }
global func Test3_Setup(object victim, object killer, object fake_killer)
{
	victim->SetPosition(280, 150);
	victim->DoEnergy(85 - victim->GetEnergy());

	var grenade = killer->CreateContents(CMC_Grenade_Frag);
	grenade->ControlUseStart(killer, +50, -50);
	grenade->ControlUseStop(killer, +50, -50);
}

global func Test4_Log() { return "K fires missile at V"; }
global func Test4_Setup(object victim, object killer, object fake_killer)
{
	victim->SetPosition(280, 150);
	victim->DoEnergy(85 - victim->GetEnergy());

	var weapon = killer->CreateContents(CMC_Weapon_RocketLauncher);
	weapon->DoAmmo(CMC_Ammo_Missiles, 1);

	var aim_x = victim->GetX() - killer->GetX();
	Test_WeaponSingleCall("UseAlt", weapon, 1, killer, aim_x, 0);
	Test_WeaponSingleCall("Use", weapon, 50, killer, aim_x, 0);
}

global func Test5_Log() { return "K fires guided missile at V, with tracer from F"; }
global func Test5_Setup(object victim, object killer, object fake_killer)
{
	// Victim must be hostile, so that tracer sticks
	SetPlayersHostile(player_victim, player_killer);
	SetPlayersHostile(player_victim, player_killer_fake);

	// Get in position!
	victim->SetPosition(480, 150);
	victim->DoEnergy(85 - victim->GetEnergy());

	fake_killer->SetPosition(180, 150);
	fake_killer->DoEnergy(85 - victim->GetEnergy());

	// Create launcher and tracer
	var weapon = killer->CreateContents(CMC_Weapon_RocketLauncher);
	weapon->DoAmmo(CMC_Ammo_Missiles, 1);

	var tracer = fake_killer->CreateContents(CMC_Weapon_Pistol);
	tracer->DoAmmo(CMC_Ammo_Bullets, 1);
	tracer->SetFiremode(1, true);

	// Let fake killer fire a tracer at the victim
	var f_aim_x = victim->GetX() - fake_killer->GetX();
	Test_WeaponSingleCall("Use", tracer, 10, fake_killer, f_aim_x, 0);

	// Let killer fire the missile into the air, than do an uplink to the tracer
	var k_aim_x = 100;
	var k_aim_y = -50;

	Test_WeaponSingleCall("UseAlt", weapon, 1, killer, k_aim_x, k_aim_y); // Start aiming
	Test_WeaponSingleCall("Use", weapon, 20, killer, k_aim_x, k_aim_y);   // Fire high
	Test_WeaponSingleCall("Use", weapon, 55, killer, k_aim_x, k_aim_y);   // Confirm uplink

	// Let the victim run to the left a little
	ScheduleCall(victim, victim.SetComDir, 60, 1, COMD_Left); 

}
