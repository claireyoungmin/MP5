#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <stack>
#include <typeinfo>

using namespace std;

enum myBlock{wall,origin,openSpace,destination,path};

class square{
public:
    int x_pos, y_pos;
	myBlock category;
	square();
	square(char,int,int);
	bool visit;
};

square::square(){
}

square::square(char s,int x, int y){
	if(s=='#')
		category = wall;
	else if(s=='o')
		category = origin;
	else if(s=='.')
		category = openSpace;
	else if(s=='*')
		category = destination;
   	x_pos = x;
	y_pos = y;
}

class maze{
public:
    square **myMaze;
	int rowSize, colSize;
	int x_origin, y_origin;
	maze(int,int,char*);
	~maze();
	string toString();
};

maze::maze(int m, int n, char* sq){
	rowSize = m;
	colSize = n;
    myMaze = new square*[m];
    for(int i = 0; i < m; i++)
        myMaze[i] = new square[n];

	for(int i=0;i<m;i++){
		for(int j = 0; j<n; j++){
			square s(sq[((i*n)+j)],i,j);            //placing each of the squares in the maze
			myMaze[i][j] = s;
			if(myMaze[i][j].category==origin){      //storing the coordinates of the origin
                x_origin = i;
                y_origin = j;
			}
		}
	}
}

maze::~maze(){
	for(int i=0; i<rowSize; i++)
    	delete[] myMaze[i];
	delete[] myMaze;
}

string maze::toString(){
	string str = "";
	for(int i=0; i<rowSize; i++){
		for(int j = 0; j<colSize; j++){
			if(myMaze[i][j].category==wall)
               str+="#";
            else if(myMaze[i][j].category==origin)
                str+="o";
            else if (myMaze[i][j].category==destination)
                str+="*";
            else if (myMaze[i][j].category==openSpace)
                str+=".";
            else
                str+="x";
		}

		str+='\n';
	}
	return str;
}


template <class T >
class agenda{};

template<>
class agenda<queue<square> >{
private:
	queue<square> adt;
public:
	void add(square a){adt.push(a);}
	bool remove(){adt.pop();}
	bool isEmpty(){return adt.empty();}
	int getSize(){return adt.size();}
	square peek(){return adt.front();}
};

template<>
class agenda<stack<square> >{
private:
	stack<square> adt;
public:
	void add(square a){adt.push(a);}
	bool remove(){adt.pop();}
	bool isEmpty(){return adt.empty();}
	int getSize(){return adt.size();}
	square peek(){return adt.top();}
};



template <class T >
class mazeSolver{
private:
    maze *m;
    stack<agenda<T > > myOptions;
    stack<square> myPath;
public:
    int flag;
    mazeSolver(int,int,char*);
    ~mazeSolver();
    void solve();
    void writesolution();
};

template <class T >
mazeSolver<T >::mazeSolver(int m, int n, char* str){
	this->m = new maze(m,n,str);
}

template <class T >
mazeSolver<T >::~mazeSolver(){
	delete m;
}

template <class T >
void mazeSolver<T >::solve(){

	square current = m->myMaze[m->x_origin][m->y_origin];       // to start at the origin
	do{
	    square top = m->myMaze[current.x_pos-1][current.y_pos];    //to go to the top, bottom, left and right squares from the current square
        square bottom= m->myMaze[current.x_pos+1][current.y_pos];
        square left = m->myMaze[current.x_pos][current.y_pos-1];
        square right = m->myMaze[current.x_pos][current.y_pos+1];

		if(myOptions.empty() || !myOptions.top().isEmpty()){
			agenda<T> a;
			if(left.category!=wall && !(left.visit))            //making sure that the square is not a wall
                a.add(left);                                    //or the square is not visited yet
			if(top.category!=wall && !(top.visit))
                a.add(top);
			if(right.category!=wall&& !(right.visit))
                a.add(right);
			if(bottom.category!=wall&& !(bottom.visit))
                a.add(bottom);
            myOptions.push(a);                              //push our agenda in our myOptions Stack of agenda
			myPath.push(current);                           //push our current square to our actual path stack of squares
			m->myMaze[current.x_pos][current.y_pos].visit = true;       // to make sure the square is marked visited
			if(!a.isEmpty()){
				current = a.peek();             //move to the next square
            }
		}
        else{       //for cases when we do not have available options anymore
            myOptions.pop();
			myPath.pop();
			myOptions.top().remove();
			if(!myOptions.top().isEmpty()){
				current = myOptions.top().peek();       //go to the next square / backtracking
            }
        }
        if(myPath.top().category==origin && myOptions.top().isEmpty())      //to handle no solutions
            break;
	}while(m->myMaze[current.x_pos][current.y_pos].category!=destination);      //do this while we have not reached destination
}

template <class T >
void mazeSolver<T >::writesolution(){
	string str;
	while(!myPath.empty()){
		if(m->myMaze[myPath.top().x_pos][myPath.top().y_pos].category!=origin && (m->myMaze[myPath.top().x_pos][myPath.top().y_pos].category!=destination))
            m->myMaze[myPath.top().x_pos][myPath.top().y_pos].category=path;        //marking the path with 'x'
		myPath.pop();
	}
		ofstream fileOutput("maze.out", ofstream::out | ofstream::app);     //writing onto the file
		fileOutput << m->toString() << endl;
		fileOutput.close();
}


int main(){
    int NumberOfMazes;
	ifstream fileInput("maze.in");
	fileInput >> NumberOfMazes;
    int row, col;
	for(int i=0; i<NumberOfMazes; i++){
		fileInput >> row;					// take in the number of rows
		fileInput >> col;                   // take in number of columns
        char lines[row*col];
		for(int j = 0; j<row*col; j++){ //read each character from file
			char c;
			fileInput >> c;
			if(c!='\n')
                lines[j] = c;
		}
        maze *my_Maze = new maze(row, col,lines);
        mazeSolver<queue<square> > n(row,col,lines);
        n.solve();      //solve using queue
        n.writesolution();
        mazeSolver<stack<square> > m(row,col,lines);
        m.solve();      //solve using stack
        m.writesolution();

	}
	fileInput.close();
	return 0;
}

