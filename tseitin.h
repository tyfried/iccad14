#ifndef __tseitin_h__
#define __tseitin_h__

#include <sstream>
#include <vector>

using namespace std;

void tseitin(int argc, char **argv);
int and_op(int product, const vector<int> &operands, ostringstream &oss);
int nand_op(int product, const vector<int> &operands, ostringstream &oss);
int or_op(int product, const vector<int> &operands, ostringstream &oss);
int nor_op(int product, const vector<int> &operands, ostringstream &oss);
int xor_op(int product, const vector<int> &operands, ostringstream &oss);
int xnor_op(int product, const vector<int> &operands, ostringstream &oss);
int not_op(int product, const vector<int> &operands, ostringstream &oss);
int buffer_op(int product, const vector<int> &operands, ostringstream &oss);

#endif