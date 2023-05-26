/** This file will create a wrapper template around a VST2.4 effect
 * 
 *  This is a ultimatively hacky approach that involves a reverse engineered dummy API
 *  that simulates the VST2.4 SDK which might not work with all VST classes.
 * 
 *  In order to use this, follow these steps:
 *  
 * 	1. Copy this file and the "audioeffectx.h" file into the "ThirdParty/src" subfolder
 *  2. Copy the VST source code files into any subdirectory of the "ThirdParty/src" subfolder
 *     Be aware that you must not include the editor source files (and you might have to delete
 *     a few lines in the effect file that spawns the editor class, but you can just follow the
 *     compile errors here).
 *  3. Create a C++ template using the File menu. Enter the exact ID you want it to have in the
 *     end. Then open the ThirdParty folder and open the file in a text editor. Grab everything in
 *     there and delete it - we'll replace the node class definition with a wrapper boilerplate!
 *  4. Include the .cpp files from the effect (they should include their headers automatically).
 *     If the VST effect implementation is split into more CPP files, just include them one after
 *     another.
 *     You will then run into a compile error that `#include "audioeffectx.h"` can't be found.
 *     In order to solve this, just point it to our dummy file one directory above:
 *     
 *     `#include "../audioeffectx.h"`
 * 
 *  5. Now you need to create a metadata class that provides the node ID, the channel amount
 *     and some other properties. Define a class and derive it from DefaultVstConfigClass below
 *     (take a look at the class documentation for an example)
 *  6. The last thing you need to do is to add the boilerplate code that creates the wrapper.
 *     (take a look at the bottom of this file).
 *  7. That's it. You can now just reexport the DspNetwork and if everything goes smooth, you
 *     have the VST effect inside scriptnode (or hardcoded FX) ready to be used! Be aware that
 *     all VST parameters are normalised (from 0 to 1).
 *
 *  If you find a VST that doesn't compile, send it over and I'll fix the dummy API stub.
 * 
*/

#include <JuceHeader.h>

#include "audioeffectx.h"

#pragma once

