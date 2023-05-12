# Matrix-Operation-using-MPI
Matrix Operations Using MPI, Initialization, Operators, Merge Sort

# How to Run
code to run on terminal
1: install MPI first 
2: open terminal and direct to the debug folder of your project 
   for example in my case it is "cd source/repos/pdcA2/x64/debug"
3: run command (4 represents number of processors, exe file will be your executable file with your project name in the debug folder)
   mpiexec –n 4 ./pdcA2.exe

# Here is the complete problem detail
Write a distributed program using ‘MPI’ to perform matrix operations as instructed below. You can
paste your code in this document. Teaching assistant can ask any student to run the code or he can
conduct a viva to check the authenticity.
Part A:
1. Create a matrix of size (m x n) and initialize the matrix by some random values or user input in
process 0.
2. Distribute the rows using relevant communication operator to all processes.
3. Each process then sort the assigned row using merge sort.
4. Finally collect sorted rows from all processes to process 0.
5. Each process should also calculate the product of each row which then can be reduced at process 0 to
find the product of the complete matrix. You can use reduce function for this task.
