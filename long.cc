/**
    PLCP
    Copyright (C) 2017 Lorraine A.K. Ayad and Panagiotis Charalampopoulos 
    and Costas S. Iliopoulos and Solon P. Pissis
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <iostream>
#include <algorithm>  
#include <vector>  
#include <fstream>
#include <sys/time.h>

#include "plcp.h"

using namespace std;

unsigned int k_mappability( unsigned char * x, struct TSwitch  sw, unsigned int * PLCP, unsigned int * P, INT * SA, INT * LCP )
{
	
	INT block_length = sw . m / ( sw . k + 2 );
	int l = strlen( (char*) x );

	for(int i=0; i<l; i++)
	{
		int j = 1;
	
		//////cout<<"invsai"<<SA[i] <<" "<<"invsaj" <<SA[i-j]<<" lcp "<< range_min_query( i, j,  LCP, x )<<endl;
		//////cout<<" block length "<<block_length<<endl;

		int rmq = LCP [ i ];
		while ( rmq >= block_length && i-j >= 0 )
		{
			if( SA[i-j] % block_length == 0  || SA[i] % block_length == 0 )
			{
				int left_i = SA[i];
				int right_i = SA[i] + rmq - 1 ;

				int left_j = SA[i-j];
				int right_j = SA[i-j] + rmq - 1 ;


				////cout<<" left and right i "<<left_i<<" "<<right_i<<endl;
				////cout<<" left and right j "<<left_j<<" "<<right_j<<endl;
				int hd_l = 0;
				int hd_r = 0;

				vector<int> * errors_i = new vector<int>();	
				vector<int> * errors_j = new vector<int>();

				/* Identify positions of errors in left and right direction */
				while( hd_l <= sw . k  ||  hd_r <= sw . k )
				{
	
					if( left_i > 0 && left_j > 0 && hd_l <= sw . k )
					{
						if ( x[ left_i - 1 ] == x[ left_j - 1 ] )
						{
							left_i--;
							left_j--;
						}
						else
						{
							if( hd_l < sw . k )
							{
								////cout<<" lefti-1 "<<left_i -1 <<endl;
								errors_i->push_back(left_i - 1 );
								errors_j->push_back(left_j - 1 );
							}
					
							hd_l++;
		
							if ( hd_l <= sw . k )
							{
								left_i--;
								left_j--;
							}

							////cout<< hd_l <<" hdl "<<endl;
						}
					}
					else hd_l++;
				

					if( right_i < l - 1 && right_j < l - 1 && hd_r <= sw . k )
					{
						if( x[right_i+1] == x[right_j+1] )
						{////cout<<" match "<<endl;
							right_i++;
							right_j++;
						}
						else
						{
							if( hd_r < sw . k )
							{	////cout<<" enter "<<endl;
								errors_i->push_back(right_i + 1 );	
								errors_j->push_back(right_j + 1 );
							}
					
							hd_r++;
						
							if ( hd_r <= sw . k )
							{
								right_i++;
								right_j++;
								////cout<<" right i "<<right_i<<endl;
							}
	
							////cout<< hd_r <<" hdr "<<endl;
						}
					}
					else hd_r++;
					
					////cout<<" left and right i loop "<<left_i<<" "<<right_i<<endl;
					////cout<<" left and right j loop "<<left_j<<" "<<right_j<<endl;
				
				}	

				////cout<<" left and right i OUT loop "<<left_i<<" "<<right_i<<endl;
				////cout<<" left and right j OUT loop "<<left_j<<" "<<right_j<<endl;
				

				if( left_i == 0 )
					errors_i->push_back( -1 );
				else errors_i->push_back( left_i - 1 );	
				if( left_j == 0 )
					errors_j->push_back( -1 );
				else errors_j->push_back( left_j - 1 );
				if( right_i == l-1 )
					errors_i->push_back( l );
				else errors_i->push_back( right_i + 1 );
				if( right_j == l-1 )
					errors_j->push_back( l );
				else errors_j->push_back( right_j + 1 );

				sort( errors_i->begin(), errors_i->end() );
				sort( errors_j->begin(), errors_j->end() );

				/*cout<<" errors i "<<endl;
				for(int i=0; i<errors_i->size(); i++)
				cout<<errors_i->at(i)<<" ";
				cout<<endl;
				cout<<" errors j "<<endl;
				for(int i=0; i<errors_j->size(); i++)
				cout<<errors_j->at(i)<<" ";
		
				cout<<endl;*/

				int z = 0;

				/* Check positions where an error occurs ( start of extension is an error ) */
				for(int a = 0; a<errors_i->size(); a++ ) 
				{

					if ( errors_i->size()-(a+1) > sw . k  )// Number of error positions is larger than sw . k
					{
						if( errors_i->at(a) != - 1 && errors_j->at(a) != - 1 )
						{
							if( errors_i->at( a + sw.k)- errors_i->at(a) > PLCP[  errors_i->at(a) ] )
							{
								PLCP[  errors_i->at(a) ] = errors_i->at( a + sw.k)- errors_i->at(a);
								P[ errors_i->at(a) ] =  errors_j->at(a);	
							}
						}

						if( errors_i->at(a) != - 1 && errors_j->at(a) != - 1 )
						{

							if(  errors_j->at( a + sw.k )- errors_j->at(a) > PLCP[  errors_j->at(a) ] )
							{
								PLCP[  errors_j->at(a) ] = errors_j->at( a + sw.k )- errors_j->at(a);
								P[ errors_j->at(a) ] =  errors_i->at(a);
							}
						}
					}
					else // Number of positions less than sw . k
					{
					
						if( errors_i->at(a) != - 1 && errors_j->at(a) != - 1 )
						{
							if( right_i - errors_i->at(a)+1 > PLCP[  errors_i->at(a) ] )
							{
								PLCP[  errors_i->at(a) ] = right_i - errors_i->at(a) + 1;
								P[ errors_i->at(a) ] =  errors_j->at(a);	
							}
						}

						if( errors_i->at(a) != - 1 && errors_j->at(a) != - 1 )
						{

							if(  right_j - errors_j->at(a) +1 > PLCP[  errors_j->at(a) ] )
							{
								PLCP[  errors_j->at(a) ] = right_j - errors_j->at(a) + 1;
								P[ errors_j->at(a) ] =  errors_i->at(a);
							}
						}
					}
				}

				/* Check positions in between errors */
				for(int a=0; a<errors_i->size(); a++)
				{
					if( errors_i->size()-(a+1) > sw . k + 1  ) // Check when number of positions is larger than sw.k
					{

						z = errors_j->at(a)+1;
						//cout<<errors_i->at(a)+1<<endl;
						//cout<<errors_i->at(a+1)<<endl;	
						for(int k =errors_i->at(a)+1; k<errors_i->at(a+1); k++)
						{	
						
							if( errors_i->at( a + sw.k + 1  ) - k > PLCP[ k ]  &&  errors_i->at( a + sw.k + 1  ) - k >= sw . m)
							{//cout<<" A I "<<k<<endl;
								PLCP[ k ] = errors_i->at( a + sw.k + 1  ) - k;
								P[ k ] =  z;
								//cout<<"----A and res "<<i<<" "<<PLCP[i]<<endl;	
							}

							z++;
						}

						z = errors_i->at(a)+1;
						//cout<<errors_j->at(a)+1<<endl;
						//cout<<errors_j->at(a+1)<<endl;
						for(int k =errors_j->at(a)+1; k<errors_j->at(a+1); k++)
						{
							
							////cout<<errors_j->at( a + sw.k + 1  )- k<<" "<<PLCP[k]<<endl;
							if(  errors_j->at( a + sw.k + 1  )- k > PLCP[ k ] &&  errors_j->at( a + sw.k + 1  )- k >= sw . m )
							{//cout<<" B I "<<k<<endl;
							//cout<<errors_j->at( a + sw.k + 1  )<<endl;
								PLCP[ k ] =  errors_j->at( a + sw.k + 1  ) - k;
								P[ k ] =  z;
								//cout<<"----B and res "<<i<<" "<<PLCP[i]<<endl;
							}
							z++;	
						}
					}
					else
					{
						/* Check remaining positions which have less than sw . k errors */
						z = errors_j->at(a)+1; 
						for(int k=errors_i->at(a)+1; k<=right_i; k++)
						{//cout<<" 1 K "<<k<<endl;
							if( right_i - k + 1 > PLCP[ k ] && right_i - k + 1 >= sw . m  )
							{//cout<<" A k "<<k<<endl;
								PLCP[ k ] = right_i - k + 1;	
								P[ k ] = z;
	
							}	
							z++;				

						}

						z = errors_i->at(a)+1;
						for(int k=errors_j->at(a)+1; k<=right_j; k++)
						{//cout<<" 2 K "<<k<<endl;
							if( right_j - k + 1 > PLCP[ k ] && right_j - k + 1 >= sw . m )
							{	//cout<<" B k "<<k<<endl;
								PLCP[ k ] = right_j - k + 1;
								P[ k ] = z;
							}
							z++;
						}
					}
				}

				delete( errors_i );
				delete( errors_j );

			}
			j++;
			if( rmq > LCP[ i-j+1 ]  )
				rmq = LCP[ i-j+1 ];
		}		

	}
		
return 1;
}
