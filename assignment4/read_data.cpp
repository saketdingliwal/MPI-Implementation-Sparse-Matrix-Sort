#include <bits/stdc++.h>
#include <mpi.h>
#include <sys/stat.h>

using namespace std;

float inf = numeric_limits<float>::infinity();


int get_work(int a,int b)
{
  if (a%b==0)
    return (a/b);
  return ((a/b)+1);
}

typedef struct node
{
  int row;
  int col;
  pair<int,float> key;
}node;

vector <vector<node> > row_order;
vector <vector<node> > col_order;

bool rowcompare(node lhs, node rhs) 
{
  if(lhs.key.second == rhs.key.second)
    return lhs.col < rhs.col;

  return lhs.key.second < rhs.key.second;
}
bool colcompare(node lhs, node rhs) 
{
  if(lhs.key.first==rhs.key.first)
    return lhs.row < rhs.row;
  
  return lhs.key.first < rhs.key.first;
}


void free_row()
{
  for(int i=0;i<row_order.size();i++)
  {
    row_order[i].clear();
  }
}
void free_col()
{
  for(int i=0;i<col_order.size();i++)
  {
    col_order[i].clear();
  }
}
void row_print()
{
  for(int i=0;i<row_order.size();i++)
  {
    for(int j=0;j<row_order[i].size();j++)
    {
      cout << row_order[i][j].row<< " " << row_order[i][j].col << " " << row_order[i][j].key.first << " " << row_order[i][j].key.second << endl;
    }
    cout << endl;
  }
}
void col_print()
{
  for(int i=0;i<col_order.size();i++)
  {
    for(int j=0;j<col_order[i].size();j++)
    {
      cout << col_order[i][j].row << " " << col_order[i][j].col << " " << col_order[i][j].key.first << " " << col_order[i][j].key.second << endl;
    }
    cout << endl;
  }
}
void row2col()
{
  for(int i=0;i<row_order.size();i++)
  {
    for(int j=0;j<row_order[i].size();j++)
    {
      int col = row_order[i][j].col;
      col_order[col].push_back(row_order[i][j]);
    }
  }
}

void col2row()
{
  for(int i=0;i<col_order.size();i++)
  {
    for(int j=0;j<col_order[i].size();j++)
    {
      int row = col_order[i][j].row;
      row_order[row].push_back(col_order[i][j]);
    }
  }
}


