#include "../GOL/mpi_utils.h"

// Funzione per scegliere il tipo di neighborhood
// La funzione oltre a restituire il tipo di vicinato scelto, invia a tutti i processi il tipo di vicinato scelto
// Questa è chiamata da tutti i processi, ma la richiesta della scelta del tipo di vicinato all'utente è gestita dal processo 0
// Tutti gli altri processi ricevono il tipo di vicinato scelto dal processo 0
int choose_neighborhood(int& rank, int& nProc) {    // passo rank e nProc per poter fare la send e la receive

	int vicinatoScelto = 0; // variabile che conterrà il tipo di vicinato scelto inizialmente settata a 0

	// la scelta del tipo di vicinato è gestita solo dal processo 0
	if (rank == 0)
	{
		std::string scelta;

		do
		{
			std::cout << "Scegliere tipo di vicinato: MOORE oppure VON.\n";
			std::cout << "Scelta:   ";
			std::cin >> scelta;

			if (scelta == "MOORE")
			{
				vicinatoScelto = 1;

				break;
			}

			else if (scelta == "VON")
			{
				vicinatoScelto = 2;

				break;
			}

		} while (true); // esege il ciclo finchè non viene inserito MOORE o VON

		// invio il tipo di vicinato scelto a tutti i processi, tranne che al processo 0 che già lo sa
		for (int i = 1; i < nProc; i++) // quindi parto da rank 1 fino ad nProc
		{
			MPI_Send(&vicinatoScelto, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			// la send prende come argomenti:
			// 1) l'indirizzo della variabile da inviare
			// 2) quanti elementi inviare
			// 3) il tipo di dato
			// 4) il rank del processo a cui inviare
			// 5) il tag
			// 6) il communicator
		}

		return vicinatoScelto;
	}

	else
	{
		// se non sono il rank 0 allora entro in questo blocco di codice e ricevo il tipo di vicinato scelto dal processo 0

		MPI_Recv(&vicinatoScelto, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// la receive prende come argomenti:
		// 1) l'indirizzo della variabile in cui salvare il valore ricevuto
		// 2) quanti elementi ricevere
		// 3) il tipo di dato
		// 4) il rank del processo da cui ricevere
		// 5) il tag
		// 6) il communicator

		return vicinatoScelto;
	}
}

// Funzione per trovare i vicini di un processo nel contesto del vicinato di Von Neumann
// ogni processo richiama la funzione find_neighborhood_VON_NEUMANN
// ogni processo ha 4 vicini: destra, sinistra, sopra e sotto
void find_neighborhood_VON_NEUMANN(int& rank, int& nProc, int& COLS_P, int& rightRank, int& leftRank, int& upperRank, int& lowerRank)
{
	upperRank = (rank - COLS_P < 0) ? nProc - COLS_P + rank : rank - COLS_P;

	lowerRank = (rank + COLS_P > nProc - 1) ? rank % COLS_P : rank + COLS_P;

	rightRank = (rank + 1) % COLS_P + (COLS_P * (rank / COLS_P));

	if (rank == 0)
		leftRank = COLS_P - 1;

	else
		leftRank = (rank - 1) % COLS_P + (COLS_P * (rank / COLS_P));
}

// Funzione per trovare i vicini di un processo nel contesto di un vicinato di Moore
// ogni processo richiama la funzione find_neighborhood_MOORE
// ogni processo ha 8 vicini: destra, sinistra, sopra, sotto, diagonale in alto a sinistra, diagonale in alto a destra, diagonale in basso a sinistra e diagonale in basso a destra
void find_neighborhood_MOORE(int& rank, int& nProc, int COLS_P, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRight) {


	// rank di destra, sinistra, sopra e sotto (NB. se il rank corrente si trova su un bordo allora alcuni vicini saranno impostati a -1)
	rightRank_b = (abs(((rank + 1) / COLS_P) - rank / COLS_P) == 0) ? rank + 1 : -1;

	leftRank_b = (abs(((rank - 1) / COLS_P) - rank / COLS_P) == 0) ? rank - 1 : -1;

	upperRank_b = (rank - COLS_P > -1) ? rank - COLS_P : -1;

	lowerRank_b = (rank + COLS_P < nProc) ? rank + COLS_P : -1;


	// rank delle diagonali (NB. se il ranl corrente si trova su un bordo allora alcuni vicini saranno impostati a -1)
	upperDiagonalLeft = (rank - COLS_P - 1 > -1) ? rank - COLS_P - 1 : -1;

	upperDiagonalRight = (rank - COLS_P + 1 > -1) ? rank - COLS_P + 1 : -1;

	lowerDiagonLeft = (rank + COLS_P - 1 < nProc) ? rank + COLS_P - 1 : -1;

	lowerDiagonalRight = (rank + COLS_P + 1 < nProc) ? rank + COLS_P + 1 : -1;

	// dobbiamo fare alcuni controlli altrimenti i vicini per i processi che si trovano sui bordi non saranno impostati correttamente
	// in particolare ora i vicini potrebbero essere comunicanti oltre i bordi della griglia

	if ((abs(upperDiagonalLeft / COLS_P - rank / COLS_P)) == 0 || (abs(upperDiagonalLeft / COLS_P - rank / COLS_P) > 1))
	{
		upperDiagonalLeft = -1;
	}

	if ((abs(upperDiagonalRight / COLS_P - rank / COLS_P)) == 0 || (abs(upperDiagonalRight / COLS_P - rank / COLS_P) > 1))
	{
		upperDiagonalRight = -1;
	}

	if ((abs(lowerDiagonLeft / COLS_P - rank / COLS_P)) == 0 || (abs(lowerDiagonLeft / COLS_P - rank / COLS_P) > 1))
	{
		lowerDiagonLeft = -1;
	}

	if ((abs(lowerDiagonalRight / COLS_P - rank / COLS_P)) == 0 || (abs(lowerDiagonalRight / COLS_P - rank / COLS_P) > 1))
	{
		lowerDiagonalRight = -1;
	}
}

// Funzione per inizializzare le matrici per ogni processo
void init(int* readM, int grid[][COLS], int& COLS_With_Ghost, int& ROWS_With_Ghost, int& rank, GraphicComponent* gc)
{

	// ogni processo setta la propria readM a 0
	for (int i = 0; i < ROWS_With_Ghost; i++)
	{
		for (int j = 0; j < COLS_With_Ghost; j++)
		{
			readM[v(i, j)] = 0;
		}
	}

	// 
	// commentare se non si vuole generare un ambiente popolato randomicamente
	for (int i = 1; i < ROWS_With_Ghost - 1; i++)
	{
		for (int j = 1; j < COLS_With_Ghost - 1; j++)
		{
			readM[v(i, j)] = rand() % 2; // genera un numero casuale tra 0 e 1 quindi la cella può essere morta o viva
		}
	}


	if (rank == 0)
	{
		// inizializza la matrice grid a 0. La matrice grid sarà la matrice TOTALE che rappresenta l'ambiente step by step
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				grid[i][j] = 0;
			}
		}

		gc = new GraphicComponent(); // inizializza il componente grafico


		/*

		// ----- GLIDER -----
		// Per visualizzare solo il glidere commentare  l'inizializzazione random di readM
		int ii = (ROWS_With_Ghost / 2) + 1;
		int jj = 2;

		readM[v(ii - 1, jj)] = 1;
		readM[v(ii, jj + 1)] = 1;
		readM[v(ii + 1, jj - 1)] = 1;
		readM[v(ii + 1, jj)] = 1;
		readM[v(ii + 1, jj + 1)] = 1;

		*/



		// ----- GLIDER GUN -----  con matrice 100x100 4 proessi 
		// Per visualizzare solo il glider gun commentare  l'inizializzazione random di readM
		int ii = (ROWS_With_Ghost / 2) + 1;
		int jj = 2;

		// Configurazione iniziale del cannone di aliante
		readM[v(ii + 1, jj + 25)] = 1;
		readM[v(ii + 2, jj + 23)] = 1;
		readM[v(ii + 2, jj + 25)] = 1;
		readM[v(ii + 3, jj + 13)] = 1;
		readM[v(ii + 3, jj + 14)] = 1;
		readM[v(ii + 3, jj + 21)] = 1;
		readM[v(ii + 3, jj + 22)] = 1;
		readM[v(ii + 3, jj + 35)] = 1;
		readM[v(ii + 3, jj + 36)] = 1;
		readM[v(ii + 4, jj + 12)] = 1;
		readM[v(ii + 4, jj + 16)] = 1;
		readM[v(ii + 4, jj + 21)] = 1;
		readM[v(ii + 4, jj + 22)] = 1;
		readM[v(ii + 4, jj + 35)] = 1;
		readM[v(ii + 4, jj + 36)] = 1;
		readM[v(ii + 5, jj + 1)] = 1;
		readM[v(ii + 5, jj + 2)] = 1;
		readM[v(ii + 5, jj + 11)] = 1;
		readM[v(ii + 5, jj + 17)] = 1;
		readM[v(ii + 5, jj + 21)] = 1;
		readM[v(ii + 5, jj + 22)] = 1;
		readM[v(ii + 6, jj + 1)] = 1;
		readM[v(ii + 6, jj + 2)] = 1;
		readM[v(ii + 6, jj + 11)] = 1;
		readM[v(ii + 6, jj + 15)] = 1;
		readM[v(ii + 6, jj + 17)] = 1;
		readM[v(ii + 6, jj + 18)] = 1;
		readM[v(ii + 6, jj + 23)] = 1;
		readM[v(ii + 6, jj + 25)] = 1;
		readM[v(ii + 7, jj + 11)] = 1;
		readM[v(ii + 7, jj + 17)] = 1;
		readM[v(ii + 7, jj + 25)] = 1;
		readM[v(ii + 8, jj + 12)] = 1;
		readM[v(ii + 8, jj + 16)] = 1;
		readM[v(ii + 9, jj + 13)] = 1;
		readM[v(ii + 9, jj + 14)] = 1;


	}
}

