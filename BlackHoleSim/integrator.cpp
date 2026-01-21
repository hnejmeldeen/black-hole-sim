#include <vector> 
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "global.h"

const double G = 1;
double M = 0.5;
double RS(double M){
    return 2 * G * M;
}  
const double c = 1;
double h;

bool blackHole(State& S, State& k1, State& k2, State& k3) {
    if (S.r < RS(M) || (S+k1*(h/2)).r < RS(M) || (S+k2*(h/2)).r < RS(M) || (S+k3*h).r < RS(M)) return true;
    return false;
}

State deriv(State S) {
    State dS;

    dS.t = S.dt, dS.r = S.dr, dS.phi = S.dphi;

    dS.dt = -RS(M)/(S.r * (S.r - RS(M))) * S.dr * S.dt;
    dS.dr = -RS(M)*(S.r-RS(M))/(2*pow(S.r, 3)) * S.dt * S.dt - RS(M)/(2*S.r*(RS(M)-S.r)) * S.dr * S.dr - (RS(M) - S.r) * S.dphi * S.dphi;
    dS.dphi = -2/S.r * S.dr * S.dphi;

    return dS;
}

void RK4(State& S, bool& absorbed) {

    h = std::min(1.0, S.r * 0.5);

    State k1 = deriv(S);
    State k2 = deriv(S + k1 * (h/2));
    State k3 = deriv(S + k2 * (h/2));
    State k4 = deriv(S + k3 * h);

    S = S + (k1 + k2*2 + k3*2 + k4) * (h/6);
    
    if (blackHole(S, k1, k2, k3)) absorbed = true;

}

void RK2(State& S) {

    h = std::min(1.0, S.r * 0.5);

    State k1 = deriv(S);
    State k2 = deriv(S + k1 * (h/2));

    S = S + k2 * h;
}