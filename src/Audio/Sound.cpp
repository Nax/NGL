/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    Sound.cpp                                      oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <vector>
#include <chrono>
#include <LumsInclude/Audio/Sound.hpp>
#include <LumsInclude/OperatingSystem.hpp>

using namespace lm;

static char soundManager[sizeof(SoundManager)];
static SoundManager* soundManagerPtr = nullptr;

static ALuint   sourceMusic;
static ALuint   sourcesFx[64];
static int      nextSound = 0;

Sound::Sound()
: _file(0)
, _format(0)
, _sampleRate(0)
, _read(0)
, _type(Type::None)
, _state(SoundState::Pending)
, _run(true)
, _id(_nbs)
{
    _nbs++;
    if (!soundManagerPtr)
    {
        soundManagerPtr = new(soundManager) SoundManager();
        alGenSources(1, &sourceMusic);
        alGenSources(64, sourcesFx);
    }
}

void
Sound::play(float x, float y, float z)
{
    if (_type == Type::None)
    {
        std::cerr << "No file loaded !" << std::endl;
        return;
    }
    _state = SoundState::Playing;
    if (_type == Type::Music)
    {
        _run = true;
        soundManagerPtr->addMusic(this);
    }
    else
        soundManagerPtr->addSound(this);
}

void
Sound::pause()
{
    _state = SoundState::Paused;
    if (_type == Type::Music)
    {
        soundManagerPtr->pauseMusic();
        std::unique_lock<std::mutex> lck(_mtx);
        _cv.notify_all();
    }
    else 
        soundManagerPtr->pauseSound(*this);
}

void
Sound::stop()
{
    _state = SoundState::Finished;
    if (_type == Type::Music)
    {
        soundManagerPtr->stopMusic();
        _run = false;
    }
    else 
        soundManagerPtr->stopSound(*this);
}

ALuint
Sound::playSound()
{
    if (_type == Type::FX)
        return (playFX());
    else
        return (playMusic());
}


ALuint
Sound::playMusic()
{
    ALuint  source;
    alGenSources(1, &source);
    std::thread    t1([this](ALuint source)
    {
        playStream(source);
    }, source);
    t1.detach();
    return (source);
}

void
Sound::playStream(ALuint source)
{
    ALuint  buffers[3];
    ALint   nbProcessed;
    ALint   status;
    int     index = 0;
    std::chrono::milliseconds dura(400);
    std::unique_lock<std::mutex> lck(_mtx);

    alGenBuffers(3, buffers);
    alSource3f(source, AL_POSITION, _x, _y, _z);
    readOGG(buffers[0], 44100);
    readOGG(buffers[1], 44100);
    readOGG(buffers[2], 44100);
    alSourceQueueBuffers(source, 3, buffers);
    alSourcePlay(source);
    while (_run)
    {
        while (_state == SoundState::Paused) _cv.wait(lck);
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &nbProcessed);
        if (nbProcessed)
        {
            alSourceUnqueueBuffers(source, 1, &buffers[index]);
            readOGG(buffers[index], 44100);
            if (!_read)
            {
                ov_pcm_seek(&_stream, 0);
                readOGG(buffers[index], 44100);
            }
            alSourceQueueBuffers(source, 1, &buffers[index]);
            index = (index == 2) ? 0 : index++;
        }
        std::this_thread::sleep_for(dura);
    }
    alSourceUnqueueBuffers(source, 1, &buffers[0]);
    alSourceUnqueueBuffers(source, 1, &buffers[1]);
    alSourceUnqueueBuffers(source, 1, &buffers[2]);
    alDeleteBuffers(3, buffers);
    alDeleteSources(1, &source);
    ov_pcm_seek(&_stream, 0);
}

ALuint
Sound::playFX()
{
    ALuint  source;
    ALuint  buffer;

    alGenBuffers(1, &buffer);
    alGenSources(1, &source);
    alSource3f(source, AL_POSITION, _x, _y, _z);
    std::vector<char>   bufferData;
    char array[BUFFER_SIZE];
    ALsizei bytes;
    ALint state;
    do
    {
        bytes = ov_read(&_stream, array, BUFFER_SIZE, 0, 2, 1, NULL);
        bufferData.insert(bufferData.end(), array, array + bytes);
    } while (bytes > 0);
    alBufferData(buffer, _format, &bufferData[0], static_cast < ALsizei > (bufferData.size()), _sampleRate);
    alSourcei(source, AL_BUFFER, buffer);
    ov_pcm_seek(&_stream, 0);
    alSourcePlay(source);
    return source;
}


void
Sound::loadFile(const std::string& name, Type type, bool resource)
{
    _type = type;
    typedef void (Sound::*sndptr_t)(std::string, bool);
    static const std::map<std::string, sndptr_t> extFuncs = {
        {"ogg", &Sound::loadFileOGG}
    };

    std::string ext = name.substr(name.find_last_of('.') + 1);

    if (extFuncs.find(ext) != extFuncs.end())
        (this->*(extFuncs.at(ext)))(name, resource);
}

void
Sound::loadFileOGG(const std::string name, bool resource)
{
    std::string path = resource ? resourcePath() + name : name;
    _file = fopen(path.c_str(), "rb");
    if (!_file)
    {
        std::cerr << "File not found !" << std::endl;
        return;
    }
    if (ov_open_callbacks(_file, &_stream, 0, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        fclose(_file);
        _file = 0;
        std::cerr << "File not found !" << std::endl;
        return;
    }
    vorbis_info* infos = ov_info(&_stream, -1);
    _format = infos->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    _sampleRate = infos->rate;
}

void
Sound::readOGG(ALuint& buffer, ALsizei nbSamples)
{
    std::vector<ALshort> samples(nbSamples);
    ALsizei totalRead  = 0;
    ALsizei totalSize  = nbSamples * sizeof(ALshort);
    char*   samplesPtr = reinterpret_cast<char*>(&samples[0]);

    while (totalRead < totalSize)
    {
        _read = ov_read(&_stream, samplesPtr + totalRead, totalSize - totalRead, 0, 2, 1, NULL);
        if (_read > 0)
            totalRead += _read;
        else    
            break;
    }
    if (totalRead > 0)
        alBufferData(buffer, _format, &samples[0], totalRead, _sampleRate);
}

Sound::~Sound()
{
    if (!_file)
        return;
    ov_clear(&_stream);
    fclose(_file);
}

size_t
Sound::_nbs = 0;