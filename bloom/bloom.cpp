#include "bloom.h"

BloomFilter::BloomFilter(uint64_t entries, double error_rate)
{
    _entries = entries;
    _error_rate = error_rate;
    _bits = ceil((entries * log(error_rate)) / log(1 / pow(2, log(2))));
    _bytes = _bits % 8 ? (uint64_t)(_bits / 8) + 1 : (uint64_t)(_bits / 8);
    _hashes = round((_bits / entries) * log(2));
    _filter = (uint8_t*)calloc(_bytes, sizeof(uint8_t));
}

BloomFilter::BloomFilter(uint64_t entries, double error_rate, uint8_t* filter)
{
    _entries = entries;
    _error_rate = error_rate;
    _bits = ceil((entries * log(error_rate)) / log(1 / pow(2, log(2))));
    _bytes = _bits % 8 ? (uint64_t)(_bits / 8) + 1 : (uint64_t)(_bits / 8);
    _hashes = round((_bits / entries) * log(2));
    _filter = filter;
}

BloomFilter::~BloomFilter()
{
    free(_filter);
}

void BloomFilter::add(const void* buffer, int length)
{
	auto hash = XXH3_128bits_withSeed(buffer, length, _hash_seed);

	for (uint8_t i = 0; i < _hashes; i++)
	{
		uint64_t bit = ( hash.low64 +  hash.high64 * i) % _bits;
		_filter[bit >> 3] = _filter[bit >> 3] | (1 << (bit % 8));
	}
}

bool BloomFilter::check(const void* buffer, int length)
{
	auto hash = XXH3_128bits_withSeed(buffer, length, _hash_seed);
	
    for (uint8_t i = 0; i < _hashes; i++)
    {
		uint64_t bit = ( hash.low64 +  hash.high64 * i) % _bits;
        if (!(_filter[bit >> 3] & 1 << (bit % 8))) { return false; }
    }

    return true;
}

bool BloomFilter::save(std::string file_name)
{
    FILE* file = fopen(file_name.c_str(), "wb");
    if (file == NULL) { return false; }

    if (fwrite(&_entries, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(&_error_rate, sizeof(double), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(&_bytes, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(_filter, _bytes, 1, file) != 1) { fclose(file); return false; }

    fclose(file); 
    return true;
}

BloomFilter* BloomFilter::load(std::string file_name)
{
    FILE* file = fopen(file_name.c_str(), "rb");
    if (file == NULL) { return nullptr; }

    uint64_t entries;
    double error_rate;
    uint64_t bytes;
	
    if (fread(&entries, sizeof(uint64_t), 1, file) != 1) { fclose(file); return nullptr; }
    if (fread(&error_rate, sizeof(double), 1, file) != 1) { fclose(file); return nullptr; }
    if (fread(&bytes, sizeof(uint64_t), 1, file) != 1) { fclose(file); return nullptr; }
	
    uint8_t* filter = (uint8_t*)calloc(bytes, sizeof(uint8_t));
    if (fread(filter, bytes, 1, file) != 1) { fclose(file); return nullptr; }
	
    fclose(file);
    return new BloomFilter(entries, error_rate, filter);
}
