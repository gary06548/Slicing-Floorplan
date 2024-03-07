#ifndef SLICING_TREE_H
#define SLICING_TREE_H

#include <vector>
#include <cmath>
#include <stack>
#include <algorithm>
#include "data_structure.h"

using namespace std;
using namespace data_structure;

//------------------  create slicing tree  ------------------
class Slicing_Tree 
{
private:
	vector <SoftModule> modules; // private member, store all modules' information
	
	// transfer string to float
	float s_to_f(string str)
	{
		float output=0;
		
		for(unsigned int i=0; i<str.length(); i++)
		{											 // "(int)input[i]-48" can change char to number 
			output = output*10 + (float)str[i] - 48; // 0's ASCII = 48, 1's ASCII = 49 ...
		}
		
		return output;
	} 
	
	// use the data from input file to save the data of all soft modules
	vector <SoftModule> module_process(vector < vector <string> > modules_data)
	{
		vector <SoftModule> output;
		
		for(unsigned int i=1; i<modules_data.size(); i++) // modules_data[0] is the number of soft modules 
		{
			SoftModule tmp;
			tmp.id = modules_data[i][0];
			tmp.origin_area = s_to_f( modules_data[i][1] );
			output.push_back(tmp);
		}
		
		return output;
	}
	
	// the function to create a new node
	NodePtr newnode(int type)
	{
		NodePtr temp = new Node;
		temp->type = type;
		temp->left = NULL;
		temp->right = NULL;
		temp->parent = NULL;
		temp->module = NULL;
		temp->index = -1; // invalid value
		return temp;
	}
	
	// create a shape by area and specified aspect ratio
	Point aspect_ratio(float area, float alpha)
	{
		Point out_pt;
		area *= 1.00001;
		
		// if the number larger than 100, set them fixed shapes
		if(modules.size() >= 100)
		{
			if(area > 2501 * 1.0001)     out_pt.x = area / 70.72836, out_pt.y = 70.72836;
			else if(area > 625 * 1.0001) out_pt.x = area / 35.36418, out_pt.y = 35.36418;
			else if(area > 156 * 1.0001) out_pt.x = area / 17.68209, out_pt.y = 17.68209;
			else                         out_pt.x = area / 8.841045, out_pt.y = 8.841045;
		}
		else // otherwise, determine its shape by aspect ratio
		{
			out_pt.x = sqrt(area / alpha);
			out_pt.y = alpha * out_pt.x;
		}

		return out_pt;
	}
	
	// create aspect ratio shape
	vector <Shape> area_to_shape(float area)
	{
		vector <Shape> output;
		Shape temp;
		
		if(modules.size() >= 100)
		{
			temp.shape = aspect_ratio(area, 1); // "1" here is useless
			temp.index.x = -1, temp.index.y = -1; // invalid value
			output.push_back(temp);
		}
		else
		{
			for(float alpha = 1.999; alpha >= 0.501; alpha -= 0.001) // alpha is aspect ratio
			{
				temp.shape = aspect_ratio(area, alpha);
				temp.index.x = -1, temp.index.y = -1; // invalid value
				output.push_back(temp);
			}
		}
		
		return output;
	}

	// compare area, return true if node_1's area is smaller then node_2, otherwise return false 
	static bool compare_area(SoftModule m1, SoftModule m2)
	{
		return m1.origin_area < m2.origin_area;
	}

