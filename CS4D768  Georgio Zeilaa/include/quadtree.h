#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "pcontacts.h"
#include <vector>
#include "coreMath.h"

class QuadTree
{
public:
	QuadTree(float x, float y, float width, float height, int level, int maxLevel);
	~QuadTree();

	//To add blobs to the tree.
	void AddObject(Particle* object);

	//A traversal way by using preorder.
	void PreOrder(QuadTree* quad);

	//To be used when checking if there are any objects in a sub tree.
	int numberofObjSubTree;

	//Contains particles which populated in preoder.
	std::vector<Particle*>particle;

private:
	//To store the specifications of a quad.
	float x;
	float y;
	float width;
	float height;
	int level;
	int maxLevel;

	//Contains all the objects of particles in a tree.
	std::vector<Particle*>objects;

	QuadTree* parent;
	QuadTree* NW;
	QuadTree* NE;
	QuadTree* SW;
	QuadTree* SE;

	//To check if this particle should be in this child of the tree.
	bool contains(QuadTree* child, Particle* object);
};

#endif