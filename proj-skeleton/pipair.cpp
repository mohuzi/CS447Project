#include <iostream>
#include <unistd.h> // for system calls
#include <list>
#include <string>
#include <vector>
#include <map>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include "pipair.h"
using namespace std;

// GLOBAL DEFINITIONS
unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence
vector<string> callgraph;	// keep track of the call graph
list<string> tokens;	// tokenize the callgraph

string getFuncfromToken(string token) {
  int i;
  for (i = 1; token[i] != '\'' && i != token.length() -1; i++) {
  }
  return token.substr(1, i - 1);
}


void parser(list<string> &tokens, map<int, string> &IDtoFunc, map<string, int> &FunctoID,map<int, vector<int> > &FuncCalls, int &maxID) {

    int ID = 0;
    

    bool startFlag = false;
    for (list<string>::iterator it=tokens.begin(); it != tokens.end(); ++it){
         if (*it == "function:"){
              ++it;
              startFlag = true;
              if(FunctoID.find(getFuncfromToken(*it)) == FunctoID.end()){
                  //cout << getFuncfromToken(*it)<<endl;
                  string func = getFuncfromToken(*it);
                  FunctoID[func] = ID;
                  IDtoFunc[ID] =func;
                  ID++;
              }
              --it;
         }
         if (*it == "function" && startFlag){
              ++it;
              
              if(FunctoID.find(getFuncfromToken(*it)) == FunctoID.end()){
                  //cout << getFuncfromToken(*it)<<endl;
                  string func = getFuncfromToken(*it);
                  FunctoID[func] = ID;
                  IDtoFunc[ID] =func;
                  ID++;
              }
              --it;
         }

           
    }
    
    string TopLevelFunc = "";
    for (list<string>::iterator it=tokens.begin(); it != tokens.end(); ++it){
        if (*it == "function:"){
              ++it;
              string func = getFuncfromToken(*it);
              TopLevelFunc = func;
              --it;
         }
         if (*it == "function" && TopLevelFunc != ""){
              ++it;
              string func = getFuncfromToken(*it);

              if(FuncCalls.find(FunctoID[TopLevelFunc]) != FuncCalls.end()) {
                   if( find(FuncCalls[FunctoID[TopLevelFunc]].begin(),FuncCalls[FunctoID[TopLevelFunc]].end(),FunctoID[func]) == FuncCalls[FunctoID[TopLevelFunc]].end() ){
                       
                      FuncCalls[FunctoID[TopLevelFunc]].push_back(FunctoID[func]);
                   }
              }
              else{
                 
                FuncCalls[FunctoID[TopLevelFunc]].push_back(FunctoID[func]);
              }
              --it;
         }
    }
    maxID = ID -1;
 
}


// Using the parse data, calculate the support for functions and function pairs, and then return the function pairs which we have inferred must always occur together
void analyze(map<int, string> &IDtoFunc, map<string, int> &FunctoID,map<int, vector<int> > &FuncCalls,int &maxID,vector<map<int,FunctionPair> > &Pairs) {
  cout << "Beginning Analysis" << flush << endl;
  vector<int> support = vector<int>(maxID,0);
	int a = 0, b = 0;
	//Calculate support for each function and function pair
	for (map<int, vector<int> >::iterator i=FuncCalls.begin(); i != FuncCalls.end(); ++i){
		vector<int> &v = i->second;
		//Remove duplicate function calls (maybe should do in the parser?)
		sort( v.begin(), v.end() );
		v.erase( unique( v.begin(), v.end() ), v.end() );
		//Go through all function pairs
		for (vector<int>::iterator j=v.begin(); j != v.end(); ++j){
			a = *j;
			support[a]++;
			for (vector<int>::iterator k=v.begin(); k != v.end(); ++k){				
				b = *k;
				if (a != b) {					
					Pairs[a][b].support++;
					Pairs[a][b].a = a;
					Pairs[a][b].b = b;
				}
			}
		}
	}
	//Calculate confidence for each function pair, and throw out any pairs that don't meet the threasholds
	//Loop through all function pairs
    for (vector<map<int,FunctionPair> >::iterator i=Pairs.begin(); i != Pairs.end(); ++i){
		for (map<int,FunctionPair>::iterator j=i->begin(); j != i->end(); ++j){
			FunctionPair &p = j->second;
			//cout << p.a << " " << p.b << " " << p.support << " " << (p.support) * 100 / support[p.a] << endl;
			if( p.support < T_SUPPORT || (p.support) * 10000 / support[p.a] < T_CONFIDENCE){
				//Doesn't meet support or confidence threasholds
				i->erase(j);
			} else {
				//Does meet thresholds, keep and record confidence
				p.confidence = (p.support) * 10000 / support[p.a];
			}
		}
	}
	cout << "Ending Analysis" << flush << endl;
}

