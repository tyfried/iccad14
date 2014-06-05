#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cassert>
#include <string>
#include <cstring>
#include "tseitin.h"
#include "util.h"
#include <sstream>
#include <random>

using namespace std;

int main(int argc, char **argv)
{	
	string directory = "benchmarks/ut";
	vector<int> cases = {1,2,3,5,7,8,13,14,15,20,26,32,36,41};
	vector<string> type1 = {"/sample/","/test/"};
	vector<string> type2 = {"sample","test"};
	string cnf = "out.cnf";
	int my_counter = 0;
	for (int i=0; i<(int)cases.size(); i++){
		string s1 = directory+to_string(cases[i]);
		for (int j=0; j<(int)type1.size(); j++){
			string s2 = s1+type1[j];
			int num;
			if (!j) num=3;
			else num = 10;
			for (int k=1; k<=num; k++){
				string s3 = s2+type2[j]+to_string(k)+".v";
				
				//run simulation
				//construct core object
				TestCase tc(s3);
				tc.cnf_file = "out/"+to_string(my_counter++)+cnf;

				//timestamp
				unsigned seed = chrono::system_clock::now().time_since_epoch().count();
				mt19937 generator (seed);  // mt19937 is a standard mersenne_twister_engine
				tc.timestamp = to_string(generator());
				//cout << tc.timestamp << endl;
				
				//NORMAL
				//generate
				tc.tseitin(my_counter);
				tc.query(tc.gen_sql(),NULL,"");
				
				//solve
				tc.solve();
				for (int i=0; i<2; i++){
					tc.query(tc.solv_sql(i),NULL,"");
				}
				
				//NIVER
				// FILE *in;
				// char buff[128];
				// string command = "./niver-x86-linux "+tc.cnf_file;
				// if(!(in = popen(command.c_str(), "r"))){
					// assert(0);
				// }
				// ifstream file("outfile.cnf");
				// if (file){
					// tc.cnf_file = "outfile.cnf";
					// for (int q=0; q<3; q++){
						// fgets(buff, sizeof(buff), in);
					// }
					// int search = 0;
					// while (buff[search++]!=':');
					// string temp;
					// while (buff[search]!='s'){
						// temp+=buff;
					// }
					// tc.gen_runtime = to_string(stod(tc.gen_runtime)+stod(temp));
				// }
				// pclose(in);
				// file.close();
				
				
				// string command = "./niver-x86-linux "+tc.cnf_file;
				// system(command.c_str());
				
	
	//plot
	//tc.query(tc.plot_sql("generation"),plot,tc.source_file);
				
				
			}
		}
	}
	//string command;
	//command = "gnuplot plot \" sqlite3 results.db 'select case_name, runtime from data'\" title \"bomb's away\"";
	//system(command.c_str());
	
	return 0;
}
