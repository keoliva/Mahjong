#ifndef SCORECALCULATOR_H
#define SCORECALCULATOR_H
#include "include/Status.h"
#include "include/Tile.h"
#include <map>
#include <string>
#include <vector>
class ScoreCalculator {
	public:
		static int calculate(MahjongType win_type, Tile *winningTile, wind roundWind, Player *winner);
	private:
		static std::map<std::string, std::string> tileFormat;
		static std::string handToFormattedString(player_tiles_t hand, player_tiles_t bonuses,
			std::vector<std::pair<MeldType, player_tiles_t>> melds);
};
#endif // SCORECALCULATOR_H