#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct sudoku {
	uint8_t **initialPuzzle;
	uint8_t **solutionPuzzle;
	bool ***possibilitiesMatrix;
	uint8_t *emptyCells;
	uint8_t emptyCellsCount;
	int8_t unsolvedCellsIndex;
};

void clearUnsolvedCells(struct sudoku * puzzle)
{
	for(uint8_t index = 0; index<=puzzle->unsolvedCellsIndex; index++)
	{
		puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]=0;
	}
}

bool isInsertable(uint8_t value, uint8_t row, uint8_t col, struct sudoku * puzzle)
{
	if(value==0 || value >=10)
	{
		return false;
	}
	//value = value%10;
	//if(value==0) return false;
	//first check matrix
	if(puzzle->possibilitiesMatrix[row][col][value-1]==false)
	{
		return false;
	}

	//now check the actual solutions puzzle
	row=row%9;
	col=col%9;
	puzzle->solutionPuzzle[row][col]=value;

	//these lines handle the 3X3 box this the point is in
	uint8_t matrixIndex = 0;
	uint8_t rowt = 0;// = (3*(matrixIndex/3)) + (row/3);
	uint8_t colt = 0;// (matrixIndex%3) + (col/3);
	rowt =  3*(row/3);
      	colt =  3*(col/3);

	while (  matrixIndex< ((3*(row%3)) + (col%3))     )
	{
		if(puzzle->solutionPuzzle[rowt][colt]==value) return false;
		matrixIndex++;
		rowt =  (3*(row/3))+(matrixIndex/3);
       		colt =  (3*(col/3))+(matrixIndex%3);
	}
	matrixIndex++;
	while (  matrixIndex<=8 )
	{
		rowt =  (3*(row/3))+(matrixIndex/3);
       		colt =  (3*(col/3))+(matrixIndex%3);
		if(puzzle->solutionPuzzle[rowt][colt]==value) return false;
		matrixIndex++;
	}

	//these lines fill in the row and column outside of the box that the cell is in
	//column first
	for(uint8_t index=0; (index/3)<(row/3); index++)
	{
		if(puzzle->solutionPuzzle[index][col]==value) return false;
	}
	//skip the 3X3 box, it is already filled in
	for(uint8_t index=3*((row/3)+1); index<=8; index++)
	{
		if(puzzle->solutionPuzzle[index][col]==value) return false;
	}
	//now the row
	for(uint8_t index=0; (index/3)<(col/3); index++)
	{
		if(puzzle->solutionPuzzle[row][index]==value) return false;
	}
	//skip the 3X3 box, already filled in
	for(uint8_t index=3*((col/3)+1); index<=8; index++)
	{
		if(puzzle->solutionPuzzle[row][index]==value) return false;
	}
	return true;
}


bool areRelatedCells(uint8_t index0, uint8_t index1)
{
	assert(index0<81);
	assert(index1<81);
	if(index0==index1) return false;
	if(index0/9 == index1/9) return true;
	if(index0%9 == index1%9) return true;
	if(index0/27 == index1/27 && ((index0%9)/3) == ((index1%9)/3)) return true;
	return false;
}

uint8_t connectionCount(uint8_t index0, uint8_t index1, struct sudoku * puzzle)
{
	if(!areRelatedCells(index0,index1)) return 0;
	uint8_t count=0;
	for(uint8_t index=0; index<=8; index++)
	{
		if(puzzle->possibilitiesMatrix[index0/9][index0%9][index] && puzzle->possibilitiesMatrix[index1/9][index1%9][index]) count++;
	}
	return count;
}

uint8_t connectionCountWithSetOfIndexes(uint8_t emptyCellsArrayIndex, uint8_t startIndex, uint8_t endIndex, struct sudoku * puzzle)
{
	//assert(endIndex<emptyCellsArrayIndex);
	uint8_t count=0;
	for(uint8_t index=startIndex; index<=endIndex; index++)
	{
		count+=connectionCount(index,emptyCellsArrayIndex,puzzle);
	}
	return count;
}

uint8_t cellPossibleValuesCount(uint8_t emptyCellsArrayIndex, struct sudoku * puzzle)
{
	uint8_t count=0;
	for(uint8_t value=0; value<=8; value++)
	{
		if(puzzle->possibilitiesMatrix[emptyCellsArrayIndex/9][emptyCellsArrayIndex%9][value])
		{
			count++;
		}
	}
	return count;
}

