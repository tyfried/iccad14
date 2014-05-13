#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "tseitin.h"
#include <cassert>
#include<sstream>

using namespace std;

//remember maximum for output file

/*
Note on Indexing:
n0==[num_vars]
out==[num_vars+1]
*/ 
void tseitin(int argc, char **argv)
{
	//input file
	ifstream input(argv[3]);
	
	//output file
	string op(argv[3]);
	op = op.replace(op.size()-2,op.size(),"_test.cnf");
	ofstream output(op.c_str());
	vector<string> out;
	
	//get to useful information
	string temp;
	int num_vars = 0;
	while (input >> temp){
		if (temp=="and" || temp=="or" || temp=="xor") break; //FIX
		if (temp=="input" || temp=="wire" || temp=="output") num_vars++;
	}
	
	//while not endmodule
	char junk;
	int num_cl = 0;
	ostringstream oss;
	while (temp!="endmodule"){
		if (temp=="wire"){
			input >> temp;
		}
		else{
			vector<int> operands;
			int product = -1;
			int val;
			bool skip = false;
			input >> junk >> junk;
			
			//get numbers
			while (junk!=';'){
				if (junk=='s'){
					input >> val >> junk >> val;
					if (product == -1) product = 2*num_vars + val;
					else operands.push_back(2*num_vars + val);
				}
				else if (junk=='n'){
					junk = input.peek();
					if (junk=='o'){
						input >> junk >> junk >> junk;
						input >> val;
						if (product==-1) product = num_vars+2+val;
						else operands.push_back(num_vars+2+val);
					}
					else if (junk < '0' || junk >'9') {
						assert(0);}
					else{
						input >> val;
						if (product==-1) product = val;
						else operands.push_back(val);
					}
				}
				else if (junk=='o'){
					input >> junk >> junk;
					if (product==-1) product = num_vars+1;
					else operands.push_back(num_vars+1);
				}
				else if (junk=='1'){
					if (temp!="and" && temp!="or") assert(0);
					input >> junk >> junk >> val;
					if (val==0 && temp=="and") skip = true;
					if (val==1 && temp=="or") skip = true;
				}
				else 
					assert(0);
				input >> junk >> junk;
			}
			
			if (skip){
				input >> temp;
				continue;
			}
			
			//'0'th issue
			if (product==0) product=num_vars;
			for (size_t i=0; i<operands.size(); i++){
				if (operands[i]==0) operands[i]=num_vars;
			}
			
			if (temp=="and") num_cl+=and_op(product,operands,oss);
			else if (temp=="nand") num_cl+=nand_op(product,operands,oss);
			else if (temp=="or") num_cl+=or_op(product,operands,oss);
			else if (temp=="nor") num_cl+=nor_op(product,operands,oss);
			else if (temp=="xor") num_cl+=xor_op(product,operands,oss);
			else if (temp=="xnor") num_cl+=xnor_op(product,operands,oss);
			else if (temp=="not") num_cl+=not_op(product,operands,oss);
			else if (temp=="buffer") num_cl+=buffer_op(product,operands,oss);
			else assert(0);
		}
		input >> temp;
	}
	
	//write out file
	output << "p cnf " << num_vars << " " << num_cl << "\n";
	output << oss.str();
	
	//close files
	input.close();
	output.flush();
	output.close();
}

int and_op(int product, const vector<int> &operands, ostringstream &oss){
	for (size_t i=0; i<operands.size(); i++){
		oss << "-" << operands[i] << " ";
	}
	oss << product << " 0\n";
	for (size_t i=0; i<operands.size(); i++){
		oss << operands[i] << " -" << product << " 0\n";
	}
	return operands.size()+1;
}

int nand_op(int product, const vector<int> &operands, ostringstream &oss){
	for (size_t i=0; i<operands.size(); i++){
		oss << "-" << operands[i] << " ";
	}
	oss << "-" << product << " 0\n";
	for (size_t i=0; i<operands.size(); i++){
		oss << operands[i] << " " << product << " 0\n";
	}
	return operands.size()+1;
}

int or_op(int product, const vector<int> &operands, ostringstream &oss){
	for (size_t i=0; i<operands.size(); i++){
		oss << operands[i] << " ";
	}
	oss << "-" << product << " 0\n";
	for (size_t i=0; i<operands.size(); i++){
		oss << "-" << operands[i] << " " << product << " 0\n";
	}
	return operands.size()+1;
}

int nor_op(int product, const vector<int> &operands, ostringstream &oss){
	for (size_t i=0; i<operands.size(); i++){
		oss << operands[i] << " ";
	}
	oss <<  product << " 0\n";
	for (size_t i=0; i<operands.size(); i++){
		oss << "-" << operands[i] << " -" << product << " 0\n";
	}
	return operands.size()+1;
}

