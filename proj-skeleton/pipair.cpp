#include <iostream>
#include <unistd.h> // for system calls
#include <algorithm>
#include <iomanip>
#include "pipair.h"

const bool DEBUG = false;

// GLOBAL DEFINITIONS
unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence
vector< string > callgraph_tokens;	// tokenize the callgraph
map< int, string > IDtoFunc;
map< string, int > FunctoID;
map< int, vector<int> > FuncCalls;
int maxID;
vector< map< int, FunctionPair > > Pairs;

string getFuncFromToken( const string &token ) { 
	unsigned first, last;
	first = token.find_first_of( "\'" ) + 1;
	last = token.find_last_of( "\'" ) ;
	
	return token.substr( first, last - first );
}// getFuncFromToken

//* 
void parser( ) {
	
	int ID = -1;
	int i;
	string func, TopLevelFunc = "";

	// This is to preset the iterator to the lines after the <<null function>> call
	for( i = 1; i < callgraph_tokens.size(); i++ ) {
		if( callgraph_tokens[ i ] == "" ) {
			i++;
			break;
		}//
	}//
	
	for( ; i < callgraph_tokens.size(); i++ ) {
		
		if( callgraph_tokens[ i ] == "" ) {			
			continue;
		}

		if( callgraph_tokens[ i ].find( "function:" ) != string::npos ||
			callgraph_tokens[ i ].find( "function" ) != string::npos ) {					
			func = getFuncFromToken( callgraph_tokens[ i ] );
			if( FunctoID.find( func ) == FunctoID.end() ) {
				ID++;
				FunctoID[ func ] = ID;
				IDtoFunc[ ID ] = func;
			}//if
		}//if
		
		if( callgraph_tokens[ i ].find( "function:" ) != string::npos ) {
			TopLevelFunc = getFuncFromToken( callgraph_tokens[ i ] );
		}//if
		else if(callgraph_tokens[ i ].find( "function" ) != string::npos ) {	

			func = getFuncFromToken( callgraph_tokens[ i ] );

            if( FuncCalls.find( FunctoID[ TopLevelFunc ]) != FuncCalls.end()) {
                if( find( FuncCalls[ FunctoID[ TopLevelFunc ] ].begin(),
						  FuncCalls[ FunctoID[ TopLevelFunc ] ].end(),
						  FunctoID[ func ]) == FuncCalls[ FunctoID[ TopLevelFunc ] ].end() ) {
                       
                    FuncCalls[ FunctoID[ TopLevelFunc ] ].push_back( FunctoID[ func ] );
                }//if
            }// if
            else {                 
				FuncCalls[ FunctoID[ TopLevelFunc ] ].push_back( FunctoID[ func ] );
            }// else
		}//else
		
	}// for
	maxID = ID;	
}// parser
//*/

