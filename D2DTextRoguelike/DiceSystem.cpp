#include "pch.h"
#include "DiceSystem.h"

DiceSystem::Result DiceSystem::RollD20()
{
	Result result;

	int roll = 0;

	do
	{
		roll = rand() % MAX_ROLL + MIN_ROLL;

		RollInfo info;
		info.roll = roll;

		if (roll <= BAD_ROLL_MAX)
		{
			info.multiplier = BAD_MULTIPLIER;
		}
		else if (roll >= GOOD_ROLL_MIN)
		{
			info.multiplier = GOOD_MULTIPLIER;
		}
		else
		{
			info.multiplier = NORMAL_MULTIPLIER;
		}

		result.rolls.push_back(info);
		result.totalMultiplier += info.multiplier;

	} while (roll == 20);

	return result;
}