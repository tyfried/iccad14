#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cassert>
#include <string>
#include "tseitin.h"
#include "util.h"

using namespace std;

int main(int argc, char **argv)
{	
	//construct core object
	TestCase tc(argv);

	//timestamp
	tc.timestamp = to_string(chrono::system_clock::to_time_t(chrono::system_clock::now()));
	
	//generate
	tc.tseitin();
	tc.query(tc.gen_sql(),NULL,"");
	
	//solve
	tc.solve();
	for (int i=0; i<2; i++){
		tc.query(tc.solv_sql(i),NULL,"");
	}
	
	//plot
	tc.query(tc.plot_sql("generation"),plot,tc.source_file);
	//string command;
	//command = "gnuplot plot \" sqlite3 results.db 'select case_name, runtime from data'\" title \"bomb's away\"";
	//system(command.c_str());
	
	return 0;
}
