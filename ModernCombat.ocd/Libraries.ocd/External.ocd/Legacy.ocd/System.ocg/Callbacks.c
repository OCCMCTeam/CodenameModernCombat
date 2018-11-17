/**
	Callbacks.c

	Data structure for defining a Call()-function. This is useful in GUI menus,
	the structure is saved in an array, because if it were a proplist the GUI
	system would try to render it as a subwindow.

	@author Marky

*/


// Defines a callback that can be used with DoCallback()
// The callback target is the object that calls this function.
global func DefineCallback(command, par0, par1, par2, par3, par4, par5, par6, par7, par8)
{
	var callback = [];
	callback[0] = this;
	callback[1] = command;
	callback[2] = [];

	callback[2][0] = par0;
	callback[2][1] = par1;
	callback[2][2] = par2;
	callback[2][3] = par3;
	callback[2][4] = par4;
	callback[2][5] = par5;
	callback[2][6] = par6;
	callback[2][7] = par7;
	callback[2][8] = par8;
	return callback;
}

// Executes a callback that was defined with DefineCallback()
// Supports function parameters if the definition via
// DefineCallback() does not provide them.
global func DoCallback(array callback)
{
	var call_from = callback[0];
	var command = callback[1];
	var par = callback[2];
	
	return call_from->Call(command, par[0] ?? Par(1), par[1] ?? Par(2), par[2] ?? Par(3), par[3] ?? Par(4), par[4] ?? Par(5), par[5] ?? Par(6), par[6] ?? Par(7), par[7] ?? Par(8), par[8] ?? Par(9));
}
