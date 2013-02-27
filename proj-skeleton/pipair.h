#ifndef _PIPAIR_
#define _PIPAIR_

enum {
  PIPE_READ = 0,
  PIPE_WRITE,
};

class FunctionPair{
  public:
    int a;
    int b;
    int support;
    int confidence;
    
    FunctionPair(): a(0), b(0), support(0), confidence(-1) {}
};

// FUNCTION PROTOTYPES ====================================

string getFuncfromToken(string token);

 // Parses the call graph
void parser(std::list<string> &tokens, std::map<int, string> &IDtoFunc, std::map<string, int> &FunctoID,std::map<int, std::vector<int> > &FuncCalls, int &maxID);

// Using the parse data, calculate the support for functions and function pairs, and then return the function pairs which we have inferred must always occur together
void analyze(map<int, string> &IDtoFunc, map<string, int> &FunctoID,map<int, vector<int> > &FuncCalls,int &maxID,vector<map<int,FunctionPair> > &Pairs);

void find_bugs();

// int callgraph_gen( char* argv ); // Generates the callgraph from bitcode
int callgraph_gen( char* argv, vector<string> &callgraph, std::list<string> &tokens);

void statistics(); // Keeps track of the statistics

#endif //_PIPAIR_