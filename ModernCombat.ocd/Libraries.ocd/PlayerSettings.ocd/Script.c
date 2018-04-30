/**
	Check and save various player settings.
	Some aspects of the game's behaviour can be configured to the players' liking.

	This library just provides a basic interface of checking / saving these player configurations.
*/

/* --- Settings Strings --- */

// Make unique strings to be saved in the Player.txt

static const CMC_IRONSIGHT_TOGGLE = "CMC_Controls_IronsightToggle";

/* --- Getters --- */

// Get the value of a certain setting
// If default is anything than nil, the value will be initialized (saved) with the default value
public func GetConfigurationValue(int player, string identifier, default)
{
	var value = GetPlrExtraData(player, identifier);
	if (value == nil && default != nil)
	{
		SetConfigurationValue(player, identifier, default);
		return default;
	}
	return value;
}

/* --- Setters --- */

// Set the value of a certain setting
public func SetConfigurationValue(int player, string identifier, value)
{
	SetPlrExtraData(player, identifier, value);
}