void find_bugs() {
}

void callgraph_gen( char* argv ) {

	string line;
	/* pipe to connect opt's stderr and our stdin */
	int pipe_callgraph[2];

	/* pid of opt */
	int opt_pid;

	/* arguments */
	char *bc_file;
	int support;
	double confidence;

	/* check arguments */
	/* !!!parse arguments yourself!!! */
	bc_file = argv;

	/* create pipe and check if pipe succeeded */
	if (pipe(pipe_callgraph) < 0) {
		cerr << "pipe" << endl;	
	}

	/* create child process */
	opt_pid = fork();
	if (!opt_pid) { /* child process, to spawn opt */

		/* close the read end, since opt only write */
		close(pipe_callgraph[PIPE_READ]);  

		/* bind pipe to stderr, and check */
		if (dup2(pipe_callgraph[PIPE_WRITE], STDERR_FILENO) < 0) {
			cerr << "dup2 pipe_callgraph" << endl;			
		}

		//redirect the stdout to a dummy pipe
		int dummyPipe[2];
		pipe(dummyPipe);
		dup2(dummyPipe[1],1);
     

		/* print something to stderr 
		fprintf(stderr, "This is child, just before spawning opt with %s.\n", bc_file);
		*/
		/* spawn opt */
		if (execl("/usr/local/bin/opt", "opt", "-print-callgraph", bc_file, (char *)NULL) < 0) {
			cerr << "execl opt" << endl;			
		}
     
		close(dummyPipe[1]);
		close(dummyPipe[0]);
	}// if

	/* parent process */
  

	/* close the write end, since we only read */
	close(pipe_callgraph[PIPE_WRITE]);

	/* since we don't need stdin, we simply replace stdin with the pipe */
	if (dup2(pipe_callgraph[PIPE_READ], STDIN_FILENO) < 0) {
		cerr << "dup2 pipe_callgraph" << endl;	
	}

	string token="";
	//mark 1
	while(cin >> token ){
		tokens.push_back(token);
	}
	//mark 1 end

	//while(getline(cin, line)) {
		//callgraph.push_back(line);
	//}
	
}// callgraph_gen


int main(int argc, char* argv[]) {	
	switch( argc ) {		
		case 4:
			T_SUPPORT = atoi(argv[2]);
			T_CONFIDENCE = atoi(argv[3]);
			// FALL THROUGH
		case 2:
			callgraph_gen( argv[1] );
			break;
		default:
			cerr << "Your Command line paramaters are wrong!" << endl;
	}; // switch   
  
	map<int, string> IDtoFunc;
	map<string, int> FunctoID;
	map<int, vector<int> > FuncCalls; 
	int maxID;

	parser(tokens,IDtoFunc,FunctoID,FuncCalls,maxID);
 
	// To see what we have in those data structure. 
  
		cout << "function map :" << endl;
		for (map<int, string>::iterator  it=IDtoFunc.begin(); it!=IDtoFunc.end(); ++it)
			cout << it->first << " => " << it->second << '\n';

		cout << endl<<"Call functions :" << endl;
		for (map<int, vector<int> >::iterator  it=FuncCalls.begin(); it!=FuncCalls.end(); ++it){
			cout << it->first << " calls:" << endl;
			for (vector<int>::iterator  it2=it->second.begin(); it2!=it->second.end(); ++it2){
				cout << *it2 <<" ";
			}
			cout << endl;
		}
  

	// Your Code here:
	vector<map<int,FunctionPair> > Pairs(maxID +1);
	analyze(IDtoFunc,FunctoID,FuncCalls,maxID,Pairs);  
	cout << "Learned Constraints :" << flush << endl;	
	//Loop through all function pairs
	for (vector<map<int,FunctionPair> >::iterator i=Pairs.begin(); i != Pairs.end(); ++i){
		for (map<int,FunctionPair>::iterator j=i->begin(); j != i->end(); ++j){
			FunctionPair &p = j->second;
			cout << "pair: (" << IDtoFunc[p.a] << "," << IDtoFunc[p.b] << "), support:" << p.support << ", condfidence:" << p.confidence << endl;
		}
	}

	//To see the original Bitcode. To use this, u need to comment out line 175 to line 179 (mark 1 to mark 1 end)
	for(int i =0; i < callgraph.size(); i++)
	{		

		cout << callgraph[i] << endl;
	}

}