// Using the parse data, calculate the support for functions and function pairs, and then return the function pairs which we have inferred must always occur together
void analyze() {
	Pairs = vector< map< int, FunctionPair > > ( maxID + 1 );
	vector< int > support = vector< int >( maxID, 0 );
	int a = 0, b = 0;

	//Calculate support for each function and function pair
	for( map< int, vector< int > >::iterator i = FuncCalls.begin(); i != FuncCalls.end(); ++i ) {

		vector< int > &v = i->second;
		//Go through all function pairs
		for( int j = 0; j < v.size(); j++ ) {
			a = v[ j ];
			support[ a ]++;
			for( int k = 0; k < v.size(); k++ ){				
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
	map< int, FunctionPair >::iterator temp;
    for ( int i = 0; i < Pairs.size(); i++ ){
		for ( map< int, FunctionPair >::iterator j = Pairs[ i ].begin(); j != Pairs[ i ].end(); ) {
			FunctionPair &p = j->second;
			temp = j;
			++j;
			p.confidence = ( float(p.support) * 100.0f) / float(support[ p.a ]);
			if( p.support < T_SUPPORT || p.confidence < float(T_CONFIDENCE)){				 
				//Doesn't meet support or confidence threasholds
				Pairs[ i ].erase( temp);
			} //if
		}//for
	}//for
}//  analyze

void find_bugs() {
	bool found = false;
	int a, b;
	string pairname = "";
	
	//Look through all top-level functions
	for( map< int, vector< int > >::iterator i = FuncCalls.begin(); i != FuncCalls.end(); ++i ) {
		vector< int > &v = i->second;
		for( int q = 0; q < v.size(); q++ ) {
			a = v[ q ];
			//Look for all the functions that should be used with any invocation of a
			for ( map< int, FunctionPair >::iterator j = Pairs[ a ].begin(); j != Pairs[ a ].end(); ++j ) {
				found = false;
				FunctionPair &p = j->second;
				//See if we find a use of p.b				
				for( int k = 0; k < v.size(); k++ ){
					b = v[ k ];
					if(p.b == b) {
						found = true;
					}
				}// for
				if(!found) {
					//Admiral Ackbar says: "It's a bug!
					if(IDtoFunc[ p.a ] < IDtoFunc[ p.b ]) {
						pairname = IDtoFunc[ p.a ] + " " + IDtoFunc[ p.b ];						
					} else {
						pairname = IDtoFunc[ p.b ] + " " + IDtoFunc[ p.a ];
					}					
					cout << "bug: " << IDtoFunc[ p.a ] << " in " << IDtoFunc[ i->first ] << " pair: (" << pairname << ") ";
					cout << "support: " << p.support << " confidence: " << fixed << setprecision (2) << p.confidence   << "%" <<endl;					
				}
			}// for		
		}// for
	}// for
}




int main(int argc, char* argv[]) {	

	string line;
	string token="";
	//mark 1

	switch( argc ) {		
		case 3:
			T_SUPPORT = atoi( argv[ 1 ] );
			T_CONFIDENCE = atoi( argv[ 2 ] );
			// FALL THROUGH
		case 1:
			while( getline(cin , token ) ) {
				callgraph_tokens.push_back( token );
				//tokens.push_back( token );
			}// while
			break;
		default:
			cerr << "Your Command line paramaters are wrong!" << endl;
	}; // switch   
	
	parser( );
 	
	
	if(DEBUG) {
		// To see what we have in those data structure. 
		cout << "function map :" << endl;
		for ( map< int, string >::iterator  it = IDtoFunc.begin(); it != IDtoFunc.end(); ++it )
			cout << it->first << " => " << it->second << '\n';

		cout << endl<<"Call functions :" << endl;
		for ( map< int, vector < int > >::iterator  it = FuncCalls.begin(); it != FuncCalls.end(); ++it ){
			cout << it->first << " calls: ";
			for ( int it2 = 0; it2 < it->second.size(); it2++ ){
				cout << it->second[ it2 ] <<" ";
			}
			cout << endl;
		}  	
	}
	
	if(DEBUG) {
		//To see the original Bitcode
		for( int i =0; i < callgraph_tokens.size(); i++ )
		{		
			cout << callgraph_tokens[ i ] << endl;
		}
	}
	
	analyze();  	
	if(DEBUG) {
		cout << "Learned Constraints :" << flush << endl;	
		//Loop through all function pairs
		for( int i = 0; i < Pairs.size(); i++ ){
			for( map< int, FunctionPair >::iterator j = Pairs[ i ].begin(); j != Pairs[ i ].end(); ++j ) {
				FunctionPair &p = j->second;
				cout << "bug: " << IDtoFunc[ p.a ] << " in " << IDtoFunc[ i ] << ", pair: (" << IDtoFunc[ p.a ] << " " << IDtoFunc[ p.b ] << "), support: " << p.support << ", condfidence: " << p.confidence << ".00%" <<endl;
			}
		}
	}
	find_bugs();

}
