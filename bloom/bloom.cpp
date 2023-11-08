#include "bloom.h"

#define _CRT_SECURE_NO_DEPRECATE

BloomFilter::BloomFilter(uint64_t entries, double error_rate)
{
    _entries = entries;
    _error_rate = error_rate;
    _bit_per_entity = (-log(error_rate) / _ln_2 / _ln_2);
    _bits = (uint64_t)((double)entries * _bit_per_entity);
    _bytes = _bits % 8 ? (uint64_t)(_bits / 8) + 1 : (uint64_t)(_bits / 8);
    _hashes = (uint8_t)ceil(_ln_2 * _bit_per_entity);
    _filter = (uint8_t*)calloc(_bytes, sizeof(uint8_t));
}

BloomFilter::~BloomFilter()
{
    free(_filter);
}

void BloomFilter::Add(const void* buffer, int length)
{
    uint64_t a = XXH64(buffer, length, _hash_seed);
    uint64_t b = XXH64(buffer, length, a);

    for (uint8_t i = 0; i < _hashes; i++)
    {
        uint64_t bit = (a + b * i) % _bits;
        _filter[bit >> 3] = _filter[bit >> 3] | (1 << (bit % 8));
    }
}

bool BloomFilter::Check(const void* buffer, int length)
{
    uint64_t a = XXH64(buffer, length, _hash_seed);
    uint64_t b = XXH64(buffer, length, a);

    for (uint8_t i = 0; i < _hashes; i++)
    {
        uint64_t bit = (a + b * i) % _bits;
        if (!(_filter[bit >> 3] & 1 << (bit % 8))) { return false; }
    }

    return true;
}

bool BloomFilter::Save(const char* file_name)
{
    FILE* file = fopen(file_name, "wb");
    if (file == NULL) { return false; }

    if (fwrite(&_entries, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(&_error_rate, sizeof(double), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(&_bytes, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (fwrite(_filter, _bytes, 1, file) != 1) { fclose(file); return false; }

    fclose(file); 
    return true;
}

bool BloomFilter::Load(const char* file_name)
{
    FILE* file = fopen(file_name, "rb");
    if (file == NULL) { return false; }

    uint64_t entries;
    double error_rate;
    uint64_t bytes;

    if (fread(&entries, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (entries != _entries) { fclose(file); return false; }

    if (fread(&error_rate, sizeof(double), 1, file) != 1) { fclose(file); return false; }
    if (error_rate != _error_rate) { fclose(file); return false; }

    if (fread(&bytes, sizeof(uint64_t), 1, file) != 1) { fclose(file); return false; }
    if (bytes != _bytes) { fclose(file); return false; }

    if (fread(_filter, bytes, 1, file) != 1) { fclose(file); return false; }

    fclose(file);
    return true;
}