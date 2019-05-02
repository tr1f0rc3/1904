using uint32 = unsigned int;

template <typename T>
class ObjectPool {
public: // cstr detor
	ObjectPool() = default;
	ObjectPool(uint32 _size) : maxSize(_size) { rooms.reserve(_size); }
	~ObjectPool();
private:
	ObjectPool(const ObjectPool<T>&) = delete;
	ObjectPool(ObjectPool<T>&&) = delete;
	
public: // room interface
	void reserve(uint32 _size);
	T* get();
	void leave(T* _room);

	size_t getFreeCnt() const { return freeCnt; }
	size_t getMaxSize() const { return maxSize; }


private:
	int freeCnt = 0;
	int maxSize = 200;
	std::vector<T*> rooms;
};

template <typename T>
inline ObjectPool<T>::~ObjectPool() {
	for (auto val : rooms) {
		if (val != nullptr) { delete val; }
	}
	rooms.clear();
}

template <typename T>
void ObjectPool<T>::reserve(uint32 _size) {
	if (static_cast<int>(_size) > freeCnt) { rooms.reserve(_size); }
}

template <typename T>
T* ObjectPool<T>::get() {
	if (freeCnt == 0) { return new T(); }
	--freeCnt;
	T* temp = rooms[freeCnt];
	rooms[freeCnt] = nullptr;
	return temp;
}

template <typename T>
void ObjectPool<T>::leave(T* _room) {
	if (_room == nullptr) { return; }
	if (freeCnt + 1 == maxSize) {
		enum { eMake = 100, };
		maxSize += eMake;
		rooms.reserve(maxSize);
	}
	++freeCnt;
}
