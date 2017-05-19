/* This file was generated automatically with transmutagen version 1.0.1. */
/* The command used to generate this file was: python -m transmutagen.gensolve --namespace cyclus --outfile transmute.c*/
#ifndef CYCLUS_SOLVE_C
#define CYCLUS_SOLVE_C



typedef struct cyclus_transmute_info_tag {
  int n;
  int nnz;
  int* i;
  int* j;
  char** nucs;
  int* nucids;
  double* decay_matrix;
} cyclus_transmute_info_t;

extern cyclus_transmute_info_t cyclus_transmute_info;

int cyclus_transmute_ij(int i, int j);

int cyclus_transmute_nucid_to_i(int nucid);


void cyclus_expm_multiply14(double* A, double* b, double* x);
#endif