// Funzione per trovare le dimensioni delle sottogriglie
// riceve come argomenti il numero di processi, il numero di colonne e righe della griglia, e il numero di processi per riga
int find_relative_COLS(int& nProc, int& COLS_Grid, int& ROWS_Grid, int& COLS_P)
{

	int N_block_grid = COLS * ROWS; // numero di celle totali della matrice originale



	// ----------  PRIMO CONTROLLO  ----------
	// Se il numero di celle della matrice originale non è divisibile per il numero di processi -> INTERROMPI IL PROGRAMMA

	if (N_block_grid % nProc != 0)
	{
		std::cout << "Il numero di celle della matrice non è divisibile equamente per il numero di processi." << "\n";

		exit(1); // termina il programma
	}





	// ----------  SECONDO CONTROLLO  ----------
	// Se il numero di celle della matrice originale è uguale al numero di processi, allora le sottomatrici avranno dimensioni 1x1

	if (N_block_grid == nProc)
	{
		COLS_Grid = 1;
		ROWS_Grid = 1;

		COLS_P = COLS; // signica che ci saranno COLS processi per riga. 
		// Esempio una matrice 3 * 3 con 9 processi avrà 3 processi per riga e la dimensione delle sottomatrici sarà 1 * 1

		return 0; // termina la funzione find_relative_COLS
	}





	int n_Block_Sub_grid = N_block_grid / nProc; // numero di celle per ogni sottogriglia

	int value = sqrt(n_Block_Sub_grid); // calcola la radice quadrata di n_Block_Sub_grid e la salva in value (NB. è salvata solo la parte intera poichè value è di tipo int)


	// ----------  TERZO CONTROLLO  ----------
	// se il quadrato di value è uguale a n_Block_Sub_grid significa che n_Block_Sub_grid è un quadrato perfetto
	if (value * value == n_Block_Sub_grid)
	{
		ROWS_Grid = value;
		COLS_Grid = value;

		COLS_P = COLS / COLS_Grid;

		return 0; // termina la funzione find_relative_COLS
	}




	int totalMultiple = 1; // alla fine sarà uguale a n_Block_Sub_grid

	std::vector<int> sub_multiple; // vettore che conterrà i divisori di n_Block_Sub_grid



	// ----------  QUARTO CONTROLLO  ----------
	// se siamo arrivati qui significa che n_Block_Sub_grid non è un quadrato perfetto pertanto dobbiamo trovare i divisori di n_Block_Sub_grid
	// esegue la scomposizione in fattori primi di n_Block_Sub_grid
	for (int i = 2; i <= n_Block_Sub_grid; i++)
	{

		if (n_Block_Sub_grid % i == 0)
		{

			sub_multiple.push_back(i); // se i è un divisore di n_Block_Sub_grid lo salva nel vettore sub_multiple

			n_Block_Sub_grid /= i; // divide n_Block_Sub_grid per i

			// se siamo arrivati a 1 significa che abbiamo trovato tutti i divisori di n_Block_Sub_grid
			if (n_Block_Sub_grid == 1)
			{
				sub_multiple.push_back(1);

				break;
			}
			else
			{
				i = 1; // altrimenti azzera i e ricomincia il ciclo 
			}
		}
	}

	for (auto i : sub_multiple)
	{
		totalMultiple *= i; // corrisponde a n_Block_Sub_grid
	}

	// la funzione subsets restituisce tutti i sottoinsiemi di sub_multiple e li salva in res che è un vettore di vettori
	std::vector<std::vector<int> > res = subsets(sub_multiple);


	for (int i = 0; i < res.size(); i++)
	{
		ROWS_Grid = 1; // inizializza ROWS_Grid a 1, alla fine conterra il numero di righe della sottomatrice 

		for (int j = 0; j < res[i].size(); j++)
		{
			ROWS_Grid *= res[i][j]; // moltiplica ROWS_Grid per ogni elemento di res[i]
		}
		COLS_Grid = totalMultiple / ROWS_Grid;
		// COLS_Grid è uguale a n_Block_Sub_grid diviso ROWS_Grid (es. se n_Block_Sub_grid è 12 e ROWS_Grid è 3 allora COLS_Grid sarà 4)

		// facciamo un controllo e verifichiamo che:
		// - ROWS sia divisibile per ROWS_GRID e COLS sia divisibile per COLS_GRID
		// - ROWS sia diverso da ROWS_GRID e COLS sia diverso da COLS_GRID
		// - ROWS_GRID sia minore di ROWS e COLS_GRID sia minore di COLS
		if (ROWS % ROWS_Grid == 0 && COLS % COLS_Grid == 0 && ROWS != ROWS_Grid && COLS != COLS_Grid && ROWS_Grid < ROWS && COLS_Grid < COLS)
		{
			COLS_P = COLS / COLS_Grid;

			return 0;
		}
	}



	exit(1); // se non si verifica nessuna delle condizioni precedenti termina il programma
}

