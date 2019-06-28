#ifndef INCLUDED_SOUND_DEVICE_
#define INCLUDED_SOUND_DEVICE_

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif __linux
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <vector>
#include <variant>

#include <iostream>
#include <string>
#include <fstream>

#include <chrono>


// using this class:
// after invoking openAL function, request the error status
// to see if something went wrong. how we deal with something
// going wrong is something we have to come up with later.

//TODO: pool pattern.
// maximum of (32) sound sources. at startup, create 32 ALsources.
// ask pool for a source.
// if no free sources, stop longest playing(????)
// classify pool into groupes:
// 1-8 unit sfx
// 8-12 explosions
// you name it.

struct Wav_Header
{
    // RIFF Chunk Descriptor
    uint8_t         RIFF[4];           // RIFF Header Magic header
    uint32_t        chunk_size;        // RIFF Chunk Size
    uint8_t         WAVE[4];           // WAVE Header
    // "fmt" sub-chunk
    uint8_t         fmt[4];            // FMT header
    uint32_t        sub_chunk_1_size;  // Size of the fmt chunk
    uint16_t        audio_format;      // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        num_channels;       // Number of channels 1=Mono 2=Sterio
    uint32_t        samples_per_sec;   // Sampling Frequency in Hz
    uint32_t        bytes_per_sec;     // bytes per second
    uint16_t        block_align;       // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bits_per_sample;   // Number of bits per sample
    // "data" sub-chunk
    uint8_t         sub_chunk_2_ID[4]; // "data"  string
    uint32_t        sub_chunk_2_size;  // Sampled data length
};

struct Wav_File
{
    std::string filename;
    Wav_Header header;
    std::vector<std::vector<uint8_t>> data; // maybe multiple channels?
    ALenum format; //AL_FORMAT_MONO8, STEREO8, MONO16, STEREO16
    float duration; // miliseconds?
};


class Sound_Device
{
    ALCdevice *m_device;
    ALCcontext *m_context; // multiple contexts?
    ALenum m_error;


    int32_t m_num_buffers; // create a fixed amount of buffers?
    int32_t m_num_sound_sources;
    std::vector<ALuint> m_buffers;
    std::vector<ALuint> m_sound_sources; // std::array<ALuint, ????> 
    // std::vector<Wav_File> m_wav_files;
	
    // horrible idea, but whatever.
	bool m_EAX;

	public:
		Sound_Device();
        ~Sound_Device();

		void play_sound(const char* filename); // provide overloads for these?
        void play_sound(uint32_t sound_source);
		void play_music(const char* filename);
        uint32_t data_to_buffer(const Wav_File& wav_file); // this is unclear! maybe typedef uint32_t to sound_source?

    private:
        ALuint find_free_source(); //uint32_t
        ALuint find_free_buffer(); //uint32_t
};


namespace msound
{
    Wav_File load_wav_file(const char* filename); //Wav_File


    // int get_file_size(FILE* inFile);
};

#endif