namespace vst2scriptnode
{

using namespace juce;
using namespace hise;
using namespace scriptnode;

/** Derive your config class from this and supply the number of channels as template argument,
 *  then add a single macro with the node ID you want it to have:
 *
 *	```
 *	struct MyConfig: public scriptnode2vst::DefaultVSTConfigClass<2> // stereo
 *	{
 *	    SN_NODE_ID("some_id");
 *	};
 *	```
 *
 *	Then pass this configuration class as template argument into the vst2scriptnode::wrapper
 *	node template, along with the VST effect class you want it to wrap.
 *
 */
template <int NumChannels> struct DefaultVstConfigClass
{
	// set to true if your doesn't generate sound from silence and can be suspended when the input 
	// signal is silent
	static constexpr bool isSuspendedOnSilence() { return true; }

	// set to true if your node produces a tail
	static constexpr bool hasTail() { return false; }

	static constexpr int getFixChannelAmount() { return NumChannels; };
};

/** The node wrapper that should be used to wrap the VST effect into a node. */
template <typename VstEffectType, typename ConfigClass> struct wrapper: public scriptnode::data::base
{
	// Metadata Definitions ------------------------------------------------------------------------
	
	SN_GET_SELF_AS_OBJECT(wrapper);
	SN_FORWARD_PARAMETER_TO_MEMBER(wrapper);
	SN_EMPTY_INITIALISE;

	wrapper():
	  obj(nullptr)
	{};

	static Identifier getStaticId() { return ConfigClass::getStaticId(); }

	struct MetadataClass
	{
		static Identifier getStaticId() { return ConfigClass::getStaticId(); }
	};

	// set to true if you want this node to have a modulation dragger
	static constexpr bool isModNode() { return false; };
	static constexpr bool isPolyphonic() { return false; };
	static constexpr bool hasTail() { return ConfigClass::hasTail(); };
	static constexpr bool isSuspendedOnSilence() { return ConfigClass::isSuspendedOnSilence(); };
	static constexpr int getFixChannelAmount() { return ConfigClass::getFixChannelAmount(); };
	
	static constexpr int NumTables = 0;
	static constexpr int NumSliderPacks = 0;
	static constexpr int NumAudioFiles = 0;
	static constexpr int NumFilters = 0;
	static constexpr int NumDisplayBuffers = 0;

	VstEffectType obj;

	void prepare(PrepareSpecs specs)
	{
		this->obj.sampleRate = specs.sampleRate;
	}
	
	void handleHiseEvent(HiseEvent& e)
	{
		
	}
	
	template <typename T> void process(T& data)
	{
		if(obj.isReplacing)
		{
			float** inputs = data.getRawChannelPointers();
			int numSamples = data.getNumSamples();

			this->obj.processReplacing(inputs, inputs, numSamples);
		}
		else
		{
			// not supported (yet)...
			jassertfalse;
		}
	}
	
	template <typename T> void processFrame(T& data)
	{
		span<float*, getFixChannelAmount()> inputs, outputs;

		for(int i = 0; i < getFixChannelAmount(); i++)
			inputs[i] = data.begin() + i;

		if(this->obj.isReplacing)
		{
			this->obj.processReplacing(inputs.begin(), inputs.begin(), 1);
		}
		else
		{
			span<float, getFixChannelAmount()> outputData;

			for(int i = 0; i < getFixChannelAmount(); i++)
				outputs[i] = outputData.begin() + i;

			this->obj.processReplacing(inputs.begin(), outputs.begin(), 1);

			memcpy(data.begin(), outputs.begin(), sizeof(float)*getFixChannelAmount());
		}
	}

	SN_EMPTY_MOD;
	SN_EMPTY_SET_EXTERNAL_DATA;
	SN_EMPTY_RESET;

	// Parameter Functions -------------------------------------------------------------------------
	
	template <int P> void setParameter(double v)
	{
		this->obj.setParameter(P, (float)v);
	}
	
	void createParameters(ParameterDataList& data)
	{
		for(int i = 0; i < obj.numParameters; i++)
		{
			char buffer[VST_CHAR_BUFFER_SIZE];

			this->obj.getParameterName(i, buffer);

			parameter::data p(buffer, { 0.0, 1.0 });

			switch(i)
			{
				case 0: registerCallback<0>(p); break;
				case 1: registerCallback<1>(p); break;
				case 2: registerCallback<2>(p); break;
				case 3: registerCallback<3>(p); break;
				case 4: registerCallback<4>(p); break;
				case 5: registerCallback<5>(p); break;
				case 6: registerCallback<6>(p); break;
				case 7: registerCallback<7>(p); break;
				case 8: registerCallback<8>(p); break;
				case 9: registerCallback<9>(p); break;
				case 10: registerCallback<10>(p); break;
				case 11: registerCallback<11>(p); break;
				case 12: registerCallback<12>(p); break;
				case 13: registerCallback<13>(p); break;
				case 14: registerCallback<14>(p); break;
				case 15: registerCallback<15>(p); break;
			}

			p.setDefaultValue((double)this->obj.getParameter(i));
			data.add(std::move(p));
		}
	}
};

} // vst2scriptnode

/** Use this as shortcut in your wrapper file.
 * 
 *  This will take the configuration class you've defined and the VST effect class
 *  (that is derived from our dummy AudioEffectX class) and melt it into one nice
 *  scriptnode object!
 * 
 *  Example: you've written a class called MyConfig derived from DefaultVstConfigClass<2>
 *           and want to use the VST effect class mda_compressor. In order to do so, add
 *           this macro after your config class and the include statements:
 * 
 * 	SN_DEFINE_VST_WRAPPER(mda_compressor, MyConfig)
 *  
 */
#define SN_DEFINE_VST_WRAPPER(EffectClass, ConfigClass) namespace project { template <int NV> using vst_wrapper = vst2scriptnode::wrapper<EffectClass, ConfigClass>; }