	// merge from the shape of children's 
	// mode 0: vertical merge, mode 1: horizontal merge
	void shape_merge(NodePtr ptr) 
	{
	    vector <Shape> L_shape = ptr->left->shapes, R_shape = ptr->right->shapes, merge_shape;
	    bool mode = ptr->type - 1; // ptr->type 1: vertical, 2: horizontal
	    
	    if(!mode) // vertical merge -> x0 < x1 < x2 <...
	    {
	    	int i = 0, j = 0;
	    	bool L_increase_x = true, R_increase_x = true;
	    	
	    	if(L_shape.size() >= 2) L_increase_x = L_shape[0].shape.x < L_shape[1].shape.x;
	    	if(R_shape.size() >= 2) R_increase_x = R_shape[0].shape.x < R_shape[1].shape.x;
			
			if(!L_increase_x) i = L_shape.size() - 1; // traverse the data from back of the vector
			if(!R_increase_x) j = R_shape.size() - 1; // because the latter elements' x are larger
	    	
	    	while(true)
		    {
		    	Shape merge_pt;
		    	
	    		merge_pt.shape.x = L_shape[i].shape.x + R_shape[j].shape.x;
		    	merge_pt.index.x = i; // left index
				merge_pt.index.y = j; // right index
		    	
		    	if(L_shape[i].shape.y > R_shape[j].shape.y)
		    	{
		    		merge_pt.shape.y = L_shape[i].shape.y;
		    		(L_increase_x)? i++ : i--;
				}
				else
				{
					merge_pt.shape.y = R_shape[j].shape.y;
		    		(R_increase_x)? j++ : j--;
				}
				
		    	merge_shape.push_back(merge_pt);
		    	
		    	if(L_increase_x && i >= L_shape.size()) break;
		    	else if( (!L_increase_x) && i < 0 ) break;
		    	else if(R_increase_x && j >= R_shape.size()) break;
		    	else if( (!R_increase_x) && j < 0 ) break;
			}
		}
		else // horizontal merge -> y0 < y1 < y2 <...
		{
			int i = L_shape.size() - 1, j = R_shape.size() - 1;
	    	bool L_decrease_y = true, R_decrease_y = true;
	    	
	    	if(L_shape.size() >= 2) L_decrease_y = L_shape[0].shape.y > L_shape[1].shape.y;
	    	if(R_shape.size() >= 2) R_decrease_y = R_shape[0].shape.y > R_shape[1].shape.y;
			
			if(!L_decrease_y) i = 0; // traverse the data from front of the vector
			if(!R_decrease_y) j = 0; // because the former elements' y are smaller
					
	    	while(true)
		    {
		    	Shape merge_pt;
		    	
				merge_pt.shape.y = L_shape[i].shape.y + R_shape[j].shape.y;
		    	merge_pt.index.x = i; // left index
				merge_pt.index.y = j; // right index
				
		    	if(L_shape[i].shape.x > R_shape[j].shape.x)
		    	{
		    		merge_pt.shape.x = L_shape[i].shape.x;
		    		(L_decrease_y)? i-- : i++;
				}
				else
				{
					merge_pt.shape.x = R_shape[j].shape.x;
		    		(R_decrease_y)? j-- : j++;
				}
				
		    	merge_shape.push_back(merge_pt);
		    	
		    	if(L_decrease_y && i < 0) break;
		    	else if( (!L_decrease_y) && i >= L_shape.size()) break;
		    	else if(R_decrease_y && j < 0) break;
		    	else if( (!R_decrease_y) && j >= R_shape.size() ) break;
			}
		}
		
		ptr->shapes = merge_shape;
	}

public:	
	// constructor -> initialize self private mamber: modules
	Slicing_Tree(vector < vector <string> > modules_data)
	{
		modules = module_process(modules_data);
	}
	
