/**
	Make clonks remove their items upon death
*/

#appendto Clonk

func Death(int killed_by)
{
	// this must be done first, before any goals do funny stuff with the clonk
	_inherited(killed_by,...);
	
	// The broadcast could have revived the clonk.
	if (GetAlive())
		return;
	
	// Some effects on dying.
	var contents = FindObjects(Find_Container(this));
	for (var item in contents)
	{
		if (item) item->RemoveObject();
	}
	return true;
}
