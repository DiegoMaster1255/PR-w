

// Celem tego programu jest prezentacja pomiaru i analizy 
//efektywnosci programu za pomocš  CodeAnalyst(tm).
// Implementacja mno¿enia macierzy jest realizowana za pomoca typowego 
// algorytmu podrêcznikowego. 
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "omp.h"

#define USE_MULTIPLE_THREADS true
#define MAXTHREADS 128
int NumThreads;
double start;

static const int N = 300;

static const int ROWS = N;     // liczba wierszy macierzy
static const int COLUMNS = N;  // lizba kolumn macierzy


static const int R = 256;

float matrix_a[N][N];    // lewy operand 
float matrix_b[N][N];    // prawy operand
float matrix_r[N][N];    // wynik

FILE *result_file;

void initialize_matrices()
{
	// zdefiniowanie zawarosci poczatkowej macierzy
	//#pragma omp parallel for 
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			matrix_a[i][j] = (float)rand() / RAND_MAX;
			matrix_b[i][j] = (float)rand() / RAND_MAX;
			matrix_r[i][j] = 0.0;
		}
	}
}

void initialize_matricesZ()
{
	// zdefiniowanie zawarosci poczatkowej macierzy
#pragma omp parallel for 
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			matrix_r[i][j] = 0.0;
		}
	}
}
void print_result()
{
	// wydruk wyniku
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			fprintf(result_file, "%6.4f ", matrix_r[i][j]);
		}
		fprintf(result_file, "\n");
	}
}

void multiply_matrices_JKI_v0()
{
	// !!TODO Trzeba ogarn¹æ jaki to ma rozmiar i jak wprowadziæ lokalnoœæ
	// I ogólnie co ma byæ, bo nie wiem

#pragma omp parallel for
	for (int j = 0; j < N; j++)
		for (int k = 0; k < N; k++) {
			for (int i = 0; i < N; i++)
				matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];
		}


}

void multiply_matrices_JKI_v1()
{
	// !!TODO Trzeba ogarn¹æ jaki to ma rozmiar i jak wprowadziæ lokalnoœæ
	// I ogólnie co ma byæ, bo nie wiem

#pragma omp parallel for
	for (int j = 0; j < N; j++)
		for (int k = 0; k < N; k++) {
			float r = matrix_b[k][j];
			int i = 0;
			for (i; i < N; i++)
				matrix_r[i][j] += matrix_a[i][k] * r;
		}
			

}



void multiply_matrices_6() {

//kod w wersji równoleg³ej
	for (int i = 0; i<N; i += R)
		for (int j = 0; j<N; j += R)
			for (int k = 0; k<N; k += R)
#pragma omp parallel for  //jeden z mo¿liwych wariantów podzia³u pracy
				for (int ii = i; ii<i + R; ii++)
					for (int jj = j; jj<j + R; jj++)
						for (int kk = k; kk<k + R; kk++)
							//wyra¿enie typu
							matrix_r[ii][jj] += matrix_a[ii][kk] * matrix_b[kk][jj];
}

void multiply_matrices_sequence()
{
	// mnozenie macierzy 
	//#pragma omp parallel for 
	for (int i = 0; i < N; i++)
		for (int k = 0; k < N; k++)
			for (int j = 0; j < N; j++)
				matrix_r[i][j] += matrix_a[i][k] * matrix_b[k][j];

}

void print_elapsed_time()
{
	double elapsed;
	double resolution;

	// wyznaczenie i zapisanie czasu przetwarzania
	elapsed = (double)clock() / CLK_TCK;
	resolution = 1.0 / CLK_TCK;
	printf("Czas: %8.4f sec \n",
		elapsed - start);

	fprintf(result_file,
		"Czas wykonania programu: %8.4f sec (%6.4f sec rozdzielczosc pomiaru)\n",
		elapsed - start, resolution);
}

int main(int argc, char* argv[])
{
	//	 start = (double) clock() / CLK_TCK ;
	if ((result_file = fopen("classic.txt", "a")) == NULL) {
		fprintf(stderr, "nie mozna otworzyc pliku wyniku \n");
		perror("classic");
		return(EXIT_FAILURE);
	}


	//Determine the number of threads to use
	if (USE_MULTIPLE_THREADS) {
		SYSTEM_INFO SysInfo;
		GetSystemInfo(&SysInfo);
		NumThreads = SysInfo.dwNumberOfProcessors;
		if (NumThreads > MAXTHREADS)
			NumThreads = MAXTHREADS;
	}
	else
		NumThreads = 1;
	fprintf(result_file, "Klasyczny algorytm mnozenia macierzy, liczba watkow %d \n", NumThreads);
	printf("liczba watkow  = %d\n\n", NumThreads);


	initialize_matrices();
	start = (double)clock() / CLK_TCK;
	multiply_matrices_JKI_v0();
	printf("JKIv0 ");
	print_elapsed_time();
	initialize_matricesZ();
	start = (double)clock() / CLK_TCK;
	multiply_matrices_JKI_v1();
	printf("JKIv1 ");
	print_elapsed_time();
	initialize_matricesZ();
	start = (double)clock() / CLK_TCK;
	multiply_matrices_6();
	printf("IIJJKK ");
	print_elapsed_time();
	initialize_matricesZ();
	start = (double)clock() / CLK_TCK;
	multiply_matrices_sequence();
	printf("Sequence: ");
	print_elapsed_time();

	fclose(result_file);

	return(0);
}