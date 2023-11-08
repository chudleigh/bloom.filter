#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../xxhash/xxhash.h"

#ifdef __linux__
        #ifdef ADD_EXPORTS
                #define ADDAPI __attribute__((visibility("default")))
        #else
                #define ADDAPI
        #endif
#elif defined(_WIN32)
        #ifdef ADD_EXPORTS
                #define ADDAPI __declspec(dllexport)
        #else
                #define ADDAPI __declspec(dllimport)
        #endif
#endif

class BloomFilter {
public:
        BloomFilter(uint64_t entries, double error_rate);
        ~BloomFilter();
        void Add(const void* buffer, int length);
        bool Check(const void* buffer, int length);
        bool Save(const char* file_name);
        bool Load(const char* file_name);

private:
		FILE * OpenFile(const char* file_name, wchar_t* mode);
        uint64_t _entries;
        double _error_rate;
        double _bit_per_entity;
        uint64_t _bits;
        uint64_t _bytes;
        uint8_t _hashes;
        uint8_t* _filter;
        const double _ln_2 = 0.693147180559945;
        const XXH64_hash_t _hash_seed = 0x32c1565a65b53543;
};

#ifdef __cplusplus
extern "C"
{
#endif

ADDAPI BloomFilter* create(uint64_t entries, double error_rate) { return new BloomFilter(entries, error_rate); }
ADDAPI void add(BloomFilter* ptr, const void* buffer, int length) { return ptr->Add(buffer, length); }
ADDAPI bool check(BloomFilter* ptr, const void* buffer, int length) { return ptr->Check(buffer, length); }
ADDAPI bool save(BloomFilter* ptr, const char* file_name) { return ptr->Save(file_name); }
ADDAPI bool load(BloomFilter* ptr, const char* file_name) {return ptr->Load(file_name); }


#ifdef __cplusplus
}
#endif
