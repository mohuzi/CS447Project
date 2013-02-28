#include <iostream>
#include <unistd.h> // for system calls
#include <algorithm>
#include "pipair.h"

// GLOBAL DEFINITIONS
unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence
vector< string > callgraph;	// keep track of the call graph
vector< string > callgraph_tokens;	// tokenize the callgraph
//list< string > tokens;
map< int, string > IDtoFunc;
map< string, int > FunctoID;
map< int, vector<int> > FuncCalls;
int maxID;

string getFuncFromToken(const string &token) {
  //int i;
  //for (i = 1; token[i] != '\'' && i != token.length() -1; i++) {
  //}
  return token.substr(1, token.find_last_of("\'") - 1 );
}


void parser( ) {

    int ID = 0;
    bool startFlag = false;
	string func, TopLevelFunc = "";

	for( int i = 0; i < callgraph_tokens.size(); i++ ) {
		if( callgraph_tokens[ i ] == "function:" ) {
			startFlag = true;
			func = getFuncFromToken( callgraph_tokens[ i + 1 ] );
			if(FunctoID.find( func ) == FunctoID.end() ){			
				FunctoID[ func ] = ID;
				IDtoFunc[ ID ] = func;
				ID++;
			}// if
		}// if
		else if( ( callgraph_tokens[ i] == "function" ) && startFlag ) {
			func = getFuncFromToken( callgraph_tokens[ i + 1 ] );
			if(FunctoID.find( func ) == FunctoID.end() ) {
				FunctoID[ func ] = ID;
				IDtoFunc[ ID ] = func;
				ID++;
			}// if
		}// else if
	}// for
	
	for (int i = 0; i < callgraph_tokens.size(); i++ ) {
		if( callgraph_tokens[ i ] == "function:" ) {
			TopLevelFunc = getFuncFromToken( callgraph_tokens[ i + 1 ] );			
		}// if
		else if( callgraph_tokens[ i ] == "function" && TopLevelFunc != "" ) {
			func = getFuncFromToken( callgraph_tokens[ i +1 ] );
            if(FuncCalls.find(FunctoID[TopLevelFunc]) != FuncCalls.end()) {
                if( find(FuncCalls[FunctoID[TopLevelFunc]].begin(),FuncCalls[FunctoID[TopLevelFunc]].end(),FunctoID[func]) == 
					FuncCalls[FunctoID[TopLevelFunc]].end() ){
                       
                    FuncCalls[FunctoID[TopLevelFunc]].push_back(FunctoID[func]);
                }
            }// if
            else {                 
				FuncCalls[FunctoID[TopLevelFunc]].push_back(FunctoID[func]);
            }// else
		}// else if
	}// for
	maxID = ID - 1;
    /*for (list<string>::iterator it=tokens.begin(); it != tokens.end(); ++it){
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
	*/
}// parser


// Using the parse data, calculate the support for functions and function pairs, and then return the function pairs which we have inferred must always occur together
void analyze( vector<map<int,FunctionPair> > &Pairs ) {

  cout << "Beginning Analysis" << flush << endl; //? what is flush?
  vector<int> support = vector<int>(maxID,0);
	int a = 0, b = 0;

	//Calculate support for each function and function pair
	for( map< int, vector<int> >::iterator i = FuncCalls.begin(); i != FuncCalls.end(); ++i ) {

		vector<int> &v = i->second;
		//Remove duplicate function calls (maybe should do in the parser?)
		sort( v.begin(), v.end() );
		v.erase( unique( v.begin(), v.end() ), v.end() );

		//Go through all function pairs
		for(int j = 0; j < v.size(); j++ ) {
			a = v[ j ];
			support[a]++;
			for(int k = 0; k < v.size(); k++ ){				
				b = v[ k ];
				if ( a != b ) {					
					Pairs[ a ][ b ].support++;
					Pairs[ a ][ b ].a = a;
					Pairs[ a ][ b ].b = b;
				}// if
			}// for
		}// for
	}// for

	//Calculate confidence for each function pair, and throw out any pairs that don't meet the threasholds
	//Loop through all function pairs
    for (int i = 0; i < Pairs.size(); i++ ){
		for (map<int,FunctionPair>::iterator j = Pairs[i].begin(); j != Pairs[i].end(); ++j){
			FunctionPair &p = j->second;
			//cout << p.a << " " << p.b << " " << p.support << " " << (p.support) * 100 / support[p.a] << endl;
			if( p.support < T_SUPPORT || (p.support) * 10000 / support[p.a] < T_CONFIDENCE){
				//Doesn't meet support or confidence threasholds
				Pairs[i].erase(j);
			} else {
				//Does meet thresholds, keep and record confidence
				p.confidence = (p.support) * 10000 / support[p.a];
			}
		}
	}
	cout << "Ending Analysis" << flush << endl;
}//  analyze

void find_bugs() {
}

void callgraph_gen( char* argv ) {

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

	string line;
	string token="";
	//mark 1
	while( cin >> token ) {
		callgraph_tokens.push_back(token);
		//tokens.push_back( token );
	}// while

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
  
	parser( );
 
	// To see what we have in those data structure. 
  
	cout << "function map :" << endl;
	for (map<int, string>::iterator  it=IDtoFunc.begin(); it!=IDtoFunc.end(); ++it)
		cout << it->first << " => " << it->second << '\n';

	cout << endl<<"Call functions :" << endl;
	for (map<int, vector<int> >::iterator  it=FuncCalls.begin(); it!=FuncCalls.end(); ++it){
		cout << it->first << " calls:" << endl;
		for (int it2 = 0; it2 < it->second.size(); it2++){
			cout << it->second[ it2 ] <<" ";
		}
		cout << endl;
	}  	

	// Your Code here:
	vector<map<int,FunctionPair> > Pairs(maxID + 1);
	analyze( Pairs );  
	cout << "Learned Constraints :" << flush << endl;	
	//Loop through all function pairs
	for(int i = 0; i < Pairs.size(); i++ ){
		for(map<int,FunctionPair>::iterator j= Pairs[i].begin(); j != Pairs[i].end(); ++j){
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
