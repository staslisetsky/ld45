enum sound_ {
    Sound_Null,
    Sound_Mwak,
    Sound_Test,
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
    static void Play();
};

sound Sound = {};

void sound::
Play()
{
    alSourcePlay(Sound.MainSource);
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

    // if (alIsExtensionPresent("EAX2.0")) {

    // }

    alGenSources(1, &Sound.MainSource);
    alGenBuffers(1, &Sound.Sounds[Sound_Mwak].Buffer);

    short *Buffer = (short *)malloc(1024 * 1024);

    int Err;
    stb_vorbis *Pew = stb_vorbis_open_filename("audio/pew_laser.ogg", &Err, 0);

    int Samples = 0;
    int Read = 0;
    do {
        short *To = Buffer + Samples;
        Read = stb_vorbis_get_samples_short(Pew, 1, &To, 44100);
        Samples += Read;
    } while (Read);

    alBufferData(Sound.Sounds[Sound_Mwak].Buffer, AL_FORMAT_MONO16, Buffer, 44100 * sizeof(short), 44100);
    alSourcei(Sound.MainSource, AL_BUFFER, Sound.Sounds[Sound_Mwak].Buffer);
}

// Sound::Play(Sound_Pew);