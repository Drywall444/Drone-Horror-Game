#include "RENDER.h"

void RENDER::renderGUI_SOLDIER_INFO()
{
	if (IN.curSELECTED_SOLDIER != entt::null)
	{
		entt::entity soldier = IN.curSELECTED_SOLDIER;
		soldierWIDGET newSOLDIER_INFO;
		auto& soldierINFO = SM.spriteREGISTER.get<soldierOBJECT>(soldier);
		newSOLDIER_INFO.HP = soldierINFO.HP;
		newSOLDIER_INFO.name = "Jane Doe\n";

		if (SM.spriteREGISTER.all_of<hasTARGET>(soldier))
		{
			newSOLDIER_INFO.hasTARGET = true;
		}
		if (SM.spriteREGISTER.all_of<FIRING>(soldier))
		{
			newSOLDIER_INFO.isSHOOTING = true;
		}
		if (SM.spriteREGISTER.all_of<IDLE>(soldier))
		{
			newSOLDIER_INFO.isIDLE = true;
		}
		if (SM.spriteREGISTER.all_of<inCOVER>(soldier))
		{
			newSOLDIER_INFO.inCOVER = true;
		}

		//pass info into widget and then render gui
	}
}