void sortEmptyCellsArray(int8_t startIndex, int8_t endIndex, struct sudoku * puzzle)
{
	if(endIndex > puzzle->unsolvedCellsIndex)
	{
		printf("endIndex: %i    unsolvedCellsIndex: %u\n", endIndex, puzzle->unsolvedCellsIndex);
	}
	assert(endIndex<=puzzle->unsolvedCellsIndex);
	
	if(startIndex > endIndex)
	{
		printf("startIndex: %i   endIndex: %i\n", startIndex, endIndex);
	}
	//assert(startIndex<=endIndex);

	//assert(startIndex>=0);
	//uint8_t index= startIndex+1;
	uint8_t max=0;
	uint8_t maxIndex=0;
	uint8_t count=0;
	uint8_t temp=0;
	//for(int8_t index=startIndex+1; index<=endIndex; index++)
	//{
	//	if(puzzle->emptyCells[index]<puzzle->emptyCells[index-1])
	//	{
	//		temp = puzzle->emptyCells[index];
	//		puzzle->emptyCells[index] = puzzle->emptyCells[index-1];
	//		puzzle->emptyCells[index-1] = temp;
//
//			index-=2;
//			if(index<startIndex) index=startIndex;
//		}
//	}
	
//	return;

	if(startIndex==0)
	{
		//The very first index should be the one within the range that 
		//has the fewest number of possible values.
		uint8_t min = 255;
		uint8_t minIndex =0;
		uint8_t valueCount=0;
		for(uint8_t index =0; index<=endIndex; index++)
		{
			valueCount = cellPossibleValuesCount(index,puzzle);
			if(valueCount<min)
			{
				min=valueCount;
				minIndex=index;
			}
		}
		temp=puzzle->emptyCells[minIndex];
		puzzle->emptyCells[minIndex]=puzzle->emptyCells[0];
		puzzle->emptyCells[0]=temp;

		startIndex=1;
	}
	
	
	for(uint8_t index=startIndex; index<=endIndex; index++)
	{
		//go through each index, determining which should be at (uint8_t)index
		max=0;
		//count=0;
		assert(index>0);
		maxIndex=index;
		for(uint8_t subIndex=index; subIndex<=endIndex; subIndex++)
		{
			//Get index of the cell that should be in first.
			//It is the one that has the most connections with all previous cells,
			//hence the 'max' variable.
			count = connectionCountWithSetOfIndexes(subIndex,0,index-1,puzzle);
			if(count>max)
			{
				maxIndex=subIndex;		
			}
		}
		//swap
		temp=puzzle->emptyCells[index];
		puzzle->emptyCells[index]=puzzle->emptyCells[maxIndex];
		puzzle->emptyCells[maxIndex]=temp;
	}
}

void createEmptyCellMatrix(struct sudoku * puzzle)
{
	puzzle->emptyCellsCount=0;
	for(uint8_t row=0; row<9; row++)
	{
		for(uint8_t col=0; col<9; col++)
		{
			if(puzzle->solutionPuzzle[row][col]==0)
			{
				puzzle->emptyCellsCount++;
			}
		}
	}
	puzzle->emptyCells = (uint8_t *) malloc(puzzle->emptyCellsCount*sizeof(uint8_t));
	
	puzzle->unsolvedCellsIndex=0;
	for(uint8_t row=0; row<9; row++)
	{
		for(uint8_t col=0; col<9; col++)
		{
			if(puzzle->solutionPuzzle[row][col]==0)
			{
				puzzle->emptyCells[puzzle->unsolvedCellsIndex]=(9*row)+col;
				puzzle->unsolvedCellsIndex++;
			}
		}
	}
	assert(puzzle->unsolvedCellsIndex==puzzle->emptyCellsCount);
	puzzle->unsolvedCellsIndex--;
}
void printPossibilitiesMatrix(struct sudoku * puzzle, uint8_t value)
{
	printf("\n[%d]\n",value);
	printf("_____________________\n");
	for(uint8_t row=0; row<9; row++)
	{
		for(uint8_t col=0; col<9; col++)
		{	
			if(col%3==0) printf("|");
			if(puzzle->possibilitiesMatrix[row][col][value-1])
			{
				printf("|%u",puzzle->possibilitiesMatrix[row][col][value-1]);
			}
			else 
			{
				printf("|%u",puzzle->possibilitiesMatrix[row][col][value-1]);
			}
		}
		printf("|\n");
		if(row>0 && row%3==2)
		{
			for(uint8_t i=0; i<21; i++)
			{
				printf("%c",0x35e);
			}
			printf("\n");

		}
	}

}

void printMatrix(uint8_t **matrix)
{
	printf("_____________________\n");
	for(uint8_t row=0; row<9; row++)
	{
		for(uint8_t col=0; col<9; col++)
		{	
			if(col>0 && col%3==0) printf("|");
			if(matrix[row][col]>0)
			{
				printf("|%u",matrix[row][col]);
			}
			else 
			{
				printf("| ");
			}
		}
		printf("|\n");
		if(row>0 && row%3==2)
		{
			for(uint8_t i=0; i<21; i++)
			{
				printf("%c",0x35e);
			}
			printf("\n");

		}
	}
	//printf("\n");
}

void printPuzzle(struct sudoku *puzzle)
{
	printf("Initial Puzzle\n");
	printMatrix(puzzle->initialPuzzle);
	printf("Working Solution\n");
	printMatrix(puzzle->solutionPuzzle);
}

