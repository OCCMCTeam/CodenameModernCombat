/**
	Some math functions

	@author Marky
 */

global func InterpolateRGBa(int progress, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, interpolation)
{
	interpolation = interpolation ?? Global.InterpolateLinear;

	var y0_split = SplitRGBaValue(y0);
	var y1_split = SplitRGBaValue(y1);
	var y2_split = SplitRGBaValue(y2);
	var y3_split = SplitRGBaValue(y3);

	var r = Call(interpolation, progress, x0, y0_split.R, x1, y1_split.R, x2, y2_split.R, x3, y3_split.R);
	var g = Call(interpolation, progress, x0, y0_split.G, x1, y1_split.G, x2, y2_split.G, x3, y3_split.G);
	var b = Call(interpolation, progress, x0, y0_split.B, x1, y1_split.B, x2, y2_split.B, x3, y3_split.B);
	var a = Call(interpolation, progress, x0, y0_split.Alpha, x1, y1_split.Alpha, x2, y2_split.Alpha, x3, y3_split.Alpha);

	return RGBa(r, g, b, a);
}


global func InterpolateLinear(int progress, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3)
{
	progress = BoundBy(progress, Min(Min(x0, x1), Min(x2, x3)), Max(Max(x0, x1), Max(x2, x3)));

	if (progress > x1 && x2 || x3)
	{
		if (x2 <= progress && progress <= x3)
		{
			return InterpolateLinear(progress, x2, y2, x3, y3);
		}
		else if (x1 <= progress && progress <= x2)
		{
			return InterpolateLinear(progress, x1, y1, x2, y2);
		}
	}
	else
	{
		var interval = x1 - x0;
		var factor = BoundBy(progress - x0, 0, interval);

		return (factor * y1 + (interval - factor) * y0) / Max(1, interval);
	}
}


global func GetPathFreeAngles(int radius, int steps)
{
	AssertObjectContext();
	steps = steps ?? 5;
	var angles = [];
	for (var angle = -180; angle < +180; angle += steps)
	{
		var x = +Sin(angle, radius);
		var y = -Cos(angle, radius);
		if (PathFree(GetX(), GetY(), GetX() + x, GetY() + y))
		{
			PushBack(angles, angle);
		}
	}
	return angles;
}


global func GetAverage(array values)
{
	var length = GetLength(values);
	if (length > 0)
	{
		var count = 0;
		for (var value in values)
		{
			count += value;
		}
		return count / length;
	}
	return nil;
}


global func ProjectileDeviationCmc(int amount)
{
	return amount * 50;
}


// Sets the 'value' value of the color. 
// @par rgba The color
// @par v The 'value' value. 
global func SetRGBaByV(int rgba, int v)
{
	v = v ?? 255;
	var color = SplitRGBaValue(rgba);
	var max = Max([color.R, color.G, color.B]);
	return RGBa(color.R * v / max, color.G * v / max, color.B * v / max, color.A);
}


// Sets the 'lightness' value of the color. 
// @par rgba The color
// @par change The change in the 'lightness' value. 
global func RGBaDoLightness(int rgba, int change)
{
	var alpha = GetRGBaValue(rgba, RGBA_ALPHA);
	var color = RGB2HSL(rgba);
	var lightness = GetRGBaValue(color, RGBA_BLUE);
	change = BoundBy(change, -lightness, RGBA_MAX - lightness);
	color = SplitRGBaValue(HSL2RGB(DoRGBaValue(color, change, RGBA_BLUE)));
	return RGBa(color.R, color.G, color.B, alpha);
}

