#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>

int error;

int audioinit(int NUM_BUFFERS, int NUM_SOURCES, ALuint *buffers, ALuint *sources)
{
    alutInit(0, NULL);
    alGetError();

    // Create the buffers
    alGenBuffers(NUM_BUFFERS, buffers);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alGenBuffers : %d", error);
      return -1;
    }

    // Generate the sources
    alGenSources(NUM_SOURCES, sources);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alGenSources : %d", error);
      return -1;
    }
    return 0;
}

int attachAudio(ALuint *source, ALuint *buffer, const char *file)//Load an audio file into the specified buffer and attach it to the specified source
{
    *buffer = alutCreateBufferFromFile(file);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alutLoadWAVFile %s : %d",file,error);
      // Delete Buffers
      //alDeleteBuffers(1, buffer);
      return -1;
    }

    alSourcei(*source, AL_BUFFER, *buffer);
    if ((error = alGetError()) != AL_NO_ERROR)
    {
      printf("alSourcei %s : %d",file,error);
      return -1;
    }
    return 0;
}

void setSourcestuff(ALfloat x,ALfloat y,ALfloat z, ALuint sid,ALenum param)
{
    ALfloat sourcevec[] = {x,y,z};
    alSourcefv (sid, param, sourcevec);
}

void setListenerstuff(ALfloat x,ALfloat y,ALfloat z,ALenum param)
{
    ALfloat listenvec[] = {x,y,z};
    alListenerfv(param,listenvec);
}
