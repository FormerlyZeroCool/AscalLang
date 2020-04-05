
#include <iostream>
#include <vector>
template <typename t>
struct singlelinknode
{
  t data;
  singlelinknode<t> *next;
  singlelinknode(t d): data(d),next(NULL) {};
  singlelinknode(): next(NULL) {};
};
template <typename t>
class linkedStack
{
    private:
    singlelinknode<t> *head;
    int len;
    public:
    ~linkedStack();
    linkedStack();
    linkedStack(const linkedStack<t> &o);
    linkedStack(t data);
    void operator=(const linkedStack<t> &o);
    void top(t &data);
    void push(t data);
    void empty();
    void pop();
    bool isEmpty();
    int length();
};
template <typename t>
   void linkedStack<t>::empty(){
      while(head != nullptr){
    singlelinknode<t> *p = head->next;
    delete head;
    head = p;
      }
    }
template <typename t>
    linkedStack<t>::~linkedStack(){
      empty();
    }
template <typename t>
    linkedStack<t>::linkedStack(){
      head = nullptr;
      len = 0;
    }

template <typename t>
void linkedStack<t>::operator=(const linkedStack<t> &o)
{
	empty();
	len = 0;
    head = nullptr;
    singlelinknode<t> *op = o.head;
    while(op != nullptr){
      push(op->data);
      op = op->next;
    }
}
template <typename t>
    linkedStack<t>::linkedStack(const linkedStack<t> &o){
	  len = 0;
      head = nullptr;
      singlelinknode<t> *op = o.head;
      while(op != nullptr){
        push(op->data);
        op = op->next;
      }

    }
template <typename t>
    linkedStack<t>::linkedStack(t data){
      head = new singlelinknode<t>(data);
      len = 1;
    }
template <typename t>
    void linkedStack<t>::top(t &data){
      if(head != nullptr)
        data = head->data;
    }
template <typename t>
    void linkedStack<t>::push(t data){
      len++;
      singlelinknode<t> *p = new singlelinknode<t>(data);
      p->next = head;
      head = p;

    }
template <typename t>
    void linkedStack<t>::pop(){
if(head != nullptr)
{
  len--;
  singlelinknode<t> *p = head->next;
  delete head;
  head = p;
}
else
{
  std::cout<<"Stack is empty! Can't pop.";
}

    }
template <typename t>
    bool linkedStack<t>::isEmpty(){
return head == nullptr;
    }

template <typename t>
  int linkedStack<t>::length()
  {
    return len;
  }

template <typename t>
class stack : public std::vector<t>{

public:
	stack(){}
	void push(t data);
	void pop();
	void top(t&data);
	bool isEmpty();
	int length();
};

template <typename t>
void stack<t>::push(t data)
{
	this->push_back(data);
}
template <typename t>
void stack<t>::pop()
{
	if(this->size() > 0)
	{
		this->pop_back();
	}
}
template <typename t>
bool stack<t>::isEmpty()
{
	return this->size() == 0;
}
template <typename t>
int stack<t>::length()
{
	return this->size();
}
template <typename t>
void stack<t>::top(t&data)
{
	if(this->size() > 0)
	{
		data = (*this)[this->size()-1];
	}
}
