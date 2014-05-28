#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "tseitin.h"
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include "util.h"

using namespace std;

void TestCase::tseitin()
{
	//timer
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();
	
	//input file
	ifstream input(source_file.c_str());
	
	//output file
	ofstream output(cnf_file.c_str());
	
	//get to useful information
	string temp;
	do {input >> temp;} while (temp!="output");
	
	//while not endmodule
	char junk;
	int num_cl = 0;
	ostringstream oss;
	Stringmap smap;
	int count = 0;
	while (temp!="endmodule"){
		if (temp=="wire" || temp=="output" || temp=="input"){
			input >> temp;
			temp.erase(temp.end()-1);
			smap[temp] = ++count;
		}
		else{
			vector<int> operands;
			int product = -1;
			bool skip = false;
			input >> junk;
			
			//get vals
			input >> junk;
			while (junk!=';'){
				string str;
				while (junk!=',' && junk!=')'){
					str.push_back(junk);
					input >> junk;
				}
				
				//special binary input case
				if (str[0] >= '0' && str[0] <= '9'){
					assert(str[0]=='1');
					assert(temp=="and" || temp=="or");
					if (str[4]=='0' && temp=="and") skip = true;
					if (str[4]=='1' && temp=="or") skip = true;
				}
				
				int val = smap[str];
				if (product==-1) product = val;
				else operands.push_back(val);
				input >> junk;
			}
			if (!skip){
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
		}
		input >> temp;
	}
	
	//write out file
	output << "p cnf " << count << " " << num_cl << "\n";
	output << oss.str();
	
	//close files
	input.close();
	output.close();
	
	//timer
	end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end-start;
	gen_runtime = to_string(elapsed.count());
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