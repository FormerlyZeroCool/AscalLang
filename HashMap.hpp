
#pragma once
/*
 * HashMap.hpp
 *
 *  Created on: Mar 31, 2022
 *      Author: andrewrubinstein
 */
#include <memory>
template <typename t, typename u>
class FlatMap {
public:
template <typename k, typename v>
struct Chunk {
    k key;
    v value;
    bool allocated = false;
    Chunk(){}
    Chunk(const k &key, const v &value): key(key), value(value), allocated(true)
    {}
    void construct(const k &key, const v &value)
    {
        this->key = key;
        this->value = value;
        this->allocated = true;
    }
    void construct(const Chunk<k, v> &o)
    {
        construct(o.getKey(), o.getValue());
    }
    void destroy()
    {
        this->allocated = false;
    }
    const k& getKey() const
    {
        return key;
    }
    v& getValue() const
    {
        return *const_cast<v*>(&value);
    }
	bool operator!=(const k &o) const
	{
		return this->getKey() != o;
	}
	bool operator==(const k &o) const
	{
		return this->getKey() == o;
	}
};

template <typename k, typename v>
class iterator {
    Chunk<k, v> *record;
    Chunk<k, v> *last;
    public:
    iterator(Chunk<k, v> *record, Chunk<k, v> *last): record(record), last(last) {}
    iterator(): record(nullptr), last(nullptr) {}
    iterator<k, v>& operator=(const iterator<k, v> &o)
    {
        this->record = o.record;
        this->size = o.size;
        return *this;
    }
    Chunk<t, u>& operator*() const
    {
        return *this->record;
    }
    bool operator==(const iterator<k, v> &o) const
    {
        return o.record == this->record;
    }
    bool operator!=(const iterator<k, v> &o) const
    {
        return o.record != this->record;
    }
    iterator<k, v>& operator++()
    {
        do{
            this->record++;
        } while(record <= last && !record->allocated);
        if(record > last || (record == last && !record->allocated))
            this->record = nullptr;
        return *this;
    }
    iterator<k, v> operator++(int)
    {
        iterator<k, v> original(*this);
        *this++;
        return original;
    }
};

FlatMap(const FlatMap<t, u> &&o): data(o.data), capacity(o.capacity)
{
    o.data = nullptr;
    o.capacity = 0;
}
FlatMap(): data(nullptr) {
    this->resize(this->capacity);
}
FlatMap(int64_t capacity): data(nullptr) {
    this->capacity = capacity;
    this->resize(this->capacity);
}
int64_t getCapacity()
{
    return this->capacity;
}
uint64_t hash(const t &key)
{
    return std::hash<t>()(key) & (this->capacity-1);
}
u& operator[](const t key)
{
    const auto it = this->find(key);
    if(it != this->end())
        return (*it).getValue();
    return (*this->insert(Chunk<t, u>(key, u()))).getValue();
}
void erase(const t &key)
{
    const auto it = this->find(key);
    if(it != this->end())
    {
        (*it).destroy();
    }
}
iterator<t, u> insert(const t &key, const u &value)
{
    return this->insert(Chunk<t ,u>(key, value));
}
iterator<t, u> insert(std::pair<t, u> &&rec)
{
    return this->insert(rec);
}
iterator<t, u> insert(std::pair<t, u> &rec)
{
    return this->insert(Chunk<t ,u>(rec.first, rec.second));
}
iterator<t, u> insert(const Chunk<t, u> &&rec)
{
    return this->insert(rec);
}
int64_t allocatedCount()
{
    int64_t count = 0;
    for(int64_t i = 0; i < this->getCapacity(); i++)
    {
        count += this->data[i].allocated;
    }
    return count;
}
iterator<t, u> insert(const Chunk<t, u> &rec)
{
    uint64_t hash = this->hash(rec.getKey());
    uint_fast32_t hashCount = 0;
    while(data[hash].allocated)
    {
        hashCount++;
        hash = rehash(hash);
        if(hashCount >= 2 && data[hash].allocated)
        {
            this->resize(this->capacity<<1);
            hash = this->hash(rec.getKey());
        }
    } 
    data[hash].construct(rec);
    return iterator<t, u>(&data[hash], &data[this->getCapacity() - 1]);
}
int64_t rehash(uint64_t hash)
{
    const auto ohash = hash;
    hash ^= ohash << 3 ^ ohash >> 3;
    hash ^= (ohash << 4) | (ohash >> 4);
    return hash & (this->capacity-1);
}
iterator<t, u> find(const t &key)
{
    uint64_t hash = this->hash(key);
    int_fast8_t hashCount = 0;
    while(data[hash] != key && hashCount < 2)
    {   
        hashCount++;
        hash = rehash(hash);
    }

    return iterator<t, u>(hashCount < 2 ? &data[hash] : nullptr, &data[this->getCapacity() - 1]);
}
FlatMap(const FlatMap<t, u> &o): capacity(o.capacity)
{
    *this = o;
}
iterator<t, u> begin()
{
    for(int i = 0; i < this->getCapacity(); i++)
        if(this->data[i].allocated)
            return iterator<t, u>(&data[i], &data[this->getCapacity()-1]);
    return iterator<t, u>(nullptr, &data[this->getCapacity()-1]);
}
iterator<t, u> end()
{
    return iterator<t, u>(nullptr, &data[this->getCapacity()-1]);
}
FlatMap& operator=(const FlatMap &o)
{
    if(this->data && o.capacity != this->capacity)
    {
        this->clear();
        delete data;
        this->data = new Chunk<t,u>[this->getCapacity()];
    }
    else if(data)
        this->clear();
    else if(o.capacity != this->capacity)
        this->data = new Chunk<t,u>[this->getCapacity()];
    this->capacity = o.getCapacity();
    for(int64_t i = 0; i < o.capacity; i++)
    {
        if(o.data[i].allocated)
        {
            this->insert(o.data[i]);
        }
    }
}
//must be power of 2
void resize(const uint64_t size)
{
    const auto old = data;
    this->data = new Chunk<t,u>[size];
    const auto capacity = this->getCapacity();
    this->capacity = size;
    if(old)
    {
        for(int64_t i = 0; i < capacity; i++)
        {
            if(old[i].allocated)
            {
                this->insert(old[i]);
                old[i].~Chunk<t, u>();
            }
        }
        delete[] old;
    }
}
void clear()
{
    for(int64_t i = 0; i < this->capacity; i++)
    {
        if(data[i].allocated)
        {
            data[i].destroy();
        }
    }
}
~FlatMap()
{
    if(data)
    {
       this->clear();
       delete[] this->data;
    }
}
private:
    Chunk<t, u> *data;
    uint64_t capacity = 128;
};