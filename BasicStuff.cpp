//const int width = 800;
//const int height = 600;
#define HIGHSCORES_SIZE 5
float currentTime, previousTime, timeDiff;
int fd;
int scorefd;
unsigned int highscore = 0;
unsigned int timecount = 0;
unsigned int highscores[HIGHSCORES_SIZE];

int window;
int oops = 0;
int highscoreflag = 0;//nope
