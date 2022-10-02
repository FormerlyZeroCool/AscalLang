/*
 * unsortedlist.h
 *
 *  Created on: Oct 14, 2019
 *      Author: andrew
 */

#pragma once
#include <iostream>
#include "Pool.hpp"
template <class t>
struct node
{
  t data;
  node<t> *previous;
  node<t> *next;
  node(t d):data(d), previous(NULL), next(NULL) {};
  node(): previous(NULL), next(NULL) {};
};

template <class t>
class iterator
{
  private:
  node<t> *currentNode;
  public:
  iterator(node<t> *starting);
  iterator<t> operator++();
  iterator<t> operator++(int i);
  iterator<t> operator--();
  iterator<t> operator--(int i);
  bool operator==(const iterator<t>);
  bool operator==(const node<t> *o);
  bool operator!=(const iterator<t>);
  bool operator!=(const node<t> *o);
  bool operator!=(const t);
  void operator=(const t data);
  t& operator*();
  t& operator->();
};

template <class t>
void iterator<t>::operator=(const t data)
{
  currentNode->data = data;
}
template <class t>
bool iterator<t>::operator==(const node<t> *o)
{
	return o == currentNode;
}
template <class t>
bool iterator<t>::operator==(const iterator<t> o)
{
  return this->currentNode == o.currentNode;
}
template <class t>
bool iterator<t>::operator!=(const iterator<t> o)
{
  return this->currentNode != o.currentNode;
}
template <class t>
bool iterator<t>::operator!=(const node<t> *o)
{
  return this->currentNode != o;
}

template <class t>
bool iterator<t>::operator!=(const t o)
{
  return this->currentNode->data != o.currentNode->data;
}
template <class t>
  iterator<t>::iterator(node<t> *starting):
  currentNode(starting){}
template <class t>
  iterator<t> iterator<t>::operator++(){
  currentNode = currentNode->next;
  return *this;
  }
template <class t>
  iterator<t> iterator<t>::operator++(int i){
    iterator clone(currentNode);
    currentNode = currentNode->next;
    return clone;
  }
template <class t>
  iterator<t> iterator<t>::operator--(){
    currentNode = currentNode->previous;
    return *this;
  }
template <class t>
  iterator<t> iterator<t>::operator--(int i){

    node<t> *prevNode = currentNode;
    currentNode = currentNode->previous;

    return iterator(prevNode);
  }
template <class t>
  t& iterator<t>::operator*(){
    return currentNode->data;
  }
template <class t>
  t& iterator<t>::operator->(){
    return currentNode->data;
  }






template <class t>
class UnsortedList
{
  private:
  static ObjectPool<node<t> > alloc;
  node<t> *head;
  node<t> *tail;
  long len;

  public:
UnsortedList();
UnsortedList(const UnsortedList<t>&);
~UnsortedList();
void operator=(const UnsortedList<t>&);
bool operator==(const UnsortedList<t>&);
bool isEmpty();
void insertHead(t);
void insertTail(t);
void removeHead();
void removeTail();
void emptyList();
long length();
void printList();
int getLength();
iterator<t> begin();
iterator<t> end();
iterator<t> rbegin();
iterator<t> rend();
};


template <class t>
void UnsortedList<t>::emptyList()
{
	  while(head != nullptr)
	  {
	    node<t>* current = head;
	    head = head->next;
	    alloc.destroy(current);

	  }

	  len = 0;
	  head = nullptr;
	  tail = nullptr;
}

