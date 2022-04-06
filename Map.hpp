/*
 * Map.h
 *
 *  Created on: Mar 31 2022
 *      Author: andrew
 */

#ifndef Map_H_
#define Map_H_
#include <stack>
#include <vector>
#include <iostream>
#include "Pool.hpp"
template <typename t, typename u>
struct NodeData: public std::pair<t, u>
{
    NodeData(const t &key, const u &value): std::pair<t, u>(key, value) {}
	bool operator<(const NodeData<t, u> &o) const
	{
		return this->first < o.first;
	}
	bool operator!=(const NodeData<t, u> &o) const
	{
		return this->first != o.first;
	}
	bool operator==(const NodeData<t, u> &o) const
	{
		return this->first == o.first;
	}
	bool operator<(const t &o)
	{
		return this->first < o;
	}
	bool operator>(const t &o)
	{
		return this->first > o;
	}
	bool operator!=(const t &o)
	{
		return this->first != o;
	}
	bool operator==(const t &o)
	{
		return this->first == o;
	}
};
template <typename t, typename u>
struct node {
	int count;
	int height;
	NodeData<t, u> data;
	node *left;
	node *right;
    node *parent;
	node():left(nullptr),right(nullptr), parent(nullptr),count(1),height(0){}
	node(t key, u value):count(1),height(0),data(key, value),left(nullptr),right(nullptr), parent(nullptr){}
	void calcHeight()
	{
		height = 1 + std::max(left!=nullptr?left->height:-1,right!=nullptr?right->height:-1);
	}
	bool operator<(const node<t, u> &o)
	{
		return this->data < o.data;
	}
	bool operator!=(const node<t, u> &o)
	{
		return this->data != o.data;
	}
	bool operator==(const node<t, u> &o)
	{
		return this->data == o.data;
	}
};
template <typename t, typename u>
struct record
{
	t &key;
	u &value;
	int count;
	record(t &key, u &value,int count):key(key), value(value),count(count){}
};
template <typename t, typename u>
class Map{
private:
    ObjectPool<node<t, u> > &pool;
	void emptyTree();
	int count(node<t, u>* p);
	int leafCount(node<t, u>* p);
	int elements;
	node<t, u>* nodeSearch(const t &dat);

public:
	node<t, u> *root;
    typedef ObjectPool<node<t, u> > alloc;
    static ObjectPool<node<t, u> > createPool()
    {
        return ObjectPool<node<t, u> >();
    }
	Map(ObjectPool<node<t, u> > &pool): pool(pool), root(nullptr), elements(0)
    {}
	Map(Map<t, u> &o);
	void operator=(Map<t, u> &o);
    u& operator[](const t &key)
    {
        auto ptr = this->nodeSearch(key);
        if(ptr)
            return ptr->data.second;
        else
        {
            return this->set(key, u()).data.second;
        }
    }
    node<t, u>* find(const t &key)
    {
        return nodeSearch(key);
    }
	~Map();
    void freeRecursive(node<t, u>* &root)
    {
        if(root->left)
            freeRecursive(root->left);
        if(root->right)
            freeRecursive(root->right);
        this->pool.destroy(root);
        root = nullptr;
    }
    template <typename e>
    void freeRecursive(node<t, u>* &root, void foo(node<t, u>*, e &), e &d)
    {
        if(root->left)
            freeRecursive(root->left);
        if(root->right)
            freeRecursive(root->right);
        foo(root, d);
        this->pool.destroy(root);
        root = nullptr;
    }
	node<t, u>& insert(const t &key, const u &value);
	node<t, u>& set(const t key, const u value);
	void erase(t key);
	int search(t key);
	int getSize();
	int getElementCount();
	int count(const t &key);
	int count(const t &&key);
	int leafCount();
	std::vector<record<t, u> > getInOrder();
	std::vector<record<t, u> > getPreOrder();
	std::vector<record<t, u> > getPostOrder();
    template <typename v, typename w>
    class iterator {
        node<v, w> *data;
        public:
        iterator(node<v, w> *start): data(start) {}
        node<v, w>* dataNode()
        {
            return data;
        } 
        const NodeData<v, w>& operator*() const
        {
            return this->data->data;
        }
        const NodeData<v, w>& operator->() const
        {
            return this->data->data;
        }
        iterator<v, w>& operator++()
        {
            if(data)
            {
                if(data->right)
                {
                    data = data->right;
                    while(data->left){ data = data->left; }
                }
                else
                {
                    while(data->parent && *data->parent < *data) { data = data->parent; }
                    data = data->parent;
                }
            }
            return *this;
        }
        iterator<v, w>& operator--()
        {
            if(data)
            {
                if(data->parent)
                {
                    if(*data->parent < *data)//right side
                    {
                        if(data)
                        {

                        }
                    }
                    else
                    {

                    }
                }
            }
            return *this;
        }
        bool operator==(const iterator<v, w> &o) const
        {
            return this->data == o.data;
        }
        bool operator<(const iterator<v, w> &o) const
        {
            if(this->data & o.data)
                return this->data.data < o.data->data;
            else return false;
        }
        bool operator!=(const iterator<v, w> &o) const
        {
            return this->data != o.data;
        }
        };

