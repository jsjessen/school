#Parallel Random Number Generator

Project 3 - Due date: 10/27/2015 (Tuesday, 11:59pm PT)via OSBLE dropbox
=======================================================================
Assignment type: Individuals or Teams of 2

In this project you will implement a parallel random number series generator, using the Linear Congruential generating model. Here, the ith random number, denoted by xi, is given by:

        xi = (a*xi-1 + b) mod P, where a and b are some positive constants and P is a big constant (typically a large prime).

Your goal is to implement an MPI program for generating a random series up to the nth random number of the linear congruential series (i.e., we need all n numbers of the series, and not just the nth number). We discussed an algorithm that uses parallel prefix in the class on 10/13/2015. Further explanation of the parallel algorithm can be found in the notes by Prof. Aluru (chapter 1). I expect you to implement this algorithm. Operate under the assumption that n>>p. Your code should have its own explicit implementation the parallel prefix operation. Your code should also get parameter values {a,b,P} and the random seed to use (same as x0), from the  user.

It should be easy to test your code by writing your own simple serial code and comparing your output.
Performance analysis:
    a) Generate speedup charts (speedups calculated over your serial implementation), fixing n to a large number such as a million and varying the number of processors from 2 to 16.
    b) Study total runtime as a function of n. Vary n from a small number such as 16 and keep doubling until it reaches a large value (e.g., 1 million).
    Compile a brief report that presents and discusses your results/findings.

    Deliverables (zipped into one zip file - with your name on it):
        i) Source code,
        ii) Report in PDF or Word

=======================================================================

int P = 5 (for testing)
int P = 2147483647 (largest signed 32-bit prime)

1. Create serial script on local computer for generating random numbers
2. Test serial script and make sure it is working, since it will be used to test the parallel version
3. Create matrix multiplication function, also locally (small, so serial)
4. Modify the matrix multiplication function to mod by P after ever addition 
5. Create parallel prefix function (must use MPI)
6. Create random number generator:
    1. Get input a, b, P, seed as command line args
    2. Broadcast input to all procs 
    3. Make matrix M
    4. Make identity matrix
    5. Make an array of size n filled with M's, except the first element is an identity matrix
    6. Use parallel prefix
    7. Have each proc perform serial computation on its block
    8. Gather the random numbers to rank 0
    9. Output an array of n random numbers (call and print to stdout)
7. Compare to serial random number generator
8. Debug
9. Analyze the performance between srand and prand
10. Submit

=======================================================================

NOTES

Input for random number generator (a, b, P, x0) where x0 is seed
broadcast input to all procs

xi = (x_i-1 * a + 1 * b) % P // matrix mult then mod P

so you don't actually need to the other half of the calculation
but you can make your code generic by using matrix mult function
(but how to include mod P?)

took pics

goal is to calculate large array of size n
partitioned into n/p blocks

(fixed) M = [a 0
	         b 1]

initially the array is filled with n copies of M

each block needs the cumulative  M^k

M^0 = identity matrix = [1 0	which is like 1 but in matrix terms
			             0 1]

need a local matrix product and global matrix product variables to keep track

for p=4

L: id		id			id		    id
G: M^(n/p)	M^(n/p)		M^(n/p)	    M^(n/p)


M^(n/p) // computed serially

P0_G * P1_G	P2_G * P3_G (use hypercube logic to determine partner)

when sending a matrix between proc, flatten out to array and rebuild on other end

L: id		M^(n/p)			id		    M^(n/p)
G: M^2n/p	M^2n/p			M^2n/p		M^2n/p

L: id		M^(n/p)			M^(2n/p)	M^(3n/p)
G: M^4n/p	M^4n/p			M^4n/p		M^4n/p

Once you know x_k (value not matrix) just use equation form not matrix mult

Can't use MPI_Scan for this :(

test output of parallel code by using the same seed with serial alg
the series should be the same for the same seed (just faster)

think in terms of functions:

	function for matrix mult (every addition should have a mod P after
				  so to fill 2x2 matrix, use four mod P's

assume p is always power of 2

lg p - 1
MPI_sendRecv with comm partner (hypercube bit)
if recv from lower rank mylocal + theirGlobal = new_mylocal
if recv from high rank, don't add to mylocal

use serial code for computation within blocks
which starts once you have M^k (right?)

The whole point of PP for this problem is calculating powers of M

M needs to be a square matrix so that multiplying them is a binary associative operation (the dimensions don't change)

-------------------------------------------------------------------------------

Parallel Prefix Operation

array x, size n

x: x0 x1 x2 x3 ... xi ... x(n-1)
y: 		   yi

		yi = x0 + x1 + ... + xi

where each element in array y holds the cumulative count up to that point in x

sum = 0
for(i = 0 to n-1)
{
	sum += xi
	yi = sum
}
// yi = yi-1 + xi

example: array n=8

each p maintains:
localsum (eventual output)
globalsum (used for propagation to others)

only update localsum if receive from lower rank p (conditional update)
groups will converge on the same globalsum (mpi reduce)

		p0 p1 p2 p3 p4 p5 p6 p7
		2  1  3  4  3  1  2  3

0_localsum:	2  1  3  4  3  1  2  3
0_globalsum:	2  1  3  4  3  1  2  3
		  			1 hop cross
1_localsum:	2  3  3  7  3  4  2  5
1_globalsum:	3  3  7  7  4  4  5  5
					2 hop cross
1_localsum:	2  3  6  10 3  4  6  9
1_globalsum:	10 10 10 10 9  9  9  9
					4 hop cross
1_localsum:	2  3  6  10 13 14 16 19 = y
1_globalsum:	19 19 19 19 19 19 19 19

log(p) steps

works for binary associative operators: +, x, -(a-b = a+(-b)), /(a/b= a*(1/b))

code:

x: n = p elements
y: yi = y(i-1) + xi

L = x
G = x
for(i = 0 to (lg(p) - 1))
{
	mate = rank obtained by toggling the ith LSB(least sig bit) of my rank
	// toggle with XOR (need to keep left shifting something)
	
	Exchange G with mate
	// receives G'

	G += G'

	if(mate < myrank)
		L += G'
}
output L
O(lg p)


What if n >> p?

each p has n/p elements

	1) local sum of n/p elements in each proc
	    z' z' z' z' z' z' z' z'

	2) p-element parallel prefix (lg p steps)
	    z0 z1 z2 z3 z4 z5 z6 z7

	3) (zi - z') + serial local prefix


code:

init: rank pi holds x[rank n/p ... (rank+1) * n/p - 1]
      localx: n/p elements  O(n/p)

algo:
	1) compute the sum of all your n/p elements  O(n/p)
		z <- sum(localx()

	2) using the z's across all p procs, 	O(lg p) // comm
	   do a parallel prefix (p-element pp)
		output z'= x0 + x1 + ... x(end)

	3) z' -= z

	4) 	sum = z'		O(n/p)
		for(i=0 to (n/p -1))
			yi = sum + xi
			sum = yi

	5) output y[0 ... n/p - 1]


assuming operation is constant time (not matrix multiplication)

computation: O(n/p)
communication: O(Tlg(p) + ulg(p))
space: = O(n/p)

-------------------------------------------------------------------------------

