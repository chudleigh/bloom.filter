#define _CRT_SECURE_NO_DEPRECATE
#ifdef __linux__
	#ifdef ADD_EXPORTS
		#define ADDAPI __attribute__((visibility("default")))
	#else
		#define ADDAPI
	#endif
#elif _WIN32
	#ifdef ADD_EXPORTS
		#define ADDAPI __declspec(dllexport)
	#else
		#define ADDAPI __declspec(dllimport)
	#endif
#else
	#error "OS not supported!"
#endif

#include <cmath>
#include <string>
#include "xxhash.h"

class BloomFilter
{
public:
	BloomFilter(uint64_t entries, double error_rate);
	~BloomFilter();
	
	void add(const void* buffer, int length);
	bool check(const void* buffer, int length);
	bool save(std::string file_name);
	static BloomFilter* load(std::string file_name);
	
	uint64_t get_entries() { return _entries; }
	double get_error_rate() { return _error_rate; }
	uint64_t get_size() { return _bytes + sizeof(uint64_t) + sizeof(double) + sizeof(uint64_t); }
		
private:
	BloomFilter(uint64_t entries, double error_rate, uint8_t* filter);
	
	uint64_t _entries;
	double _error_rate;
	uint64_t _bits;
	uint64_t _bytes;
	uint8_t _hashes;
	uint8_t* _filter;
	const XXH64_hash_t _hash_seed = 0x32c1565a65b53543;
};

#ifdef __cplusplus
extern "C"
{
#endif

ADDAPI BloomFilter* create(uint64_t entries, double error_rate) { return new BloomFilter(entries, error_rate); }
ADDAPI void add(BloomFilter* ptr, const void* buffer, int length) { return ptr->add(buffer, length); }
ADDAPI bool check(BloomFilter* ptr, const void* buffer, int length) { return ptr->check(buffer, length); }
ADDAPI bool save(BloomFilter* ptr, const char* file_name) { return ptr->save(file_name); }
ADDAPI BloomFilter* load(const char* file_name) { return BloomFilter::load(file_name); }
ADDAPI uint64_t get_entries(BloomFilter* ptr) { return ptr->get_entries(); }
ADDAPI double get_error_rate(BloomFilter* ptr) { return ptr->get_error_rate(); }
ADDAPI uint64_t get_size(BloomFilter* ptr) { return ptr->get_size(); }

#ifdef __cplusplus
}
#endif
