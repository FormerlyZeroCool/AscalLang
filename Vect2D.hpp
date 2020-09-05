/*
 * Vect2D.h
 *
 *  Created on: Apr 17, 2020
 *      Author: andrew
 */

#ifndef VECT2D_HPP_
#define VECT2D_HPP_

#include <vector>
template <typename t>
class Vect2D: public std::vector<t> {
private:
	int width,height;
public:
	Vect2D(int width,int height):width(width),height(height){}
	int getWidth()
	{
		return width;
	}
	int getHeight()
	{
		return height;
	}
	t get(int x,int y)
	{
		return (*this)[x+ y*width];
	}
	void insertAt(int x,int y,t value)
	{
		if(this->size()>x+ y*width)
			(*this)[x+ y*width] = value;
		else
		{
			while(this->size()<x+ y*width)
			{
				this->push_back(0);
			}
			this->push_back(value);
		}
	}
};

#endif /* VECT2D_HPP_ */
