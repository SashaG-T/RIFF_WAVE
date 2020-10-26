# About
C++ Header only RIFF WAVE file loader.

I needed a simple WAVE file loader, specifically RIFF WAVE, so I built this.
It's a header only file. I figured I'd atleast share it with others. I used [this site](http://soundfile.sapp.org/doc/WaveFormat/) to get the file specification details.

# Usage
Just include it in your project how you'd include any header file.

## Constructors:
* `WAVE()`
  * Should call load(const char\* filename) if this is used!
* `WAVE(const char\* filename)`
* `WAVE(unsigned int NumChannels, unsigned int SampleRate, unsigned int SampleSize, unsigned int SampleCount)`
  * NumChannels -> 1 == Mono, 2 == Stereo, etc.
  * SampleRate -> Samples per second, 44100, 8000, etc.
  * SampleSize -> Size of each sample in bytes (Bit Depth of WAV file divided by 8)
  * SampleCount -> How many samples to reserve in memory.

## Member Functions:
* `void load(const char\* filename)`
* `std::string about()`
  * Returns an string with details about the object.
* `void save(const char\* filename)`
* `template<typename Type> void append(const Type& t)` \[may be bugged\]
  * Append samples to the audio data. This is a template so you can append whatever you are using to store your samples. (Have a bit depth or 16? Then push a short. 8? Then char, Stereo with 32-bit depth? Then append two ints!)
* `void setData(std::vector<char> const& data)`
  * Set the audio data using another std::vector<char>.
* `char& operator[](size_t idx)`
  * Returns the char at index idx in audio data. (Not as fancy as append but useful for copying audio data.
* `std::vector<char> toCharVector()`
  * This returns a vector that contains the WAV file as it would appear on disk (RIFF header and all). This is useful for when you want to load from memory and not from disk.

