#pragma once

namespace scriptnode
{
namespace meta
{
/** Derive your config class from this and supply the number of channels as template argument,
 *  then add a single macro with the node ID you want it to have:
 *
 *    ```
 *    struct MyConfig: public scriptnode::meta::default_config<2> // stereo
 *    {
 *        SN_NODE_ID("some_id");
 *    };
 *    ```
 *
 *    Then pass this configuration class as template argument into the vst2scriptnode::wrapper
 *    node template, along with the VST effect class you want it to wrap.
 *
 */
template <int NumChannels> struct default_config
{
    // set to true if your doesn't generate sound from silence and can be suspended when the input
    // signal is silent
    static constexpr bool isSuspendedOnSilence() { return true; }

    // set to true if your node produces a tail
    static constexpr bool hasTail() { return false; }

    static constexpr bool isProcessingHiseEvent() { return false; }

    static constexpr int getFixChannelAmount() { return NumChannels; };
    
    static constexpr int NumTables = 0;
    static constexpr int NumSliderPacks = 0;
    static constexpr int NumAudioFiles = 0;
    static constexpr int NumFilters = 0;
    static constexpr int NumDisplayBuffers = 0;
};
}
}