// Funzione per trovare tutti i sottoinsiemi di un insieme. Riceve come argomento un vettore di interi e restituisce un vettore di vettori
std::vector<std::vector<int> > subsets(std::vector<int>& A)
{

	std::vector<int> subset;
	std::vector<std::vector<int> > res;
	int index = 0;
	calcSubset(A, res, subset, index);
	return res;

}

// Funzione ricorsiva per trovare tutti i sottoinsiemi di un insieme
void calcSubset(std::vector<int>& A, std::vector<std::vector<int> >& res, std::vector<int>& subset, int index)
{
	// Aggiungi il sottoinsieme corrente a res
	res.push_back(subset);

	for (int i = index; i < A.size(); i++) {
		// Includi l'elemento corrente nel sottoinsieme
		subset.push_back(A[i]);

		// Ricorsione per trovare tutti i sottoinsiemi che includono l'elemento corrente
		calcSubset(A, res, subset, i + 1);

		// Escludi l'elemento corrente dal sottoinsieme e passa alla prossima iterazione
		subset.pop_back();
	}
}

void exchange_VON_NEUMANN(int* curr_grid, int& rank, int& rightRank, int& leftRank, int& upperRank, int& lowerRank, int& COLS_With_Ghost, int& ROWS_With_Ghost)
{
	MPI_Status status[4];
	MPI_Request send_req[4], recv_req[4];

	MPI_Datatype contiguous_row, col;

	// Creazione Type per le righe

	MPI_Type_contiguous(COLS_With_Ghost - 2, MPI_INT, &contiguous_row);
	MPI_Type_commit(&contiguous_row);

	// Creazione Type per le colonne

	MPI_Type_vector(ROWS_With_Ghost - 2, 1, COLS_With_Ghost, MPI_INT, &col);
	MPI_Type_commit(&col);


	// Receive Upper
	MPI_Irecv(&curr_grid[v(ROWS_With_Ghost - 1, 1)], 1, contiguous_row, upperRank, 0, MPI_COMM_WORLD, &recv_req[0]);

	// Send Upper
	MPI_Isend(&curr_grid[v(1, 1)], 1, contiguous_row, upperRank, 0, MPI_COMM_WORLD, &send_req[0]);

	// Receive Lower
	MPI_Irecv(&curr_grid[v(0, 1)], 1, contiguous_row, lowerRank, 1, MPI_COMM_WORLD, &recv_req[1]);

	// Send Lower
	MPI_Isend(&curr_grid[v(ROWS_With_Ghost - 2, 1)], 1, contiguous_row, lowerRank, 1, MPI_COMM_WORLD, &send_req[1]);


	// Receive Left
	MPI_Irecv(&curr_grid[v(1, COLS_With_Ghost - 1)], 1, col, leftRank, 2, MPI_COMM_WORLD, &recv_req[2]);

	// Send Left
	MPI_Isend(&curr_grid[v(1, 1)], 1, col, leftRank, 2, MPI_COMM_WORLD, &send_req[2]);

	// Reveice Right
	MPI_Irecv(&curr_grid[v(1, 0)], 1, col, rightRank, 3, MPI_COMM_WORLD, &recv_req[3]);

	// Send Right
	MPI_Isend(&curr_grid[v(1, COLS_With_Ghost - 2)], 1, col, rightRank, 3, MPI_COMM_WORLD, &send_req[3]);


	MPI_Waitall(4, recv_req, status); // aspetta che tutte le receive siano completates
}

