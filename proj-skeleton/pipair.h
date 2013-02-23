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
<<<<<<< HEAD
void parser(); // Parses th
void callgraph_gen( char* argv );
void statistic();

=======
void parser(); // Parses the call graph
void statistics(); // Keeps track of the statistics
int callgraph_gen( char* argv ); // Generates the callgraph from bitcode
>>>>>>> Parser Finished
#endif //_PIPAIR_
