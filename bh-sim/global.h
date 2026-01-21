#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <vector> 
#include <cstdint>
#include <iostream>

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color() = default;

    Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
        : r{r_}, g{g_}, b{b_}, a{a_} {}
};

struct Triple {
    double x, y, z;
    
    double phi() {
        return atan2(y, x);
    }

    double theta() {
        return acos(z / sqrt(x*x+y*y+z*z));
    }

    Triple() = default;

    Triple(double x_, double y_, double z_)
        : x{x_}, y{y_}, z{z_} {}

    double dot(const Triple v2) {
        return x * v2.x + y * v2.y + z * v2.z;
    }

    Triple cross(const Triple v2) {
        return {y*v2.z-z*v2.y, z*v2.x-x*v2.z, x*v2.y-y*v2.x};
    }

    double mag() {
        return sqrt(x*x + y*y + z*z);
    }
};

inline Triple operator*(const Triple& T, double k) {
    return {
        k * T.x,
        k * T.y,
        k * T.z,
    };
}

inline Triple operator+(const Triple& a, const Triple& b) {
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
    };
}

void unitVec(Triple& T) {
    T = T * (1/sqrt(T.x*T.x + T.y*T.y + T.z*T.z));
}

Triple rotateVec(Triple v1, Triple v2, const double phi) { //MUST Normalize v2
    unitVec(v2);
    return (v1 * cos(phi) 
        + v2.cross(v1) * sin(phi)
        + v2 * v2.dot(v1) * (1 - cos(phi)));
}

struct State {
    double t, r, phi;
    double dt, dr, dphi;

    State() = default;

    State(double t_, double r_, double phi_, 
        double dt_, double dr_, double dphi_)
        : t{t_}, r{r_}, phi{phi_}, dt{dt_}, dr{dr_}, dphi{dphi_} {}

};

inline State operator+(const State& a, const State& b) {
    return {
        a.t + b.t,
        a.r + b.r,
        a.phi + b.phi,
        a.dt + b.dt,
        a.dr + b.dr,
        a.dphi + b.dphi
    };
}

inline State operator*(const State& s, double k) {
    return {
        k * s.t,
        k * s.r,
        k * s.phi,
        k * s.dt,
        k * s.dr,
        k * s.dphi
    };
}

struct Light {

    Triple position; 
    Color color;
    double R;

    Light() = default;

    Light(Triple pos, Color c, double r)
        : position{pos}, color{c}, R{r} {}

    bool hasAbsorbed(Triple ray) {
        return (sqrt((ray.x - position.x) * (ray.x - position.x)
                    +(ray.y - position.y) * (ray.y - position.y)
                    +(ray.z - position.z) * (ray.z - position.z)) 
                        < R);
    }
};

struct Select {
    Light* selected;
    Color original_color;
    int whichObject;

    Select() = default;

    Select(Light* selected_)
        : selected{selected_}, original_color{selected_->color}, whichObject{2} {}

    void enforce(std::vector<Light>& lights_list) {
        if (selected == &lights_list[0]) {
            whichObject = 1;
            original_color = selected->color;
            selected->color = {40,40,40,255};
        } else if (selected == &lights_list[1]) {
            whichObject = 2;
            original_color = selected->color;
        } else {
            whichObject = 3;
            original_color = selected->color;
            selected->color = {255,255,255,255};
        }
    }
};

extern const double PI;

extern const double FOV;
extern const double WIDTH;
extern const double HEIGHT;

extern const int BOX;

extern const double G;
extern double M;
extern const double c;
extern double h;

extern std::vector<Light> lights;
extern Triple position;

#endif