void insertValueAndSetPossibilities(uint8_t value, uint8_t row, uint8_t col, struct sudoku *puzzle)
{
	value = value%10;
	if(value==0) return;
	row=row%9;
	col=col%9;
	assert(puzzle->possibilitiesMatrix[row][col][value-1]);
	puzzle->solutionPuzzle[row][col]=value;
	//these statements handle the matrix at the point of insertion
	for(uint8_t index=0; index<(value-1); index++)
	{
		puzzle->possibilitiesMatrix[row][col][index]=false;
	}
	puzzle->possibilitiesMatrix[row][col][value-1]=true;
	for(uint8_t index=value; index<=8; index++)
	{
		puzzle->possibilitiesMatrix[row][col][index]=false;
	}

	//these lines handle the 3X3 box this the point is in
	uint8_t matrixIndex = 0;
	uint8_t rowt = 0;// = (3*(matrixIndex/3)) + (row/3);
	uint8_t colt = 0;// (matrixIndex%3) + (col/3);
	uint8_t boxCounter = 0;
	rowt =  3*(row/3);			
	colt =  3*(col/3);
	while (  matrixIndex< ((3*(row%3)) + (col%3))     )
	{
		puzzle->possibilitiesMatrix[rowt][colt][value-1]=false;
		matrixIndex++;
		rowt =  (3*(row/3))+(matrixIndex/3);
       		colt =  (3*(col/3))+(matrixIndex%3);
		boxCounter++;
		//printf("[%d][%d] : [%d][%d]\n", row, col, rowt, colt);
	}
	matrixIndex++;
	while (  matrixIndex<=8 )
	{
		rowt =  (3*(row/3))+(matrixIndex/3);
       		colt =  (3*(col/3))+(matrixIndex%3);
		puzzle->possibilitiesMatrix[rowt][colt][value-1]=false;
		matrixIndex++;
		boxCounter++;
		//printf("[%d][%d] : [%d][%d]\n", row, col, rowt, colt);

	}
	assert(boxCounter==8);
	//these lines fill in the row and column outside of the box that the cell is in
	//column first
	uint8_t colCounter=0;
	for(uint8_t index=0; (index/3)<(row/3); index++)
	{
		puzzle->possibilitiesMatrix[index][col][value-1]=false;
		colCounter++;
	}
	//skip the 3X3 box, it is already filled in
	for(uint8_t index=3*((row/3)+1); index<=8; index++)
	{
		puzzle->possibilitiesMatrix[index][col][value-1]=false;
		colCounter++;
	}
	assert(colCounter==6);
	//now the row
	uint8_t rowCounter = 0;
	for(uint8_t index=0; (index/3)<(col/3); index++)
	{
		puzzle->possibilitiesMatrix[row][index][value-1]=false;
		assert(puzzle->possibilitiesMatrix[row][index][value-1]==false);
		rowCounter++;
	}
	//skip the 3X3 box, already filled in
	for(uint8_t index=3*((col/3)+1); index<=8; index++)
	{
		puzzle->possibilitiesMatrix[row][index][value-1]=false;
		assert(puzzle->possibilitiesMatrix[row][index][value-1]==false);
		rowCounter++;
	}
	assert(rowCounter==6);

	//printPossibilitiesMatrix(puzzle,value);

}

void copyInitialPuzzleToSolutionMatrix(struct sudoku * puzzle)
{
	for(uint8_t row=0; row<9; row++)
	{
		for(uint8_t col=0; col<9; col++)
		{
			insertValueAndSetPossibilities(puzzle->initialPuzzle[row][col],row,col,puzzle);
		}
	}
}

struct sudoku* newPuzzle()
{
	struct sudoku * puzzle = (struct sudoku *) malloc(sizeof(struct sudoku));
	assert(puzzle);
	//assert(puzzle->initialPuzzle);
	puzzle->initialPuzzle = (uint8_t **) malloc(81*sizeof(uint8_t *));
	assert(puzzle->initialPuzzle);
	puzzle->solutionPuzzle = (uint8_t **) malloc(81*sizeof(uint8_t *));
	puzzle->possibilitiesMatrix = (bool ***) malloc(729*sizeof(bool **));
	for(uint8_t row=0; row<9; row++)
	{
		puzzle->initialPuzzle[row] = (uint8_t *) malloc(9*sizeof(uint8_t));
		puzzle->solutionPuzzle[row] = (uint8_t *) malloc(9*sizeof(uint8_t));
		puzzle->possibilitiesMatrix[row] = (bool **) malloc(81*sizeof(bool *));
		for(uint8_t col=0; col<9; col++)
		{
			puzzle->initialPuzzle[row][col]=0;
			puzzle->solutionPuzzle[row][col]=0;
			puzzle->possibilitiesMatrix[row][col] = (bool *) malloc(9*sizeof(bool));
			for(uint8_t k=0; k<9; k++)
			{
				puzzle->possibilitiesMatrix[row][col][k]=true;
			}
		}
	}
	puzzle->emptyCellsCount=0;//(int)malloc(sizeof(int));
	puzzle->unsolvedCellsIndex=0;//(int)malloc(sizeof(int));
	puzzle->emptyCells=NULL;
	return puzzle;
}

