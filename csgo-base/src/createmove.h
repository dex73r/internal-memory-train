#pragma once
#include "src_headers.h"
#include "constants/definitions.h"
#include "math.h"

#include <cstdio>

void rcs(CUserCmd *cmd, CEntity *local)
{
	if (local->getshotsfired() <= 1)
		return;
	if (local->getactiveweapon() == 0xFFFFFFFF)
		return;
	vec3f punchAngles = (*local->getaimpunchangle()) * 2.0f;
	if (punchAngles != 0.f)
	{
		cmd->viewangles -= punchAngles;
		cmd->viewangles.clamp();
	}
}
void bhop(CUserCmd *cmd, CEntity *local)
{
	if (local->gethealth() < 1)
		return;
	static bool bLastJumped = false;
	static bool bShouldFake = false;

	if (!bLastJumped && bShouldFake)
	{
		bShouldFake = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (local->flags() & FL_ONGROUND)
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

bool IsBallisticWeapon(void *weapon)
{
	if (weapon == nullptr)
		return false;
	char *pWeapon = (char *)weapon;
	int id = *(pWeapon + 0x2F88);
	return !(id >= WEAPON_KNIFE_CT && id <= WEAPON_KNIFE_T || id == 0 || id >= WEAPON_KNIFE_BAYONET);
}

bool TargetMeetsRequirements(CEntity *p){
	bool ok = true;
	if (!p) return !ok;
	if (p->isdormant()) return !ok;
	if (p->gethealth() < 1) return !ok;
	CEntity *local = g_pEntityList->getcliententity(g_pEngine->GetLocalPlayer())
	if (p->getteam() == local->getteam()) return !ok;
	if (p == local) return !ok;
	return ok;
}

int CLegitBot::GetTargetCrosshair(CEntity *pLocal)
{
	int target = -1;
	const float FoV = 20.f;
	float minFoV = FoV;

	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);
	View += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		CEntity *pEntity = g_pEntityList->getcliententity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			float fov = FovToPlayer(ViewOffset, View, pEntity);
			if (fov < minFoV)
			{
				minFoV = fov;
				target = i;
			}
		}
	}
	printf("targetid fov: %2.f\n", minFoV);

	return target;
}

void aimbot(CUserCmd *cmd, CEntity *local)
{
	CEntity* pTarget = nullptr;
	CEntity* pLocal = local;
	bool FindNewTarget = true;
	int TargetID = -1;

	//knife
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)g_pEntityList->entfromhandle(local->getactiveweapon())
	if (!pWeapon)
		return;
	if (pWeapon->GetAmmoInClip() == 0 || !IsBallisticWeapon(pWeapon))
		return;

	// Make sure we have a ok target
	if (/* IsLocked && */ TargetID >= 0/*  && HitBox >= 0 */)
	{
		pTarget = g_pEntityList->getcliententity(TargetID);
		if (TargetMeetsRequirements(pTarget))
		{
			Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
			Vector View; g_pEngine->GetViewAngles(View);
			View += *local->getaimpunchangle() * 2.f;
			// if somthing fails by calculations, its this
			float nFoV = FovToPlayer(ViewOffset, View, pTarget);
			printf("target fov: %.2f\n", nFoV);
			if (nFoV < FoV)
				FindNewTarget = false;
		}
	}
	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		TargetID = GetTargetCrosshair();
		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	// If we finally have a ok target
	if (TargetID >= 0 && pTarget)
	{
		if (/* Key >= 0 && !GUI.GetKeyState(Key) */ false)
		{
			TargetID = -1;
			pTarget = nullptr;
			HitBox = -1;
			return;
		}
		
	}
}

bool __fastcall hkCreateMove(void *, void *, float, CUserCmd *cmd)
{
	if (cmd->command_number == 0) //if command_number is 0 then ExtraMouseSample is being called
		return 0;

	CEntity *local = g_pEntityList->getcliententity(g_pEngine->GetLocalPlayer());
	if (!local)
		return 0;

	bhop(cmd, local);
	rcs(cmd, local);
	//aimbot(cmd, local);
	return 0;
}