/*--- CMC Default scenario script ---*/

/*--- Engine callbacks ---*/

public func Initialize()
{
	_inherited(...);
	
	InitializeAmbience();
	InitializeDeco();
}


/*--- Custom callbacks ---*/


// Override this function with ambience
// settings that are specific to this
// map, such as sky parallaxity, but
// do not change over the course of
// the individual rounds.
private func InitializeAmbience()
{
	_inherited(...); // Does nothing for now
}


// Override this function to create
// static deco objects that do not
// change over and do not need to
// be reset between rounds.
//
// Note: might be removed, because this
//       is essentially the same as
//       having an Objects.c file.
private func InitializeDeco()
{
	_inherited(...); // Does nothing for now
}