void zeroPuzzle(struct sudoku * puzzle)
{
	//printf("about to zero all memory\n");
	for(uint8_t row=0; row<9; row++)
	{
		//printf("flag2\n");
		//printf("col: %d ",i);
		for(uint8_t col=0; col<9; col++)
		{
			//printf("row: %d ", j);
			puzzle->initialPuzzle[row][col]=0;
			puzzle->solutionPuzzle[row][col]=0;
			for(uint8_t k=0; k<9; k++)
			{
				puzzle->possibilitiesMatrix[row][col][k]=false;
			}
			//printf("\n");
		}
	}
	if(puzzle->emptyCells)
	{
		for(uint8_t index=0; index<puzzle->emptyCellsCount; index++)
		{
			puzzle->emptyCells[index]=0;
		}
	}
	puzzle->emptyCellsCount=0;
	puzzle->unsolvedCellsIndex=0;

}

void freePuzzle(struct sudoku * puzzle)
{
	//zero all memory first
	zeroPuzzle(puzzle);
	//printf("about to free memory\n");
	//free all memory
	
	//printf("flag12\n");
	if(puzzle->emptyCells!=NULL) 
	{
		printf("flag13\n");
		assert(puzzle->emptyCells);
		free(puzzle->emptyCells);
		printf("flag14\n");
	}


	assert(puzzle->initialPuzzle[0]);
	for(uint8_t row=0; row<9; row++)
	{
		assert((*puzzle).initialPuzzle[row]!=NULL);
		//assert(isOnHeap(puzzle->initialPuzzle[row]));
		//printf("flag5\n %lli \n %lli \n", puzzle->initialPuzzle[row], &row);
		//printf("row: %i\n", row);	
		free((puzzle->initialPuzzle[row]));
		//printf("flag6\n");
		free((puzzle->solutionPuzzle[row]));
		//printf("flag8\n");
		for(uint8_t col =0; col<9; col++)
		{
			//printf("flag10\n");
			free((puzzle->possibilitiesMatrix[row][col]));
		}
		//printf("flag9\n");
		//free((puzzle->possibilitiesMatrix[row][0]));
		//printf("flag11\n");
		free((puzzle->possibilitiesMatrix[row]));
		//printf("flag12\n");
	}
	//printf("flag4\n");
	free((puzzle->initialPuzzle));
	free((puzzle->solutionPuzzle));
		//free(&(puzzle->emptyCellsCount));
	//free(&(puzzle->unsolvedCellsIndex));
	//printf("flag15\n");
	free(puzzle);
	//printf("flag16\n");
}

uint8_t onlyPossibility(uint8_t row, uint8_t col, struct sudoku * puzzle)
{
	uint8_t count = 0;
	uint8_t value = 1;
	uint8_t answer = 0;
	row = row%9;
	col = col%9;
	while(value<=9)
	{
		if( (puzzle->possibilitiesMatrix[row][col][value-1]) )
		{
			answer=value;
			count++;
		}
		value++;
	}
	if(count==1) 
	{
		printf("Didn't need to check other cells. [%d][%d]=%d\n", row, col, answer);
		printPossibilitiesMatrix(puzzle,answer);
		return answer;
	}
	
	//these lines handle the 3X3 box this the point is in
	uint8_t matrixIndex = 0;
	uint8_t rowt = 0;// = (3*(matrixIndex/3)) + (row/3);
	uint8_t colt = 0;// (matrixIndex%3) + (col/3);
	bool continueChecking = false;
	value=1; //recycle variable
	while(puzzle->possibilitiesMatrix[row][col][value-1]==false) value++;
	assert(count>0);
	while(count>0)
	{
		assert(value<=9);
		if(puzzle->possibilitiesMatrix[row][col][value-1])
		{
			
			matrixIndex=0;
			continueChecking = true;
			rowt =  3*(row/3);
       			colt =  3*(col/3);
			while (  matrixIndex< ((3*(row%3)) + (col%3))  && continueChecking )
			{
				if(puzzle->possibilitiesMatrix[rowt][colt][value-1]) continueChecking=false;	
				matrixIndex++;
				rowt =  (3*(row/3))+(matrixIndex/3);
       				colt =  (3*(col/3))+(matrixIndex%3);
			}
			matrixIndex++;
			while (  matrixIndex<=8 && continueChecking)
			{
				rowt =  (3*(row/3))+(matrixIndex/3);
       				colt =  (3*(col/3))+(matrixIndex%3);
				if(puzzle->possibilitiesMatrix[rowt][colt][value-1]) continueChecking=false;
				
				matrixIndex++;
			}
			
			if(continueChecking)
			{
				printf("Only %d possible in box. [%d][%d]\n", value, row,col);
				printPossibilitiesMatrix(puzzle,value);
				
				return value;
			}
			continueChecking = true;
			//these lines fill in the row and column outside of the box that the cell is in
			//column first
			for(uint8_t index=0; index<row && continueChecking; index++)
			{
				if(puzzle->possibilitiesMatrix[index][col][value-1]) continueChecking=false;
			}
			//skip the cell
			for(uint8_t index=row+1; index<=8 && continueChecking; index++)
			{
				if(puzzle->possibilitiesMatrix[index][col][value-1]) continueChecking=false;
			}
			if(continueChecking)
			{
				printf("Only %d possible in the column. [%d][%d]\n",value, row,col);
				printPossibilitiesMatrix(puzzle,value);

				return value;
			}
			continueChecking = true;
			//now the row
			for(uint8_t index=0; index<col && continueChecking; index++)
			{
				if(puzzle->possibilitiesMatrix[row][index][value-1]) continueChecking=false;
			}
			//skip the cell
			for(uint8_t index=col+1; index<=8 && continueChecking; index++)
			{
				if(puzzle->possibilitiesMatrix[row][index][value-1]) continueChecking=false;
			}
			if(continueChecking)
			{
				printf("Only %d possible in the row.\n", value);
				return value;
			}
			count--;
		}
		value++;
	}
	return 0;
}

