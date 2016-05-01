#ifndef AUDIOSTUFF_H_INCLUDED
#define AUDIOSTUFF_H_INCLUDED

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>


extern int audioinit(int NUM_BUFFERS, int NUM_SOURCES, ALuint *buffers, ALuint *sources);
extern int attachAudio(ALuint *source, ALuint *buffer, const char *file);
extern void setSourcestuff(ALfloat x,ALfloat y,ALfloat z, ALuint sid,ALenum param);
extern void setListenerstuff(ALfloat x,ALfloat y,ALfloat z,ALenum param);


#endif // AUDIOSTUFF_H_INCLUDED
