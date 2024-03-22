#pragma once

#include <iostream>
#include "mpi.h"
#include <string>   
#include <stdlib.h>   
#include <time.h>
#include <Windows.h>
#include <vector>
#include <signal.h>
#include <stdbool.h>
#include <cstdlib>
#include <cmath>

#include "GraphicComponent.h"

#define MAX_STEPS 500

#define v(r, c) (((r) * (COLS_With_Ghost)) + (c))

int choose_neighborhood(int& rank, int& nProc);



void find_neighborhood_VON_NEUMANN(int& rank, int& nProc, int& COLS_P, int& rightRank, int& leftRank, int& upperRank, int& lowerRank);

void find_neighborhood_MOORE(int& rank, int& nProc, int COLS_P, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRigh);



void init(int* readM, int grid[][COLS], int& COLS_With_Ghost, int& ROWS_With_Ghost, int& rank, GraphicComponent* gc);



int find_relative_COLS(int& nProc, int& COLS_Grid, int& ROWS_Grid, int& COLS_P);

std::vector<std::vector<int> > subsets(std::vector<int>& A);

void calcSubset(std::vector<int>& A, std::vector<std::vector<int> >& res, std::vector<int>& subset, int index);



void exchange_VON_NEUMANN(int* curr_grid, int& rank, int& rightRank, int& leftRank, int& upperRank, int& lowerRank, int& COLS_With_Ghost, int& ROWS_With_GhosT);

void exchange_MOORE(int* curr_grid, int& rank, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRight, int& COLS_With_Ghost, int& ROWS_With_Ghost);



void swap(int*& readM, int*& writeM);



void transFuncCell(int* readM, int* writeM, int& i, int& j, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& vicinatoScelto);

void transFunc(int& rank, int* readM, int* writeM, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& vicinatoScelto);



void Rebuild_grid(int grid[][COLS], int*& readM, int& rank, int& nProc, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& ROWS_Grid, int& COLS_Grid, int& COLS_P, GraphicComponent* gc);



void show_VON_NEUMANN_neighborhood(int& rank, int& rightRank, int& leftRank, int& upperRank, int& lowerRank);

void show_MOORE_neighborhood(int& rank, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRight);