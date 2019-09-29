enum sound_ {
    Sound_Null,
    Sound_BGMusic,
    Sound_Mwak,
    Sound_Test,
    Sound_Pew,
    Sound_AsteroidExplode,
    Sound_Count,
};

struct loaded_sound {
    sound_ Id;
    ALuint Buffer;
    u8 *Data;
    u8 *Size;
};

struct sound {
    ALuint MainSource;
    loaded_sound Sounds[Sound_Count];
    static void Play(sound_ Sound);
};

sound Sound = {};

void sound::
Play(sound_ SoundId)
{
    loaded_sound PlayedSound = Sound.Sounds[SoundId];
    // alSourceQueueBuffers(Sound.MainSource, 1, &PlayedSound.Buffer);
    alSourcei(Sound.MainSource, AL_BUFFER, PlayedSound.Buffer);
    alSourcePlay(Sound.MainSource);
}

loaded_sound
LoadSound(char *Filename)
{
    loaded_sound Sound = {};

    alGenBuffers(1, &Sound.Buffer);

    short *Buffer = (short *)malloc(1024 * 1024 * 10);
    int Err;
    stb_vorbis *Vorbis = stb_vorbis_open_filename(Filename, &Err, 0);

    ALenum Format = AL_FORMAT_MONO16;
    if (Vorbis->channels == 2) {
        Format = AL_FORMAT_STEREO16;
    }

    int Samples = 0;
    int Read = 0;
    do {
        short *To = Buffer + Samples;
        Read = stb_vorbis_get_samples_short(Vorbis, 1, &To, Vorbis->sample_rate);
        Samples += Read;
    } while (Read);

    alBufferData(Sound.Buffer, Format, Buffer, Samples * sizeof(short), Vorbis->sample_rate);

    free(Buffer);

    return Sound;
}

void
InitOpenal()
{
    ALCcontext *Context;
    ALuint MainBuffer;
    ALenum Error;

    // Initialization
    ALCdevice *Device = alcOpenDevice(0);

    if (Device) {
        Context = alcCreateContext(Device, 0);
        alcMakeContextCurrent(Context);
    }

    alGenSources(1, &Sound.MainSource);

    // if (alIsExtensionPresent("EAX2.0")) {

    // }

    Sound.Sounds[Sound_Pew] = LoadSound("audio/pew_laser.ogg");
    Sound.Sounds[Sound_AsteroidExplode] = LoadSound("audio/boom_1.ogg");

}

// Sound::Play(Sound_Pew);