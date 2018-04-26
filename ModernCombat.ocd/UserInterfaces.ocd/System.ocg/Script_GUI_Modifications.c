/**
	Functions for manipulating GUI things easily.

	@author Marky
 */


/*
	Moves the position of a GUI element by the desired amount.
	This only changes the proplist layout, you have to perform
	GUI updates manually.

	@par horizontally Position string by which to shift horizontally.
                      Passing 'nil' means no shift.
   
	@par vertically Position string by which to shift vertically.
                    Passing 'nil' means no shift.
 */
global func GUI_ShiftPosition(proplist layout, string horizontally, string vertically)
{
	AssertNotNil(layout);
	
	if (horizontally)
	{
		layout.Left = Format("%s %s", layout.Left ?? ToPercentString(0), horizontally);
		layout.Right = Format("%s %s", layout.Right ?? ToPercentString(1000), horizontally);
	}
	
	if (vertically)
	{
		layout.Top = Format("%s %s", layout.Top ?? ToPercentString(0), horizontally);
		layout.Bottom = Format("%s %s", layout.Bottom ?? ToPercentString(1000), horizontally);
	}
	
	return layout;
}
