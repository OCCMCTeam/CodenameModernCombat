/*
	Misc.c
	
	Some functions for backporting to OC.
*/


global func SetActionDataAttach(int action_object_vertex, int action_target_vertex)
{
	// Attach action_object_vertex of the object with the DFA_ATTACH procedure
    // action_target_vertex of its action target.
	return SetActionData((action_object_vertex << 8) + action_target_vertex);
}
