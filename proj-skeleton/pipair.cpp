#include <iostream>
#include <unistd.h> // for system calls

#include <string>
#include <vector>

using namespace std;

unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence

enum 
{
  PIPE_READ = 0,
  PIPE_WRITE,
};


 



void parser(std::vector<string> &callgraph){



}

void statistic(){
  
}

int callgraph_gen( char* argv, vector<string> &callgraph ) {

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
    return 1;
  }

  /* create child process */
  opt_pid = fork();
  if (!opt_pid) { /* child process, to spawn opt */

    /* close the read end, since opt only write */
    close(pipe_callgraph[PIPE_READ]);  

    /* bind pipe to stderr, and check */
    if (dup2(pipe_callgraph[PIPE_WRITE], STDERR_FILENO) < 0) {
      cerr << "dup2 pipe_callgraph" << endl;
      return 1;
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
      return 1;
    }
     
    close(dummyPipe[1]);
    close(dummyPipe[0]);


    /* unreachable code */
    return 0;
  }

  /* parent process */
  

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
	while(getline(cin, line)) {
		callgraph.push_back(line);
	}
  /* "That's all folks." */
  return 0;
}


int main(int argc, char* argv[]) {
	vector<string> callgraph;
	switch( argc ) {
		case 2:
			callgraph_gen( argv[1], callgraph );
			break;
		case 4:
			T_SUPPORT = atoi(argv[2]);
			T_CONFIDENCE = atoi(argv[3]);
			callgraph_gen( argv[1], callgraph );
			break;
		default:
			cerr << "Your Command line paramaters are wrong!" << endl;
	}; // switch
	

  //cout <<"\n\n\n\n\n\n\n\n\n\n" << endl;
  
	for(int i =0; i < callgraph.size(); i++)
	{		

     cout << callgraph[i] << endl;
	}

}
