#include "Pool.hpp"
#include <vector>
#include <iostream>
int main()
{
    ObjectPool<long> a;
    std::vector<long*> list;
    for(int i = 0; i < 100000010; i++)
        list.push_back(a.construct(i));
    for(int i = 0; i < 100000000; i++){
        a.destroy(list.back());
        list.pop_back();
    }
    a.destroy(list[2]);
    list.erase(list.begin() + 2);
    for(int i = 0; i < 5; i++)
        list.push_back(a.construct(i));

    for(auto el:list)
        std::cout<<*el<<" ";

    
    for(auto el:list)
        a.destroy(el);
}