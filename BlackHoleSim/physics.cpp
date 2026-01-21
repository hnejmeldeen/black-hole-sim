#include <vector> 
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iostream>

#include "global.h"
#include "integrator.cpp"

const double PI = 3.1415926535;

const double FOV = 60;
const double WIDTH = 160;
const double HEIGHT = 120;

const int BOX = 12;

double r;
double f;
Triple rhat;


std::vector<uint8_t> pixels(WIDTH * HEIGHT * 4, 0);

Light* sendRay(Triple& position, Triple& direction, std::vector<Light>& lights) {

    int counter = 0;
    
    double t = 0;
    double theta = PI/2;
    double phi = 0;

    double dr = sqrt(1 - 2/r) * direction.dot(rhat);
    double dphi = sqrt(c*c - direction.dot(rhat) * direction.dot(rhat)) / r;
    double dt = sqrt(dr*dr/f + r*r * dphi*dphi / (f*f));

    Triple phihat = direction + rhat * -direction.dot(rhat);
    unitVec(phihat);
    Triple thetahat = phihat.cross(rhat);

    State S = {t, r, phi, dt, dr, dphi};

    bool absorbed = false;

    while (!absorbed) {
        
        bool strongField = (S.r < 6*G*M);
        if (strongField) RK4(S, absorbed);
        else RK2(S);

        if (absorbed) return &lights[0];

        Triple globalPos = position * (S.r / r);

        globalPos = rotateVec(globalPos, thetahat, S.phi);

        if (S.r > BOX) {
            absorbed = true;
            return &lights[1];
        } 

        for (int i = 2; i < lights.size(); ++i) {
            if (lights[i].hasAbsorbed(globalPos)) {
                absorbed = true;
                return &lights[i];
            }
        }

        if (counter == 1000) {
            absorbed = true;
        }

        ++counter;

    }
    return &lights[0];
}


void buildScene(Triple& position, std::vector<Light>& lights) {

    r = position.mag();
    f = (1 - 2*G*M/(c*c*r));
    rhat = {position.x/r, position.y/r, position.z/r};

    double r_plane = sqrt(position.x*position.x + position.y*position.y);
    Triple axis_h = { -position.y/r_plane, position.x/r_plane, 0 };
    Triple axis_v = axis_h.cross(rhat);


    for (int y = 0; y < HEIGHT; ++y) {
        double alpha = (y - HEIGHT/2)*FOV/WIDTH*PI/180;
        for (int x = 0; x < WIDTH; ++x) {
            double beta = (x - WIDTH/2)*FOV/WIDTH*PI/180;

            Triple direction = position * -1;
            unitVec(direction);
            direction = rotateVec(direction, axis_h, -alpha);
            direction = rotateVec(direction, axis_v, -beta);

            Color pix = (*sendRay(position, direction, lights)).color;

            int i = (y * WIDTH + x) * 4;
            pixels[i + 0] = pix.r;
            pixels[i + 1] = pix.g;
            pixels[i + 2] = pix.b;
            pixels[i + 3] = pix.a;
        }
    }
}