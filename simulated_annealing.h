#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <vector>
#include <cstdlib>
#include "data_structure.h"
#include "slicing_tree.h" 

using namespace std;
using namespace data_structure;

//-------- execute simulated annealing by slicing tree --------
class Simulated_Annealing 
{
private:
	// randomly select 2 index of adjacent operands	
	void rand_adjacent_operands(unsigned int &index1, unsigned int &index2, vector <AddressId> nodes_address)
	{
		while(true)
		{
			index2 = rand() % nodes_address.size();
			if(index2 < 2) continue;
			
			if(nodes_address[index2].address->type == 0) // operand
			{
				index1 = index2 - 1;
				
				while(true)
				{
					if(nodes_address[index1].address->type == 0) break;
					else index1--;
				}
				
				break;
			}
		}
	}
	
	// randomly select 1 index of an operator (H or V)
	void rand_operator(unsigned int &index, vector <AddressId> nodes_address)
	{
		while(true)
		{
			index = rand() % nodes_address.size();
			
			if(nodes_address[index].address->type != 0) // operator
			{
				while(true)
				{
					if(nodes_address[index - 1].address->type == 0) break;
					else index--;
				}
				
				break;
			}
		}
	}
	
	// randomly select 2 index of adjacent operand and operator
	// true: find index successfully, false: cannot find
	bool rand_operator_operand(unsigned int &index1, unsigned int &index2, vector <AddressId> nodes_address)
	{
		int count = 0;
		
		while(true)
		{
			index2 = rand() % nodes_address.size();
			
			if(index2 < 2) continue;
			
			index1 = index2 - 1; // index2 is bigger than index1
			
			if(nodes_address[index1].address->type != 0 && nodes_address[index2].address->type == 0) 
			{   
				// ..."V3"... -> check whether adjacent operator are same or not
				if(index2 + 1 < nodes_address.size())
				{
					if(nodes_address[index2 + 1].address->type != nodes_address[index1].address->type) break;
				}
			}
			else if(nodes_address[index1].address->type == 0 && nodes_address[index2].address->type != 0) 
			{   
				// ..."3V"... -> need to check additional condition (operator_num < operand_num)
				// check whether adjacent operator are same or not
				if(nodes_address[index1 - 1].address->type != nodes_address[index2].address->type)
				{
					// check whether operand_num is greater than operator_num after changing
					if(nodes_address[index1].operand_num - 1 > nodes_address[index1].operator_num + 1) break;
				}	
			}
			
			if(count > 20) return false;
			else count++;
		}
		
		return true;
	}
	
	// calculate the minimum area of the slicing tree
	float cost_evaulation(NodePtr root, Point &best_shape)
	{
		vector <Shape> shapes = root->shapes;
		float cost = shapes[0].shape.x * shapes[0].shape.y, tmp_cost = 0;
		
		if(shapes.size() == 1)
		{
			best_shape.x = shapes[0].shape.x;
			best_shape.y = shapes[0].shape.y;
			root->index = 0;
			return cost;
		}
		
		for(unsigned int i = 0; i < root->shapes.size(); i++)
		{
			tmp_cost = shapes[i].shape.x * shapes[i].shape.y;
			
			if(tmp_cost < cost) 
			{
				cost = tmp_cost;
				best_shape.x = shapes[i].shape.x;
				best_shape.y = shapes[i].shape.y;
				root->index = i;
			}
		}
		
		return cost;
	}
	
	// select which movement to conduct
	void selece_move(int &movecase)
	{
		float E0, E1; // energy of each selection (larger energy for larger probability selection)
		float probability = (float) (rand() % 100) / 100; // 0% ~ 99%, increment = 1%
		
		E0 = 0.33, E1 = 0.33; // E2 = 1 - E0 - E1 = 0.34
		
		if(probability <= E0)           movecase = 0;
		else if(probability <= E0 + E1) movecase = 1;
		else                            movecase = 2;
	}

