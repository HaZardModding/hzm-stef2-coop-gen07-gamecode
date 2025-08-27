#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/gamefix.hpp"
#include "coop_forcefield.hpp"

CoopForcefield coopForcefield;


void CoopForcefield::init()
{
}

bool CoopForcefield::passthroughBullettAtack(Entity* owner, trace_t &trace, const Vector &start, Vector &end,int &meansofdeath)
{
	Entity* eTrace = NULL;
	if (trace.ent) {
		eTrace = trace.ent->entity;
	}

	if (!eTrace || !eTrace->isSubclassOf(ScriptSlave) || !owner->isSubclassOf(Player) || meansofdeath != MOD_PHASER && meansofdeath != MOD_VAPORIZE) {
		return false;
	}

	Entity *ent;
	trace_t trace2 = trace;
	ScriptSlave* forceField = (ScriptSlave*)eTrace;

	ScriptVariable* var = NULL;
	var = owner->entityVars.GetVariable("globalCoop_scannedCoopForcefieldFreq");
	if (!var || !strlen(var->stringValue()) || atof(var->stringValue()) <= 0.00) {
		return false;
	}

	str sFfFrequency = va("%.2f", forceField->coop_scriptSlaveGetForcefieldNumber());

	//frequency does not match
	if (sFfFrequency != var->stringValue()) {
		//gi.Printf(va("NO-MATCH : BulletAttack Forcefield Frequency -> %s , %s\n", sFfFrequency.c_str(), var->stringValue()));
		return false;
	}

	eTrace->CancelEventsOfType(EV_BecomeSolid);
	eTrace->ProcessPendingEvents();
	eTrace->setSolidType(SOLID_NOT);

	if (!gameFixAPI_inMultiplayer() || multiplayerManager.fullCollision()) {
		trace = G_FullTrace(start, vec_zero, vec_zero, end, owner, MASK_SHOT, false, "BulletAttack");
	}else {
		trace = G_Trace(start, vec_zero, vec_zero, end, owner, MASK_SHOT, false, "BulletAttack");
	}

	Event* even2 = new Event(EV_BecomeSolid);
	eTrace->PostEvent(even2, 0.01f);

	if (trace.ent) {
		ent = trace.ent->entity;
	}

	return false;
}

void CoopForcefield::scan(Entity* owner, Equipment* scanner)
{
	if (!owner || !owner->isSubclassOf(Player)) {
		return;
	}

	Vector start;
	Vector end;
	trace_t trace;

	Player* player = (Player*)owner;
	player->GetViewTrace(trace, MASK_SHOT, 5000.0f);
	start = trace.endpos;
	end = start + Vector(0.0f, 0.0f, 5000.0f);
	
	if (!trace.ent) {
		return;
	}

	Entity* eFF;
	eFF = trace.ent->entity;
	if (eFF->isSubclassOf(ScriptSlave)) {
		ScriptSlave* forceField = (ScriptSlave*)eFF;
		if (forceField->coop_scriptSlaveGetForcefieldNumber() > 0.0f) {
			//player->hudPrint(va("FF FREQ: %.2f\n", forceField->_forcefieldNumber));
			player->addHud("coop_tricorderFreq");
			player->entityVars.SetVariable("globalCoop_scannedCoopForcefieldFreq",va("%.2f",forceField->coop_scriptSlaveGetForcefieldNumber()));
			gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand coop_tricorderFreq1 title %.2f\"\n",forceField->coop_scriptSlaveGetForcefieldNumber()));
		}
	}
}

void CoopForcefield::scanEnd(Entity* owner, Equipment* scanner)
{
	if (!owner || !scanner || !owner->isSubclassOf(Player)) {
		return;
	}

	Player* player = (Player*)owner;
	player->removeHud("coop_tricorderFreq");
}