#ifndef __TSEITIN_H__
#define __TSEITIN_H__

#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

typedef std::unordered_map<string,int> Stringmap;

int and_op(int product, const vector<int> &operands, ostringstream &oss);
int nand_op(int product, const vector<int> &operands, ostringstream &oss);
int or_op(int product, const vector<int> &operands, ostringstream &oss);
int nor_op(int product, const vector<int> &operands, ostringstream &oss);
int xor_op(int product, const vector<int> &operands, ostringstream &oss);
int xnor_op(int product, const vector<int> &operands, ostringstream &oss);
int not_op(int product, const vector<int> &operands, ostringstream &oss);
int buffer_op(int product, const vector<int> &operands, ostringstream &oss);

#endif