void singlePossibleSolutions(struct sudoku * puzzle)
{
	uint8_t count =0;
	uint8_t workingIndex=0;
	uint8_t insertValue=0;
	uint8_t row=0;
	uint8_t col=0;
	while (count < puzzle->unsolvedCellsIndex + 1)
	{
		row = puzzle->emptyCells[workingIndex]/9;
		col = puzzle->emptyCells[workingIndex]%9;
		insertValue = onlyPossibility(row,col,puzzle);
		printf("Solution: [%d][%d] : %d\n",row,col,insertValue);
		if(insertValue!=0){assert(puzzle->possibilitiesMatrix[row][col][insertValue-1]);}
		insertValueAndSetPossibilities(insertValue, row, col, puzzle);
		if(insertValue!=0)
		{
			puzzle->emptyCells[workingIndex]=puzzle->emptyCells[puzzle->unsolvedCellsIndex];
			puzzle->unsolvedCellsIndex--;
			count=0;
			if (workingIndex<puzzle->unsolvedCellsIndex)
			{
				workingIndex = (workingIndex+1)%(puzzle->unsolvedCellsIndex+1);
			}
			else
			{
				workingIndex = puzzle->unsolvedCellsIndex;	
			}

		}
		else
	       	{
			count++;
			workingIndex = (workingIndex+1)%(puzzle->unsolvedCellsIndex+1);
		}
	}
}

void upSolve(struct sudoku * puzzle, uint8_t * upSolveArray)
{
	uint8_t index=0;
	uint8_t value=1;
	//bool printCount=0;
	//assert(false);
	uint64_t counter =0;
	uint64_t counter0=0;
	while(index<=puzzle->unsolvedCellsIndex)
	{
		counter++;
		if(counter==0) counter0++;
		//if(printCount==0)
		//{
			//printf("Index: %d  Value: %d  [%d][%d]\r", index,value,puzzle->emptyCells[index]/9,puzzle->emptyCells[index]%9);
		//}
		//else
		//{
			//printf("Index: %d  Value: %d  [%d][%d]\r", index,value,puzzle->emptyCells[index]/9,puzzle->emptyCells[index]%9);

		//}
		assert(puzzle->unsolvedCellsIndex!=-1);
		assert(index<=puzzle->unsolvedCellsIndex);
		assert(puzzle->unsolvedCellsIndex<puzzle->emptyCellsCount);
		//printf("loopCheck: index=%d  puzzle->unsolvedCellsIndex=%d\n", index, puzzle->unsolvedCellsIndex);
		if(isInsertable(value,puzzle->emptyCells[index]/9,puzzle->emptyCells[index]%9,puzzle))
		{
			assert(index<=puzzle->unsolvedCellsIndex);
			assert(puzzle->emptyCells[index]<=80);
			assert(puzzle->possibilitiesMatrix[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9][value-1]);
			puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]=value;
			upSolveArray[index]=value;
			index++;
			value=1;
			//printf("value %d inserted at [%d][%d]\n",value, puzzle->emptyCells[index]/9, puzzle->emptyCells[index]%9);
		}
		else
		{
			//printf("value %d not inserted at [%d][%d], incrementing value\n", value, puzzle->emptyCells[index]/9, puzzle->emptyCells[index]%9);
		value++;
			while(value>=10 )
			{
				//printf("end of values reached, decrementing index\n");
				assert(puzzle->unsolvedCellsIndex>=0);
				if(index<=puzzle->unsolvedCellsIndex)
				{
					assert(puzzle->emptyCells[index]<=80);

					puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]=0;
					upSolveArray[index]=0;
				}
				index--;
				assert(index<=puzzle->unsolvedCellsIndex);
				if(index<=puzzle->unsolvedCellsIndex)
				{
					assert(puzzle->emptyCells[index]<=80);
					value=puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]+1;
				}
			}
		}
	}
	printf("\nIndex: %d  unsolvedCellsIndex: %d  (upSolve) counter: %lu counter0: %lu\n", index, puzzle->unsolvedCellsIndex, counter, counter0);

}

