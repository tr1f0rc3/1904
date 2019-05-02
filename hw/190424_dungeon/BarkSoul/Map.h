#pragma once

class Map
{
public:
	static Map* getInstance();
	void releaseInstance();
	static inline Map* instance = nullptr;
	// interface
	std::vector<Pos> MapToPos(int);
	enum {mapLimit = 200};
	void loadMap(std::string _filePath, char(&_input)[mapLimit][mapLimit]);
	std::vector<std::string> lineToWords(std::string _data);
	
private:
	// cstr 
	Map() = default;
	Map(const Map& _input) = delete;
	Map(Map&& _input) = delete;
	~Map() { delete instance; instance = nullptr; }
	// data
	std::vector<std::string> readText(std::string _filePath);
	void writeText(std::string _filePath, std::vector<std::string> _data);
	std::vector<std::string>& makeMapRectangle(std::vector<std::string>& _data);
};

inline Map* Map::getInstance() {
	if (instance == nullptr) {
		auto temp = new Map;
		instance = temp;
	}
	return instance;
}

inline void Map::releaseInstance() {
	delete instance;
	instance = nullptr;
}

#define LOAD_MAP(x,y)		(Map::getInstance()->loadMap((x),(y)))
#define RELEASE_RAIDUS()	(Map::releaseInstance())