int main(int argc,char * argv[])
{

	int rank;
	int num_proc;
  string input_name = argv[1];
  string output_name = argv[2];
  int max_col = atoi(argv[3]);
  int max_row = atoi(argv[4]);
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD,&num_proc);
  int num_elem;
  int disp_row[num_proc];
  int count_row[num_proc];
  int disp_col[num_proc];
  int count_col[num_proc];
  int row_size = get_work(max_row,num_proc);
  int col_size = get_work(max_col,num_proc);
  max_row = row_size * num_proc;
  max_col = col_size * num_proc;

  if(rank==0)
  {
    FILE *pFile;
    long lSize;
    pFile = fopen ( input_name.c_str()  , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
    // struct stat st;
    // stat(input_name.c_str(), &st);
    // lSize = st.st_size;
    fseek(pFile, 0, SEEK_END); // seek to end of file
    lSize = ftell(pFile); // get current file pointer
    fseek(pFile, 0, SEEK_SET);
    row_order.resize(max_row);
    col_order.resize(max_col);
    num_elem = lSize / (3*sizeof(int) + sizeof(float));
    // cout << lSize <<endl;
    for(int i=0;i<num_elem;i++)
    {
      int row;
      int col;
      int key_1;
      float key_2;
      size_t result = fread(&row,sizeof(int),1,pFile);
      result = fread(&col,sizeof(int),1,pFile);
      result = fread(&key_1,sizeof(int),1,pFile);
      result = fread(&key_2,sizeof(float),1,pFile);
      node elem;
      elem.row = row;
      elem.col = col;
      elem.key = make_pair(key_1,key_2);
      row_order[row].push_back(elem);
    }
    fclose(pFile);
    row2col();
    free_row();
    col2row();
  }
  int *row_count;
  int *col_count;
  row_count = (int*) malloc(sizeof(int)*max_row);
  col_count = (int*) malloc(sizeof(int)*max_col);
  if(rank==0)
  {
    for(int i=0;i<row_order.size();i++)
    {
      row_count[i] = row_order[i].size();
    }
    for(int i=0;i<col_order.size();i++)
    {
      col_count[i] = col_order[i].size();
    }
  }
  MPI_Bcast(row_count,max_row,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(col_count,max_col,MPI_INT,0,MPI_COMM_WORLD);

  int counter=0;
  for(int i=0;i<num_proc;i++)
  {
    int summ =0;
    for(int j=i*row_size;j<(i+1)*row_size;j++)
    {
      summ += row_count[j];
    }
    count_row[i] = summ;
  }
  for(int i=0;i<num_proc;i++)
  {
    int summ =0;
    for(int j=i*col_size;j<(i+1)*col_size;j++)
    {
      summ += col_count[j];
    }
    count_col[i] = summ;
  }

  disp_row[0] = 0;
  for(int i=1;i<num_proc;i++)
  {
    disp_row[i] = count_row[i-1] + disp_row[i-1];
  }
  disp_col[0] = 0;
  for(int i=1;i<num_proc;i++)
  {
    disp_col[i] = count_col[i-1] + disp_col[i-1];
  }


  node *linearized_mat;
  if(rank==0)
  {
    linearized_mat = (node *) malloc(sizeof(node)*num_elem);
  }
  node *my_row;
  my_row = (node *) malloc(sizeof(node)*count_row[rank]);

  node *my_col;
  my_col = (node *) malloc(sizeof(node)*count_col[rank]);

  MPI_Datatype type[4] = {MPI_INT,MPI_INT,MPI_INT,MPI_FLOAT};
  int blocklen[4] = {1,1,1,1};
  MPI_Aint disp[4] = {0,4,8,12};


  MPI_Datatype mpinode;
  MPI_Type_struct(4,blocklen,disp,type,&mpinode);
  MPI_Type_commit(&mpinode);

  while(counter<4)
  {
    if(rank==0)
    {
      int put=0;
      for(int i=0;i<row_order.size();i++)
      {
        for(int j=0;j<row_order[i].size();j++)
        {
          linearized_mat[put] = row_order[i][j];
          put++;
        }
      }
    }
    MPI_Scatterv(linearized_mat,count_row,disp_row,mpinode,my_row,count_row[rank],mpinode,0,MPI_COMM_WORLD);
    int l_bound = 0;
    int r_bound = 0;
    for(int i=rank*row_size;i<(rank+1)*row_size;i++)
    {
      r_bound = l_bound + row_count[i];
      sort(my_row +l_bound,my_row + r_bound,rowcompare);
      l_bound = r_bound;
    }
    MPI_Gatherv(my_row,count_row[rank],mpinode,linearized_mat,count_row,disp_row,mpinode,0,MPI_COMM_WORLD);
    if(rank==0)
    {
      int put = 0;
      for(int i=0;i<row_order.size();i++)
      {
        for(int j=0;j<row_order[i].size();j++)
        {
          row_order[i][j].key = linearized_mat[put].key;
          put++;
        }
      }
      free_col();
      row2col();
      put = 0;
      for(int i=0;i<col_order.size();i++)
      {
        for(int j=0;j<col_order[i].size();j++)
        {
          linearized_mat[put] = col_order[i][j];
          put++;
        }
      }
    }
    MPI_Scatterv(linearized_mat,count_col,disp_col,mpinode,my_col,count_col[rank],mpinode,0,MPI_COMM_WORLD);
    l_bound = 0;
    r_bound = 0;
    for(int i=rank*col_size;i<(rank+1)*col_size;i++)
    {
      r_bound = l_bound + col_count[i];
      sort(my_col + l_bound,my_col + r_bound,colcompare);
      l_bound = r_bound;
    }
    MPI_Gatherv(my_col,count_col[rank],mpinode,linearized_mat,count_col,disp_col,mpinode,0,MPI_COMM_WORLD);
    if(rank==0)
    {
      int put = 0;
      for(int i=0;i<col_order.size();i++)
      {
        for(int j=0;j<col_order[i].size();j++)
        {
          col_order[i][j].key = linearized_mat[put].key;
          put++;
        }
      }
      free_row();
      col2row();
    }
    counter+=1;
  }
  if(rank==0)
  {
    FILE * pFile;
    pFile = fopen (output_name.c_str(), "wb");
    for(int i=0;i<row_order.size();i++)
    {
      for(int j=0;j<row_order[i].size();j++)
      {
        fwrite(&row_order[i][j].row,sizeof(int),1,pFile);
        fwrite(&row_order[i][j].col,sizeof(int),1,pFile);
        fwrite(&row_order[i][j].key.first,sizeof(int),1,pFile);
        fwrite(&row_order[i][j].key.second,sizeof(float),1,pFile);
      }
    }
    fclose(pFile);
  }
  
  MPI_Finalize();
  return 0;
}
