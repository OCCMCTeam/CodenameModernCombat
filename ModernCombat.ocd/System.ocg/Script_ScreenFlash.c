/**
	Fade screen alpha effect.

	Can stack multiple 
*/

static const FxFlashScreenRGBa = new Effect 
{
	Effect = func (string new_effect_name, string layer, int color, int alpha_add, int fade_time)
	{
		if (("FxFlashScreenRGBa" == new_effect_name)
	    &&  (this.ColorLayer == layer))
		{
	    	if (this.Color == color)
	    	{
				AddAlphaTrack(this.Time, alpha_add, fade_time);
			}
			return -1;
		}
	},

	Start = func (int temporary, string layer, int color, int alpha_add, int fade_time)
	{
		if (!temporary)
		{
			var HUD = Target->~GetHUDController();
			if (HUD && GetType(HUD.GetColorLayer) == C4V_Function)
			{
				this.Color = SplitRGBaValue(color);
				this.ColorLayer = layer;
				this.AlphaMax = 250;
				this.AlphaMin = 0;
				this.AlphaTracks = [];
				AddAlphaTrack(0, alpha_add, fade_time);
			}
			else
			{
				return -1;
			}
		}
	},

	Timer = func (int time)
	{
		var overlay = this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer);

		var current_alpha = 0;
		for (var track in this.AlphaTracks)
		{
			current_alpha += InterpolateLinear(time - track.T0, 0, track.Alpha, track.Length, 0);
		}
		current_alpha = BoundBy(current_alpha, this.AlphaMin, this.AlphaMax);

		overlay->Update({BackgroundColor = RGBa(this.Color.R, this.Color.G, this.Color.B, current_alpha)});

		if (current_alpha == this.AlphaMin)
		{
			return FX_Execute_Kill;
		}
	},

	Stop = func (int temp)
	{
		if (!temp && this.Target)
		{
			this.Target->GetHUDController()->GetColorLayer(this.Target, this.ColorLayer)->Update({BackgroundColor = nil});
		}
	},

	AddAlphaTrack = func (int time_offset, int alpha_add, int fade_time)
	{
		var track = { T0 = time_offset, Alpha = alpha_add, Length = fade_time};
		PushBack(this.AlphaTracks, track);
	},
};