void exchange_MOORE(int* curr_grid, int& rank, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRight, int& COLS_With_Ghost, int& ROWS_With_Ghost)
{
	MPI_Datatype contiguous_row, col;

	MPI_Status s, s1;

	// Creazione Type per le righe
	MPI_Type_contiguous(COLS_With_Ghost - 2, MPI_INT, &contiguous_row);
	MPI_Type_commit(&contiguous_row);

	// Creazione Type per le colonne
	MPI_Type_vector(ROWS_With_Ghost - 2, 1, COLS_With_Ghost, MPI_INT, &col);
	MPI_Type_commit(&col);


	// SEND al vicinato di Moore

	// UPPER
	if (upperRank_b != -1)
		MPI_Send(&curr_grid[v(1, 1)], 1, contiguous_row, upperRank_b, 0, MPI_COMM_WORLD);

	// LOWER
	if (lowerRank_b != -1)
		MPI_Send(&curr_grid[v(ROWS_With_Ghost - 2, 1)], 1, contiguous_row, lowerRank_b, 0, MPI_COMM_WORLD);

	// Left
	if (leftRank_b != -1)
		MPI_Send(&curr_grid[v(1, 1)], 1, col, leftRank_b, 0, MPI_COMM_WORLD);

	// Right
	if (rightRank_b != -1)
		MPI_Send(&curr_grid[v(1, COLS_With_Ghost - 2)], 1, col, rightRank_b, 0, MPI_COMM_WORLD);


	// UPPER DIAGONAL LEFT
	if (upperDiagonalLeft != -1)
		MPI_Send(&curr_grid[v(1, 1)], 1, MPI_INT, upperDiagonalLeft, 0, MPI_COMM_WORLD);

	// UPPER DIAGONAL RIGHT
	if (upperDiagonalRight != -1)
		MPI_Send(&curr_grid[v(1, COLS_With_Ghost - 2)], 1, MPI_INT, upperDiagonalRight, 0, MPI_COMM_WORLD);


	// LOWER  DIAGONAL LEFT
	if (lowerDiagonLeft != -1)
		MPI_Send(&curr_grid[v(ROWS_With_Ghost - 2, 1)], 1, MPI_INT, lowerDiagonLeft, 0, MPI_COMM_WORLD);

	// LOWER DIAGONAL RIGHT
	if (lowerDiagonalRight != -1)
		MPI_Send(&curr_grid[v(ROWS_With_Ghost - 2, COLS_With_Ghost - 2)], 1, MPI_INT, lowerDiagonalRight, 0, MPI_COMM_WORLD);




	// RECEIVE dal vicinato di Moore

	// UPPER
	if (upperRank_b != -1)
		MPI_Recv(&curr_grid[v(0, 1)], 1, contiguous_row, upperRank_b, 0, MPI_COMM_WORLD, &s);

	// LOWER
	if (lowerRank_b != -1)
		MPI_Recv(&curr_grid[v(ROWS_With_Ghost - 1, 1)], 1, contiguous_row, lowerRank_b, 0, MPI_COMM_WORLD, &s);

	// LEFT
	if (leftRank_b != -1)
		MPI_Recv(&curr_grid[v(1, 0)], 1, col, leftRank_b, 0, MPI_COMM_WORLD, &s1);

	// RIGHT
	if (rightRank_b != -1)
		MPI_Recv(&curr_grid[v(1, COLS_With_Ghost - 1)], 1, col, rightRank_b, 0, MPI_COMM_WORLD, &s1);


	// UPPER DIAGONAL LEFT
	if (upperDiagonalLeft != -1)
		MPI_Recv(&curr_grid[v(0, 0)], 1, MPI_INT, upperDiagonalLeft, 0, MPI_COMM_WORLD, &s1);


	// UPPER DIAGONAL RIGHT
	if (upperDiagonalRight != -1)
		MPI_Recv(&curr_grid[v(0, COLS_With_Ghost - 1)], 1, MPI_INT, upperDiagonalRight, 0, MPI_COMM_WORLD, &s1);

	// LOWER DIAGONAL LEFT
	if (lowerDiagonLeft != -1)
		MPI_Recv(&curr_grid[v(ROWS_With_Ghost - 1, 0)], 1, MPI_INT, lowerDiagonLeft, 0, MPI_COMM_WORLD, &s1);

	// LOWER DIAGONAL RIGHT
	if (lowerDiagonalRight != -1)
		MPI_Recv(&curr_grid[v(ROWS_With_Ghost - 1, COLS_With_Ghost - 1)], 1, MPI_INT, lowerDiagonalRight, 0, MPI_COMM_WORLD, &s1);

}

