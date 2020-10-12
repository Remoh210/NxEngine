#include "System.h"

bool BaseSystem::IsValid()
{
	for (uint32 i = 0; i < componentFlags.size(); i++)
	{
		if ((componentFlags[i] & BaseSystem::FLAG_OPTIONAL) == 0)
		{
			return true;
		}
	}
	return false;
}

bool SystemList::RemoveSystem(BaseSystem & system)
{
	for (uint32 i = 0; i < systems.size(); i++)
	{
		if (&system == systems[i])
		{
			systems.erase(systems.begin() + i);
			return true;
		}
	}
	return false;
}
