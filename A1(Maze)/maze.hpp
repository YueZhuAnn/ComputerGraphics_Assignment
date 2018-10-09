// Fall 2018

#pragma once

class Maze
{
public:
	Maze( size_t dim );
	~Maze();

	void reset();

	size_t getDim() const;

	int getValue( int x, int y ) const;
	void setValue( int x, int y, int h );
	int getTotalBlock();
	bool getStatus();
	void digMaze();
	void printMaze(); // for debugging
	int getStartPos();
private:
	bool status;
	size_t m_dim;
	int *m_values;
	int totalBlock;
	int startPos;
	void recDigMaze(int r, int c);
	int numNeighbors(int r, int c);
	void calculateTotalBlock();
	void activateStatus();
};