void downSolve(struct sudoku * puzzle, uint8_t * downSolveArray)
{
	uint8_t index=0;
	uint8_t value=9;
	uint64_t counter=0;
	uint64_t counter0=0;
	while(index<=puzzle->unsolvedCellsIndex)
	{
	//	printf("Index: %d Value: %d  [%d][%d]\n", index,value,puzzle->emptyCells[index]/9,puzzle->emptyCells[index]%9);
		counter++;
		if(counter==0) counter0++;
		if(isInsertable(value,puzzle->emptyCells[index]/9,puzzle->emptyCells[index]%9,puzzle))
		{
			puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]=value;
			downSolveArray[index]=value;
			index++;
			value=9;
			//printf("value inserted\n");
		}
		else
		{
			//printf("value not inserted, decrementing value\n");
		value--;
	
			while(value>=10 || value==0)
			{
				//printf("end of values reached, decrementing index\n");
				if(index<=puzzle->unsolvedCellsIndex)
				{
					puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]=0;
					downSolveArray[index]=0;
				}
				index--;
				if(index<=puzzle->unsolvedCellsIndex)
				{
					value=puzzle->solutionPuzzle[puzzle->emptyCells[index]/9][puzzle->emptyCells[index]%9]-1;
				}
			}
		}
	}
	printf("Index: %d  unsolvedCellsIndex: %d  (downSolve) counter: %lu counter0: %lu\n", index, puzzle->unsolvedCellsIndex, counter, counter0);

}


