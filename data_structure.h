#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <iostream>
#include <vector> 

using namespace std;

//------------- define data structure --------------
namespace data_structure
{
	struct Point 
	{
		float x, y; // coordinate
	}; 
	
	struct SoftModule
	{
		string id;            // name
		float origin_area;    // from input file
		float width, height;  // final width and height
	};
	
	struct Shape
	{
		Point shape;  // x represents the width, while y represents the height
		Point index;  // x represents left child's index, y represents right child's index
	};
	
	struct Node // An expression of slicing tree node
	{
		int type;              // 0: operand, 1: vertical, 2: horizontal
		SoftModule *module;    // the data only exist in leaf node
		vector <Shape> shapes; // x represents the width, while y represents the height
		unsigned int index;    // index for top down
		struct Node *left, *right, *parent; // pointers that point to children and parent
	};
	
	typedef Node* NodePtr; // synonym for Node *
	
	struct AddressId // use to store each node's address and its information
	{
		NodePtr address;                // node address
		string id;                      // the node's ID
		int operator_num, operand_num;  // the total number of operands and operators in front of this node
	}; 
	
};

#endif
