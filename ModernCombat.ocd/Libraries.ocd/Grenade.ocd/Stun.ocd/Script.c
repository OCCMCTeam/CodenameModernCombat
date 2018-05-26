/**
	Include this in objects that are affected by stun grenades.
	
	The default implementation is suited for Clonks, but can be expanded or overriden
	for vehicles, structures, etc.

	@author Marky
 */

func IsAffectedByStunGrenade(object grenade, int full_range, int short_range)
{
	return !Contained()
	    && GetAlive()
	    // In short range, or has a clear view on the grenade / flash
	    && (ObjectDistance(grenade) <= short_range
	    || !PathFree(GetX(), GetY() - 4, grenade->GetX(), grenade->GetY() - 4));
}


func BlindedByStunGrenade(object grenade, int intensity)
{
	// Blinding effect
	var flash = GetEffect("BlindedByFlash", this);
	if (flash)			
	{
		flash->AddIntensity(intensity); //       EffectVar(0, obj, effect) += (100-Min((EffectVar(0, obj, effect)*100/300)/2, 50))*intensity/100;
	}
	else
	{
		CreateEffect(BlindedByFlash, 100, 1, intensity);
	}
	
	this->~OnBlindedByStunGrenade(grenade, intensity);
}


func OnBlindedByStunGrenade(object grenade, int intensity) // Overloadable callback, because a helicopter for example would not play the ear ringing sound
{
	// Additional sound effects
	if (intensity > 38)
	{
		if (!GetEffect("SoundDelay")) // TODO: Is this a global system for blocking sounds?
		{
			AddEffect("SoundDelay", this, 1, 25);
			this->~PlaySoundSmallPain(); // TODO
		}
		this->~PlaySoundFlashbang();
	}
}


/* --- Blinding effect --- */


local BlindedByFlash = new Effect
{
	Start = func (int temp, int intensity)
	{
		if (!temp)
		{
			if (intensity <= 0)
			{
				return FX_Execute_Kill;
			}

			this.ColorLayer = Format("%v", CMC_Grenade_Stun);
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
		UpdateStatusIcons();
	},

	Stop = func (int temp)
	{
		if (!temp && this.Target)
		{
			this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer)->Update({BackgroundColor = nil});
		}
	},
	
	AddIntensity = func (int additional)
	{
		var subtract = Min((this.Intensity * 100 / 300) / 2, 50);
		var percentage = (100 - subtract);
		this.Intensity += percentage * additional / 100;
	},

	DecreaseIntensity = func ()
	{
		this.Intensity = Min(850, this.Intensity - 2);

		return this.Intensity > 0;
	},

	UpdateScreenEffect = func ()
	{
		var overlay = this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer);
		overlay->Update({BackgroundColor = RGBa(255, 255, 255, BoundBy(this.Intensity, 0, 255)), });
	},
	
	UpdateStatusIcons = func ()
	{
	},
};




/*
public func FxIntFlashbangTimer(object pTarget, int iEffectNumber, int iEffectTime)
{
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