    iterator<t, u> begin() const
    {
        node<t, u> *p = root;
        while(p && p->left) { p = p->left; }
        return iterator<t, u>(p);
    }
    iterator<t, u> end() const
    {
        return iterator<t, u>(nullptr);
    }
};

template <typename t, typename u>
Map<t, u>::Map(Map<t, u> &o): pool(o.pool),elements(0),root(nullptr)
{
	std::vector<record<t, u> > data(o.getPreOrder());
	for(record<t, u> r : data)
	{
		this->insert(r.key, r.value);
	}
}
template <typename t, typename u>
void Map<t, u>::operator=(Map<t, u> &o)
{
	this->emptyTree();
	for(record<t, u> r : this->begin())
	{
		this->insert(r.key, r.value);
	}
}
template <typename t, typename u>
std::vector<record<t, u> > Map<t, u>::getInOrder()
{
	std::vector<record<t, u> > data;
	data.reserve(getElementCount());
	std::stack<node<t, u> *> toBeProcessed;
	node<t, u> * p = root;
	while(!toBeProcessed.empty() || p != nullptr)
	{
		if(p != nullptr)
		{
			toBeProcessed.push(p);
			p = p->left;
		}
		else
		{
			p = toBeProcessed.top();
			toBeProcessed.pop();
			data.push_back(record<t, u>(p->data,p->count));
			p = p->right;
		}

	}

	return data;
}
template <typename t, typename u>
int Map<t, u>::count(const t &key)
{
	return search(key);
}
template <typename t, typename u>
int Map<t, u>::count(const t &&key)
{
    return count(key);
}
template <typename t, typename u>
int Map<t, u>::count(node<t, u>* p)
{
	int l =0,r=0;
	if(p != nullptr)
	{
		if(p->left != nullptr)
		{
			l = count(p->left)+1;
		}
		if(p->right != nullptr)
		{
			r = count(p->right)+1;
		}
	}
	return l + r;
}
template <typename t, typename u>
int Map<t, u>::leafCount()
{
	return leafCount(root);
}
template <typename t, typename u>
int Map<t, u>::leafCount(node<t, u>* p)
{
	int count = 0;
	if(p->left != nullptr)
		count += leafCount(p->left);
	if(p->right != nullptr)
		count += leafCount(p->right);
	if((p->left != nullptr && p->right != nullptr) ||
			(p->left == nullptr && p->right != nullptr) )
		count++;
	return count;
}
template <typename t, typename u>
std::vector<record<t, u> > Map<t, u>::getPostOrder()
{
	std::vector<record<t, u> > data;
	data.reserve(getElementCount());
	std::stack<node<t, u> *> toBeProcessed;
	node<t, u> * p = root;
	do{
		while(p != nullptr)
		{
			toBeProcessed.push(p);
			toBeProcessed.push(p);
			p = p->left;
		}
		p = toBeProcessed.top();
		toBeProcessed.pop();
		if(!toBeProcessed.empty() && toBeProcessed.top() == p)
		{
			p = p->right;
		}
		else
		{
			data.push(record<t, u>(p->data,p->count));
			p = nullptr;
		}
	}while(!toBeProcessed.empty());

	return data;
}
template <typename t, typename u>
std::vector<record<t, u> > Map<t, u>::getPreOrder()
{
	std::vector<record<t, u> > data;
	data.reserve(getElementCount());
	std::stack<node<t, u> *> toBeProcessed;
	node<t, u> * p = root;
	if(root != nullptr)
		toBeProcessed.push(p);
	while(!toBeProcessed.empty())
	{
		p = toBeProcessed.top();
		toBeProcessed.pop();
		data.push_back(record<t, u>(p->data,p->count));
		if(p->right != nullptr)
			toBeProcessed.push(p->right);
		if(p->left != nullptr)
			toBeProcessed.push(p->left);
	}

	return data;
}
template <typename t, typename u>
int Map<t, u>::getSize()
{
	 return 0;
}
template <typename t, typename u>
node<t, u>* Map<t, u>::nodeSearch(const t &dat)
{
	node<t, u> *p = root;
	while(p != nullptr && p->data != dat)
	{
		if(p->data > dat)
		{
			p = p->left;
		}
		else
		{
			p = p->right;
		}
	}
	return p;
}