int xor_op(int product, const vector<int> &operands, ostringstream &oss){
	assert(operands.size()==2);
	for (int i=0; i<2; i++){
		for (int j=0; j<2; j++){
			if (i) oss << operands[0] << " ";
			else oss << "-" << operands[0] << " ";
			if (j) oss << operands[1] << " ";
			else oss << "-" << operands[1] << " ";
			if ((i^j)&1) oss << product << " 0\n";
			else oss << "-" << product << " 0\n";
		}
	}
	return 4;
}

int xnor_op(int product, const vector<int> &operands, ostringstream &oss){
	assert(operands.size()==2);
	for (int i=0; i<2; i++){
		for (int j=0; j<2; j++){
			if (i) oss << operands[0] << " ";
			else oss << "-" << operands[0] << " ";
			if (j) oss << operands[1] << " ";
			else oss << "-" << operands[1] << " ";
			if (!((i^j)&1)) oss << product << " 0\n";
			else oss << "-" << product << " 0\n";
		}
	}
	return 4;
}

int not_op(int product, const vector<int> &operands, ostringstream &oss){
	assert(operands.size()==1);
	oss << "-" << operands[0] << " -" << product << " 0\n";
	oss << operands[0] << " " << product << " 0\n";
	return 2;
}

int buffer_op(int product, const vector<int> &operands, ostringstream &oss){
	assert(operands.size()==1);
	oss << "-" << operands[0] << product << " 0\n";
	oss << operands[0] << " -" << product << " 0\n";
	return 2;
}
		

// #include <iostream>
// #include <fstream>
// #include <string>
// #include <cstdlib>
// #include <cstdio>
// #include <vector>
// #include "tseitin.h"

// using namespace std;