template <class t>
bool UnsortedList<t>::isEmpty()
{
	return (head == nullptr);
}
template <class t>
void UnsortedList<t>::removeHead()
{
	if(head != nullptr)
	{
		len--;
		node<t> *p = head;
		head = head->next;
    if(head)
      head->previous = nullptr;
		alloc.destroy(p);
	}
}
template <class t>
void UnsortedList<t>::removeTail()
{
	if(head != nullptr)
	{
		len--;
		node<t> *p = tail;
		tail = tail->previous;
    if(tail)
      tail->next = nullptr;
		alloc.destroy(p);
	}
}
template <class t>
long UnsortedList<t>::length()
{
	return this->len;
}
template <class t>
bool UnsortedList<t>::operator==(const UnsortedList<t>& other)
{
	node<t> *p = this->head;
	node<t> *o = other.head;
	bool isEqual = true;
	while(p != nullptr && o != nullptr && isEqual)
	{
		if(p->data != o->data)
			isEqual = false;
		p = p->next;
		o = o->next;
	}
	return isEqual;
}
template <class t>
iterator<t> UnsortedList<t>::begin()
{
  return iterator<t>(head);
}
template <class t>
iterator<t> UnsortedList<t>::end()
{
  return iterator<t>(nullptr/*tail->next*/);
}
template <class t>
iterator<t> UnsortedList<t>::rbegin()
{
  return iterator<t>(tail);
}
template <class t>
iterator<t> UnsortedList<t>::rend()
{
  return iterator<t>(nullptr/*head->previous*/);
}


template <class t>
int UnsortedList<t>::getLength()
{
  return len;
}
template <class t>
void UnsortedList<t>::printList()
{
  node<t>* p = head;

  while(p != nullptr)
  {
    std::cout << (int)p->data << " ";
    p = p->next;
  }
  p = tail;
  std::cout<<std::endl;
  /* while(p != nullptr)
  {
  // std::cout << p->data << " ";
    p = p->previous;
  }*/
}

template <class t>
void UnsortedList<t>::insertTail(t item)
{
    if(tail == nullptr)
    {
        tail = alloc.construct(item);
        head = tail;
    }
    else
    {
        node<t> *p = alloc.construct(item);
        tail->next = p;
        p->previous = tail;
        tail = p;
    }
    len++;
}
template <class t>
void UnsortedList<t>::insertHead(t item)
{
    if(head == nullptr)
    {
        head = alloc.construct(item);
        tail = head;
    }
    else
    {
        node<t> *p = alloc.construct(item);
        p->next = head;
        head->previous = p;
        head = p;
    }
    len++;
}
template <class t>
UnsortedList<t>::UnsortedList(const UnsortedList<t> &o)
{
  len = 0;
  head = nullptr;
  tail = nullptr;
  node<t>* currentOther = o.head;
  while(currentOther != nullptr)
  {
    insertTail(currentOther->data);
    currentOther = currentOther->next;
  }
}
template <class t>
UnsortedList<t>::UnsortedList()
{
  len = 0;
  head = nullptr;
  tail = nullptr;
}
template <class t>
UnsortedList<t>::~UnsortedList()
{
  while(head != nullptr)
  {
    node<t>* current = head;
    head = head->next;
    alloc.destroy(current);

  }
  head = nullptr;
  tail = nullptr;
}

template <class t>
void UnsortedList<t>::operator=(const UnsortedList<t> &o)
{

	len = 0;
  if(this->head != o.head && o.head != nullptr)
  {

  node<t>* currentOther = o.head;
  node<t>* currentThis = head;
  node<t>* prev = nullptr;
    while(currentThis!=nullptr && currentOther!=nullptr)
    {
      currentThis->previous = prev;
      currentThis->data = currentOther->data;
      prev = currentThis;
      currentThis = currentThis->next;
      currentOther = currentOther->next;
      len++;
    }
    tail = prev;
    while(currentOther!=nullptr)
    {
      insertTail(currentOther->data);
      currentOther = currentOther->next;
    }
    while(currentThis != nullptr)
    {
      node<t>* current = currentThis;
      currentThis = currentThis->next;
      alloc.destroy(current);
    }
    tail->next = nullptr;
  }
  else
  {
   while(head != nullptr)
    {
      node<t>* current = head;
      head = head->next;
      alloc.destroy(current);
    }
    head = nullptr;
    tail = nullptr;
  }


}