void fullSolve(struct sudoku * puzzle)
{
	assert(puzzle->unsolvedCellsIndex>=0);
	printf("full solve, unsolved cells:  %d\n",puzzle->unsolvedCellsIndex+1); 
	int8_t lastSimilarityIndex=puzzle->unsolvedCellsIndex; //This variable only counts the similarities after the first set of
	//contiguous similarities (matching pairs).
	uint8_t differenceCount =0;
	int8_t similarityCount =0;
	uint8_t *upSolveArray = (uint8_t *)malloc((puzzle->unsolvedCellsIndex+1)*sizeof(uint8_t));
	uint8_t *downSolveArray = (uint8_t *)malloc((puzzle->unsolvedCellsIndex+1)*sizeof(uint8_t));
	uint8_t arrayLength = puzzle->unsolvedCellsIndex+1;//this winds up being useful for zeroing the arrays
	//at the end, since puzzle->unsolvedCellsIndex usually changes a lot.
	uint8_t *firstSolution = (uint8_t *)malloc(81*sizeof(uint8_t));
	bool firstPass = true;
	//zero initialize arrays
	for(uint8_t index=0; index<=80; index++)
	{
		firstSolution[index]=0;
	}
	for(uint8_t index=0; index < arrayLength; index++)
	{
		upSolveArray[index]=0;
		downSolveArray[index]=0;
	}
	
	while(lastSimilarityIndex>=0)
	{
		printf("last similarity index: %i\n", lastSimilarityIndex);
		for(uint8_t index=0; index < arrayLength; index++)
		{
			upSolveArray[index]=0;
			downSolveArray[index]=0;
		}
		//assert(false);
		
		//upsolve
		printf("Up-solving\n");
		upSolve(puzzle,upSolveArray);
		//assert(false);
		printMatrix(puzzle->solutionPuzzle);
		if(firstPass)
		{
			for(uint8_t index=0; index<=80; index++)
			{
				firstSolution[index] = puzzle->solutionPuzzle[index/9][index%9];
			}
			firstPass=false;
		}
		clearUnsolvedCells(puzzle);
		
		//downsolve
		printf("Down-solving\n");
		downSolve(puzzle,downSolveArray);
		printMatrix(puzzle->solutionPuzzle);
		clearUnsolvedCells(puzzle);
		int8_t indexPointer=0; //must have potential to be -1
		//assert(false);
		
		//temporary: print solve arrays
		printf("\n Before rearrengement\n up-solve array\n");
		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", upSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) printf("|");
		}
		printf("\n down-solve array\n");
		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", downSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) printf("|");

		}
		printf("\n");
		assert(puzzle->unsolvedCellsIndex>=0);
		if(upSolveArray[indexPointer]!=downSolveArray[indexPointer])
		{
			assert(indexPointer<=puzzle->unsolvedCellsIndex);
		}
		
		while(upSolveArray[indexPointer]==downSolveArray[indexPointer] && indexPointer<=lastSimilarityIndex)
		{
			//assert(false);
			//since any contiguous set of matching pairs must be accurate, add them to the final matrix
			printf("[%d][%d]=%d\n",puzzle->emptyCells[indexPointer]/9,puzzle->emptyCells[indexPointer]%9,upSolveArray[indexPointer]);

			insertValueAndSetPossibilities(upSolveArray[indexPointer], puzzle->emptyCells[indexPointer]/9, puzzle->emptyCells[indexPointer]%9,puzzle);
			
			indexPointer++;
		}
		uint8_t temp =0;
		indexPointer--;//indexPointer now points to the index of the last element of contiguous maching pairs
		//in the arrays
			
		similarityCount=0;
		for(uint8_t index = indexPointer+1; index<=lastSimilarityIndex; index++)
		{
			//count the similarities to see if the algorithm has done all it can.
			//This is needed since there may not be a solution
			if(upSolveArray[index]==downSolveArray[index])
			{
			       	similarityCount++;
			}
		}
				assert(puzzle->unsolvedCellsIndex>=0);
		//uint8_t flipCounter=0;
		for(uint8_t index=0; index<=indexPointer; index++)
		{
			//Now flip around the values in the puzzle empty cells array
			//so that these newly filled in cells are no longer being looked at.
			//Move them all to the working index on the empty cells arrays and the solve arrays
			//and then decrement the index
			
			//assert(false);	
			assert(puzzle->unsolvedCellsIndex<puzzle->emptyCellsCount);
			assert(puzzle->unsolvedCellsIndex>=0);
			assert(indexPointer!=0); //this shouldn't happen due to earlier logic
			
			temp = puzzle->emptyCells[index];
			puzzle->emptyCells[index] = puzzle->emptyCells[puzzle->unsolvedCellsIndex];
			puzzle->emptyCells[puzzle->unsolvedCellsIndex] = temp;
			
			temp = upSolveArray[index];
			upSolveArray[index] = upSolveArray[puzzle->unsolvedCellsIndex];
			upSolveArray[puzzle->unsolvedCellsIndex] = temp;
			
			temp = downSolveArray[index];
			downSolveArray[index] = downSolveArray[puzzle->unsolvedCellsIndex];
			downSolveArray[puzzle->unsolvedCellsIndex] = temp;
			
			puzzle->unsolvedCellsIndex--;
			//flipCounter++;
			//if(puzzle->unsolvedCellsIndex<0) puzzle->unsolvedCellsIndex=0;
		}
		//puzzle->unsolvedCellsIndex = flipCounter-1;//this is wrong

		printf("\n After rearrengement\n up-solve array\n");
		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", upSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) printf("|");

		}
		printf("\n down-solve array\n");
		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", downSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) printf("|");

		}
		printf("\n");


				
		for(uint8_t index = 1; index<=lastSimilarityIndex; index++)
		{
			//Since any points on the array that aren't matching are obviously wrong,
			//move them to the back of the array. Matching ones are shuffled to the front. Only
			//the first set of contiuous matching pairs are known to be correct between the
			//upsolved and downsolved solutions
			assert(puzzle->unsolvedCellsIndex<puzzle->emptyCellsCount);

			if(upSolveArray[index-1]!=downSolveArray[index-1] && upSolveArray[index]==downSolveArray[index])
			{
				//assert(false);
				temp = puzzle->emptyCells[index];
				puzzle->emptyCells[index] = puzzle->emptyCells[index-1];
				puzzle->emptyCells[index-1] = temp;
				
				temp = upSolveArray[index];
				upSolveArray[index] = upSolveArray[index-1];
				upSolveArray[index-1] = temp;
				
				temp = downSolveArray[index];
				downSolveArray[index] = downSolveArray[index-1];
				downSolveArray[index-1] = temp;
				
				index-=2;
				if(index<0) index = 0;
			}

		}
		
		for(uint8_t index=0; index<similarityCount; index+=0)
		{
			if(firstSolution[puzzle->emptyCells[index]]!=upSolveArray[index] && upSolveArray[index]==downSolveArray[index])
			{
				temp = puzzle->emptyCells[index];
				puzzle->emptyCells[index] = puzzle->emptyCells[similarityCount-1];
				puzzle->emptyCells[similarityCount-1] = temp;
				
				temp = upSolveArray[index];
				upSolveArray[index] = upSolveArray[similarityCount-1];
				upSolveArray[similarityCount-1] = temp;
				
				temp = downSolveArray[index];
				downSolveArray[index] = downSolveArray[similarityCount-1];
				downSolveArray[similarityCount-1] = temp;
	
				similarityCount--;
			}
			else
			{
				index++;
			}
		}

		lastSimilarityIndex=similarityCount-1;
		//we now have an array divider for the sorting functions
		

		printf("\n After sorting\n up-solve array\n");

		if(-1==puzzle->unsolvedCellsIndex) {printf("|");}
		if(-1==lastSimilarityIndex) {printf("|");}
		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", upSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) {printf("|");}
			if(index==lastSimilarityIndex) {printf("|");}

		}
		printf("\n down-solve array\n");
		
		if(-1==puzzle->unsolvedCellsIndex) {printf("|");}
		if(-1==lastSimilarityIndex) {printf("|");}

		for(uint8_t index = 0; index<arrayLength; index++)
		{
			printf("[%d]", downSolveArray[index]);
			if(index==puzzle->unsolvedCellsIndex) {printf("|");}
			if(index==lastSimilarityIndex) {printf("|");}
		}
		printf("\n");
		
		//now sort the cells so that they are easier to upsolve
		//and downsolve next time
		sortEmptyCellsArray(0,lastSimilarityIndex,puzzle);
		sortEmptyCellsArray(lastSimilarityIndex+1, puzzle->unsolvedCellsIndex,puzzle);
		
		temp=0;
		printf("Similarity count: %d   unsolved cells: %d\n", similarityCount, puzzle->unsolvedCellsIndex+1);
	}
	printf("print current solution anyways:\n");
	printMatrix(puzzle->solutionPuzzle);
	//zero and free memory
	for(uint8_t index=0; index < arrayLength; index++)
	{
		upSolveArray[index]=0;
		downSolveArray[index]=0;
	}
	for(uint8_t index=0; index<=80; index++)
	{
		firstSolution[index]=0;
	}
	free(firstSolution);
	printf("About to free solvearrays\n");

	free(upSolveArray);
	free(downSolveArray);
	printf("Freed solvearrays\n");
}

