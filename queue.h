/*
 * queue.h
 *
 *  Created on: Oct 16, 2019
 *      Author: andrew
 */

#ifndef QUEUE_H_
#define QUEUE_H_



#include <iostream>
template <typename t>
class queue
{
    private:
    t *data;
    int front = 0;
    int rear = 0;
    int len;
    int size;
    public:
    ~queue();
    queue(int = 5);
    queue(const queue<t> &o);
    void operator=(const queue<t> &o);
    void top(t &data);
    void push(t data);
    void deque();
    bool isEmpty();
    bool isFull();
    int length();
};
template <typename t>
queue<t>::~queue(){
	delete [] data;
    }
template <typename t>
queue<t>::queue(int size):size(size){
      data = new t[size];
      len = 0;
    }

template <typename t>
queue<t>::queue(const queue<t> &o){
	len = 0;
	data = new t[o.size];
	size = o.size;
    t *op = o.data;
    int counter = 0;
    while(counter<o.size){
      push(op[counter]);
      counter++;
    }
    }
template <typename t>
void queue<t>::operator=(const queue<t> &o)
{
	delete [] data;
	len = 0;
	size = o.size;
    t *op = o.data;
    int counter = 0;
    while(counter<o.size){
      push(op[counter]);
      counter++;
    }
}
template <typename t>
    void queue<t>::top(t &dat){
      if(!isEmpty())
        dat = data[front];
    }
template <typename t>
    void queue<t>::push(t dat){
	if(!isFull()){
      len++;
      int index = rear++%size;
      data[index] = dat;
	}
	else
	{
		std::cout<<"Queue full can't add"<<std::endl;
	}

    }
template <typename t>
    void queue<t>::deque(){
	if(!isEmpty())
	{
		len--;
		front = (++front) %size;
	}
	else
	{
		std::cout<<"Queue is empty! Can't dequeue."<<std::endl;
	}

    }
template <typename t>
    bool queue<t>::isEmpty()
	{
	return rear == front;
    }
template <typename t>
    bool queue<t>::isFull()
	{
		return rear-front == -1 || (rear == size-1 && front == 0);
    }

template <typename t>
  int queue<t>::length()
  {
    return len;
  }



#endif /* QUEUE_H_ */
