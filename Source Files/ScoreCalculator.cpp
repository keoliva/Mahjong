#include "include/ScoreCalculator.h"
#include <curl/curl.h>

#include <sstream>
using namespace std;
map<string, string> ScoreCalculator::tileFormat = {
	{"BambooTile", "b"}, {"CharacterTile", "c"}, {"CircleTile", "d"}, // SUIT tiles
	{"DragonTile", "D"}, {"WindTile", "W"}, // HONOR tiles
};

struct DataStream {
	string data;
	size_t size;
	DataStream() { data.reserve(500); };
};
typedef struct DataStream data_t;
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
int ScoreCalculator::calculate(MahjongType win_type, Tile *winningTile, wind roundWind, Player *winner)
{
	// include round wind and seat wind to the input
	char round_wind = wind_strings[roundWind][0];
	char player_wind = wind_strings[winner->_wind][0];
	string winds = "rw W";
	winds += round_wind; winds += " sw W";
	winds += player_wind; winds += " ";

	// including modifier (how the mahjong came about) to input
	if (win_type != REGULAR_DISCARD) {
		string type_of_win = win_type_strings[win_type];
		// lower(type_of_win)
		int len = type_of_win.size();
		for (int i = 0; i < len; i++)
			type_of_win[i] = tolower(type_of_win[i]);
		winds += type_of_win;
	}
	// append winning tile to input
	string input;
	input += handToFormattedString(winner->hand, winner->bonuses, winner->melds);
	input += "w ";
	input += tileFormat[winningTile->className];
	if (dynamic_cast<SuitTile*>(winningTile)) {
		input += to_string(dynamic_cast<SuitTile*>(winningTile)->suit);
	} else {
		input += tolower(dynamic_cast<AttributeTile*>(winningTile)->attribute.at(0));
	}
	
	// append round wind, seat win, and modifier to input
	input += winds;

	// make the http request to use the MCR Mahjong Hand Score Calculator
	string url = "http://mahjonghand.appspot.com/?sit=" + input;
	CURL *curl;
	CURLcode res;	
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		data_t response;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
		// Perform the request
		res = curl_easy_perform(curl);
		// Check for errors 
		if (res != CURLE_OK)
			cout << "curl_easy_perform() failed: %s\n" << curl_easy_strerror(res) << endl;
		size_t i = response.data.find_last_of("points: ");
		stringstream data(response.data.substr(i));
		int points;
		data >> points;
		// finally, cleanup
		curl_easy_cleanup(curl);
		return points;
	}
	return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	size_t actualSize = size * nmemb;
	data_t *res_data = (data_t *)userdata;
	const char *data = (const char *)ptr;
	res_data->data.append(data, actualSize);
	return actualSize;
}

const vector<string> flowerTypes = { "PLUMB", "ORCHID", "CHRYSANTHEMUM", "BAMBOO" };
const vector<string> seasonTypes = { "SPRING", "SUMMER", "AUTUMN", "WINTER" };
// follows the format described in http://mahjonghand.appspot.com/
string ScoreCalculator::handToFormattedString(player_tiles_t hand, player_tiles_t bonuses,
	vector<pair<MeldType, player_tiles_t>> melds) {
	// tiles in player's hand
	bool(*eq)(const Tile &a, const Tile &b) = [](const Tile &a, const Tile &b) {
		return a.get_className() == b.get_className();
	};
	string hand_string;
	hand_string += "h ";
	bool isSuitTile = false;
	player_tiles_t::iterator it = hand.begin();
	Tile *lastTile = nullptr, *currTile = nullptr;
	for (; it != hand.end(); it++) {
		currTile = it->get();
		if (lastTile) {
			if (!eq(*lastTile, *currTile)) {
				hand_string += tileFormat[lastTile->className];
				isSuitTile = (dynamic_cast<SuitTile*>(currTile)) ? true : false;
			}
		}
		if (isSuitTile) {
			hand_string += dynamic_cast<SuitTile*>(currTile)->suit;
		} else {
			string attribute = dynamic_cast<AttributeTile*>(currTile)->attribute;
			hand_string += tolower(attribute.at(0));
		}
		lastTile = currTile;
	}
	hand_string += " ";

	// tiles in the melded sets
	if (melds.empty()) return hand_string;
	string melds_string;
	melds_string += "m ";
	isSuitTile = false;
	for (pair<MeldType, player_tiles_t> meld : melds) {
		isSuitTile = (dynamic_cast<SuitTile*>(meld.second.back().get())) ? true : false;
		player_tiles_t::iterator it = meld.second.begin();
		for (; it != meld.second.end(); it++) {
			currTile = it->get();
			if (isSuitTile) {
				melds_string += dynamic_cast<SuitTile*>(currTile)->suit;
			} else {
				string attribute = dynamic_cast<AttributeTile*>(currTile)->attribute;
				melds_string += tolower(attribute.at(0));
			}
			melds_string += tileFormat[currTile->className];
		}

	}
	hand_string += melds_string;

	// Flowers and Seasons
	if (bonuses.empty()) return hand_string;
	string bonus_string;
	bonus_string += "f ";
	it = bonuses.begin();
	int index;
	AttributeTile *tile;
	it = bonuses.begin();
	for (; it != bonuses.end(); it++) {
		tile = dynamic_cast<AttributeTile*>((*it).get());
		if (tile->className == "FlowerTile") {
			index = find(flowerTypes.begin(), flowerTypes.end(), tile->attribute) - flowerTypes.begin();
			index++; // must be mapped to a value between F1-F4
		} else { // it->className == "SeasonTile"
			index = find(seasonTypes.begin(), seasonTypes.end(), tile->attribute) - seasonTypes.begin();
			index += 5; // must be mapped to a value between F5-F8
		}
		bonus_string += index;
	}
	bonus_string += "F "; // end of Flowers(for revealed flowers and seasons) tile group

	hand_string += bonus_string;
	return hand_string;
}
