#ifndef _PIPAIR_
#define _PIPAIR_

enum 
{
  PIPE_READ = 0,
  PIPE_WRITE,
};


/*class OurDataStructure {

	private:
	public:
	
};
*/
void parser(); // Parses the call graph
void statistics(); // Keeps track of the statistics
int callgraph_gen( char* argv ); // Generates the callgraph from bitcode
#endif //_PIPAIR_
