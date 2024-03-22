// Il gioco della vita di Conway
// Autori: Santagada - Vacca

#include "GraphicComponent.h"
#include "mpi_utils.h"


int main(int argc, char* argv[])
{
	// Rank e numero di processi
	int rank, nProc;

	// Rank da utilizzare per VON_NEUMANN senza confini
	int rightRank = 0, leftRank = 0, upperRank = 0, lowerRank = 0;	// sono le variabili che contengono il  numero dei rank dei processi vicini

	// Rank da utilizzare per MOORE con confini
	int rightRank_b = 0, leftRank_b = 0, upperRank_b = 0, lowerRank_b = 0;	// sono le variabili che contengono il  numero dei rank dei processi vicini

	int upperDiagonalLeft = -1, upperDiagonalRight = -1, lowerDiagonLeft = -1, lowerDiagonalRight = -1;

	// Variabile che conterrà il tipo di vicinato scelto dall'utente (1 per MOORE e 2 per VON_NEUMANN)
	int vicinatoScelto = 0;

	// Variabili che rappresentano la dimensione delle sottomatrici per ogni processo, COLS_P è il numero di processi per riga 
	int ROWS_Grid = 1, COLS_Grid = 1, COLS_P = 1;

	// Variabili che rappresentano la dimensione delle sottomatrici con l'aggiunta delle celle fantasma inizialmente settate a 0
	int COLS_With_Ghost = 0, ROWS_With_Ghost = 0;

	// Componente grafica allegro per stampa 
	GraphicComponent* gc = NULL;

	// Puntatori per conservare i valori delle sottomatrici di ogni processo ad ogni step
	int* readM = NULL;
	int* writeM = NULL;

	// Griglia dove ogni processo invia il risultato dei propri calcoli, ricostruita e amministrata dal processo 0
	int grid[ROWS][COLS];

	MPI_Init(&argc, &argv); // Inizializzazione di MPI

	MPI_Comm_size(MPI_COMM_WORLD, &nProc); // Numero di processi
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Rank del processo

	// Funzione per scegliere il tipo di vicinato, la variabile vicinatoScelto conterrà il valore scelto (1 per MOORE e 2 per VON_NEUMANN)
	vicinatoScelto = choose_neighborhood(rank, nProc);

	// Costruzione della sotto_matrice di ogni processo 
	find_relative_COLS(nProc, COLS_Grid, ROWS_Grid, COLS_P);

	//DA TOGLIERE 
	if (rank == 0)
		std::cout << "Le sotto matrici sono di dimensioni: " << ROWS_Grid << "x" << COLS_Grid << std::endl;

	// Dai valori ottenuti con la funzione find_relative_COLS sommo sia per le colonne che le righe le celle fantasma
	COLS_With_Ghost = COLS_Grid + 2;
	ROWS_With_Ghost = ROWS_Grid + 2;

	// In base alla scelta eseguita dall'utente sul tipo di vicinato trovo i processi tra cui deve esserci lo scambio 
	if (vicinatoScelto == 1) 
	{
		find_neighborhood_MOORE(rank, nProc, COLS_P, rightRank_b, leftRank_b, upperRank_b, lowerRank_b, upperDiagonalLeft, upperDiagonalRight, lowerDiagonLeft, lowerDiagonalRight);
	}
	else
	{
		find_neighborhood_VON_NEUMANN(rank, nProc, COLS_P, rightRank, leftRank, upperRank, lowerRank);
	}


	// Creazione dell'array  che conterrà i valori delle sottomatrici per ogni processo

	readM = new int[ROWS_With_Ghost * COLS_With_Ghost];  // Matrice di lettura

	writeM = new int[ROWS_With_Ghost * COLS_With_Ghost]; // Matrice di scrittura


	srand(time(NULL));


	// Inizializzazione delle matrici per ogni processo
	init(readM, grid, COLS_With_Ghost, ROWS_With_Ghost, rank, gc);

	MPI_Barrier(MPI_COMM_WORLD);

	double starttime = MPI_Wtime();

	// Ciclo per il numero di iterazioni
	for (int i = 0; i < MAX_STEPS; i++)
	{

		if (vicinatoScelto == 1)
			exchange_MOORE(readM, rank, rightRank_b, leftRank_b, upperRank_b, lowerRank_b, upperDiagonalLeft, upperDiagonalRight, lowerDiagonLeft, lowerDiagonalRight, COLS_With_Ghost, ROWS_With_Ghost);

		else if (vicinatoScelto == 2)
			exchange_VON_NEUMANN(readM, rank, rightRank, leftRank, upperRank, lowerRank, COLS_With_Ghost, ROWS_With_Ghost);


		Rebuild_grid(grid, readM, rank, nProc, ROWS_With_Ghost, COLS_With_Ghost, ROWS_Grid, COLS_Grid, COLS_P, gc);
		transFunc(rank, readM, writeM, ROWS_With_Ghost, COLS_With_Ghost, vicinatoScelto);
		swap(readM, writeM);
	}


	double endtime = MPI_Wtime();

	if (rank == 0)
	{
		// stampa del tempo impiegato in secondi
		printf("Tempo impiegato: %f secondi.\n", endtime - starttime);
	}

	// Deallocazione della memoria
	delete[] readM; 
	delete[] writeM;

	MPI_Finalize();

	return 0;
}