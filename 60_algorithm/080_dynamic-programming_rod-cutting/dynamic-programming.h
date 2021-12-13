/*
  bellman principle of dynamic programming

  Dynamic (time dependent) programming is an algorithmic method to
  solve optimization problems by dividing a problem up into
  sub-problems, and memorizing their intermediate results
  (memoization, from "memo"). The method was invented and named in
  1940 by R. Bellman but was already known in physics e.g. the
  transformation of lagrange functions into hamilton-functions by
  means of legendre.

  When developing a dynamic-programming algorithm, we follow a
  sequence of four steps:

  1. Characterize the structure of an optimal solution

  2. Recursively define the value of an optimal solution

  3. Compute the value of an optimal solution, typically in a
     bottom-up fashion

  4. Construct an optimal solution from computed information


  NB: The relation to "greedy algorithms", which have many
  similarities to dynamic programming. In particular, problems to
  which greedy algorithms apply have optimal substru cture. One major
  difference between greedy algorithms and dynamic programming is that
  instead of first finding optimal solutions to subproblems and then
  making an informed choice, greedy algorithms first make a "greedy"
  choice - the choice that looks best at the time - and then solve a
  resulting subproblem, without bothering to solve all possible
  related smaller subproblems. Surprisingly, in some cases this
  strategy works!


  Overlapping Subproblems

  The space of subproblems must be "small" in the sense that a
  recursive algorithm for the problem solves the same subproblems over
  and over, rather than always generating new subproblems.

  When a recursive algorihtm revisits the same problem repeatedly, we
  say that the optimization problem has overlapping subproblems.

  NB: Two subproblems of the same problem are "independent" if they do
  not share resources. Two subproblems are "overlapping" if they are
  really the same subproblem that occurs as a subproblem of different
  problems.

  In contrast, a problem for which a divide-and-conquer approach is
  suitable usually generates brand-new problems at each step of the
  recursion. Dynamic-programming algorithms typically take advantage
  of overlapping subproblems by solving each subproblem once and then
  storing the solution in a table where it can be looked up when
  needed, using constant time per lookup.

  REFERENCES
  - Algorigthms [Cormen, Leiserson, Rivest, Stein]
 */

#ifndef DYNAMIC_PROGRAMMING_H
#define DYNAMIC_PROGRAMMING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// enable debug printout (dot)
//#define DEBUG 1

// utils
#define max(a, b) ((a) > (b) ? (a) : (b))


int cut_rod(const int *prices, const int nprices, int length);
//int memoized_cut_rod_aux(const int *prices, const int nprices, int length);
int memoized_cut_rod(const int *prices, const int nprices, int length);
int bottom_up_cut_rod(const int *prices, const int nprices, int length);
int extended_bottom_up_cut_rod(const int *prices, const int nprices, int length);
void print_cut_rod_solution(const int *prices, const int nprices, int length);


#endif /* DYNAMIC_PROGRAMMING_H */
