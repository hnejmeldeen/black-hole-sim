#include <vector> 
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "global.h"

const double G = 1;
double M = 0.5;
double RS(double M);
const double c = 1;
double h;

bool blackHole(State& S, State& k1, State& k2, State& k3);
State deriv(State S);
void RK4(State& S, bool& absorbed);
void RK2(State& S);