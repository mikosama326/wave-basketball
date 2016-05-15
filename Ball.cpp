#include "Ball.h"
#include "Vector.h"
#include "BasicStuff.h"
#include<GL/glut.h>
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//Ball class stuff
extern void gameOverScreen();
extern void saveme();

Ball :: Ball()
{
    ballpos.setvals(400,300);
    ballspeed.setvals(1,2);
    radius = BALLRADIUS;
    g = -9.8;
    score = 0;
    flag = 0;// 1 if inside the basket
}

void Ball :: moveball()
{
    float a[2];

    previousTime = currentTime;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDiff = currentTime - previousTime;

    //timecount= (int)currentTime;
    //timecount /= 1000;

    timeDiff /= 3;//Some scaling. -- 3

    a[0] = 0;
    a[1] = timeDiff*g/1000;

    ballspeed = ballspeed + a;

    ballpos = ballpos + ballspeed*timeDiff;

    #ifdef DEBUG
    char log[512];
    sprintf(log, "%f: %f %f %f %f \n",currentTime/1000.0, ballpos.xcomp, ballpos.ycomp, ballspeed.xcomp, ballspeed.ycomp);
    write(fd, log, strlen(log));
    #endif // DEBUG

    //ballpos.adjust();

    if(ballpos.xcomp < -BALLRADIUS || ballpos.ycomp < -BALLRADIUS || ballpos.xcomp > WIDTH+BALLRADIUS || ballpos.ycomp > HEIGHT+BALLRADIUS)
    {
        //if(ballpos.xcomp < 0) ballpos.xcomp *= -1;
        //if(ballpos.ycomp < 0) ballpos.ycomp *= -1;

        //Vector maybe = ballpos + ballspeed*timeDiff;

        //if(maybe.xcomp < 0 || maybe.ycomp < 0)
        //{
            char log[512];
            sprintf(log,"DIES !!\nOne of our positions is out of bounds: %f %f\n",ballpos.xcomp,ballpos.ycomp);
            write(fd, log, strlen(log));

            if(ballpos.xcomp < -BALLRADIUS) {ballpos.xcomp = 0; ballspeed.xcomp *= -1;}
            if(ballpos.ycomp < -BALLRADIUS) {ballpos.ycomp = 10; ballspeed.ycomp *= -1;}
            if(ballpos.xcomp > WIDTH+BALLRADIUS) {ballpos.xcomp = WIDTH; ballspeed.xcomp *= -1;}
            if(ballpos.ycomp > HEIGHT+BALLRADIUS) {ballpos.ycomp = HEIGHT; ballspeed.ycomp *= -1;}


            //oops = 1;
            //saveme();
            //glutDisplayFunc(gameOverScreen);
            //glutPostRedisplay();

            //glutDestroyWindow(window);
            //_exit(1);
        //}
    }

    #ifdef DEBUG
    sprintf(log, "After adjust: %f %f %f %f\n\n", ballpos.xcomp, ballpos.ycomp, ballspeed.xcomp, ballspeed.ycomp);
    write(fd, log, strlen(log));
    #endif // DEBUG

    checkscore();

    collide();
}

void Ball :: collide()
{
    float x,y;
    float ballx = ballpos.xcomp;
    float bally = ballpos.ycomp;

    if(bally <= 0)
        ballspeed.ycomp = -ballspeed.ycomp;
    if(bally >= HEIGHT)
    {
        ballspeed.ycomp = -ballspeed.ycomp;
        glutPostRedisplay();
    }
    if(ballx <= 0 || ballx >= WIDTH)
    {
        ballspeed.xcomp = -ballspeed.xcomp;
        glutPostRedisplay();
    }

    for(x=0;x<WIDTH*FLOORRES;x++)
    {
        y = floorval[(int)x];
        if(ballx == x && bally == y)
            glutPostRedisplay();
        Vector p;
        p.setvals(x,y);
        p = ballpos - p;
        if(/*(x-ballx)*(x-ballx)+(y-bally)*(y-bally)*/ p.magnitude() <= BALLRADIUS/*BALLRADIUS*/)
        {
            //p = ballpos - p;
            /*#ifdef DEBUG
            char log[512];
            sprintf(log, "%f: %f %f %f %f \n",currentTime/1000.0, ballpos.xcomp, ballpos.ycomp, ballspeed.xcomp, ballspeed.ycomp);
            write(fd, log, strlen(log));
            #endif // DEBUG*/

            ballspeed = (p/p.magnitude()) * ballspeed.magnitude();
        }
    }
    glutPostRedisplay();
}

void Ball :: draw_ball()
{
    float xc = ballpos.getxcomp();
    float yc = ballpos.getycomp();

    float theta, step = 0.8;//CHANGE STEP TO ADJUST GRAIN OF BALL

    glColor3f(0.8745,0.4921875,0);
    glPointSize(2.0);

    glBegin(GL_POLYGON);
    for(theta = 0;theta <= 360; theta+=step)
        glVertex2f(xc + radius*cos(theta),yc + radius*sin(theta));
    glEnd();

    glColor3f(0,0,0);
    glPointSize(1.5);
    glBegin(GL_POINTS);
    for(theta = 0;theta <= 360; theta+=step)
        glVertex2f(xc + radius*cos(theta),yc + radius*sin(theta));
    glEnd();
}

void Ball :: drawVectors()
{
    float scale = 2;
    glBegin(GL_LINES);
    glVertex2f(ballpos.xcomp,ballpos.ycomp);
    glVertex2f(ballpos.xcomp + ballspeed.xcomp*scale,ballpos.ycomp);
    glVertex2f(ballpos.xcomp,ballpos.ycomp);
    glVertex2f(ballpos.xcomp,ballpos.ycomp + ballspeed.ycomp*scale);
    glEnd();
}

void Ball :: checkscore()
{
    int delta = 100;
    int i,j;
    switch(flag)
    {
        case 0:
            if(ballpos.ycomp <= (407+delta) && ballpos.ycomp >= (407-delta))
            {
                if(ballpos.xcomp <= (WIDTH - 50 + delta) && ballpos.xcomp >= (WIDTH - 50 - delta))
                    flag = 1;
            }
            else
                flag = 0;
            break;
        case 1:
            if(ballpos.ycomp <= (320+delta) && ballpos.ycomp >= (320-delta))
                if(ballpos.xcomp <= (WIDTH - 50 + delta) && ballpos.xcomp >= (WIDTH - 50 - delta))
                    score += 1;
            flag = 0;
            break;
    }

    if(score > highscore)
    {
        highscore = score;
        highscoreflag = 1;
    }
}

void Ball :: reset()
{
    ballpos.setvals(400,300);
    ballspeed.setvals(1,2);
    radius = BALLRADIUS;
    g = -9.8;
    score = 0;
    flag = 0;// 1 if inside the basket
}

int Ball :: getScore()
{
    return score;
}
