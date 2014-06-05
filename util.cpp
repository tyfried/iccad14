#include <iostream>
#include <string>
#include <cstring>
#include "sqlite3.h"
#include "util.h"
#include <vector>
#include <chrono>
#include <cassert>
#include <sstream>
#include <fstream>

using namespace std;

const char *SolverNames[] = {"glucose","lingeling","minisat"};

int plot(void* pArg,int argc,char **argv,char **cNames)
{	
	cout << "Hello!" << endl;
	ofstream dat;
	dat.open("data.dat",ofstream::app);
	char *s_f = static_cast<char*>(pArg);
	for (int i=0; i<argc; i++){
		dat << string(s_f) << " " << argv[i] << endl;
	}
	dat.close();
	return 0;
}

void TestCase::solve()
{
	for (int i=0; i<2; i++){	
		string command = "./"+string(SolverNames[i])+" "+cnf_file;
		
		FILE *in;
		char buff[512];
		
		//timer
		chrono::time_point<chrono::system_clock> start, end;
		start = chrono::system_clock::now();
		if(!(in = popen(command.c_str(), "r"))){
			assert(0);
		}
		end = chrono::system_clock::now();
		
		//update timer info
		chrono::duration<double> elapsed = end-start;
		sol_runtime.push_back(to_string(elapsed.count()));
		
		int sat = 1;
		ofstream of("sat.txt",ofstream::app);
		while(fgets(buff, sizeof(buff), in)!=NULL){
			of << buff;
			if (strstr(buff,"UNSATISFIABLE")) sat--;
		}
		pclose(in);
		
		satisfied.push_back(sat);
		assert(satisfied[i]==satisfied[0]);
	}
}


TestCase::TestCase(string s_f)
{
	//source_file = string(argv[3]);
	//cnf_file = string(argv[4]);
	source_file = s_f;
	
	
	//name
	size_t found1 = source_file.find_first_of("/");
	size_t found2 = source_file.find_first_of("/",found1+1);
	name = source_file.substr(found1+1,found2-found1-1);
	
	//type
	found1 = source_file.find_first_of("/",found2+1);
	type = source_file.substr(found2+1,found1-found2-1);
	
	//number
	found2 = source_file.find_first_of("0123456789",found1+1);
	found1 = source_file.find_first_of(".",found2+1);
	number = source_file.substr(found2,found1-found2);
	
	//gen_algorithm (GET FROM FILE LATER)
	gen_algorithm = "tseitin";
}

string TestCase::gen_sql()
{
	string notes = "";
	string temp = "insert into Generation (case_name,case_type,case_number,algorithm,runtime,timestamp,notes) ";
	temp+= "values ('"+name+"','"+type+"',"+number+",'"+gen_algorithm+"','"+gen_runtime+"','"+timestamp+"','"+notes+"');";
	return temp;
}

string TestCase::plot_sql(string relation)
{
	string temp = "select runtime from "+relation+";";
	return temp;
}

string TestCase::solv_sql(int s)
{
	string notes = "";
	string temp = "insert into "+string(SolverNames[s])+" (case_name,case_type,case_number,runtime,timestamp,satisfiable,notes) ";
	temp+= "values ('"+name+"','"+type+"',"+number+",'"+sol_runtime[s]+"','"+timestamp+"',"+to_string(satisfied[s])+",'"+notes+"');";
	return temp;
}

void TestCase::query(string sql, int (*callback)(void*,int,char**,char**),string param)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	
	rc = sqlite3_open("results.db",&db);
	if (rc){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}
	
	rc = sqlite3_exec(db,sql.c_str(),callback,static_cast<void*>(const_cast<char*>(param.c_str())),&zErrMsg);
	if (rc!=SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
}