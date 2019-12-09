#pragma once
template<typename T>
class rp {

	enum { maxArrSize = 1000, };
	static inline rp<T>* rpInstance = nullptr;

public:

	static rp<T>* getInstance() {
		if (rpInstance == nullptr) {
			rpInstance = new rp<T>;
		}
		return rpInstance;
	}

	static void releaseInstance() {
		if (rpInstance != nullptr) {
			delete rpInstance;
			rpInstance = nullptr;
		}
	}

public:
	T* getRoom() {
		int index = pos++;
		pos %= maxArrSize;
		return &(arr[index]);
	}
 
private:
	T arr[maxArrSize];
	int pos = 0;

private:
	rp() = default;
	rp(unsigned int seed);
	rp(const rp&) = delete;
	rp(rp&&) = delete;
	~rp() = default;
	rp operator=(const rp&) = delete;

private:
	int Random() {
		srand(static_cast<int>(time(0)));
		return rand();
	}
	int RandomRange(int lo, int hi) {
		return lo + (Random() % (hi - lo + 1));
	}
};

// int* temp = rp<int>::getInstance()->getRoom();