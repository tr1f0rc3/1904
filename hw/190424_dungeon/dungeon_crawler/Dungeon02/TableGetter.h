struct WeaponData
{
	int nID;
	int nData;

	static const char* GetTableName() { return "Weapon"; }
};

struct ArmorData
{
	int nID;
	int nData;

	static const char* GetTableName() { return "Armor"; }
};


template<typename T>
class Table {
public:
	static Table* getInstance()	{
		if (instance == nullptr) {
			instance = new Table<T>();
		}
		return instance;
	}
	void init() {}
	
	T* getTable(std::string _id)	{
		auto it = table.find(_id);
		if (it != std::end(table))	{
			return &(*it).second;
		}
		return nullptr;
	}
private:
	inline static Table* instance = nullptr;
	std::map<std::string, T> table;
};


#define GET_ITEM_TABLE(n) ( Table<ArmorData>::getInstance()->getTable((n)) )

int main()
{
	using namespace std;

	GetWeaponTb(1);
	GetArmorTb(1);



	getchar();
}