// /*
// Note on Indexing:
// n0==[num_vars]
// out==[num_vars+1]
// */ 
// void tseitin(int argc, char **argv)
// {
	// //input file
	// ifstream input(argv[3]);
	
	// //output file
	// string op(argv[3]);
	// op = op.replace(op.size()-2,op.size(),".cnf");
	// ofstream output(op.c_str());
	// vector<string> out;
	
	// //get to useful information
	// string temp;
	// int num_vars = 0;
	// while (input >> temp){
		// if (temp=="and" || temp=="or" || temp=="xor") break;
		// if (temp=="input" || temp=="wire" || temp=="output") num_vars++;
	// }
	
	// //while not endmodule
	// char junk;
	// int num_cl = 0;
	// while (temp!="endmodule"){
		
		// //and
		// if (temp=="and"){
			// int o,i1,i2;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;
			
			// //fix '0' issue
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			// else if (i2==0) i2=num_vars;
			

			// //cnf
			// // output << "-" << i1 << " -" << i2 << " " << o << " 0\n";
			// // output << i1 << " -" << o << " 0\n";
			// // output << i2 << " -" << o << " 0\n";
			// out.push_back("-" + to_string(i1) + " -" + to_string(i2) + " " + to_string(o) + " 0\n");
			// out.push_back(to_string(i1) + " -" + to_string(o) + " 0\n");
			// out.push_back(to_string(i2) + " -" + to_string(o) + " 0\n");
			// num_cl+=3;
		// }
		
		// //nand
		// else if(temp=="nand"){
			// int o,i1,i2;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;
			
			// //fix '0' issue
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			// else if (i2==0) i2=num_vars;
			

			// //cnf
			// // output << "-" << i1 << " -" << i2 << " " << o << " 0\n";
			// // output << i1 << " -" << o << " 0\n";
			// // output << i2 << " -" << o << " 0\n";
			// out.push_back("-" + to_string(i1) + " -" + to_string(i2) + " -" + to_string(o) + " 0\n");
			// out.push_back(to_string(i1) + " " + to_string(o) + " 0\n");
			// out.push_back(to_string(i2) + " " + to_string(o) + " 0\n");
			// num_cl+=3;
		// }
		
		// //or
		// else if (temp=="or"){
			// int o,i1,i2,i3;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;

			// if (junk!=';'){
				// input >> i3 >> junk >> junk;
				
				// //fix 0th issue
				// if (o==0) o=num_vars;
				// else if (i1==0) i1=num_vars;
				// else if (i2==0) i2=num_vars;
				// else if (i3==0) i3=num_vars;
				
				// //write out cnf
				// // output << i1 << " " << i2 << " " << i3 << " -" << o << " 0\n";
				// // output << "-" << i1 << " " << o << " 0\n";
				// // output << "-" << i2 << " " << o << " 0\n";
				// // output << "-" << i3 << " " << o << " 0\n";
				// out.push_back(to_string(i1) + " " + to_string(i2) + " " + to_string(i3) + " -" + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i1) + " " + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i2) + " " + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i3) + " " + to_string(o) + " 0\n");
				// num_cl+=4;
			// }
			// else{
				// //fix 0th issue
				// if (o==0) o=num_vars;
				// else if (i1==0) i1=num_vars;
				// else if (i2==0) i2=num_vars;
				
				// //write out cnf
				// // output << i1 << " " << i2 << " -" << o << " 0\n";
				// // output << "-" << i1 << " " << o << " 0\n";
				// // output << "-" << i2 << " " << o << " 0\n";
				// out.push_back( to_string(i1) + " " + to_string(i2) + " -" + to_string(o) + " 0\n");
				// out.push_back( "-" + to_string(i1) + " " + to_string(o) + " 0\n");
				// out.push_back( "-" + to_string(i2) + " " + to_string(o) + " 0\n");
				// num_cl+=3;
			// }
		// }
		
		// //nor
		// else if(temp=="nor"){
			// int o,i1,i2,i3;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;

			// if (junk!=';'){
				// input >> i3 >> junk >> junk;
				
				// //fix 0th issue
				// if (o==0) o=num_vars;
				// else if (i1==0) i1=num_vars;
				// else if (i2==0) i2=num_vars;
				// else if (i3==0) i3=num_vars;
				
				// //write out cnf
				// // output << i1 << " " << i2 << " " << i3 << " -" << o << " 0\n";
				// // output << "-" << i1 << " " << o << " 0\n";
				// // output << "-" << i2 << " " << o << " 0\n";
				// // output << "-" << i3 << " " << o << " 0\n";
				// out.push_back(to_string(i1) + " " + to_string(i2) + " " + to_string(i3) + " " + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i1) + " -" + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i2) + " -" + to_string(o) + " 0\n");
				// out.push_back("-" + to_string(i3) + " -" + to_string(o) + " 0\n");
				// num_cl+=4;
			// }
			// else{
				// //fix 0th issue
				// if (o==0) o=num_vars;
				// else if (i1==0) i1=num_vars;
				// else if (i2==0) i2=num_vars;
				
				// //write out cnf
				// // output << i1 << " " << i2 << " -" << o << " 0\n";
				// // output << "-" << i1 << " " << o << " 0\n";
				// // output << "-" << i2 << " " << o << " 0\n";
				// out.push_back( to_string(i1) + " " + to_string(i2) + " " + to_string(o) + " 0\n");
				// out.push_back( "-" + to_string(i1) + " -" + to_string(o) + " 0\n");
				// out.push_back( "-" + to_string(i2) + " -" + to_string(o) + " 0\n");
				// num_cl+=3;
			// }
		// }
		
		// //not
		// else if (temp=="not"){
			// int o,i1;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			
			// //write out cnf
			// // output << "-" << i1 << " -" << o << " 0\n";
			// // output << i1 << " " << o << " 0\n";
			// out.push_back( "-" + to_string(i1) + " -" + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " " + to_string(o) + " 0\n");
			// num_cl+=2;
		// }
		
		// //buffer
		// else if(temp=="buf"){
			// int o,i1;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			
			// out.push_back( "-" + to_string(i1) + " " + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " -" + to_string(o) + " 0\n");
			// num_cl+=2;
			
		// }
		
		// //xnor
		// else if(temp=="xnor"){
			// int o,i1,i2;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;
			
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			// else if (i2==0) i2=num_vars;
			
			// out.push_back( "-" + to_string(i1) + " -" + to_string(i2) + " " + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " " + to_string(i2) + " " + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " -" + to_string(i2) + " -" + to_string(o) + " 0\n");
			// out.push_back( "-" + to_string(i1) + " " + to_string(i2) + " -" + to_string(o) + " 0\n");
			// num_cl+=4;
		// }
		
		// //xor
		// else{
			// int o,i1,i2;
			// if (num_cl==0){
				// o = num_vars+1;
				// input >> junk >> junk >> junk >> junk >> junk >> junk;
			// }
			// else{
				// input >> junk >> junk;
				// input >> o >> junk >> junk;
			// }
			// input >> i1 >> junk >> junk;
			// input >> i2 >> junk >> junk;
			
			// if (o==0) o=num_vars;
			// else if (i1==0) i1=num_vars;
			// else if (i2==0) i2=num_vars;
			
			// //write out cnf
			// // output << "-" << i1 << " -" << i2 << " -" << o << " 0\n";
			// // output << i1 << " " << i2 << " -" << o << " 0\n";
			// // output << i1 << " -" << i2 << " " << o << " 0\n";
			// // output << "-" << i1 << " " << i2 << " " << o << " 0\n";
			// out.push_back( "-" + to_string(i1) + " -" + to_string(i2) + " -" + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " " + to_string(i2) + " -" + to_string(o) + " 0\n");
			// out.push_back( to_string(i1) + " -" + to_string(i2) + " " + to_string(o) + " 0\n");
			// out.push_back( "-" + to_string(i1) + " " + to_string(i2) + " " + to_string(o) + " 0\n");
			// num_cl+=4;
		// }
		
		// //update
		// input >> temp; //buggy?
	// }
	
	// //write out file
	// output << "p cnf " << num_vars << " " << num_cl << "\n";
	// for (size_t i=0; i<out.size(); i++){
		// output << out[i];
	// }
	
	// //close files
	// input.close();
	// output.flush();
	// output.close();
// }