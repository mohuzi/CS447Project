#include "pipair.h"
#include <iostream>
#include <unistd.h> // for system calls
<<<<<<< HEAD
=======
#include <list>
>>>>>>> Parser Finished
#include <string>
#include <vector>
#include <map>
#include <vector>
using namespace std;

// Global Def
unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence
vector<string> callgraph;

void parser(){

}

<<<<<<< HEAD
void statistic(){
  
=======
 
string getFuncfromToken(string token){
  int i;
  for(i = 1; token[i] != '\'' && i != token.length() -1 ; i++ ){}
  return token.substr(1,i-1);
>>>>>>> Parser Finished
}

void callgraph_gen( char* argv ) {

<<<<<<< HEAD
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
	if ( pipe( pipe_callgraph ) < 0 ) {
		cerr << "pipe" << endl;
		return;
	}
=======
void parser(std::list<string> &tokens, std::map<int, string> &IDtoFunc, std::map<string, int> &FunctoID,std::map<int, std::vector<int> > &FuncCalls){
    int ID = 0;
    

    bool startFlag = false;
    for (std::list<string>::iterator it=tokens.begin(); it != tokens.end(); ++it){
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
    for (std::list<string>::iterator it=tokens.begin(); it != tokens.end(); ++it){
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

 
}
>>>>>>> Parser Finished

	/* create child process */
	opt_pid = fork();
	if (!opt_pid) { /* child process, to spawn opt */

<<<<<<< HEAD
		/* close the read end, since opt only write */
		close(pipe_callgraph[PIPE_READ]);  
=======
int callgraph_gen( char* argv, vector<string> &callgraph, std::list<string> &tokens ) {
>>>>>>> Parser Finished

		/* bind pipe to stderr, and check */
		if (dup2(pipe_callgraph[PIPE_WRITE], STDERR_FILENO) < 0) {
			cerr << "dup2 pipe_callgraph" << endl;
			return;
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
			return;
		}
     
		close(dummyPipe[1]);
		close(dummyPipe[0]);
	}

	/* parent process */
  

	/* close the write end, since we only read */
	close(pipe_callgraph[PIPE_WRITE]);

	/* since we don't need stdin, we simply replace stdin with the pipe */
	if(dup2(pipe_callgraph[PIPE_READ], STDIN_FILENO) < 0) {
		cerr << "dup2 pipe_callgraph" << endl;
		return;
	}

<<<<<<< HEAD
=======
  /* close the write end, since we only read */
  close(pipe_callgraph[PIPE_WRITE]);

  /* since we don't need stdin, we simply replace stdin with the pipe */
  if (dup2(pipe_callgraph[PIPE_READ], STDIN_FILENO) < 0) {
    cerr << "dup2 pipe_callgraph" << endl;
    return 1;
  }

  /* we print w/e read from the pipe 
  char c = '\0';
  while (scanf("%c", &c) >= 1) {
    printf("%c", c);
  }
	*/

  string token="";
  //mark 1
  while(cin >> token ){
      tokens.push_back(token);
  }
  //mark 1 end

>>>>>>> Parser Finished
	while(getline(cin, line)) {
		callgraph.push_back(line);
	}
}


int main(int argc, char* argv[]) {
<<<<<<< HEAD
	
	switch( argc ) {
		case 2:
			callgraph_gen( argv[1] );
=======
	vector<string> callgraph;
  list<string> tokens;

	switch( argc ) {
		case 2:
			callgraph_gen( argv[1], callgraph , tokens );
>>>>>>> Parser Finished
			break;
		case 4:
			T_SUPPORT = atoi(argv[2]);
			T_CONFIDENCE = atoi(argv[3]);
<<<<<<< HEAD
			callgraph_gen( argv[1] );
=======
			callgraph_gen( argv[1], callgraph , tokens);
>>>>>>> Parser Finished
			break;
		default:
			cerr << "Your Command line paramaters are wrong!" << endl;
	}; // switch
	

<<<<<<< HEAD
 
=======
   
  
  std::map<int, string> IDtoFunc;
  std::map<string, int> FunctoID;
  std::map<int, std::vector<int> > FuncCalls; 
  parser(tokens,IDtoFunc,FunctoID,FuncCalls);
 
  // To see what we have in those data structure. 
  
        cout << "function map :" << endl;
      for (std::map<int, string>::iterator  it=IDtoFunc.begin(); it!=IDtoFunc.end(); ++it)
            std::cout << it->first << " => " << it->second << '\n';

      cout << endl<<"Call functions :" << endl;
      for (std::map<int, std::vector<int> >::iterator  it=FuncCalls.begin(); it!=FuncCalls.end(); ++it){
            cout << it->first << " calls:" << endl;
            for (std::vector<int>::iterator  it2=it->second.begin(); it2!=it->second.end(); ++it2){
              cout << *it2 <<" ";
            }
            cout << endl;
      }
>>>>>>> Parser Finished
  

  // Your Code here:


  //To see the original Bitcode. To use this, u need to comment out line 175 to line 179 (mark 1 to mark 1 end)
  for(int i =0; i < callgraph.size(); i++)
	{		

     cout << callgraph[i] << endl;
	}

}
