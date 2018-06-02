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
	UpdateCursorBar(Cursor_Angle, -Cursor_Spread, CNAT_Left);
	UpdateCursorBar(Cursor_Angle, +Cursor_Spread, CNAT_Right);
}

func UpdateCursorBar(int angle, spread, int overlay)
{
	var full_size = 1000;
	var precision = 1000;
	
	
	var sin = Sin(angle + spread, full_size, precision);
	var cos = Cos(angle + spread, full_size, precision);
	
	var dist = Sin(spread, Cursor_Distance * full_size, precision);
	var xoff = Cos(angle, dist, precision);
	var yoff = Sin(angle, dist, precision);
	
	
	SetObjDrawTransform
	(
		+cos, -sin, xoff,
        +sin, +cos, yoff,
        overlay
	);
}
