#include <iostream>
#include <unistd.h> // for system calls
#include <algorithm>
#include "pipair.h"

// GLOBAL DEFINITIONS
unsigned int T_SUPPORT = 3; // default support
unsigned int T_CONFIDENCE = 65; // default confidence
vector< string > callgraph_tokens;	// tokenize the callgraph
map< int, string > IDtoFunc;
map< string, int > FunctoID;
map< int, vector<int> > FuncCalls;
int maxID;

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
	unsigned foundTop;
	string func, TopLevelFunc = "";
	
	for( i = 1; i < callgraph_tokens.size(); i++ ) {
		
		if( callgraph_tokens[ i ] == "" ) {
			// no more functions are left so we stop
			break;
		}
		ID++;
		func = getFuncFromToken( callgraph_tokens[ i ] );
		FunctoID[ func ] = ID;
		IDtoFunc[ ID ] = func;
		
	}// for
	maxID = ID;
	
	for( ; i < callgraph_tokens.size(); i++ ) {
		
		if( callgraph_tokens[ i ] == "" ) {
			continue;
		}
		
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
	}//for
	
}// parser
//*/

// Using the parse data, calculate the support for functions and function pairs, and then return the function pairs which we have inferred must always occur together
void analyze( vector< map< int, FunctionPair > > &Pairs ) {

	cout << "Beginning Analysis" << flush << endl; 
	vector< int > support = vector< int >( maxID, 0 );
	int a = 0, b = 0;

	//Calculate support for each function and function pair
	for( map< int, vector< int > >::iterator i = FuncCalls.begin(); i != FuncCalls.end(); ++i ) {

		vector< int > &v = i->second;
		//Remove duplicate function calls (maybe should do in the parser?)
		//sort( v.begin(), v.end() );
		//v.erase( unique( v.begin(), v.end() ), v.end() );

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
    for ( int i = 0; i < Pairs.size(); i++ ){
		for ( map< int, FunctionPair >::iterator j = Pairs[ i ].begin(); j != Pairs[ i ].end(); ++j ) {
			FunctionPair &p = j->second;
			cout << p.a << " " << p.b << " " << p.support << " " << (p.support * 100 )/ support[p.a] << endl;
			if( p.support < T_SUPPORT || ( ( p.support * 100 ) / support[ p.a ] ) < T_CONFIDENCE){
				//Doesn't meet support or confidence threasholds
				Pairs[ i ].erase( j );
				//cout << "i val: " << i << endl;
			} //if
			else {
				//Does meet thresholds, keep and record confidence
				p.confidence = ( p.support * 100 ) / support[ p.a ];
			}//else
		}//for
	}//for
	cout << "Ending Analysis" << flush << endl;
}//  analyze

void find_bugs() {
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
	
	// Your Code here:
	vector< map< int, FunctionPair > > Pairs( maxID + 1 );
	analyze( Pairs );  
	cout << "Learned Constraints :" << flush << endl;	
	//Loop through all function pairs
	for( int i = 0; i < Pairs.size(); i++ ){
		for( map< int, FunctionPair >::iterator j = Pairs[ i ].begin(); j != Pairs[ i ].end(); ++j ) {
			FunctionPair &p = j->second;
			cout << "bug: " << IDtoFunc[ p.a ] << " in " << IDtoFunc[ i ] << ", pair: (" << IDtoFunc[ p.a ] << " " << IDtoFunc[ p.b ] << "), support: " << p.support << ", condfidence: " << p.confidence << ".00%" <<endl;
		}
	}

	//To see the original Bitcode. To use this, u need to comment out line 175 to line 179 (mark 1 to mark 1 end)
	for( int i =0; i < callgraph_tokens.size(); i++ )
	{		
		//cout << callgraph_tokens[ i ] << endl;
	}

}
