/**
	Additional functions for CMC
	
	@author Marky
 */

/*
	Creates a GUI dimension in percent and em units.

	@par percent The 
	@par px The size in pixels with font size 14 (line height = 21 pixels), roughly.
            Will be converted to em units, so it scales with the user selected font size.

	@return a GUI_Dimension proplist.	
 */
global func GuiDimensionCmc(int percent, int px)
{
	return GUI_Dimension->Dimension(percent, 48 * px, nil, 1000);
}
