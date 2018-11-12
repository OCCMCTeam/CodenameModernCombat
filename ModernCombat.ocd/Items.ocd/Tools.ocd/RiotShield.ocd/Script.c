/**
	Riot Shield
*/

#include Shield


/* --- Engine callbacks --- */

public func Initialize(...)
{
	SetGraphics(nil, Shield);
	this.PictureTransformation = Trans_Scale(); // Scale the picture of the box mesh, so that it does not appear in the game
	SetGraphics(nil, CMC_Tool_RiotShield, 1, GFXOV_MODE_Picture);
	return _inherited(...);
}

/* --- Properties --- */

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
