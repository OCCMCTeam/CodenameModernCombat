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
		flash->AddIntensity(intensity);
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
		if (!temp && this.Target && HasColorOverlay())
		{
			this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer)->Update({BackgroundColor = nil});
		}
	},

	AddIntensity = func (int additional)
	{
		var subtract = Min(this.Intensity / 6, 50);
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
		if (HasColorOverlay())
		{
			var overlay = this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer);
			overlay->Update({BackgroundColor = RGBa(255, 255, 255, BoundBy(this.Intensity, 0, 255)), });
		}
	},

	UpdateStatusIcons = func ()
	{
		// Distinction between contained and non-contained is not made until I know the original reason; Will most likely come up again once we add vehicles

		var graphics = nil;
		if (this.Intensity > 220)
		{
			graphics = "Full";
		}

		var symbol_helper = this.Target->ShowStatusSymbol(CMC_Icon_Stunned);
		symbol_helper->SetSymbolGraphics(CMC_Icon_Stunned, graphics)
		             ->SetSymbolColor(CMC_Icon_Stunned, nil, nil, nil, BoundBy(this.Intensity, 0, 255));
	},

	HasColorOverlay = func ()
	{
		return this.Target->GetHUDController() && this.Target->GetHUDController().GetColorLayer;
	}
};

