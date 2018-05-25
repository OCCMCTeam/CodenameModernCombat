#include CMC_Library_Grenade

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";

local ContactIncinerate = 1;
local NoBurnDecay = 1;
local Collectible = true;
/*-- Blendgranate --*/

/* --- Properties --- */

local Grenade_SmokeColor = 0xff00ccff;
local Grenade_ContainedDamage = 20;
local Grenade_Radius = 250;

/* --- Sounds --- */

func PlaySoundActivate()
{
	Sound("Grenade::StunGrenade::Activate");
}

func PlaySoundDetonation()
{
	Sound("Grenade::StunGrenade::Explosion");
}

func PlaySoundHit()
{
	Sound("Grenade::StunGrenade::Hit?", {multiple = true});
}

func PlaySoundFlashbang(object target)
{
	var for_player = target->GetOwner();
	target->Sound("Grenade::StunGrenade::Bang?", { player = for_player});
	target->Sound("Grenade::StunGrenade::EarRinging?", { player = for_player});
}

/* --- Detonation --- */


func OnDetonation()
{
	var full_range = this.Grenade_Radius;
	var short_range = full_range / 5;

	var targets = FindObjects(Find_OCF(OCF_CrewMember), Find_Func("IsClonk"), Find_Distance(full_range));
	for (var target in targets)
	{
		// Targets in helicopters can be blinded (TODO: Replace this by better criteria!)
		var container = target->Contained();
		if (container && !container->~IsHelicopter())
		{
			continue;
		}

		var distance = ObjectDistance(target);

		// Objects in the immediate area are always blinded, others only with a free path
		if (distance > short_range && !PathFree(GetX(), GetY(), target->GetX(), target->GetY() - 4)) // TODO: Should be replaced by some kind of "HeadY" callback, because for crouching clonks the position might be different, or a "can see" callback in the Clonk
		{
			continue;
		}

		// Now blind them, determine how much; allies are blinded half as much only
		var intensity = (400 - distance) * 300 / full_range; // TODO: The numbers behind the calculation were taken from the previous implementation, no idea what they mean
		if (!Hostile(GetController(), target->GetOwner()))
		{
			intensity /= 2;
		}

		// Blind only if there is intensity
		if (intensity < 0)
		{
			continue;
		}

		// Blinding effect
		var flash = GetEffect("BlindedByFlashbang", target);
		if (flash)			
		{
			flash->AddIntensity(intensity); //       EffectVar(0, obj, effect) += (100-Min((EffectVar(0, obj, effect)*100/300)/2, 50))*intensity/100;
		}
		else
		{
			target->CreateEffect(BlindedByFlashbang, 100, 1, intensity);
		}

		// Additional sound effects
		if (intensity > 38)
		{
			if (!GetEffect("SoundDelay", target)) // TODO: Is this a global system for blocking sounds?
			{
				AddEffect("SoundDelay", this, 1, 25, target);
				target->~PlaySoundSmallPain(); // TODO
			}
			PlaySoundFlashbang(target);
		}
	}
}

/* --- Blinding effect --- */

local BlindedByFlashbang = new Effect
{
	Start = func (int temp, int intensity)
	{
		if (!temp)
		{
			if (intensity <= 0)
			{
				return FX_Execute_Kill;
			}

			this.Intensity = intensity;
		}
	},

	Timer = func (int time)
	{
		if (!DecreaseIntensity())
		{
			return FX_Execute_Kill;
		}

		UpdateScreenEffect();
	},

	DecreaseIntensity = func ()
	{
		this.Intensity = Min(850, this.Intensity - 2);

		return this.Intensity > 0;
	},

	UpdateScreenEffect = func ()
	{
		// TODO
	},
};

/*
public func FxIntFlashbangTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
  //Kein Blendobjekt vorhanden: Neues erstellen
  var flash = EffectVar(1,pTarget,iEffectNumber);
  if (!flash)
  {
    flash = ScreenRGB(pTarget,RGB(255,255,255), 0, 0, false, SR4K_LayerLight);
    EffectVar(1,pTarget,iEffectNumber) = flash;
  }


  //Blendicon-Konfiguration für jeden Spieler getrennt
  var inum;
  for (var i = 0; i < GetPlayerCount(); i++)
  {
    //Spieler, der den geblendeten Clonk steuert, auslassen
    if (i == GetOwner(pTarget))
      continue;

    //Geblendeter Clonk befindet sich in Rauch: Keine Icons setzen
    var srgb = GetScreenRGB(GetPlayerByIndex(GetOwner(pTarget)), SR4K_LayerSmoke, pTarget);
    if (srgb && srgb->GetAlpha() < 200)
    {
      Message("@", pTarget, GetPlayerByIndex(i));
      continue;
    }

    //Spieler hat keinen Clonk oder dieser nicht sein Besitz: Abbruch
    var pCursor = GetCursor(GetPlayerByIndex(i));
    if (!pCursor)
    {
      Message("@", pTarget, GetPlayerByIndex(i));
      continue;
    }

    pCursor = pCursor->~GetRealCursor();
    if (!pCursor)
      pCursor = GetCursor(GetPlayerByIndex(i));

    //Clonk des Spielers verschachtelt und kein Pilot: Abbruch
    if (Contained(pCursor) && !(Contained(pCursor)->~GetPilot() == pCursor))
    {
      Message("@", pTarget, GetPlayerByIndex(i));
      continue;
    }

    //Clonk des Spielers ist durch Blendgranate geblendet: Abbruch
    var srgb = GetScreenRGB(GetPlayerByIndex(i), SR4K_LayerLight, pCursor);
    if (srgb && srgb->GetAlpha() < 40)
      continue;

    //Clonk des Spielers ist durch Rauch geblendet: Abbruch
    srgb = GetScreenRGB(GetPlayerByIndex(i), SR4K_LayerSmoke, pCursor);
    if (srgb && srgb->GetAlpha() < 200)
    {
      Message("@", pTarget, GetPlayerByIndex(i));
      continue;
    }

    //Bei mehreren Icons vorhergehende nicht entfernen
    var flag = 0;
    if (inum != 0)
      flag = MSG_Multiple;

    //Ansonsten Icon setzen
    if (!Contained(pTarget))
    {
      //Geblendeter im Freien: Icon gibt Blendungsgrad wieder
      if (intensity > 220)
        CustomMessage(Format("<c %x>{{SM28}}</c>", RGBa(255,255,255,BoundBy(a, 1, 254))), pTarget, GetPlayerByIndex(i), 0, 0, 0, 0, 0, flag);
      else
        CustomMessage(Format("<c %x>{{SM07}}</c>", RGBa(255,255,255,BoundBy(a, 1, 254))), pTarget, GetPlayerByIndex(i), 0, 0, 0, 0, 0, flag);
    }
    else
      //Ansonsten nur allgemeines Icon anzeigen
      CustomMessage("{{SM07}}", pTarget, GetPlayerByIndex(i), 0, 0, 0, 0, 0, flag);
    inum++;
  }
}
*/

