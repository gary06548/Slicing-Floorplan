#ifndef FILE_PROCESS_H
#define FILE_PROCESS_H

#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include "data_structure.h"

using namespace std;
using namespace data_structure;

//------- read input file and write output file -------
class File_Process 
{
private:
	// transfer sentence to substrings
	vector <string> sentence_to_substrings(string input) // Ex: save "Hello World" as "Hello" and "World" 
	{
		vector <string> output;
		int start_pos = 0;
	
		for(int i=0; i<input.length(); i++) 
		{
			if(input[i] == ' ')
			{
				output.push_back( input.substr(start_pos, i-start_pos) );
				start_pos = i+1;
			}
			else if(i == input.length()-1) // final character -> save word and break the loop
			{
				output.push_back( input.substr(start_pos, i+1-start_pos) );
				break;
			}
		}
	
		return output;
	}

	// transfer string to integer number
	int StoI(string num)
	{
		int out = 0;
		
		for(unsigned int i=0; i<num.length(); i++)
			out = out*10 + (int)num[i] - 48;
		
		return out;
	}

public:
	// read input file
	void in_file(vector < vector <string> > &modules_data , char *argv)
	{
		ifstream infile(argv, ios::in);
		string in;
		vector <string> substring_data;
		
		if(!infile)
		{
			cerr<<"File could not be opened"<<endl;
			exit(1);
		}
		else
		{
			while(!infile.eof())
			{
				getline(infile,in);
				substring_data = sentence_to_substrings(in);
				modules_data.push_back(substring_data);
			}
		}
		
		modules_data.pop_back(); // remove the empty element
	}
	
	// write output file
	void out_file(vector <NodePtr> operands_address, vector <AddressId> solution, Point best_shape, char *argv) 
	{
		ofstream outfile (argv, ios::out);
		NodePtr root = solution[ solution.size() - 1 ].address;
		
		outfile<<fixed<<setprecision(4)<<best_shape.x<<" "<<best_shape.y<<" "<<best_shape.x * best_shape.y<<endl;
		
		unsigned int index = 0, target = 0;
		
		// output all modules' width and height
		while( !operands_address.empty() )
		{
			if( StoI( operands_address[index]->module->id )  == target)
			{
				outfile<<fixed<<setprecision(8)<<operands_address[index]->module->width<<" ";
				outfile<<operands_address[index]->module->height<<endl;
				
				operands_address.erase(operands_address.begin() + index);
				target++;
				index = 0;
			}
			else
			{
				index++;
			}
		}
		
		for(unsigned int i=0; i<operands_address.size(); i++)
		{
			outfile<<fixed<<setprecision(8)<<operands_address[i]->module->width<<" "<<operands_address[i]->module->height<<endl;
		}
		
		for(unsigned int i=0; i<solution.size(); i++)
		{
			if(i != solution.size() - 1)
				outfile<<solution[i].id<<" ";
			else
				outfile<<solution[i].id;
		}
	}

};

#endif




