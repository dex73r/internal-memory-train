#pragma once
#include "src_headers.h"
#include "constants/definitions.h"
#include "math.h"

#include <ctime>
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

const float FoV = 10.0f;
const float Inacc = 1.0f;
const float Speed = 1.0f;

bool IsBallisticWeapon(void *weapon)
{
	if (weapon == nullptr)
		return false;
	char *pWeapon = (char *)weapon;
	int id = *(pWeapon + 0x2F88);
	return !(id >= WEAPON_KNIFE_CT && id <= WEAPON_KNIFE_T || id == 0 || id >= WEAPON_KNIFE_BAYONET);
}

bool TargetMeetsRequirements(CEntity *p)
{
	bool ok = true;
	if (!p)
		return !ok;
	if (p->isdormant())
		return !ok;
	if (p->gethealth() < 1)
		return !ok;
	CEntity *local = g_pEntityList->getcliententity(g_pEngine->GetLocalPlayer());
	if (p->getteam() == local->getteam())
		return !ok;
	if (p == local)
		return !ok;
	return ok;
}

int GetTargetCrosshair(CEntity *pLocal)
{
	int target = -1;

	float minFoV = FoV;
	Vector ViewOffset = pLocal->getorigin() + pLocal->getviewoffset();
	Vector View;
	g_pEngine->GetViewAngles(View);
	View += (*pLocal->getaimpunchangle()) * 2.f;
	for (int i = 0; i < g_pEngine->GetMaxClients(); i++)
	{
		CEntity *pEntity = g_pEntityList->getcliententity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			float fov = FovToPlayer(ViewOffset, View, pEntity);
			// printf("gtc fov: %.2f\n", fov);
			if (fov < minFoV)
			{
				minFoV = fov;
				target = i;
			}
		}
	}

	return target;
}

void ayyCalcAngle(Vector src, Vector dst, Vector &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D(); //delta.Length
	angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.x = (atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.y += 180.0f;
}

bool AimAtPoint(CEntity *pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{

	// Get the full angles
	if (point.Length() == 0)
		return false;
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	static Vector Inaccuracy;

	if (timeSoFar > 0.2)
	{
		Inaccuracy.Init(-50 + rand() % 100, -50 + rand() % 100, -50 + rand() % 100);
		Inaccuracy.NormalizeInPlace();
		Inaccuracy *= Inacc;
		start_t = clock();
	}

	point += Inaccuracy;
	Vector angles;
	Vector src = pLocal->getorigin() + pLocal->getviewoffset();
	ayyCalcAngle(src, point, angles);
	//NormaliseViewAngle(angles);
	angles.NormalizeAngles();
	if (angles[0] != angles[0] || angles[1] != angles[1])
		return false;
	/* bool brcs = false;
	if (brcs)
	{
		Vector AimPunch = pLocal->getaimpunchangle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			angles -= AimPunch * 2;
			angles->Normalise();
		}
	} */

	// IsLocked = true;

	Vector ang = angles - pCmd->viewangles;
	bool v = false;
	if (ang.Length() > Speed)
	{
		Normalize(ang, ang);
		ang *= Speed;
	}
	else
		v = true;
	pCmd->viewangles += ang;
	g_pEngine->SetViewAngles(pCmd->viewangles);
	return v;
}
void aimbot(CUserCmd *cmd, CEntity *local)
{
	CEntity *pTarget = nullptr;
	CEntity *pLocal = local;
	bool FindNewTarget = true;
	static int TargetID = -1;

	//knife
	CBaseCombatWeapon *pWeapon = (CBaseCombatWeapon *)g_pEntityList->entfromhandle(local->getactiveweapon());
	if (!pWeapon)
		return;
	if (pWeapon->GetAmmoInClip() == 0 || !IsBallisticWeapon(pWeapon))
		return;
	// Make sure we have a ok target
	if (TargetID >= 0)
	{
		pTarget = g_pEntityList->getcliententity(TargetID);
		if (TargetMeetsRequirements(pTarget))
		{
			Vector ViewOffset = pLocal->getorigin() + pLocal->getviewoffset();
			Vector View;
			g_pEngine->GetViewAngles(View);
			View += *local->getaimpunchangle() * 2.f;
			// if somthing fails by calculations, its this
			float nFoV = FovToPlayer(ViewOffset, View, pTarget);
			// printf("ab fov: %.2f\n", nFoV);
			if (nFoV < FoV)
				FindNewTarget = false;
		}
	}
	// Find a new target, apparently we need to
	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		TargetID = GetTargetCrosshair(local);
		// printf("target=%d\n", TargetID);
		if (TargetID >= 0)
			pTarget = g_pEntityList->getcliententity(TargetID);
		else
			pTarget = nullptr;
	}

	// If we finally have a ok target
	if (TargetID >= 0 && pTarget)
	{
		if (/* Key >= 0 && !GUI.GetKeyState(Key) */ false)
		{
			TargetID = -1;
			pTarget = nullptr;
			return;
		}
		Vector AimPoint = pTarget->GetBonePosition(6);
		bool bSendPacket = false;
		// printf("- we before aimatpoint:");
		if (AimAtPoint(pLocal, AimPoint, cmd, bSendPacket))
		{
			// printf("aim at point success\n");
			//IsLocked = true;
			if (/* Menu::Window.LegitBotTab.AimbotAutoFire.GetState()  */ false && !(cmd->buttons & IN_ATTACK))
			{
				cmd->buttons |= IN_ATTACK;
			}
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
	aimbot(cmd, local);
	return 0;
}