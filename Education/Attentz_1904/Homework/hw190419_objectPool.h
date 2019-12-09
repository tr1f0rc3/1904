#include <vector>

using uint32 = unsigned int;

template <typename T>
class Pool {
public: // cstr detor
	Pool() = default;
	Pool(uint32 _size) : maxSize(_size) { rooms.reserve(_size); }
	~Pool() {
		for (auto val : rooms) {
			if (val != nullptr) { delete val; }
		}
		rooms.clear();
	}
private:
	Pool(const Pool<T>&) = delete;
	Pool(Pool<T>&&) = delete;
	
public: // room interface
	void reserve(uint32 _size) {
		if (static_cast<int>(_size) > curr) { rooms.reserve(_size); }
	}
	T* get() {
		if (curr == 0) { return new T(); }
		--curr;
		T* temp = rooms[curr];
		rooms[curr] = nullptr;
		return temp;
	}
	void leave(T* _room) {
		if (_room == nullptr) { return; }
		if (curr + 1 == maxSize) {
			enum {eMake= 10,};
			maxSize += eMake;
			rooms.reserve(maxSize);
		}
		++curr;
		rooms.push_back(_room);
	}

	size_t getFreeObjectCnt() const{ return curr; }
	size_t getMaxFreeObjectCnt() const{ return maxSize; }


private:
	int curr = 0;
	int maxSize = 3;
	std::vector<T*> rooms;
	int maxTop = 0;

};
