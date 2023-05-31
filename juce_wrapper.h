/** JUCE AudioProcessor Wrapper
 
 1. Copy the entire JUCE project folder into src/
 2. Create a C++ template with the ID you want.
 2. Open the .h file that you've created in the IDE and Include the plugin processor implementation file
   from your JUCE project (usually at Source/PluginProcessor.cpp). You might also need to include
 3. Fix all compile errors. This might include adding missing modules to the projucer project,
   removing the inclusion of the local "JuceHeader.h" file in every included file and stripping
   every code that is related to the plugin editor (you can return nullptr in the AudioProcessor::createEditor() method). Note that the plugin will use the JUCE codebase
   that is shipped with HISE, so make sure that there are no library mismatch issues.
 4. Remove the entry point of the plugin that creates an instance of the audio processor (otherwise this will
   create an instance of this audio processor instead of your plugin. Remove the function "createEditor()"
   usually found on the bottom of the PluginProcessor.cpp class.
 4. Once the compilation goes through, you can start adding the wrapper code. Replace the entire content
   of the namespace project with a using definition that wraps the processor into a node.
 5. Create a metaconfig class that describes the channel amount
 
 */

#include <JuceHeader.h>

#pragma once

#include "wrap_config.h"

#ifndef JucePlugin_Name
#define JucePlugin_Name "unused"
#endif

namespace juce2scriptnode
{
using namespace juce;
using namespace hise;
using namespace scriptnode;

template <typename JuceProcessorType, typename ConfigClass>
   struct wrapper: public data::base,
                   public ConfigClass
{
    SN_GET_SELF_AS_OBJECT(wrapper);
    SN_FORWARD_PARAMETER_TO_MEMBER(wrapper);
    SN_EMPTY_INITIALISE;

    static Identifier getStaticId() { return ConfigClass::getStaticId(); }
    
    JuceProcessorType obj;
    
    struct MetadataClass
    {
        static Identifier getStaticId() { return ConfigClass::getStaticId(); }
    };
    
    // set to true if you want this node to have a modulation dragger
    static constexpr bool isModNode() { return false; };
    static constexpr bool isPolyphonic() { return false; };
    
    // Scriptnode Callbacks ------------------------------------------------------------------------
    
    void prepare(PrepareSpecs specs)
    {
        obj.prepareToPlay(specs.sampleRate, specs.blockSize);
        
        if constexpr(ConfigClass::isProcessingHiseEvent())
            currentMidiBuffer.ensureSize(512);
    }
    
    void reset()
    {
        obj.reset();
    }
    
    juce::MidiBuffer currentMidiBuffer;
    
    void handleHiseEvent(HiseEvent& e)
    {
        if constexpr (ConfigClass::isProcessingHiseEvent())
            currentMidiBuffer.addEvent(e.toMidiMesage(), e.getTimeStamp());
    }
    
    template <typename T> void process(T& data)
    {
        AudioSampleBuffer buffer(data.getRawChannelPointers(), data.getNumChannels(), data.getNumSamples());
        
        obj.processBlock(buffer, currentMidiBuffer);
        
        if constexpr(ConfigClass::isProcessingHiseEvent())
            currentMidiBuffer.clear();
    }
    
    template <typename T> void processFrame(T& data)
    {
        // not implemented, just don't use it...
        jassertfalse;
    }
    
    SN_EMPTY_MOD;
    SN_EMPTY_SET_EXTERNAL_DATA;
    
    // Parameter Functions -------------------------------------------------------------------------
    
    template <int P> void setParameter(double v)
    {
        obj.setParameter(P, parameterRanges[P].convertTo0to1((float)v));
    }
    
    Array<NormalisableRange<float>> parameterRanges;
    
    void createParameters(ParameterDataList& data)
    {
        auto ptree = obj.getParameters();
        
        for(int i = 0; i < jmin(16, obj.getNumParameters()); i++)
        {
            auto pid = obj.getParameterName(i);
            auto pdef = obj.getParameterDefaultValue(i);
            
            if(auto r = dynamic_cast<RangedAudioParameter*>(ptree[i]))
            {
                parameterRanges.add(r->getNormalisableRange());
            }
            else
            {
                parameterRanges.add({0.0, 1.0});
            }
            
            scriptnode::InvertableParameterRange pr;
            
            auto fr = parameterRanges.getLast();
            
            pr.rng.start = (double)fr.start;
            pr.rng.end = (double)fr.end;
            pr.rng.interval = (double)fr.interval;
            pr.rng.skew = (double)fr.skew;
            pr.rng.symmetricSkew = (double)fr.symmetricSkew;
            
            // Create a parameter like this
            parameter::data p(pid, pr);
            
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
            
            p.setDefaultValue(fr.convertFrom0to1(pdef));
            data.add(std::move(p));
        }
    }
};
}

#define SN_DEFINE_JUCE_WRAPPER(node_id, JuceProcessorClass, ConfigClass) namespace project { template <int NV> using node_id = juce2scriptnode::wrapper<JuceProcessorClass, ConfigClass>; }
