#include Library_Stackable

// For ammo interface
public func IsAmmo()  { return true; }
public func MaxAmmo() { return 12; }

// For ammo bag, supply box
public func GetSupplyBoxCost()	{ return 10; } // Creating this ammo type in a supply box uses this many points per ammo 
public func MaxStackCount()     { return 4; }  // This many fit in one ammo box

local Name = "$Name$";