template <typename t, typename u>
int Map<t, u>::search(t dat)
{
	node<t, u> *p = nodeSearch(dat);
	int result = 0;
	if(p != nullptr)
	{
		result = p->count;
	}
	return result;
}
template <typename t, typename u>
void Map<t, u>::erase(t dat)
{
	node<t, u> *p = root;
	node<t, u> *parent = nullptr;
    int8_t dir = 0;
	while(p != nullptr && (dir = p->data != dat))
	{
		parent = p;
		if(dat < p->data.first)
		{
            dir = 1;
			p = p->left;
		}
		else
		{
            dir = -1;
			p = p->right;
		}
	}
    if(!parent && p)
    {
        if(p->right)
        {
            root = p->right;
            root->parent = nullptr;
            auto w = root;
            while(w->left){ w = w->left; }
            w->left = p->left;
            p->left->parent = w;
        }
        else if(p->left)
        {
            root = p->left;
            root->parent = nullptr;
        }
        else root = nullptr;
		this->pool.destroy(p);
    }
	else if(p != nullptr)
	{
		//decrement count of elements in tree
		elements--;
		if(dir == 1)
		{
			//std::cout<<"remove left "<<p->data<<std::endl;
			//Given parent->left = p;
			parent->left = p->right;
            parent->left->parent = parent;

			if(p->right != nullptr)
			{
				node<t, u>* d  = p->right;
				while(d->left != nullptr)
				{
					d = d->left;
				}
				d->left = p->left;
                d->left->parent = d;
			}
			else
            {
				parent->left = p->left;
                parent->left->parent = parent;
            }

			this->pool.destroy(p);
		}
		else
		{
			//std::cout<<"remove right "<<p->data<<std::endl;
			//Given parent->right = p;
			parent->right = p->left;
			if(p->left != nullptr)
			{
                parent->right->parent = parent;
				node<t, u>* d  = p->left;
				while(d != nullptr && d->right != nullptr)
				{
					d = d->right;
				}

				d->right = p->right;
                d->right->parent = d;
			}
			else
            {
				parent->right = p->right;
                if(parent->right)
                    parent->right->parent = parent;
            }

			this->pool.destroy(p);

		}
	}
}
template <typename t, typename u>
node<t, u>& Map<t, u>::set(const t key, const u value)
{
    return this->insert(key, value);
}
template <typename t, typename u>
node<t, u>& Map<t, u>::insert(const t &key, const u &value)
{
	//increment count of elements in tree
	node<t, u> *p = root;
	if(p == nullptr)
	{
		root = this->pool.construct(key, value);
        p = root;
		elements++;
	}
	else
	{
		node<t, u> *last = root;
        int8_t dir = 0;
	    while(p != nullptr && (dir=p->data != key))
	    {
            last = p;
	    	if(p->data > key)
	    	{
                dir = 1;
	    		p = p->left;
	    	}
	    	else
	    	{
                dir = -1;
	    		p = p->right;
	    	}
	    }
        p = last;
		if(dir == 1)
		{
			p->left = this->pool.construct(key, value);
            p->left->parent = p;
            p = p->left;
			elements++;
		}
		else if(dir == -1)
		{
			p->right = this->pool.construct(key, value);
            p->right->parent = p;
            p = p->right;
			elements++;
		}
		p->count++;
	}
    return *p;
}
/*template <typename t, typename u>
Map<t, u>::Map():size(0),elements(0),root(nullptr){}*/
template <typename t, typename u>
Map<t, u>::~Map()
{
	emptyTree();
}
template <typename t, typename u>
void Map<t, u>::emptyTree()
{
    if(this->root)
        this->freeRecursive(this->root);
    
	elements = 0;
}
template <class t, class u>
int Map<t, u>::getElementCount()
{
	return elements;
}
#endif /* Map_H_ */
