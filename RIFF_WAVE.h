#ifndef RIFF_WAVE_H_
    #define RIFF_WAVE_H_

#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>

const int i = 1;
#define Native_Endianness ((*(char*)&i) == 0)
#define Big_Endian 1
#define Little_Endian 0

class WAVE {
    unsigned short NumChannels;
    unsigned int SampleRate, SampleSize;
    std::vector<char> data;
    template<typename T>
    void write(std::fstream& f, T data) {
        if(Native_Endianness == Little_Endian) {
            f.write((char*)&data, sizeof(T));
        } else {
            char* buf = (char*)&data;
            for(int i = sizeof(T) - 1; i >= 0; i--) {
                f.put(buf[i]);
            }
        }
    }
    template<typename T>
    void read(std::fstream& f, T& data) {
        if(Native_Endianness == Little_Endian) {
            f.read((char*)&data, sizeof(T));
        } else {
            char* buf = (char*)&data;
            for(int i = sizeof(T) - 1; i >= 0; i--) {
                f.get(buf[i]);
            }
        }
    }

    void readSample(std::fstream& f, char& data) {
        if(Native_Endianness == Little_Endian) {
            f.read(&data, SampleSize);
        } else {
            char* buf = (char*)&data;
            for(int i = SampleSize - 1; i >= 0; i--) {
                f.get(buf[i]);
            }
        }
    }
public:

    WAVE(const char* filename) {
        load(filename);
    }

    WAVE() {}

    WAVE(unsigned int NumChannels, unsigned int SampleRate, unsigned int SampleSize, unsigned int SampleCount)
    :   NumChannels(NumChannels), SampleRate(SampleRate), SampleSize(SampleSize), data(SampleCount * SampleSize * NumChannels)
    {}

    void load(const char* filename) {
        std::fstream f(filename, std::ios_base::in | std::ios_base::binary);
        if(f) {
            unsigned int size;
            f.imbue(std::locale::classic());
            f.seekg(22, f.beg);
            read<unsigned short>(f, NumChannels);
            read<unsigned int>(f, SampleRate);
            f.seekg(34, f.beg);
            unsigned short BitsPerSample;
            read<unsigned short>(f, BitsPerSample);
            SampleSize = BitsPerSample / 8;
            f.seekg(40, f.beg);
            read<unsigned int>(f, size);
            data.resize(size);
            for(int i = 0; i < size / SampleSize; i++) {
                readSample(f, data[i * SampleSize]);
            }
            f.close();
        }
    }

    std::string about() {
        std::stringstream ss;
        ss << "Channels: " << NumChannels << std::endl << "Sample Rate: " << SampleRate << std::endl << "Sample Size: " << SampleSize << std::endl << "Size: " << data.size() << std::endl;
        return ss.str();
    }

    void save(const char* filename) {
        std::fstream f(filename, std::ios_base::out | std::ios_base::binary);
        f.imbue(std::locale::classic());
        f.write("RIFF", 4);
        write<unsigned int>(f, 36 + data.size());
        f.write("WAVE", 4);
        f.write("fmt ", 4);
        write<unsigned int>(f, 16);
        write<unsigned short>(f, 1);
        write<unsigned short>(f, NumChannels);
        write<unsigned int>(f, SampleRate);
        write<unsigned int>(f, SampleRate * NumChannels * SampleSize);
        write<unsigned short>(f, NumChannels * SampleSize);
        write<unsigned short>(f, SampleSize * 8);
        f.write("data", 4);
        write<unsigned int>(f, data.size());
        for(int i = 0; i < data.size(); i++) {
            f.put(data[i]);
        }
        f.close();
    }

    template<typename Type>
    void append(const Type& t) {
        if(Native_Endianness == Little_Endian) {
            if constexpr(sizeof(Type) == 1) {
                data.push_back((char)t);
            } else {
                data.reserve(data.size() + sizeof(Type));
                ((Type*)&data[data.size()]) = t;
            }
        } else {
            if constexpr(sizeof(Type) == 1) {
                data.push_back((char)t);
            } else {
                char* buf = (char*)t;
                data.reserve(data.size() + sizeof(Type));
                for(int i = sizeof(Type) - 1; i >= 0; i--) {
                    data.push_back(buf[i]);
                }
            }
        }
    }

    void setData(std::vector<char> const& data) {
        this->data = data;
    }

    std::vector<char>& getData() {
        return data;
    }

    char& operator[](size_t idx) {
        return data[idx];
    }

    std::vector<char> toCharVector() {
        std::vector<char> retVec;
        retVec.reserve(44 + data.size());
        auto ps = [&](unsigned short s) {
            char* buf = (char*)&s;
            if(Native_Endianness == Little_Endian) {
                retVec.push_back(buf[0]);
                retVec.push_back(buf[1]);
            } else {
                retVec.push_back(buf[1]);
                retVec.push_back(buf[0]);
            }
        };
        auto pi = [&](unsigned int i) {
            char* buf = (char*)&i;
            if(Native_Endianness == Little_Endian) {
                retVec.push_back(buf[0]);
                retVec.push_back(buf[1]);
                retVec.push_back(buf[2]);
                retVec.push_back(buf[3]);
            } else {
                retVec.push_back(buf[3]);
                retVec.push_back(buf[2]);
                retVec.push_back(buf[1]);
                retVec.push_back(buf[0]);
            }
        };
        pi(0x46464952); //"RIFF" in little endian form (it will be flip to big endian when written)
        pi(36 + data.size());
        pi(0x45564157); //"WAVE" in little endian form (it will be flip to big endian when written)
        pi(0x20746d66); //"fmt " in little endian form (it will be flip to big endian when written)
        pi(16);
        ps(1);
        ps(NumChannels);
        pi(SampleRate);
        pi(NumChannels * SampleRate * SampleSize);
        ps(NumChannels * SampleSize);
        ps(SampleSize * 8);
        pi(0x61746164); //"data" in little endian form (it will be flip to big endian when written)
        pi(data.size());
        for(char& c : data) {
            retVec.push_back(c);
        }
        return retVec;
    }

};

#endif // RIFF_WAVE_H_
