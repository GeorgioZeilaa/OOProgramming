#include <quadtree.h>
#include <cstdlib>
#include <iostream>
QuadTree::QuadTree(float _x, float _y, float _width, float _height, int _level, int _maxLevel) :
	x(_x),
	y(_y),
	width(_width),
	height(_height),
	level(_level),
	maxLevel(_maxLevel)
{
	if (level == maxLevel) {
		return;
	}
	//Recursion way for creating a quadtree with top left, right and bottom left, right. The depth depends on the maxlevel.
	NW = new QuadTree(x, y, width / 2.0f, height / 2.0f, level + 1, maxLevel);
	NE = new QuadTree(x + width / 2.0f, y, width / 2.0f, height / 2.0f, level + 1, maxLevel);
	SW = new QuadTree(x, y + height / 2.0f, width / 2.0f, height / 2.0f, level + 1, maxLevel);
	SE = new QuadTree(x + width / 2.0f, y + height / 2.0f, width / 2.0f, height / 2.0f, level + 1, maxLevel);
	numberofObjSubTree = 0;
}

void QuadTree::AddObject(Particle* object)
{
	//printf("level: %d\n", level);
	
	if (level == maxLevel) {
		objects.push_back(object);
		object->setLevel(level);
		return;
	}
	if (contains(NW, object)) {
		NW->numberofObjSubTree++;
		NW->AddObject(object); return;
	}
	else if (contains(NE, object)) {
		NE->numberofObjSubTree++;
		NE->AddObject(object); return;
	}
	else if (contains(SW, object)) {
		SW->numberofObjSubTree++;
		SW->AddObject(object); return;
	}
	else if (contains(SE, object)) {
		SE->numberofObjSubTree++;
		SE->AddObject(object); return;
	}
	if (contains(this, object)) {
		object->setLevel(level);
		objects.push_back(object);
	}
}

bool QuadTree::contains(QuadTree* child, Particle* object)
{
	//printf("child: x: %f  y: %f\n", child->x, child->y);
	return !(object->getPosition().x - object->getRadius() < child->x ||
		object->getPosition().y - object->getRadius() < child->y ||
		object->getPosition().x + object->getRadius() > child->x + child->width ||
		object->getPosition().y + object->getRadius() > child->y + child->height);
}

//void QuadTree::PreOrder(QuadTree* quad) 
//{
//	if (quad == NULL) return;
//	printf("Current Level: %d  x: %f    y: %f    w: %f     h: %f      Number of objects in a sub tree: %d\n", quad->level, quad->x, quad->y, quad->width, quad->height, quad->numberofObjSubTree);
//	if (quad->numberofObjSubTree > 0)
//	{
//		particle = quad->objects;
//	}
//	else
//	{
//		PreOrder(quad->NW);
//		PreOrder(quad->NE);
//		PreOrder(quad->SW);
//		PreOrder(quad->SE);
//	}
//
//}

void QuadTree::PreOrder(QuadTree* quad) 
{
	if (quad != nullptr) 
	{
		//printf("Current Level: %d  x: %f    y: %f    w: %f     h: %f      Number of objects in a sub tree: %d\n", quad->level, quad->x, quad->y, quad->width, quad->height, quad->numberofObjSubTree);
		if (quad->numberofObjSubTree > 0) 
		{

			printf("Current Level: %d  x: %f    y: %f    w: %f     h: %f      Number of objects in a sub tree: %d\n", quad->level, quad->x, quad->y, quad->width, quad->height, quad->numberofObjSubTree);
			//To add the objects can are in one quad and ready for collision detection.
			particle = quad->objects;
		}
		else 
		{
			//return;
		}
		PreOrder(quad->NW);
		PreOrder(quad->NE);
		PreOrder(quad->SW);
		PreOrder(quad->SE);
	}
}

QuadTree::~QuadTree()
{
	//delete NW,NE,SW,SE;
	objects.clear();
	particle.clear();
}