#ifndef __UTIL_H_
#define __UTIL_H_

#include <string>
#include <vector>

using namespace std;

enum Solver{GLUCOSE,LINGELING,MINISAT};
extern const char *SuitNames[MINISAT+1];

int plot(void* pArg,int argc,char **argv,char **cNames);

struct TestCase{
	string timestamp;
	string source_file;
	string cnf_file;
	string name;
	string type;
	string number;
	string gen_algorithm;
	string gen_runtime;
	vector<string> sol_runtime;
	vector<vector<string>> sol_options;
	vector<int> satisfied;
	
	TestCase(char **argv);
	void tseitin();
	void query(string sql, int (*callback)(void*,int,char**,char**),string param);
	void solve();
	string gen_sql();
	string solv_sql(int s);
	string plot_sql(string relation);
};

#endif