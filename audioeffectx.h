
// =====================================================================
//
// Dummy VST 2.4 SDK
// This file acts as the VST2.4 SDK and allows compilation of effects
// that are derived from AudioEffectX
//
//s =====================================================================

#pragma once

#ifndef __audioeffect__
#define __audioeffect__ 1
#endif

using VstInt32 = int32_t;
using audioMasterCallback = void*;

constexpr int VST_CHAR_BUFFER_SIZE = 32;
constexpr int kVstMaxProgNameLen =   VST_CHAR_BUFFER_SIZE;
constexpr int kVstMaxParamStrLen =   VST_CHAR_BUFFER_SIZE;
constexpr int kVstMaxProductStrLen = VST_CHAR_BUFFER_SIZE;
constexpr int kVstMaxVendorStrLen =  VST_CHAR_BUFFER_SIZE;
#define vst_strncpy memcpy

#define DECLARE_VST_DEPRECATED(x) x

template <typename T> void value2string(T value, char* buffer, size_t length)
{
	auto s = std::to_string(value);
	memset(buffer, 0, length);
	memcpy(s.data(), buffer, jmin(length, s.length()));
}

#define float2string value2string<float>
#define int2string value2string<int>

enum kVstEventType
{
	kVstMidiType
};

struct VstMidiEvent
{
	kVstEventType type = kVstMidiType;
	int deltaFrames = 0;
	char midiData[3];
};

struct VstEvents
{
	int numEvents = 0;
	VstMidiEvent** events = nullptr;
};

enum kVstPinFlags
{
	kVstPinIsActive = 1,
	kVstPinIsStereo,
	numkVstPinFlags
};

struct VstPinProperties
{
	int flags = 0;

	char shortLabel[VST_CHAR_BUFFER_SIZE];
	char label[VST_CHAR_BUFFER_SIZE];
};

enum VstPlugCategory
{
	kPlugCategEffect,
	numCategories
};

class AudioEffect
{
public:

	virtual ~AudioEffect() {};
};

class AudioEffectX: public AudioEffect
{
public:

	AudioEffectX(audioMasterCallback callback, int numPrograms, int numParameters_):
	  numParameters(numParameters_)
	{};

	virtual ~AudioEffectX() {};

	virtual void process(float **inputs, float **outputs, VstInt32 sampleFrames) {};
	virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {};
	virtual VstInt32 processEvents(VstEvents* events) { return 1;}
	virtual void resume() {};
	virtual void suspend() {};

	virtual void setProgram(VstInt32 program) {};
	virtual void setProgramName(char *name) {};
	virtual void getProgramName(char *name) {};
	virtual bool getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text) { return false; }
	virtual bool copyProgram(VstInt32 destination) { return false; }

	virtual void setParameter(VstInt32 index, float value) {}
	virtual float getParameter(VstInt32 index) { return 0.0f; }
	virtual void getParameterLabel(VstInt32 index, char *label) {};
	virtual void getParameterDisplay(VstInt32 index, char *text) {};
	virtual void getParameterName(VstInt32 index, char *text) {};

	virtual VstInt32 canDo(char* text) { return false; };
	virtual bool getEffectName(char* name) { return false; };
	virtual bool getVendorString(char* text) { return false; };
	virtual bool getProductString(char* text) { return false; };
	virtual VstInt32 getVendorVersion() { return 0; };
	virtual VstPlugCategory getPlugCategory() { return kPlugCategEffect; };
	virtual bool getInputProperties(VstInt32 index, VstPinProperties* properties) { return false; };
	virtual bool getOutputProperties(VstInt32 index, VstPinProperties* properties) { return false; }; 
	
	int numParameters = 0;
	int numPrograms = 0;
	int numInputs = 0;
	int numOutputs = 0;
	bool isReplacing = true;
	bool synth = false;
	double sampleRate = 0.0;

protected:

	int curProgram = 0;

	void setSampleRate(double nr) { sampleRate = nr; }

	void setBlockSize(int) {};

	double getSampleRate() const { return sampleRate; }
	void setNumInputs(int numInputs_) { numInputs = numInputs_; }
    void setNumOutputs(int numOutputs_) { numOutputs = numOutputs_; }
	void canProcessReplacing(bool isReplacing_=true) { isReplacing = isReplacing_; }
	void canDoubleReplacing() {}
	void programsAreChunks(bool) {};
    void isSynth(bool synth_=true) { synth = synth_; }
	void setUniqueID(int) {};
	void canMono(bool=true) {};
	void wantEvents(bool=true) {};
};

