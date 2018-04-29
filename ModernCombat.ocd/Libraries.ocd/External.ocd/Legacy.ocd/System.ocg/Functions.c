/*
	Functions.c

	Functions for copying functions;

	@author Marky
 */


/**
 Gets the name of a function.
 This is somewhat  trivial, however such a function  did not exist.

 @return string {@code function_name} in {@code source_type.function_name} of {@code call}.
*/
global func GetFunctionName(func call)
{
	return RegexReplace(Format("%v", call), "(.+)\\.(.+)", "$2");
}
