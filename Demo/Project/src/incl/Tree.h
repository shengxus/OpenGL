// encapsulate the tree render as one class
// add definition here

#ifndef TREE_H
#define TREE_H

#include "Object.h"
class Tree:public Object{
public:
	virtual void render();
};
#endif