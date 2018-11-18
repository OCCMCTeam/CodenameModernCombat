/**
	Gets material properties for sound and impact effects.
 */


global func GetMaterialProperties(int x, int y)
{
	var props = {};

	var material = GetMaterial(x, y);
	var texture = GetTexture(x, y);
	props.material = material;
	props.texture = texture;
	props.in_liquid = GBackLiquid(x, y);
	props.color = SplitRGBaValue(GetAverageTextureColor(texture));

	var is_solid = GBackSolid(x, y);
	var is_soft = GetMaterialVal("DigFree" , "Material", material)
	           || GetMaterialVal("Soil"    , "Material", material)
	           || GetMaterialVal("Instable", "Material", material);
	           
	var is_metal = WildcardMatch(texture, "*metal*")
	            || WildcardMatch(texture, "*plate*");

	if (is_solid)
	{
		props.impact = true;
		if (is_metal)
		{
			props.sound_category = "Metal";
			props.dust_factor = 0;
			props.spark_factor = 2;
		}
		else if (is_soft)
		{
			props.sound_category = "Soft";
			props.dust_factor = 2;
			props.spark_factor = 0;
		}
		else
		{
			props.sound_category = "Hard";
			props.dust_factor = 1;
			props.spark_factor = 1;
		}

		var particle = GetMaterialVal("PXSGfx", "Material", material);
		var shape = GetMaterialVal("Shape", "Material", material);
		props.shape = shape;
		if (particle != "")
		{
			props.particle_type = particle;
		}
		else if (shape <= 1) // Flat , Octagon, FlatTop
		{
			props.particle_type = "DebrisFlat";
			props.particle_phase = PV_Random(0, 3);
		}
		else if (shape == 2) // Smooth
		{
			props.particle_type = "DebrisSmooth";
		}
		else // Rough
		{
			props.particle_type = "DebrisRough";
			props.particle_phase = PV_Random(0, 3);
		}
	}
	
	return props;
}
