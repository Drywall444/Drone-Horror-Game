#include "sprite.h"

void SPRITE_MANAGER::decideSOLDIER_ACTIONS()
{
	auto allSOLDIERS = spriteREGISTER.view<spriteOBJECT>();
	for (auto& soldier : allSOLDIERS)
	{
		//Soldiers carry out orders if survival is not in question. If being shot at soldiers will quit moving, but remeber the order, and return fire. 
		soldierHEALTH(soldier);
		//Survial Check
		//Carry out orders if not in danger
		//Decide soldier texture based on decided state

	}
}

