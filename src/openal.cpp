enum sound_ {
    Sound_Null,
    Music_Intro,
    Music_Main,
    Sound_Mwak,
    Sound_Test,
    Sound_Pew,
    Sound_AsteroidExplode,
    Sound_Count,
};

struct loaded_sound {
    sound_ Id;
    ALuint Buffer;
    ALuint Source;
    u8 *Data;
    u8 *Size;
};

struct sound {
    ALuint MusicSource;
    ALuint SFXSource;

    loaded_sound Sounds[Sound_Count];
    static void Play(sound_ Sound);
    static void Loop(sound_ Sound);
};

sound Sound = {};

void sound::
Play(sound_ SoundId)
{
    loaded_sound PlayedSound = Sound.Sounds[SoundId];
    // alSourcei(PlayedSound.Source, AL_BUFFER, PlayedSound.Buffer);

    ALint State;
    alGetSourcei(PlayedSound.Source, AL_SOURCE_STATE, &State);

    if (State != AL_PLAYING) {
        alSourcei(PlayedSound.Source, AL_BUFFER, PlayedSound.Buffer);
        alSourcePlay(PlayedSound.Source);
    } else {
        alSourceStop(PlayedSound.Source);
        alSourcei(PlayedSound.Source, AL_BUFFER, PlayedSound.Buffer);
    }

    alSourcePlay(PlayedSound.Source);
}

void sound::
Loop(sound_ SoundId)
{
    loaded_sound PlayedSound = Sound.Sounds[SoundId];
    ALint State;
    alGetSourcei(PlayedSound.Source, AL_SOURCE_STATE, &State);

    if (State != AL_PLAYING) {
        alSourcei(PlayedSound.Source, AL_BUFFER, PlayedSound.Buffer);
        alSourcei(PlayedSound.Source, AL_LOOPING, AL_TRUE);
        alSourcePlay(PlayedSound.Source);
    }
}

loaded_sound
LoadSound(ALuint Source, char *Filename)
{
    loaded_sound Sound = {};
    Sound.Source = Source;

    alGenBuffers(1, &Sound.Buffer);

    short *Buffer = (short *)malloc(1024 * 1024 * 100);
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

        if (Vorbis->channels == 2) {
            Read = stb_vorbis_get_samples_short_interleaved(Vorbis, Vorbis->channels, To, Vorbis->sample_rate);
            Read *= Vorbis->channels;
        } else {
            Read = stb_vorbis_get_samples_short(Vorbis, Vorbis->channels, &To, Vorbis->sample_rate);
        }

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

    alGenSources(1, &Sound.MusicSource);
    alGenSources(1, &Sound.SFXSource);

    // if (alIsExtensionPresent("EAX2.0")) {

    // }

    // Sound.Sounds[Music_Intro] = LoadSound(Sound.MusicSource, "audio/intro.ogg");
    // Sound.Sounds[Music_Main] = LoadSound(Sound.MusicSource, "audio/music.ogg");
    // Sound.Sounds[Sound_AsteroidExplode] = LoadSound(Sound.SFXSource, "audio/boom_1.ogg");
    // Sound.Sounds[Sound_Pew] = LoadSound(Sound.SFXSource, "audio/pew_laser.ogg");
    // alSourcei(Sound.MusicSource, AL_BUFFER, Sound.Sounds[Music_Main].Buffer);
    // alSourcei(Sound.MusicSource, AL_BUFFER, Sound.Sounds[Music_Intro].Buffer);
    // alSourceQueueBuffers(PlayedSound.Source, 1, &PlayedSound.Buffer);

    // ALint State;
    // alGetSourcei(PlayedSound.Source, AL_SOURCE_STATE, &State);

    // if (State != AL_PLAYING) {
        // alSourcePlay(Sound.MusicSource);
    // }

    // Sound.Sounds[Sound_Pew] = LoadSound("audio/pew_laser.ogg");
    // Sound.Sounds[Sound_Pew].SOur
    // Sound.Sounds[Sound_AsteroidExplode] = LoadSound("audio/boom_1.ogg");
    // Sound.Sounds[Sound_Pew] = LoadSound("audio/pew_laser.ogg");
    // Sound.Sounds[Sound_Pew].SOur
    // Sound.Sounds[Sound_AsteroidExplode] = LoadSound("audio/boom_1.ogg");

}

// Sound::Play(Sound_Pew);