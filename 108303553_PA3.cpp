#include <vector>
#include "file_process.h"
#include "data_structure.h"
#include "slicing_tree.h"
#include "simulated_annealing.h"

using namespace std;

//******************* drive program *******************
int main(int argc, char *argv[])
{
	//----------------------------  data declaration  ----------------------------
	vector < vector <string> > modules_data;
	vector <AddressId> nodes_address, best_solution;
	vector <NodePtr> operands_address;
	Point best_shape;
	
	srand(6);
	
	//-------  read iput file and store the data into a 2D string vector  --------
	File_Process file;
	file.in_file(modules_data, argv[1]); // read input file
	
	//------------------  create slicing tree by input data  ---------------------
	Slicing_Tree tree(modules_data);
	nodes_address = tree.buildtree(operands_address);
	
	//--------------  use slicing tree to do simulated annealing  ----------------
	Simulated_Annealing annealing;
	best_solution = annealing.SA(nodes_address, best_shape);
	
	//---------------------------  write output file  ----------------------------
	file.out_file(operands_address, best_solution, best_shape, argv[2]); // write output file
	
	return 0;
} 


