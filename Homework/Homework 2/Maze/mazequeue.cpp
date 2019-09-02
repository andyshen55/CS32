#include <iostream>
#include <queue>
#include <string>
using namespace std;

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec);
// Return true if there is a path from (sr,sc) to (er,ec)
// through the maze; return false otherwise

int main()
{
	string maze[10] = {
		"XXXXXXXXXX",
		"X....X...X",
		"X.XX.XX..X",
		"XXX....X.X",
		"X.XXX.XXXX",
		"X.X...X..X",
		"X...X.X..X",
		"XXXXX.X.XX",
		"X........X",
		"XXXXXXXXXX"
	};

	string maze2[8] =
	{
		"XXXXXXXX",
		"X..XXXXX",
		"XX...XXX",
		"X.XX..XX",
		"XX.XX.XX",
		"XXXXX..X",
		"X.XXXX.X",
		"XXXXXXXX"
	};

	string maze3[10] = {
	{ 'X','X','X','X','X','X','X','X','X','X'},
	{ 'X','.','.','.','.','.','.','.','.','X'},
	{ 'X','X','.','X','.','X','X','X','X','X'},
	{ 'X','.','.','X','.','X','.','.','.','X'},
	{ 'X','.','.','X','.','.','.','X','.','X'},
	{ 'X','X','X','X','.','X','X','X','.','X'},
	{ 'X','.','X','.','.','.','.','X','X','X'},
	{ 'X','.','.','X','X','.','X','X','.','X'},
	{ 'X','.','.','.','X','.','.','.','.','X'},
	{ 'X','X','X','X','X','X','X','X','X','X'}
	};

	if (pathExists(maze3, 10, 10, 6, 4, 1, 1))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;

	if (pathExists(maze, 10, 10, 3, 5, 8, 8))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;

	if (pathExists(maze2, 8, 8, 1, 1, 6, 6))
		cout << "Solvable!" << endl;
	else
		cout << "Out of luck!" << endl;
}

class Coord
{
public:
	Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
	int r() const { return m_r; }
	int c() const { return m_c; }
private:
	int m_r;
	int m_c;
};

bool pathExists(string maze[], int nRows, int nCols, int sr, int sc, int er, int ec)
{
	queue<Coord> coordQueue;
	coordQueue.push(Coord(sr, sc));
	maze[sr][sc] = 'X';
	while (!coordQueue.empty())
	{
		Coord currentCoord = coordQueue.front();
		coordQueue.pop();
		int currentRow = currentCoord.r();
		int currentCol = currentCoord.c();
		//check if end coordinates have been reached
		if (currentRow == er && currentCol == ec)
			return true;
		//check if south is open
		if (currentRow < nRows && maze[currentRow + 1][currentCol] == '.')
		{
			coordQueue.push(Coord(currentRow + 1, currentCol));
			maze[currentRow + 1][currentCol] = 'X';
		}
		//check if west is open
		if (currentCol > 0 && maze[currentRow][currentCol - 1] == '.')
		{
			coordQueue.push(Coord(currentRow, currentCol - 1));
			maze[currentRow][currentCol - 1] == 'X';
		}
		//chekc if north is open
		if (currentRow > 0 && maze[currentRow - 1][currentCol] == '.')
		{
			coordQueue.push(Coord(currentRow - 1, currentCol));
			maze[currentRow - 1][currentCol] = 'X';
		}
		//check if east is open
		if (currentCol < nCols && maze[currentRow][currentCol + 1] == '.')
		{
			coordQueue.push(Coord(currentRow, currentCol + 1));
			maze[currentRow][currentCol + 1] = 'X';
		}
	}
	//if stack is empty, no solution was found
	return false;
}