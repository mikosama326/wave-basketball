#ifndef BASICSTUFF_H_INCLUDED
#define BASICSTUFF_H_INCLUDED


#define FLOORRES 1
#define WIDTH 800
#define HEIGHT 600
#define TIMEMAX 120

extern float floorval[];
extern float currentTime;
extern float previousTime;
extern float timeDiff;
extern int fd;
extern unsigned int timecount;
extern int scorefd;
extern unsigned int highscore;
extern int window;
extern int oops;
extern int highscoreflag;
extern unsigned int highscores[5];

#ifndef HIGHSCORES_SIZE
#define HIGHSCORES_SIZE 5
#endif // HIGHSCORES_SIZE

#define DEBUG //Activates all the debugging constructs in the program.

#endif // BASICSTUFF_H_INCLUDED
