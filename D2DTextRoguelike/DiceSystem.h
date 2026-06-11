#pragma once
class DiceSystem
{
public:
	struct RollInfo
	{
		int roll = 0;
		float multiplier = 1.0f;
	};

	struct Result
	{
		std::vector<RollInfo> rolls;
		float totalMultiplier = 0.0f;

		bool IsFumble() const
		{
			return !rolls.empty() && rolls.front().roll == 1;
		}
	};

public:
	static Result RollD20();

private:
	static constexpr int MIN_ROLL = 1;
	static constexpr int MAX_ROLL = 20;

	static constexpr int BAD_ROLL_MIN = 1;
	static constexpr int BAD_ROLL_MAX = 5;

	static constexpr int GOOD_ROLL_MIN = 16;
	static constexpr int GOOD_ROLL_MAX = 20;

	static constexpr float BAD_MULTIPLIER = 0.5f;
	static constexpr float NORMAL_MULTIPLIER = 1.0f;
	static constexpr float GOOD_MULTIPLIER = 1.5f;

};

