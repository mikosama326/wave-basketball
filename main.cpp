#include <iostream>

using namespace std;

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <SOIL/SOIL.h>

#include "BasicStuff.h"
#include "Vector.h"
#include "Ball.h"
#include "AudioStuff.h"

#define START_BUTTON_X 300
#define START_BUTTON_Y 150

#define EXIT_BUTTON_X 315
#define EXIT_BUTTON_Y 130

#define EXIT_BUTTON_SMALL_X 700
#define EXIT_BUTTON_SMALL_Y 30

#define SCORESTR_LENGTH 10

#define NUM_BUFFERS 9
#define NUM_SOURCES 9


//#define FLOORRES 1

/* ------ Warning: Total mess up ahead ----- */
/* --------- READ AT YOUR OWN RISK  -------- */

float floorval[WIDTH*FLOORRES];
Ball b;
float uymax = 200, fy = 0;
float ux = WIDTH/2.0;
int floorstate = 1; // 0 is up; 1 is down

enum GameState{MAINMENU, PLAYING, GAMEOVER};
GameState gamestate;

//Variables for holding the loaded textures. For speed.
GLuint MainMenuImg, StartButtonImg, StartButtonImgOver, BackdropImg, SmallExitImg, ExitButtonImg, GameOverImg, StartButton, HighScoreImg;

int starttime;//handles the weird issue with the timer starting earlier than the game

float reshapex = 1, reshapey = 1;//Meant for handling what happens if we reshape the game.
int width = WIDTH,height = HEIGHT;

//Variables for openAL
ALuint buffers[NUM_BUFFERS];
ALuint sources[NUM_SOURCES];
ALuint mainscreensrc, mainscreenbuffer;
ALuint playsrc, playbuffer;
ALuint gameoversrc, gameoverbuffer;

void scoreSave1();// Forward declaration for the scoreSave function
void saveLeaderboard();// Another forward declaration

/*void saveme()
{
    gamestate = GAMEOVER;
    int i;
    for(i=0;i<NUM_SOURCES;i++)
        alSourceStop(sources[i]);
}*/

void switchtoGameOver();
void switchtoPlay();

void my_exit()// A custom exit function
{
    /*if(score > highscores[HIGHSCORES_SIZE-1])
        for(i=0;i<HIGHSCORES_SIZE;i++)
        {
            if(score > highscores[i])
            {
                for(j=HIGHSCORES_SIZE-1;j >= (i+1);j--)
                    highscores[j] = highscores[j-1];
                highscores[i] = score;
                return;
            }
        }*/

    alSourceStop(mainscreensrc);
    glutIdleFunc(0); // Turn off Idle function if used.
    glutDestroyWindow(window);
    saveLeaderboard();
    scoreSave1();
    saveLeaderboard();
    close(fd);

    //Delete openAL sources
    alDeleteSources(NUM_SOURCES,sources);
    alDeleteSources(1,&playsrc);
    alDeleteSources(1,&gameoversrc);
    //Delete openAL buffers
    alDeleteBuffers(NUM_BUFFERS, buffers);
    alDeleteBuffers(1, &playbuffer);
    alDeleteBuffers(1, &gameoverbuffer);

    alDeleteSources(1,&mainscreensrc);
    alDeleteBuffers(1,&mainscreenbuffer);

    alutExit();

    _exit(0);// For some reason, the exit() function isn't quite working. So we're using _exit() instead. I know, that's bad. Whatever.
}