	// construct the tree in the polish expression(12V3V4V5...) and return the root 
	vector <AddressId> buildtree(vector <NodePtr> &operands)
	{
		vector <AddressId> output;        // store each node's address and information 
		stack <NodePtr> stk;              // use to store pointers to create slicing tree
		NodePtr ptr, ptr_right, ptr_left; // ptr: current pointer, ptr_right and ptr_left point to children
		AddressId tmp;                    // temporary data
		bool direction_flag = false;      // false: H, true: V
		bool vertical_flag = false;       // false: don't execute additional vertical merge, true: execute
		bool first_vertical = false;      // determine whether vertical merge has happened or not
		int h1_num = 0, h2_num = 0, h3_num = 0, h4_num = 0; // record the number of each type
		int boundary1 = 0, boundary2 = 0, boundary3 = 0;    // these boundaries divide the procedure into 4 parts
		int h1_remain = 0, h2_remain = 0, h3_remain = 0;    // remain number of each type
		int h2_bias = 0, h3_bias = 0, h4_bias = 0;          // bias of each type
				
		tmp.operand_num = 0, tmp.operator_num = 0; // initialize the number of operand and operator to 0
		
		//----------------------------- mode 1 ---------------------------
		if(modules.size() >= 100)
		{
			sort(modules.begin(), modules.end(), compare_area); // small to big
		
			for(unsigned int i=0; i<modules.size(); i++)
			{
				if(modules[i].origin_area > 2501)      h4_num++;
				else if(modules[i].origin_area > 625)  h3_num++;
				else if(modules[i].origin_area > 156)  h2_num++;
				else                                   h1_num++;
			}
			
			boundary1 = h1_num + h2_num;
			boundary2 = boundary1 + h3_num;
			boundary3 = boundary2 + h4_num;
			h1_remain = h1_num % 16; 
			h2_bias = 8 - (h1_remain + 1) / 2;
			
			h2_remain = (h2_num - h2_bias) % 8;
			h3_bias = (h2_remain >= 0)? 4 - (h2_remain + 1) / 2 : 4 - (h2_remain + 9) / 2; // 9 = 1 + 8
			
			h3_remain = (h3_num - h3_bias) % 4;
			h4_bias = (h3_remain >= 0)? 2 - (h3_remain + 1) / 2 : 2 - (h3_remain + 5) / 2; // 5 = 1 + 4
			
			for(unsigned int i=0; i<modules.size(); i++)
			{
				ptr = newnode(0); // 0: operand 
				stk.push(ptr);
				ptr->module = &modules[i];
				ptr->shapes = area_to_shape(ptr->module->origin_area);
				
				tmp.address = ptr, tmp.id = ptr->module->id, tmp.operand_num += 1;
				output.push_back(tmp);
				operands.push_back(ptr);
				
				if(i < h1_num)
				{
					if( (i + 1) % 16 == 0 ) 
					{
						if(first_vertical) vertical_flag = true;
						else first_vertical = true;
					}
					else if( i % 16 == 0 )
					{
						if(i == h1_num - 1) 
						{
							if(h1_remain == 1)
							{
								continue;
							}
							else
							{
								if(first_vertical) vertical_flag = true;
								else first_vertical = true;
								
								goto vertical;
							}
						}
						else continue;
					}
					
					if(i == h1_num - 1)
					{
						if( (h2_bias == 0 && h2_num != 0) 
						 || (h3_bias == 0 && h3_num != 0 && h2_num == 0)
						 || (h4_bias == 0 && h4_num != 0 && h3_num == 0 && h2_num == 0) )
						{
							if(first_vertical) vertical_flag = true;
							else first_vertical = true;
						}
					}
				}
				else if(i < boundary1)
				{	
					if( (i - h1_num - h2_bias + 1) % 8 == 0 ) 
					{
						if(first_vertical) vertical_flag = true;
						else first_vertical = true;
					}
					else if( (i - h1_num - h2_bias) % 8 == 0 ) 
					{
						if(i == boundary1 - 1) 
						{
							if(h2_remain == 1)
							{
								continue;
							}
							else
							{
								vertical_flag = true;
								goto vertical;
							}
						}
						else continue;
					}
					
					if(i == boundary1 - 1) 
					{
						if( (h3_bias == 0 && h3_num != 0)
						 || (h4_bias == 0 && h4_num != 0 && h3_num == 0) )
						{
							if(first_vertical) vertical_flag = true;
							else first_vertical = true;
						}
					}
				}
				else if(i < boundary2)
				{
					if( (i - boundary1 - h3_bias + 1) % 4 == 0 ) 
					{
						if(first_vertical) vertical_flag = true;
						else first_vertical = true;
					}
					else if( (i - boundary1 - h3_bias) % 4 == 0 ) 
					{
						if(i == boundary2 - 1) 
						{
							if(h3_remain == 1)
							{
								continue;
							}
							else
							{
								vertical_flag = true;
								goto vertical;
							}
						}
						else continue;
					}
					
					if(i == boundary2 - 1) 
					{
						if(h4_bias == 0 && h4_num != 0)
						{
							if(first_vertical) vertical_flag = true;
							else first_vertical = true;
						}
					}
				}
				else if(i < boundary3)
				{
					if( (i - boundary2 - h4_bias + 1) % 2 == 0 ) 
					{
						if(first_vertical) vertical_flag = true;
						else first_vertical = true;
					}
					else if( (i - boundary2 - h4_bias) % 2 == 0 )
					{
						if(i == boundary3 - 1) 
						{
							vertical_flag = true;
							goto vertical;
						}
						else continue;
					}
				}
				
				if(stk.size() >= 2)
				{
					ptr = (direction_flag)? newnode(1) : newnode(2); // 1: vertical, 2: horizontal
			
					// Pop two top nodes
					ptr_right = stk.top(); // Store top
					stk.pop();	 // Remove top
					ptr_left = stk.top();
					stk.pop();
		
					// make them children and update the node's PE
					ptr->right = ptr_right; 
					ptr->left = ptr_left;
					ptr_right->parent = ptr;
					ptr_left->parent = ptr;
		
					stk.push(ptr); // Add this subexpression to stack
					shape_merge(ptr);
					
					tmp.id = (direction_flag)? "V" : "H";
					tmp.address = ptr, tmp.operator_num += 1;
					output.push_back(tmp);
				}
				
	vertical:
				if(vertical_flag)
				{
					vertical_flag = false;
					
					ptr = newnode(1); // 1: vertical, 2: horizontal
					// Pop two top nodes
					ptr_right = stk.top(); // Store top
					stk.pop();	 // Remove top
					ptr_left = stk.top();
					stk.pop();
					
					// make them children and update the node's PE
					ptr->right = ptr_right; 
					ptr->left = ptr_left;
					ptr_right->parent = ptr;
					ptr_left->parent = ptr;
		
					stk.push(ptr); // Add this subexpression to stack
					shape_merge(ptr);
					
					tmp.address = ptr, tmp.id = "V", tmp.operator_num += 1;
					output.push_back(tmp);
				}
			}
			
			while(stk.size() != 1)
			{
				ptr = newnode(1); // 1: vertical, 2: horizontal
				// Pop two top nodes
				ptr_right = stk.top(); // Store top
				stk.pop();	 // Remove top
				ptr_left = stk.top();
				stk.pop();
				
				// make them children and update the node's PE
				ptr->right = ptr_right; 
				ptr->left = ptr_left;
				ptr_right->parent = ptr;
				ptr_left->parent = ptr;
	
				stk.push(ptr); // Add this subexpression to stack
				shape_merge(ptr);
				
				tmp.address = ptr, tmp.id = "V", tmp.operator_num += 1;
				output.push_back(tmp);
			}
		}
		else //----------------------------- mode 2 ---------------------------
		{
			for(unsigned int i=0; i<modules.size(); i++)
			{
				ptr = newnode(0); // 0: operand
				stk.push(ptr); 
				ptr->module = &modules[i];
				ptr->shapes = area_to_shape(ptr->module->origin_area);
				
				tmp.address = ptr, tmp.id = ptr->module->id, tmp.operand_num += 1;
				output.push_back(tmp);
				operands.push_back(ptr);
				
				if(stk.size() >= 2)
				{
					ptr = newnode(1); // 1: vertical
		
					// Pop two top nodes
					ptr_right = stk.top(); // Store top
					stk.pop();	 // Remove top
					ptr_left = stk.top();
					stk.pop();
		
					// make them children and update the node's PE
					ptr->right = ptr_right; 
					ptr->left = ptr_left;
					ptr_right->parent = ptr;
					ptr_left->parent = ptr;
		
					stk.push(ptr); // Add this subexpression to stack
					shape_merge(ptr);
					
					tmp.address = ptr, tmp.id = "V", tmp.operator_num += 1;
					output.push_back(tmp);
				}
			}
		}
		
		return output; // the pointer direct to the root
	}
	
};

#endif



