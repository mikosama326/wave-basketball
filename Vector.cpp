#include "Vector.h"
#include<math.h>

extern void gameOverScreen();

Vector :: Vector()
 {
    xcomp = 0;
    ycomp = 0;
 }

Vector :: Vector(float x, float y)
 {
    xcomp = x;
    ycomp = y;
 }

 void Vector :: setvals(float xval, float yval)
 {
    xcomp = xval;
    ycomp = yval;
 }

 float Vector :: getxcomp()
 {
    return xcomp;
 }

 float Vector :: getycomp()
 {
    return ycomp;
 }

 float Vector :: magnitude()
 {
    return sqrt(xcomp*xcomp + ycomp*ycomp);
 }

 Vector Vector :: operator+(Vector b)
 {
    Vector res;
    res.xcomp = xcomp + b.xcomp;
    res.ycomp = ycomp + b.ycomp;
    return res;
 }

 Vector Vector :: operator-(Vector b)
 {
    Vector res;
    res.xcomp = xcomp - b.xcomp;
    res.ycomp = ycomp - b.ycomp;
    return res;
 }

 Vector Vector :: operator*(float scalar)
 {
    Vector res;
    res.xcomp = scalar*xcomp;
    res.ycomp = scalar*ycomp;
    return res;
 }

 Vector Vector :: operator/(float scalar)
 {
    Vector res;
    res.xcomp = xcomp/scalar;
    res.ycomp = ycomp/scalar;
    return res;
 }

 Vector Vector :: operator+(const float val[2])
 {
    Vector res;
    res.xcomp = xcomp + val[0];
    res.ycomp = ycomp + val[1];
    return res;
 }

 void Vector :: adjust()
 {
    #ifdef DEBUG
    if(xcomp < 0 || ycomp < 0)
    {
        char log[512];
        sprintf(log,"DIES !!\nOne of our positions is out of bounds: %f %f\n",xcomp,ycomp);
        write(fd, log, strlen(log));
        oops = 1;
        //glutDisplayFunc(gameOverScreen);
        //glutPostRedisplay();

        glutDestroyWindow(window);
        _exit(1);
    }
    #endif // DEBUG

    //xcomp = (xcomp < 0)? 0 : xcomp;
    //ycomp = (ycomp < 0)? 0 : ycomp;

    /*if(xcomp < 0 || ycomp < 0)
    {
        xcomp = 400;
        ycomp = 300;
    }*/
 }
