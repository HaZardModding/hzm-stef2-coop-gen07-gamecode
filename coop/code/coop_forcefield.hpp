#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"

class CoopForcefield
{
public:
	void init();
	bool passthroughBullettAtack(Entity* owner, trace_t &trace, const Vector &start, Vector &end,int &meansofdeath);
	void scan(Entity* owner, Equipment* scanner);
	void scanEnd(Entity* owner, Equipment* scanner);
};

#endif