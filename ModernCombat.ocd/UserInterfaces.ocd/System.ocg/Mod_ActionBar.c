#appendto GUI_Controller_ActionBar

func AssembleActionBar()
{
	// Assemble it as usually
	var action_bar = inherited(...);

	// Remove the default background graphics and replace it with a rectangle
	action_bar.Symbol = nil;
	action_bar.BackgroundColor = GUI_CMC_Background_Color_Default;

	// Done
	return action_bar;
}
