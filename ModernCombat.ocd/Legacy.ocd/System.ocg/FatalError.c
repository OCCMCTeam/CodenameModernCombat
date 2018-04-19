/**
 Fatal errors can format the string input in the same way that Log() can.
 
 LibraryShooter.ocd uses this, but it is not available in the 8.1 engine of OC.
 
*/

global func FatalError(string message, ...)
{
	inherited(Format(message, ...));
}