// Funzione per scambiare i puntatori delle matrici, la matrice di lettura diventa la matrice di scrittura e viceversa
void swap(int*& readM, int*& writeM)
{
	int* p = readM; // p punta alla matrice di lettura

	readM = writeM; // la matrice di lettura diventa la matrice di scrittura

	writeM = p; // la matrice di scrittura diventa la matrice di lettura
}

// Funzione che applica le regole per decidere se una cella deve essere accesa o spenta
void transFuncCell(int& rank, int* readM, int* writeM, int& i, int& j, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& vicinatoScelto)
{
	int cont = 0;

	// calcoliamo il numero di celle vive attorno alla cella corrente
	for (int di = -1; di < 2; di++)
	{
		for (int dj = -1; dj < 2; dj++)
		{
			if ((di != 0 || dj != 0) && readM[v((i + di), (j + dj))] == 1)
				cont++;
		}
	}

	if (readM[v(i, j)] == 1) // se la cella corrente è viva
	{
		if (cont == 2 || cont == 3) // se il numero di celle vive attorno alla cella corrente è 2 o 3 allora la cella corrente rimane viva
			writeM[v(i, j)] = 1;

		else
			writeM[v(i, j)] = 0; // altrimenti la cella corrente muore
	}

	else // se la cella corrente è morta
	{
		if (cont == 3) // se il numero di celle vive attorno alla cella corrente è 3 allora la cella corrente diventa viva
			writeM[v(i, j)] = 1;

		else // altrimenti la cella corrente rimane morta
			writeM[v(i, j)] = 0;
	}
}