GLuint LoadPNG(const char* filename)// To load an image as a texture.
{
    GLuint texture = SOIL_load_OGL_texture(filename,SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (texture == 0) return 0;
        //Log("Texture Load Error: " + string(filename));

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

void drawQuadTex(float xmin, float ymin, float xmax, float ymax, GLuint tex)// To draw a texture rectangle
{
    if(tex == 0)
        return;
    //Bind our texture.
    glBindTexture(GL_TEXTURE_2D, tex);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0);
    glVertex2f(xmin,ymin);

    glTexCoord2f(1, 0);
    glVertex2f(xmax,ymin);

    glTexCoord2f(1, 1);
    glVertex2f(xmax,ymax);

    glTexCoord2f(0, 1);
    glVertex2f(xmin,ymax);

    glEnd();
}

void printtext(const char str[])// Will print whatever string you give it. Use RasterPos2f before you call this.
{
    glPushMatrix();
    unsigned int i;
    for(i=0;i < strlen(str);i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[i]);
    glPopMatrix();
}

void drawbox(float xmin, float ymin, float xmax, float ymax)// Draws a rectangle.
{
    glBegin(GL_QUADS);
    glVertex2f(xmin,ymin);
    glVertex2f(xmax,ymin);
    glVertex2f(xmax,ymax);
    glVertex2f(xmin,ymax);
    glEnd();
}

void drawbox_border(float xmin, float ymin, float xmax, float ymax)// Draws a rectangular border
{
    glBegin(GL_LINE_LOOP);
    glVertex2f(xmin,ymin);
    glVertex2f(xmax,ymin);
    glVertex2f(xmax,ymax);
    glVertex2f(xmin,ymax);
    glEnd();
}

void mainmenu()// Main Menu display function
{
    glClear(GL_COLOR_BUFFER_BIT);

    #ifdef DEBUG
    char log[30] = "In the Main Menu.\n";
    write(fd, log, strlen(log));
    #endif // DEBUG

    drawQuadTex(0,0,WIDTH,HEIGHT,MainMenuImg);

    drawQuadTex(START_BUTTON_X,START_BUTTON_Y,START_BUTTON_X + 170,START_BUTTON_Y + 60,StartButton); //CHECK

    glColor3f(1,1,1);
    glLineWidth(2.0);
    drawbox_border(1,1,WIDTH-1,HEIGHT-1);

    glFlush();
}

void basket()// Draws a basket.
{
    glColor3f(0.671875,0,0);
    glBegin(GL_QUADS);
    glVertex2f(WIDTH-100,400);
    glVertex2f(WIDTH,400);
    glVertex2f(WIDTH,407);
    glVertex2f(WIDTH-100,407);
    glEnd();

    glColor3f(0.4,0.4,0.4);
    glBegin(GL_QUADS);
    glVertex2f(WIDTH-95,400);
    glVertex2f(WIDTH-5,400);
    glVertex2f(WIDTH-15,320);
    glVertex2f(WIDTH-85,320);
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(WIDTH-100,400);
    glVertex2f(WIDTH,400);
    glVertex2f(WIDTH,407);
    glVertex2f(WIDTH-100,407);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex2f(WIDTH-95,400);
    glVertex2f(WIDTH-5,400);
    glVertex2f(WIDTH-15,320);
    glVertex2f(WIDTH-85,320);
    glEnd();
}

void floor(float uy)// Draws the EQ floor thing
{
    float sigmasq = 4000;
    float x,y;

    glPointSize(2.0);
    glBegin(GL_POINTS);
    for(x=0;x<WIDTH;x++)
    {
        y = exp(-(x - ux)*(x - ux)/(2*sigmasq)) * uy;
        //y =  (1/sqrt(2*3.14*sigmasq)) * exp(-(x - ux)*(x - ux)/(2*sigmasq)) * uy;

        if(y < 0 || uy == 0)
            y = 0;
        floorval[(int)(x)] = y;

        glColor3f(0,0,0);
        glVertex2f(x,y--);
        glColor3f(0.2,0.2,1);
        for(;y>=0; y-=FLOORRES)
            glVertex2f(x,y);
    }
    glEnd();
    //glFlush();
}

void intToString(char *str, int num) // Function that converts an integer into a string. Yeah, surprising. I know.
{
    strcpy(str,"");
    sprintf(str,"%d",num);
}

void printscore()// Draws the score box
{
    char scorestr[SCORESTR_LENGTH];
    intToString(scorestr,b.getScore());

    glPushMatrix();
    glTranslatef(600,450,0);
    glPushMatrix();
    glColor3f(0.1,0.1,0.1);

    drawbox(100,100,190,130);

    glLineWidth(2.0);
    glColor3f(1,0,0);

    drawbox_border(100,100,190,130);

    glColor3f(1,1,1);
    glRasterPos2f(103,103);
    printtext(scorestr);

    glPopMatrix();
    glPopMatrix();
}

void timer()// Draws and updates the timer
{
    char timerstr[10];

    timecount = glutGet(GLUT_ELAPSED_TIME) - starttime;
    timecount /= 1000;
    timecount = TIMEMAX - timecount;
    if(timecount == 0)
    {
        gamestate = GAMEOVER;
        return;
    }

    strcpy(timerstr,"");
    sprintf(timerstr,"%.2d : %.2d",timecount/60,timecount%60);

    glPushMatrix();
    glTranslatef(-40,450,0);
    glPushMatrix();
    glColor3f(0.1,0.1,0.1);

    drawbox(100,100,190,130);

    glLineWidth(2.0);
    glColor3f(1,0,0);

    drawbox_border(100,100,190,130);

    glColor3f(1,1,1);
    glRasterPos2f(115,103);
    printtext(timerstr);

    glPopMatrix();
    glPopMatrix();
}

void drawfloor()// Updates the floor values for the EQ
{
    if(fy < uymax && floorstate == 0)
        fy+=2;
    else if(fy > 0 && floorstate == 1)
        fy-=2;
    floor(fy);
}

void gameOverScreen()// Game Over display function
{
    char highscorestr[SCORESTR_LENGTH];
    char scorestr[SCORESTR_LENGTH];

    glClear(GL_COLOR_BUFFER_BIT);

    //Draw the Game Over backdrop
    drawQuadTex(0,0,WIDTH,HEIGHT,GameOverImg);

    drawQuadTex(EXIT_BUTTON_X,EXIT_BUTTON_Y,EXIT_BUTTON_X+140,EXIT_BUTTON_Y+50,ExitButtonImg);

    if(highscoreflag)
    {
        glPushMatrix();
        //glColor3ub(215,0,0);
        //glRasterPos2f(320,330);
        //printtext("New High Score!!");
        drawQuadTex(320,310,320+180,310+60,HighScoreImg);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //Print score
    intToString(scorestr,b.getScore());
    glPushMatrix();
    glColor3f(1,0,0);
    glRasterPos2f(328,290);
    printtext(scorestr);
    glPopMatrix();

    //Print current highscore
    intToString(highscorestr,highscore);
    glPushMatrix();
    glColor3f(1,0,0);
    glRasterPos2f(328,235);
    printtext(highscorestr);
    glPopMatrix();

    if(oops)
    {
        glPushMatrix();
        glColor3f(1,1,1);
        glRasterPos2f(400,100);
        printtext("Oops, we've run into a problem.");

        glRasterPos2f(400,50);
        printtext("Check log.txt for the details.");
        glPopMatrix();
    }
    //A supposed border. I can't actually see it. But whatever.
    //glColor3f(1,1,1);
    //glLineWidth(2.0);
    //drawbox_border(1,1,WIDTH-1,HEIGHT-1);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    #ifdef DEBUG
    char log[30] = "In the Game Over screen.\n";
    write(fd, log, strlen(log));
    #endif // DEBUG

    glFlush();
}

void playGame()// The actual game display function
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    drawQuadTex(0,0,WIDTH,HEIGHT,BackdropImg);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //Draw score
    glColor3f(1,1,1);
    glRasterPos2f(640,555);
    printtext("Score:");
    printscore();

    //Draw timer
    glColor3f(1,1,1);
    glRasterPos2f(10,555);
    printtext("Time:");
    timer();

    if(timecount == 0)
    {
        switchtoGameOver();
        glutPostRedisplay();
    }

    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_BLEND);

    glLineWidth(2);
    basket();

    drawfloor();
    b.draw_ball();


    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    //glEnable(GL_TEXTURE_2D);
    drawQuadTex(EXIT_BUTTON_SMALL_X,EXIT_BUTTON_SMALL_Y,EXIT_BUTTON_SMALL_X+89,EXIT_BUTTON_SMALL_Y+32,SmallExitImg);
    //b.drawVectors();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    #ifdef DEBUG
    char log[30] = "In the Game.\n";
    write(fd, log, strlen(log));
    #endif // DEBUG

    glFlush();

    //glutSwapBuffers();
}

