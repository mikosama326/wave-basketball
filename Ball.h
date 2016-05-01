#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED

#include "Vector.h"
#include "BasicStuff.h"
#define BALLRADIUS 35

class Ball
{
    protected:
    Vector ballpos,ballspeed;
    float radius;
    float g;
    unsigned int score;
    int flag;

    public:

    Ball();
    void moveball();
    void collide();
    void draw_ball();
    void checkscore();
    void drawVectors();
    int getScore();
    void reset();
};

#endif // BALL_H_INCLUDED
