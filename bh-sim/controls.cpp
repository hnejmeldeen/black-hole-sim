
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

#include "global.h"
#include "physics.cpp"

int screen_mult = 5;

Triple position;
const double theta_max = 0.95 * PI;
const double theta_min = PI - theta_max;
const double min_mass = 0;

Select cursor;

struct DragState {
    bool dragging = false;
    double lastX = 0.0;
    double lastY = 0.0;
} drag;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get cursor position at the time of click
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos /= screen_mult, ypos = (HEIGHT - ypos / screen_mult);

        r = position.mag();
        f = (1 - 2*G*M/(c*c*r));
        rhat = {position.x/r, position.y/r, position.z/r};

        double r_plane = sqrt(position.x*position.x + position.y*position.y);
        Triple axis_h = { -position.y/r_plane, position.x/r_plane, 0 };
        Triple axis_v = axis_h.cross(rhat);

        double alpha = (ypos - HEIGHT/2)*FOV/WIDTH*PI/180;
        double beta = (xpos - WIDTH/2)*FOV/WIDTH*PI/180;

        Triple direction = position * -1;
        unitVec(direction);
        direction = rotateVec(direction, axis_h, -alpha);
        direction = rotateVec(direction, axis_v, -beta);

        cursor.selected->color = cursor.original_color;
        cursor.selected = sendRay(position, direction, lights);
        cursor.enforce(lights);
    }
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

    const double scale = 0.001;

    if (cursor.whichObject == 1) {
        double M_temp = M - yoffset * scale;
        if (2 * G * M_temp > min_mass && 2 * G * M_temp < position.mag() / 2) {
            M = M_temp;
        } else if (2 * G * M_temp < min_mass) {
            M = min_mass;
        }
    } else if (cursor.whichObject == 2) {
        double R_temp = position.mag() - yoffset * scale * 2;
        if (R_temp > 4 * G * M && R_temp < BOX) {
            position.x *= R_temp/position.mag();
            position.y *= R_temp/position.mag();
            position.z *= R_temp/position.mag();
        } 
    } else {
        double R_temp = cursor.selected->position.mag() - yoffset * scale * 2;
        if (R_temp > 0 && R_temp < BOX) {
            cursor.selected->position.x *= R_temp/cursor.selected->position.mag();
            cursor.selected->position.y *= R_temp/cursor.selected->position.mag();
            cursor.selected->position.z *= R_temp/cursor.selected->position.mag();
        }
    }
}


void processArrowKeys(GLFWwindow* window, double deltaTime) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        cursor.selected->color = cursor.original_color;
        cursor.selected = &lights[1];
        cursor.enforce(lights);
    }

    if (cursor.whichObject == 1) return;
    Triple* selectedPos;
    if (cursor.whichObject == 2) selectedPos = &position;
    else selectedPos = &(cursor.selected->position);

    double dx = 0.0;
    double dy = 0.0;

    double speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 3.0 : 1.2;
    const double scale = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        dx -= scale;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        dx += scale;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        dy -= scale;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        dy += scale;

    if (dx == 0.0 && dy == 0.0)
        return;


    double xtemp = selectedPos->x;
    double ytemp = selectedPos->y;

    selectedPos->x = xtemp * cos(dx) - ytemp * sin(dx);
    selectedPos->y = xtemp * sin(dx) + ytemp * cos(dx);

    double r_plane = sqrt(selectedPos->x*selectedPos->x + selectedPos->y*selectedPos->y);

    Triple phihat = { -selectedPos->y/r_plane, selectedPos->x/r_plane, 0 };

    Triple new_position = rotateVec(*selectedPos, phihat, dy);
    if (cursor.whichObject == 2 && (new_position.phi() - position.phi() < 1)) 
        position = new_position;
    else if (new_position.phi() - (*selectedPos).phi() < 1) {
        cursor.selected->position = new_position;
    }

}