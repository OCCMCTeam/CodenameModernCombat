#include CMC_Cursor_Default

local Cursor_Angle = 0;

func InitGraphics()
{
	SetGraphics("Bar", GetID(), CNAT_Left, GFXOV_MODE_ExtraGraphics);
	SetGraphics("Bar", GetID(), CNAT_Right, GFXOV_MODE_ExtraGraphics);
}

func UpdateAimPosition(int x, int y)
{
	// Save aiming angle, because the bars need to be rotated this much
	Cursor_Angle = Angle(0, 0, x, y, 1000);
	inherited(x, y);
}

func UpdateGraphics(int distance)
{
	UpdateCursorBar(Cursor_Distance, Cursor_Angle, -Cursor_Spread, CNAT_Left);
	UpdateCursorBar(Cursor_Distance, Cursor_Angle, +Cursor_Spread, CNAT_Right);
}

// Updates a bar the way a cursor would handle:
// - Bars point towards the cursor position
func UpdateCursorBar(int distance, int angle, spread, int overlay)
{
	// Defaults
	distance = distance ?? Cursor_Distance;
	// FIXME: Has to take the firearm muzzle position into account - this could replace the bottom margin, by the way
	// Currently you will see that it goes to the center of the clonk, but the spread is actually from the muzzle

	var full_size = 1000;
	var half_size = 500;
	var precision = 1000;
	
	var min_length = 30;    // Minimal length for the bar. If the cursor gets nearer, the bar fades out
	var max_length = 100;   // Maximal length for the bar.
	
	var margin_top = 10;    // Margin from the top of the cone graphics to the crosshair
	var margin_bottom = 10; // Margin from the bottom of the cone graphics to the user
	
	// Determine offset position
	var cursor_top    = Max(distance - margin_top, min_length);
	var cursor_bottom = Max(margin_bottom, cursor_top - max_length);
	var cursor_length = cursor_top - cursor_bottom;
	var cursor_center = cursor_top + cursor_bottom;

	// Determine parameters for transformation
	var relative_length = full_size * cursor_length / GetDefHeight();
	var sin = Sin(angle + spread, full_size, precision);
	var cos = Cos(angle + spread, full_size, precision);

	var dist_side = Sin(spread, cursor_center * half_size, precision);
	var xoff_side = Cos(angle, dist_side, precision);
	var yoff_side = Sin(angle, dist_side, precision);
	var xoff_center = +Sin(angle, cursor_center * half_size, precision) - Sin(angle, distance * full_size, precision);
	var yoff_center = -Cos(angle, cursor_center * half_size, precision) + Cos(angle, distance * full_size, precision);
	
	// Draw it!
	SetObjDrawTransform
	(
		+cos, -sin * relative_length / full_size, xoff_center + xoff_side,
        +sin, +cos * relative_length / full_size, yoff_center + yoff_side,
        overlay
	);
	
	// Update alpha if the cone is too near.
	SetClrModulation(RGBa(255, 255, 255, BoundBy(distance * 128 / min_length, 0, 255)), overlay);
}

// Updates a bar the way a cone would handle:
// - Bars emit from the origin
func UpdateConeBar(int distance, int angle, spread, int overlay, int margin_top, int margin_bottom)
{
	// Defaults
	distance = distance ?? Cursor_Distance;

	var full_size = 1000;
	var half_size = 500;
	var precision = 1000;
	
	var min_length = 30;    // Minimal length for the bar. If the cursor gets nearer, the bar fades out
	var max_length = 100;   // Maximal length for the bar.
	
	margin_top = margin_top ?? 10;    // Margin from the top of the cone graphics to the crosshair
	margin_bottom = margin_bottom ?? 10; // Margin from the bottom of the cone graphics to the user
	
	// Determine offset position
	var cursor_top    = Max(distance - margin_top, min_length);
	var cursor_bottom = Max(margin_bottom, cursor_top - max_length);
	var cursor_length = cursor_top - cursor_bottom;
	var cursor_center = cursor_top + cursor_bottom;

	// Determine parameters for transformation
	var relative_length = full_size * cursor_length / GetDefHeight();
	var sin = Sin(angle + spread, full_size, precision);
	var cos = Cos(angle + spread, full_size, precision);

	//var dist_side = Sin(spread, cursor_center * half_size, precision);
	var xoff_side = 0; //Cos(angle, dist_side, precision);
	var yoff_side = 0; //Sin(angle, dist_side, precision);
	var xoff_center = +Sin(angle, cursor_center * half_size, precision);
	var yoff_center = -Cos(angle, cursor_center * half_size, precision);
	
	// Draw it!
	SetObjDrawTransform
	(
		+cos, -sin * relative_length / full_size, xoff_center + xoff_side,
        +sin, +cos * relative_length / full_size, yoff_center + yoff_side,
        overlay
	);
	
	// Update alpha if the cone is too near.
	SetClrModulation(RGBa(255, 255, 255, BoundBy(distance * 128 / min_length, 0, 255)), overlay);
}
