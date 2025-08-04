#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"

void coop_radarReset(Player* player);
float coop_radarAngleTo( const Vector& source , const Vector& dest );
float coop_radarSignedAngleTo( const Vector& source , const Vector& dest , const Vector& planeNormal );
void coop_radarUpdate( Player *player );

#endif