	// determine each leaf node's shape
	void top_down(NodePtr ptr)
	{
		if(ptr != NULL)
		{
			// top down: determine children's index
			if(ptr->type != 0) // operator
			{
				ptr->left->index = ptr->shapes[ ptr->index ].index.x;
				ptr->right->index = ptr->shapes[ ptr->index ].index.y;
				
				// traverse the slicing tree in post order
				top_down(ptr->left);
				top_down(ptr->right);
			}
			else // operand
			{
				ptr->module->width = ptr->shapes[ ptr->index ].shape.x;
				ptr->module->height = ptr->shapes[ ptr->index ].shape.y;
			}
		}
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

	//============================ movement ============================
	// M1 : swap operand 1 and operand 2
	// index1 is the front operand, while index2 is the back operand (in NPE)
	void Move1(unsigned int index1, unsigned int index2, vector <AddressId> &nodes_address)
	{
		NodePtr ptr1 = nodes_address[index1].address, ptr2 = nodes_address[index2].address;
		NodePtr parent1 = ptr1->parent, parent2 = ptr2->parent;
		
		// update Normalized Polish Expression
		swap(nodes_address[index1].id, nodes_address[index2].id);
		swap(nodes_address[index1].address, nodes_address[index2].address);
		
		if(parent1 == parent2) // these two nodes have same parent
		{
			parent1->left = ptr2;
			parent1->right = ptr1;
			shape_merge(parent1);
		}
		else
		{
			if(parent1->left == ptr1)
				parent1->left = ptr2;
			else
				parent1->right = ptr2;
			ptr2->parent = parent1;
			
			if(parent2->left == ptr2)
				parent2->left = ptr1;
			else
				parent2->right = ptr1;
			ptr1->parent = parent2;
			
			// update nodes' shape until meet the root of the slicing tree
			while(parent1 != NULL)
			{
				shape_merge(parent1);
				parent1 = parent1->parent;
			}
				 
			while(parent2 != NULL)
			{
				shape_merge(parent2);
				parent2 = parent2->parent;
			}
		}
	}
	
	// M2 : complement the operator chain for given order
	void Move2(unsigned int index, vector <AddressId> &nodes_address)
	{
		bool done = false;
		unsigned int count = 0; // the number of changed nodes
		NodePtr ptr = nodes_address[index].address, pre_ptr = NULL;
		
		while(ptr != NULL)
		{
			if(pre_ptr == ptr->left) done = true;
			
			if(!done) 
			{
				ptr->type = (ptr->type == 1)? 2 : 1;
				count++;
			}
			
			shape_merge(ptr);
			
			pre_ptr = ptr;
			ptr = ptr->parent;
		}
		
		// update Normalized Polish Expression
		for(unsigned int i=0; i<count; i++)
			nodes_address[index + i].id = (nodes_address[index + i].id == "V")? "H" : "V";
	}
	
	// M3 : swap adjacent operand and operator
	void Move3(unsigned int index1, unsigned int index2, vector <AddressId> &nodes_address)
	{
		NodePtr operator_ptr = NULL, operand_ptr = NULL;
		bool done = false;
		
		if(nodes_address[index1].address->type != 0) // index1 is an operator, index2 is an operand
		{
			operator_ptr = nodes_address[index1].address;
			operand_ptr = nodes_address[index2].address;
			
			// update Normalized Polish Expression
			nodes_address[index1].operand_num++;
			nodes_address[index1].operator_num--;
		}
		else // index1 is an operand, index2 is an operator
		{
			operator_ptr = nodes_address[index2].address;
			operand_ptr = nodes_address[index1].address;
			
			// update Normalized Polish Expression
			nodes_address[index1].operand_num--;
			nodes_address[index1].operator_num++;
		}
		
		// update Normalized Polish Expression
		swap(nodes_address[index1].id, nodes_address[index2].id);
		swap(nodes_address[index1].address, nodes_address[index2].address);
		
		if(operand_ptr->parent == operator_ptr) // first meet operand, then operator
		{
			NodePtr cur_ptr = operator_ptr->parent, pre_ptr = operator_ptr;
			
			while(cur_ptr != NULL)
			{
				if(pre_ptr == cur_ptr->right && !done) // change direction: right_bottom -> left top
				{
					if(cur_ptr->right == operator_ptr)
					{
						cur_ptr->right = operator_ptr->right;
						operator_ptr->right->parent = cur_ptr;
					}
					
					operator_ptr->right = operator_ptr->left;
					operator_ptr->left = cur_ptr->left;
					cur_ptr->left->parent = operator_ptr;
					shape_merge(operator_ptr); // update target node's shape
					
					cur_ptr->left = operator_ptr;
					operator_ptr->parent = cur_ptr;
					done = true;
				}
				else
				{
					if(cur_ptr->left == operator_ptr)
					{
						cur_ptr->left = operator_ptr->right;
						operator_ptr->right->parent = cur_ptr;
					}
					else if(cur_ptr->right == operator_ptr)
					{
						cur_ptr->right = operator_ptr->right;
						operator_ptr->right->parent = cur_ptr;
					}
				}
							
				shape_merge(cur_ptr);
				pre_ptr = cur_ptr;
				cur_ptr = cur_ptr->parent;
			}
		}
		else // first meet operator, then operand
		{
			NodePtr operator_parent = operator_ptr->parent; // operator_ptr must be its parent's left child
			NodePtr operand_parent = operand_ptr->parent;   // operand_ptr must be its parent's left child
			
			// update operator's parent's left child
			operator_parent->left = operator_ptr->left;
			operator_ptr->left->parent = operator_parent;
			
			// update operator's left and right child
			operator_ptr->left = operator_ptr->right;
			operator_ptr->right = operand_ptr;
			operand_ptr->parent = operator_ptr;
			
			// update operand's parent's left child
			if(operator_parent != operand_parent)
				operand_parent->left = operator_ptr;
			else
				operand_parent->right = operator_ptr;
				
			operator_ptr->parent = operand_parent;
			
			// update each node's shape
			NodePtr cur_ptr = operator_ptr;
			
			while(cur_ptr != NULL)
			{
				shape_merge(cur_ptr);
				cur_ptr = cur_ptr->parent;
			}
		}
	}

public:	
	// Simulated Annealing
	vector <AddressId> SA(vector <AddressId> nodes_address, Point &best_shape)
	{
		float T0 = 1;                           // initial temperature
		float T = T0;                           // temperature for iteration
		int movecase = 0;                       // move case
		int movetime = 0;                       // number of movements
		int uphill = 0;                         // number of climb
		int N = (nodes_address.size() + 1);     // self defined parameter
		int reject = 0;                         // number of rejection
		unsigned int index1 = 0;                // index of nodes_address
		unsigned int index2 = 0;                // index of nodes_address
		float pre_cost = 0, cur_cost = 0;       // cost
		float uphill_probability = 0;           // uphill probability
		Point shape;                            // best shape of each iteration  
		NodePtr root = nodes_address[nodes_address.size()-1].address; // final element is the root of this slicing tree
		vector <AddressId> best_solution = nodes_address;             // initial best solution is origin expression
		float best_cost = cost_evaulation(root, best_shape);          // initial cost
		
		// determine each node's width and height by initial NPE
		top_down(root);
		pre_cost = best_cost;
		
		// if the number of modules larger than 100, skip simulated annealing
		if(nodes_address.size() >= 100) return best_solution;
		
		//======================  Simulated Annealing =======================
		while(true)
		{
			movetime = 0, uphill = 0, reject = 0;
			
			while(true)
			{
				bool movedone = false;
				selece_move(movecase);
				
				switch(movecase)
				{
					case 0: // move 1
						rand_adjacent_operands(index1, index2, nodes_address);
						Move1(index1, index2, nodes_address);
						break;
					case 1: // move 2
						rand_operator(index1, nodes_address);
						Move2(index1, nodes_address);
						break;
					case 2: // move 3
						if( rand_operator_operand(index1, index2, nodes_address) )
						{
							Move3(index1, index2, nodes_address);
							movedone = true;
						}
						break;
					default: // invalid condition of movement 
						cout<<"Error !!!"<<endl;
						break;
				}
				
				movetime++;
				cur_cost = cost_evaulation(root, shape);
				uphill_probability = (float)(rand() % 100) / 100; // 0% ~ 99%, increment is 1%
				
				float normalize_cost = (pre_cost - cur_cost) / pre_cost;
				
				if(cur_cost < pre_cost || uphill_probability < exp(normalize_cost / T)) 
				{
					if(cur_cost > pre_cost) // worse -> uphill
					{
						uphill++;
					}
					
					pre_cost = cur_cost;
						
					if(pre_cost < best_cost)// better -> update best solution
					{
						best_solution = nodes_address;
						top_down(root);
						best_shape = shape;
						best_cost = pre_cost;
					}
				}
				else
				{
					reject++;
					
					// return back to previous state
					switch(movecase)
					{
						case 0: // move 1
							Move1(index1, index2, nodes_address);
							break;
						case 1: // move 2
							Move2(index1, nodes_address);
							break;
						case 2: // move 3
							if(movedone)
								Move3(index1, index2, nodes_address);
							break;
						default: // invalid condition of movement 
							cout<<"Error !!!"<<endl;
							break;
					}
					
				}
				
				if( uphill > N || movetime > 5 * N ) break; 
			}
			
			T = (T < 0.005)? 0.78 * T : 0.85 * T;
			
			if( (reject/movetime > 0.95) || (T < 0.00001) ) break; // or out of time 
		}
		
		return best_solution;
	}

};

#endif