// Funzione che serve per calcolare il nuovo stato di ogni cella
void transFunc(int& rank, int* readM, int* writeM, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& vicinatoScelto)
{
	for (int i = 1; i < ROWS_With_Ghost - 1; i++)
	{
		for (int j = 1; j < COLS_With_Ghost - 1; j++)
		{
			transFuncCell(rank, readM, writeM, i, j, ROWS_With_Ghost, COLS_With_Ghost, vicinatoScelto);
		}
	}
}

// funione per ricostruire la griglia
void Rebuild_grid(int grid[][COLS], int*& readM, int& rank, int& nProc, int& ROWS_With_Ghost, int& COLS_With_Ghost, int& ROWS_Grid, int& COLS_Grid, int& COLS_P, GraphicComponent* gc)
{
	int* readM_without_ghost; // matrice senza ghost cells

	int ROWS_WithOut_Ghost = ROWS_With_Ghost - 2; // numero di righe senza ghost cells

	int COLS_WithOut_Ghost = COLS_With_Ghost - 2; // numero di colonne senza ghost cells

	int size = ROWS_WithOut_Ghost * COLS_WithOut_Ghost; // dimensione della matrice senza ghost cells


	readM_without_ghost = new int[(ROWS_WithOut_Ghost) * (COLS_WithOut_Ghost)];
	{
		int* readM_without_ghost;

		int ROWS_WithOut_Ghost = ROWS_With_Ghost - 2;

		int COLS_WithOut_Ghost = COLS_With_Ghost - 2;

		int size = ROWS_WithOut_Ghost * COLS_WithOut_Ghost;

		readM_without_ghost = new int[(ROWS_WithOut_Ghost) * (COLS_WithOut_Ghost)];

#define v_No_Ghost(r, c) (((r) * (COLS_WithOut_Ghost)) + (c))

		for (int i = 0; i < ROWS_WithOut_Ghost; i++)
		{
			for (int j = 0; j < COLS_WithOut_Ghost; j++)
			{
				readM_without_ghost[v_No_Ghost(i, j)] = 0;
			}

		}

		if (rank != 0)
		{
			for (int i = 1; i < ROWS_With_Ghost - 1; i++)
			{
				for (int j = 1; j < COLS_With_Ghost - 1; j++)
				{
					readM_without_ghost[v_No_Ghost(i - 1, j - 1)] = readM[v(i, j)];
				}
			}

			MPI_Send(readM_without_ghost, size, MPI_INT, 0, rank, MPI_COMM_WORLD);
		}

		if (rank == 0)
		{
			MPI_Status s;

			int i_proc0 = 0;
			int j_proc0 = 0;

			for (int i = 1; i < ROWS_With_Ghost - 1; i++)
			{

				for (int j = 1; j < COLS_With_Ghost - 1; j++)
				{

					readM_without_ghost[v_No_Ghost(i - 1, j - 1)] = readM[v(i, j)];
				}
			}

			for (int x = 0; x < ROWS_WithOut_Ghost; x++)
			{
				for (int j = 0; j < COLS_WithOut_Ghost; j++)
				{
					grid[i_proc0][j_proc0] = readM_without_ghost[v_No_Ghost(x, j)];

					j_proc0 += 1;
				}

				i_proc0 += 1;

				j_proc0 -= COLS_WithOut_Ghost;
			}

			for (int i = 1; i < nProc; i++)
			{
				MPI_Recv(readM_without_ghost, size, MPI_INT, i, i, MPI_COMM_WORLD, &s);

				int i_start = (i / COLS_P) * ROWS_Grid;

				int j_start = (i % COLS_P) * COLS_Grid;

				for (int x = 0; x < ROWS_WithOut_Ghost; x++)
				{

					for (int j = 0; j < COLS_WithOut_Ghost; j++)
					{
						grid[i_start][j_start] = readM_without_ghost[v_No_Ghost(x, j)];

						j_start += 1;
					}

					i_start += 1;

					j_start -= COLS_WithOut_Ghost;
				}
			}

			gc->drawScene(grid);
		}

		return;
	}
}

void show_VON_NEUMANN_neighborhood(int& rank, int& rightRank, int& leftRank, int& upperRank, int& lowerRank) {

	std::cout << "VON NEUMANN-NO_BOUNDARIES\n";

	std::cout << "    " << upperRank << "\n";

	std::cout << leftRank << "<--" << rank << "-->" << rightRank << "\n";

	std::cout << "    " << lowerRank << "\n";

	std::cout << "\n";

	std::cout << "\n";
}

void show_MOORE_neighborhood(int& rank, int& rightRank_b, int& leftRank_b, int& upperRank_b, int& lowerRank_b, int& upperDiagonalLeft, int& upperDiagonalRight, int& lowerDiagonLeft, int& lowerDiagonalRight) {

	std::cout << "MOORE_WITH_BOUNDARIES\n";

	std::cout << upperDiagonalLeft << "   " << upperRank_b << "   " << upperDiagonalRight << "\n";

	std::cout << leftRank_b << "   " << rank << "   " << rightRank_b << "\n";

	std::cout << lowerDiagonLeft << "   " << lowerRank_b << "   " << lowerDiagonalRight << "\n";

	std::cout << "\n";

	std::cout << "\n";

	std::cout << "\n";



}