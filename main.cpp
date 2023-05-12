//***************************************************************************************
//Developer: Arslan Rasheed
//Fast Nuces Lahore
//Created: 12/5/23
//***************************************************************************************

#include <mpi.h>
#include <iostream>
#include<cstdlib>
#include <string>

using namespace std;

//code to run on terminal
//mpiexec –n 4 ./pdcA2.exe
//cd source/repos/pdcA2/x64/debug


int static MIN_SIZE = 2;
int static MAX_SIZE = 20;

//Declare and Return Matrix
int** declare_MxN_matrix(int row, int col) {
	int** matrix = new int* [row];
	for (int i = 0; i < row; i++) {
		matrix[i] = new int[col];
	}
	return matrix;
}

//Generate Random Numbers
void randomGenerator(int** matrix, int row, int col) {

	srand((unsigned)time(NULL));

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int randm = 1 + rand() % 10;
			matrix[i][j] = randm;
		}
	}
}

//input from user
int** inputFromUser(int row, int col) {
	int** matrix = new int* [row];
	int inp = -1;

	for (int i = 0; i < row; i++) {

		matrix[i] = new int[col];

		for (int j = 0; j < col; j++) {
			cout << "\nEnter Value of row ( " << i + 1 << " ) and column ( " << j + 1 << " ) :  ";
			cin >> inp;
			matrix[i][j] = inp;
		}
	}

	return matrix;
}

void printMatrix(int** matrix, int row, int col) {
	cout << endl;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			cout << matrix[i][j] << " ";
		}
		cout << "\n";
	}
}

void merge(int* matrixRow, int left, int mid, int right) {
	int i, j, k;
	int left_arr_len = mid - left + 1;
	int right_arr_len = right - mid;

	int* LeftArray = new int[left_arr_len];
	int* RightArray = new int[right_arr_len];


	for (i = 0; i < left_arr_len; i++)
		LeftArray[i] = matrixRow[left + i];
	for (j = 0; j < right_arr_len; j++)
		RightArray[j] = matrixRow[mid + 1 + j];

	i = 0;
	j = 0;
	k = left;

	//merge the array till same length of array
	while (i < left_arr_len && j < right_arr_len) {
		if (LeftArray[i] <= RightArray[j]) {
			matrixRow[k] = LeftArray[i];
			i++;
		}
		else {
			matrixRow[k] = RightArray[j];
			j++;
		}
		k++;
	}

	//add remaning elements
	while (i < left_arr_len) {
		matrixRow[k] = LeftArray[i];
		i++;
		k++;
	}
	while (j < right_arr_len) {
		matrixRow[k] = RightArray[j];
		j++;
		k++;
	}
}

void mergeSort(int* MatrixRow, int left, int right) {
	if (left < right) {
		int mid = left + (right - left) / 2;
		//array breakdown
		mergeSort(MatrixRow, left, mid);
		mergeSort(MatrixRow, mid + 1, right);
		//merge back
		merge(MatrixRow, left, mid, right);
	}
}


int main(int argc, char** argv) {

	//Matrix Variables
	int num_of_rows = 0, num_of_cols = 0;
	int** matrix = nullptr;
	int* matrixRow = nullptr;
	long long row_product;
	long long global_product;
	int col = -1;

	//MPI variables
	int process_rank;
	int process_size;
	int tag = 0;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);
    
	// Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if (process_rank == 0) {
        // Print the message
        cout << "My rank is: " << process_rank << "  . I am the Root";

		cout << "\nEnter Number of Rows: ";
		cin >> num_of_rows;
		cout << "\nEnter Number of Columns: ";
		cin >> num_of_cols;

		if (num_of_rows < MIN_SIZE || num_of_cols < MIN_SIZE || num_of_rows > MAX_SIZE || num_of_cols > MAX_SIZE) {
			cout << "\nMIN ROW AND COLUMN SHOULD BE 2 AND MAX SIZE SHOULD BE 20";
			return 0;
		}

		//number of rows should be equal to number of processes
		MPI_Comm_size(MPI_COMM_WORLD, &process_size);
		if (num_of_rows != process_size - 1) {
			cout << endl << "<<<<<<<<<<<<<<<<<<<<<<  Number of rows should be equal to number of processes - 1  >>>>>>>>>>>>>>>>>>>>\n\n";
			return 0;
		}
		else {
			//
			int choice = -1;
			do
			{
				cout << "\n\nPress 1 to Declare matrix randomly.";
				cout << "\nPress 2 to take input from user.";
				cout << "\nEnter Choice:  ";
				try
				{
					cin >> choice;
					if (choice <= 0 or choice >= 9) {
						throw "Invalid Input";
					}
				}
				catch (string& s)
				{
					cout << "Exception: " << s;
				}

			} while (choice != 1 and choice != 2);


			if (choice == 1) {
				//initialize matrixay
				matrix = declare_MxN_matrix(num_of_rows, num_of_cols);

				//initialize with random numbers
				randomGenerator(matrix, num_of_rows, num_of_cols);
			}
			else {
				//initialize matrix using user input
				matrix = inputFromUser(num_of_rows, num_of_cols);
			}

			//print
			cout << "\nThe Matrix is: ";
			printMatrix(matrix, num_of_rows, num_of_cols);


			//send data to processes
			for (int i = 1; i < process_size; i++) {
				MPI_Send(&num_of_cols, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
				MPI_Send(matrix[i-1], num_of_cols, MPI_INT, i, tag, MPI_COMM_WORLD);
			}

			//reveive data from processes
			for (int i = 1; i < process_size; i++) {
				MPI_Recv(matrix[i-1], num_of_cols, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			//print sorted matrix
			cout << "\nThe Sorted Matrix is: ";
			printMatrix(matrix, num_of_rows, num_of_cols);

		}
    }
    else {
		
		//receive number of columns 
		MPI_Recv(&col, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//initialize array dynamically
		matrixRow = new int[col];
		//receive row array
		MPI_Recv(matrixRow, col, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (col == -1) {
			cout << "\nReceive Failed";
		}
		else {
			cout << "---------------------------------";
			cout << "\nAt Rank " << process_rank << " Received Row is:  ";
			for (int i = 0; i < col; i++) {
				cout << matrixRow[i] << " ";
			}
			//call merge sort
			mergeSort(matrixRow, 0, col - 1);

			cout << "\nAt Rank " << process_rank << " Sorted Row is:  ";
			for (int i = 0; i < col; i++) {
				cout << matrixRow[i] << " ";
			}

			//send back to root process 0
			MPI_Send(matrixRow, col, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
    }

	//All processes gather here
	MPI_Barrier(MPI_COMM_WORLD);


	// Each process calculates the product of its row
	row_product = 1;
	for (int i = 0; i < col; i++) {
		if (process_rank != 0) {
			//process 0 will not involve in product computation
			row_product *= matrixRow[i];
		}
	}

	cout << "\nRow product at process: " << process_rank << " is: " << row_product << endl;

	// Reduce the row products to find the global product
	MPI_Reduce(&row_product, &global_product, 1, MPI_LONG_LONG, MPI_PROD, 0, MPI_COMM_WORLD);


	if (process_rank == 0) {
		// Print the global product
		cout << "\nThe Global Product of Matrix is: " << global_product;
	}
    

	//Deallocating memory
	if (matrix != nullptr) {
		for (int i = 0; i < num_of_rows; i++) {
			delete[]matrix[i];
		}
		delete[]matrix;
	}
	if (matrixRow != nullptr) {
		delete[]matrixRow;
	}

    // Finalize the MPI environment.
    MPI_Finalize();
}
