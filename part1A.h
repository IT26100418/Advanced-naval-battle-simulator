#ifndef PART1A_H
#define PART1A_H

#include "types.h"

void runPart1A(Battlefield *field);

void runPart1ARound(
    Battlefield *field,
    double minimumBAngle,
    double maximumBAngle,
    BattleResult *result);

void runBattleshipAttack(
    Battlefield *field,
    double minimumBAngle,
    double maximumBAngle,
    BattleResult *result);

#endif
