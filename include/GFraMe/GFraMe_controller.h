/**
 * @file include/GFraMe/GFraMe_controller.h
 */
#ifndef __GFRAME_CONTROLLER_H_
#define __GFRAME_CONTROLLER_H_

#include <GFraMe/GFraMe_error.h>
#include <SDL2/SDL_events.h>

struct stGFraMe_controller {
    char a :1;
    char b :1;
    char x :1;
    char y :1;
    char l1 :1;
    char r1 :1;
    char l2 :1; // should actually be an axis
    char r2 :1; // should actually be an axis
    char l3 :1;
    char r3 :1;
    char up :1;
    char down :1;
    char left :1;
    char right :1;
    char select :1;
    char start :1;
    char home :1;
    char dummy :7;
    float lx; // left horizontal axis
    float ly; // left vertical axis
    float rx; // right horizontal axis
    float ry; // right vertical axis
};

extern struct stGFraMe_controller *GFraMe_controllers;
extern int GFraMe_controller_max;

void GFraMe_controller_init(int autoConnect);
void GFraMe_controller_close();
void GFraMe_controller_bind();
void GFraMe_controller_unbind();
void GFraMe_controller_update(SDL_Event *e);
void GFraMe_controller_printStates();

#endif