void solve(int ** matrix)
{
	if(!matrix) return;
	for(uint8_t i =0; i<9; i++)
	{
		if(!matrix[i]) return;
	}
	struct sudoku *puzzle = newPuzzle();
	for(uint8_t i=0; i<81; i++)
	{
		puzzle->initialPuzzle[i/9][i%9]=(uint8_t)(matrix[i/9][i%9]%9);
	}
	copyInitialPuzzleToSolutionMatrix(puzzle);
	//check for invalid input
	for(uint8_t i=0; i<81; i++)
	{
		if(puzzle->initialPuzzle[i/9][i%9]!=puzzle->solutionpuzzle[i/9][i%9])
		{
			freePuzzle(puzzle);
			return;		
		}
	}
	createEmptyCellMatrix(puzzle);
	singlePossibleSolutions(puzzle);
	fullSolve(puzzle);
	for(uint8_t i=0; i<81; i++)
	{
		if(puzzle->initialPuzzle[i/9][i%9]!=0)
		matrix[i/9][i%9] = (int)(puzzle->solutionPuzzle[i/9][i%9]%9);
	}
	freePuzzle(puzzle);
}


int main (void)
{
	struct sudoku *p = newPuzzle();
	assert(p);
	assert(p->initialPuzzle);
	p->initialPuzzle[0][0]=8;
	p->initialPuzzle[0][1]=4;
	p->initialPuzzle[0][3]=3;
	p->initialPuzzle[1][1]=6;
	p->initialPuzzle[1][3]=7;
	p->initialPuzzle[1][6]=9;
	p->initialPuzzle[2][4]=5;
	//s
	//p->initialPuzzle[3][0]=1;
	//p->initialPuzzle[3][1]=8;
	//p->initialPuzzle[3][5]=7;
	//p->initialPuzzle[4][0]=7;
	//p->initialPuzzle[4][4]=2;
	//e
	p->initialPuzzle[4][8]=9;
	//s
	//p->initialPuzzle[5][3]=1;
	//p->initialPuzzle[5][7]=8;
	//p->initialPuzzle[5][8]=5;
	//e
	p->initialPuzzle[6][4]=1;
	p->initialPuzzle[7][2]=1;
	p->initialPuzzle[7][5]=2;
	p->initialPuzzle[7][7]=4;
	p->initialPuzzle[8][5]=6;
	p->initialPuzzle[8][7]=1;
	p->initialPuzzle[8][8]=8;

	//printMatrix(p->initialPuzzle);	
	copyInitialPuzzleToSolutionMatrix(p);
	createEmptyCellMatrix(p);
	//printMatrix(p->solutionPuzzle);
	//printf("Partial Solve\n");
	singlePossibleSolutions(p);
	//printf("flag0\n");
	//printMatrix(p->solutionPuzzle);
	printf("Full Solve\n");
	fullSolve(p);
	printPuzzle(p);
	//printMatrix(p->solutionPuzzle);
	//printf("flag1\n");
	//printPossibilitiesMatrix(p,9);

	freePuzzle(p);
	//free(p);
	//printf("end\n");
}
