/**
	Some math functions

	@author Marky
 */

global func InterpolateRGBa(int progress, int x0, int y0, int x1, int y1, int x2, int y2, int x4, int y4, interpolation)
{
	interpolation = interpolation ?? Global.InterpolateLinear;
	
	return y0; // FIXME: I am really lazy at the moment
}

global func InterpolateLinear(int progress, int x0, int y0, int x1, int y1, int x2, int y2, int x4, int y4)
{
}
