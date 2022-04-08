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
    uint8_t displacementCount = 0;
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
//pushes out allocated block to other location if possible
//this means deallocatingprevious chunk to make available for new cuckoo's egg(key,value pair)
bool cuckoo(const Chunk<t, u> &rec, const uint64_t hash, const int_fast8_t count)
{
    const uint64_t index = hash & (this->getCapacity() - 1);
    if(this->data[index].displacementCount <= 1 && this->data[index].allocated && rec.getKey() != this->data[index].getKey())
    {
        const t &alternateObject = this->data[index].key;
        uint64_t altHash = this->hash(alternateObject);
        for(int_fast8_t i = 0; i < count; i++)
            altHash = this->rehash(altHash);
        const uint64_t newIndex = altHash & this->getCapacity() - 1;

        if(!this->data[newIndex].allocated)
        {
        //std::cout<<rec.getKey()<<": "<<hash<<"\n"<<alternateObject<<": "<<altHash<<"\n";
            this->data[newIndex].key = std::move(this->data[index].key);
            this->data[newIndex].value = std::move(this->data[index].value);
            this->data[newIndex].allocated = true;
            this->data[newIndex].displacementCount = this->data[index].displacementCount + 1;
            this->data[index].displacementCount = 0;
            this->data[index].allocated = false;
            return true;
        }
    }
    return false;
}
iterator<t, u> insert(const Chunk<t, u> &rec)
{
    uint64_t hash = this->hash(rec.getKey());
    const uint64_t ohash = hash;
    int_fast8_t hashCount = 0;
    this->cuckoo(rec, hash, 0);
    while(data[hash & (this->capacity-1)].allocated && data[hash & (this->capacity-1)].getKey() != rec.getKey())
    {
        hashCount++;
        hash = this->rehash(hash);
        if(hashCount == 1) this->cuckoo(rec, hash, 1);
        if(hashCount >= 2 && data[hash & (this->capacity-1)].allocated && data[hash & (this->capacity-1)].getKey() != rec.getKey())
        {
            this->resize(this->capacity<<1);
            hash = this->hash(rec.getKey());
        }
    } 
    hash &= (this->capacity-1);
    data[hash].construct(rec);
    //const auto alloced = this->allocatedCount();
    //std::cout<<"capacity, alloced, fill% "<<this->getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/this->getCapacity())<<"\n";
    return iterator<t, u>(&data[hash], &data[this->getCapacity() - 1]);
}
uint64_t hash(const t &key)
{
    return std::hash<t>()(key);
}
int64_t rehash(int64_t hash)
{
    static const uint64_t prime = 0x100000001b3, offset = 0xcbf29ce484222325;
    const auto ohash = hash;
    hash ^= (hash << 31) ^ (hash >> 29);
    hash += offset;
    hash ^= ((ohash << 15) ^ (ohash)) * prime;
    return hash + 1997;
}
iterator<t, u> find(const t &key)
{
    uint64_t hash = this->hash(key);
    int_fast8_t hashCount = 0;
    bool nequal = 0;
    while((nequal = (data[hash & (this->capacity-1)] != key)) && hashCount < 2)
    {   
        hashCount++;
        hash = rehash(hash);
    }

    return iterator<t, u>(!nequal ? &data[hash & (this->capacity-1)] : nullptr, &data[this->getCapacity() - 1]);
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
        delete[] data;
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
    uint64_t capacity = 8;
};