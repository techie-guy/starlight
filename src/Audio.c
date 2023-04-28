/*

#include <AL/al.h>
#include <AL/alc.h>
#include <dr_wav.h>

	ALCdevice* device = alcOpenDevice(NULL);
	ALCcontext* context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	unsigned int channels;
    unsigned int sample_rate;
    drwav_uint64 total_samples;
    short* data = drwav_open_file_and_read_pcm_frames_s16("assets/sounds/sound1.wav", &channels, &sample_rate, &total_samples, NULL);

	ALuint buffer;
    alGenBuffers(1, &buffer);

	ALenum al_format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    alBufferData(buffer, al_format, data, total_samples * channels * sizeof(short), sample_rate);

    ALuint source;
    alGenSources(1, &source);

	alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);

	ALint state;
    do
	{
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    } while (state == AL_PLAYING);

	alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    drwav_free(data, NULL);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

*/