void mouseOverButtons(int x, int y)// The mouseover callback function
{
    y = height-y;
    switch(gamestate)
    {
        case MAINMENU:
            if(x >= START_BUTTON_X && x <= (START_BUTTON_X + 170))
                if(y >= (START_BUTTON_Y) && y <= ((START_BUTTON_Y) + 60))
                {
                    StartButton = StartButtonImgOver;
                    break;
                }

            StartButton = StartButtonImg;
            break;
        case PLAYING:
        case GAMEOVER:
        break;
    }
    glutPostRedisplay();
}

void myinit() //Program initializing
{
    gluOrtho2D(0,WIDTH,0,HEIGHT);
    glClearColor(0.5,0.5,0.5,1);
    currentTime = glutGet(GLUT_ELAPSED_TIME);
}

void gameinit1();//Forward definition of my game init functtion

void playinit()// Initializes some parameters after clicking 'Start'
{
    b.reset();
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    starttime = glutGet(GLUT_ELAPSED_TIME);
}

void mouse(int btn, int state, int x, int y)// Mouse event callback function
{
    y = height-y;
    switch(gamestate)
    {
        case MAINMENU:
            if(x >= START_BUTTON_X && x <= (START_BUTTON_X + 140))
                if(y >= (START_BUTTON_Y) && y <= ((START_BUTTON_Y) + 50))
                    if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
                    {
                        switchtoPlay();
                        glutPostRedisplay();
                    }
            break;
        case PLAYING:
        if(x >= EXIT_BUTTON_SMALL_X && x <= (EXIT_BUTTON_SMALL_X + 170))
                if(y >= (EXIT_BUTTON_SMALL_Y) && y <= ((EXIT_BUTTON_SMALL_Y) + 60))
                    if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
                    {
                        switchtoGameOver();
                        glutPostRedisplay();
                    }
        break;
        case GAMEOVER:
            if(x >= EXIT_BUTTON_X && x <= (EXIT_BUTTON_X + 170))
                if(y >= (EXIT_BUTTON_Y) && y <= ((EXIT_BUTTON_Y) + 60))
                    if(btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
                        my_exit();
        break;
    }
    glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)// Key down callback function
{
    //playAudio("AcchiKocchiChangeSound.wav");
    if(gamestate == PLAYING)
        switch(key)
        {
            case 'z':
                ux = 0;
                alSourcePlay(sources[0]);
                break;
            case 'x':
                ux = 100;
                alSourcePlay(sources[1]);
                break;
            case 'c':
                ux = 200;
                alSourcePlay(sources[2]);
                break;
            case 'v':
                ux = 300;
                alSourcePlay(sources[3]);
                break;
            case 'b':
                ux = 400;
                alSourcePlay(sources[4]);
                break;
            case 'n':
                ux = 500;
                alSourcePlay(sources[5]);
                break;
            case 'm':
                ux = 600;
                alSourcePlay(sources[6]);
                break;
            case ',':
                ux = 700;
                alSourcePlay(sources[7]);
                break;
            case '.':
                ux = 800;
                alSourcePlay(sources[8]);
                break;
        }
    fy = 0;
    floorstate = 0;
    //glutPostRedisplay();
}

void myidle()// Idle function
{
    if(gamestate == PLAYING)
        b.moveball();
}

void upkeys(unsigned char key, int x, int y)// Key up callback function
{
    if(gamestate == PLAYING)
    {
        switch(key)
        {
            case 'z':
                alSourceStop(sources[0]);
                alSourceRewind(sources[0]);
                break;
            case 'x':
                alSourceStop(sources[1]);
                alSourceRewind(sources[1]);
                break;
            case 'c':
                alSourceStop(sources[2]);
                alSourceRewind(sources[2]);
                break;
            case 'v':
                alSourceStop(sources[3]);
                alSourceRewind(sources[3]);
                break;
            case 'b':
                alSourceStop(sources[4]);
                alSourceRewind(sources[4]);
                break;
            case 'n':
                alSourceStop(sources[5]);
                alSourceRewind(sources[5]);
                break;
            case 'm':
                alSourceStop(sources[6]);
                alSourceRewind(sources[6]);
                break;
            case ',':
                alSourceStop(sources[7]);
                alSourceRewind(sources[7]);
                break;
            case '.':
                alSourceStop(sources[8]);
                alSourceRewind(sources[8]);
                break;
        }
        fy = uymax;
        floorstate = 1;
    }
    //glutPostRedisplay();
}

void textInit()//Initializes textures so that game loads fast
{
    MainMenuImg = LoadPNG("images/ABMainMenu.png");
    StartButtonImg = LoadPNG("images/StartButton.png");
    StartButton = StartButtonImg;
    StartButtonImgOver = LoadPNG("images/StartButtonOver.png");
    BackdropImg = LoadPNG("images/Backdrop.png");
    SmallExitImg = LoadPNG("images/ExitButtonSmall.png");
    ExitButtonImg = LoadPNG("images/ExitButton.png");
    GameOverImg = LoadPNG("images/ABGameOver.png");
    HighScoreImg = LoadPNG("images/HighScore.png");
}

void readScores();// Yep. Another forward declaration

void gameinit()// Initalizing some parameters for the game:: OBSOLETE
{
    char buf[10];
    int res;

//    strcpy(buttonimg,"StartButton.png");//Main menu start button image

    readScores();

    scorefd = open("highscore.txt",O_RDWR | O_CREAT, 0755);

    if(scorefd < 0)
    {
        printf("Score open error.\n");
        return;
    }

    res = read(scorefd, buf, 10);

    if(res == -1)
    {
        printf("Score read error.\n");
        return;
    }
    /*if(strlen(buf) == 0)
        highscore = 0;
    else*/
        highscore = atoi(buf);
    #ifdef DEBUG
    printf("Highscore read: %d\nBuffer contents:%s\n",highscore,buf);
    #endif // DEBUG
}

void saveLeaderboard()//Writes the leaderboard values to a file
{
    int res;

    unlink("leaderboard");

    scorefd = open("leaderboard",O_RDWR | O_CREAT,0755);

    //unlink("highscore");
    //ftruncate(scorefd, 0);

    //printf(buf,"%d",highscore);
    res = write(scorefd, highscores, HIGHSCORES_SIZE*sizeof(unsigned int));

    if( res == -1)
    {
        printf("Score write error.\n");
        return;
    }
    close(scorefd);
}

void readScores()//Reads from the leaderboard file
{
    int res;
    int i;
    //strcpy(buttonimg,"StartButton.png");//Main menu start button image

    scorefd = open("leaderboard",O_RDWR | O_CREAT,0755);

    if(scorefd < 0)
    {
        printf("Leaderboard open error.\n");
        return;
    }

    res = read(scorefd, highscores, HIGHSCORES_SIZE*sizeof(unsigned int));

    if(res == -1)
    {
        printf("Score read error.\n");
        return;
    }

    if(res == 0)
        for(i=0;i < HIGHSCORES_SIZE;i++)
            highscores[i] = 0;

    //#ifdef DEBUG
    for(i=0;i < HIGHSCORES_SIZE;i++)
        printf("%d ",highscores[i]);
    printf("\n");
    //#endif // DEBUG

    close(scorefd);
}

void gameinit1()// Initalizing some parameters for the game
{
    unsigned int data;
    int res;
    //strcpy(buttonimg,"StartButton.png");//Main menu start button image

    scorefd = open("highscore",O_RDWR | O_CREAT,0755);

    if(scorefd < 0)
    {
        printf("Score open error.\n");
        return;
    }

    res = read(scorefd, &data, sizeof(data));

    if(res == -1)
    {
        printf("Score read error.\n");
        return;
    }

    if(res == 0)
        highscore = 0;
    else
        highscore = data;

    close(scorefd);

    readScores();
        //highscore = atoi(buf);
    #ifdef DEBUG
    printf("Highscore read: %d\n",highscore);
    #endif // DEBUG
}

void scoreSave()// Saves your highscore for later::OBSOLETE
{
    char buf[10];
    ftruncate(scorefd, 0);
    sprintf(buf,"%d",highscore);
    if(write(scorefd,buf,strlen(buf)) == -1)
    {
        printf("Score write error.\n");
        return;
    }
    close(scorefd);
}

void scoreSave1()// Saves your highscore for later
{
    //char buf[10];
    unsigned int data;
    int res;
    data = highscore;

    unlink("highscore");

    scorefd = open("highscore",O_RDWR | O_CREAT,0755);

    //unlink("highscore");
    //ftruncate(scorefd, 0);

    //printf(buf,"%d",highscore);
    res = write(scorefd,&data,sizeof(data));

    if( res == -1)
    {
        printf("Score write error.\n");
        return;
    }
    close(scorefd);
}

void setAudio()
{
    int i;
    int error;

    audioinit();

    //Generate buffers and sources
    if(audioinitsrc(NUM_BUFFERS,NUM_SOURCES,buffers,sources) == -1)
        exit(1);
    if(audioinitsrc(1,1,&mainscreenbuffer,&mainscreensrc) == -1)
        exit(1);
    if(audioinitsrc(1,1,&playbuffer,&playsrc) == -1)
        exit(1);
    if(audioinitsrc(1,1,&gameoverbuffer,&gameoversrc) == -1)
        exit(1);

    //Set the keypress buffers
    char files[9][100] = {
    "audiofiles/c4.wav",
    "audiofiles/d4.wav",
    "audiofiles/dSharp4.wav",
    "audiofiles/f4.wav",
    "audiofiles/g4.wav",
    "audiofiles/a4.wav",
    "audiofiles/aSharp4.wav",
    "audiofiles/c5.wav",
    "audiofiles/d5.wav",
    };

    for(i=0;i<9;i++)
    {
        if(attachAudio(&sources[i],&buffers[i],files[i]) == -1)
        {
            alDeleteBuffers(NUM_BUFFERS, buffers);
            exit(1);
        }
    }

    //Set the Main Menu and Play screen buffers
    if(attachAudio(&mainscreensrc,&mainscreenbuffer,"audiofiles/HideFromAll.wav") == -1)
    {
        alDeleteBuffers(NUM_BUFFERS, buffers);
        alDeleteBuffers(1, &mainscreenbuffer);
        exit(1);
    }

    if(attachAudio(&gameoversrc,&gameoverbuffer,"audiofiles/HideFromAllEnd2.wav") == -1)
    {
        alDeleteBuffers(NUM_BUFFERS, buffers);
        alDeleteBuffers(1, &mainscreenbuffer);
        exit(1);
    }
    /*alSourcei(gameoversrc, AL_BUFFER, mainscreenbuffer);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alSourcei %s : %x\n","gameover",error);
      _exit(1);
    }*/

    if(attachAudio(&playsrc,&playbuffer,"audiofiles/HideFromAllBG.wav") == -1)
    {
        alDeleteBuffers(NUM_BUFFERS, buffers);
        alDeleteBuffers(1, &mainscreenbuffer);
        alDeleteBuffers(1, &playbuffer);
        exit(1);
    }

    //gameoversrc = mainscreensrc;

    /*alSourcei(gameoversrc, AL_BUFFER, mainscreenbuffer);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alSourcei %s : %x\n","gameover",error);
      _exit(1);
    }*/

    alSourcei(mainscreensrc,AL_LOOPING, 1);
    alSourcei(playsrc, AL_LOOPING, 1);
    alSourcei(gameoversrc, AL_LOOPING, 1);

    //setSourcestuff(0,0,0,mainscreensrc,AL_POSITION);
    //setListenerstuff(1000,300,0,AL_POSITION);
}

void switchtoPlay()
{
    playinit();
    gamestate = PLAYING;
    alSourceStop(mainscreensrc);
    alSourcePlay(playsrc);
    glutDisplayFunc(playGame);
}

void switchtoGameOver()
{
    gamestate = GAMEOVER;
    int i;
    for(i=0;i<NUM_SOURCES;i++)
        alSourceStop(sources[i]);

    alSourceStop(playsrc);
    alSourcePlay(gameoversrc);
    glutDisplayFunc(gameOverScreen);
}

void myReshape(int w, int h)
{
    //reshapex = (float)w/WIDTH;
    //reshapey = (float)h/HEIGHT;
    //glClearColor(0.5,0.5,0.5,1);
    //glutReshapeWindow(w,h);
    width = w;
    height = h;
}

int main(int argc,char *argv[])
{
    #ifdef DEBUG
    //Initialize log file.
    fd = open("log.txt",O_WRONLY | O_CREAT | O_TRUNC,0755);
    if(fd < 0)
    {
        printf("Open error.\n");
        return 1;
    }
    #endif // DEBUG

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIDTH,HEIGHT);
    window = glutCreateWindow("Audio Basketball -- By TheDarkMiko");

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    myinit();
    textInit();
    gameinit1();

    setAudio();

    gamestate = MAINMENU;
    glutDisplayFunc(mainmenu);
    glutKeyboardUpFunc(upkeys);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mouseOverButtons);
    glutKeyboardFunc(keys);
    glutIdleFunc(myidle);
    glutReshapeFunc(myReshape);
    alSourcePlay(mainscreensrc);
    glutMainLoop();

    return 0;
}
