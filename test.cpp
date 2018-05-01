#include <bits/stdc++.h>

using namespace std ;

#define mp make_pair
#define pb push_back
#define x first
#define y second

typedef long long int lli ;
const int MAXN = 5000 ;
bool arr[MAXN][MAXN] ;
int main(int argc,char* argv[]){
	int nrows = atoi(argv[1]) ;
	int ncols = atoi(argv[2]) ;
	int nelem = atoi(argv[3]) ;
	srand(time(NULL)) ;
	FILE *file = fopen(argv[4],"wb") ;
	for(int i=0 ; i<nelem ; i++){
		int row = -1 ; int col = -1 ;
		do{
			row = rand()%nrows ; col = rand()%ncols ;
		}while(arr[row][col]) ;
		arr[row][col] = true ;
		int ii = rand() ; float ff = (float) rand() ;
		// cout << "(" << row << "," << col << "," << ii << "," << ff << ")\n" ;
		fwrite(&row,4,1,file) ;
		fwrite(&col,4,1,file) ;
		fwrite(&ii,4,1,file) ;
		fwrite(&ff,4,1,file) ;
	}
	fclose(file) ;
}