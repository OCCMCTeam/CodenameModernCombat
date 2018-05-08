/**
	Properties.c

	@author Marky
 */

/**
	Causes a fatal error if the child declares properties that the parent does not have.

	This is useful for ensuring consistent properties in case that the parent definition
	declares certain properties and the child relies on these property names being
	use in the parent when defining/overwriting these properties.

	Example:
	At first, child and parent both have the property "my_custom_property". Later,
	someone finds a name conflict and renames it to "namespace_custom_property",
	but forgets to rename the property assignment in the child.

	This helps in debugging, as mentioned above, but it cannot avoid reassigning
	an already existing property with a new meaning, and other cases.

	Info:
	The class is local, e.g. proplist->AssertSamePropertyNames()
	
	@par parent The reference proplist, for comparison. Can be any
	            proplist, not necessary a prototype.
	            
	@par whitelist A list of properties that may exist in the child
	               without existing in the parent. This is useful
	               for engine properties and the like.
 */
global func AssertSamePropertyNames(proplist parent, array whitelist)
{
	if (parent != nil )
	{
		var child_properties = GetProperties(this);
		var parent_properties = GetProperties(parent);
		var missing_properties = [];
		for (var property in child_properties)
		{
			if (!IsValueInArray(parent_properties, property) && !IsValueInArray(whitelist, property))
			{
				PushBack(missing_properties, property);
			}
		}
		
		if (GetLength(missing_properties) > 0)
		{
			FatalError("The proplist %v defines the following properties that do not exist in the reference proplist %v: %v", this, parent, missing_properties);
		}
	}
}
