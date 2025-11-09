#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    inline constexpr auto masterLevel = "masterLevel";
    
    // BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB
    inline constexpr auto bdLevel = "bdLevel";
    inline constexpr auto bdTune = "bdTune";
    inline constexpr auto bdDecay = "bdDecay";
    inline constexpr auto bdTone = "bdTone";
    
    inline constexpr auto sdLevel = "sdLevel";
    inline constexpr auto sdTune = "sdTune";
    inline constexpr auto sdDecay = "sdDecay";
    inline constexpr auto sdSnappy = "sdSnappy";
    
    inline constexpr auto ltLevel = "ltLevel";
    inline constexpr auto ltTune = "ltTune";
    inline constexpr auto ltDecay = "ltDecay";
    
    inline constexpr auto mtLevel = "mtLevel";
    inline constexpr auto mtTune = "mtTune";
    inline constexpr auto mtDecay = "mtDecay";
    
    inline constexpr auto htLevel = "htLevel";
    inline constexpr auto htTune = "htTune";
    inline constexpr auto htDecay = "htDecay";
    
    inline constexpr auto rsLevel = "rsLevel";
    inline constexpr auto rsTune = "rsTune";
    
    inline constexpr auto cpLevel = "cpLevel";
    inline constexpr auto cpTone = "cpTone";
    
    inline constexpr auto chLevel = "chLevel";
    inline constexpr auto chTone = "chTone";
    
    inline constexpr auto ohLevel = "ohLevel";
    inline constexpr auto ohDecay = "ohDecay";
    inline constexpr auto ohTone = "ohTone";
    
    inline constexpr auto cyLevel = "cyLevel";
    inline constexpr auto cyDecay = "cyDecay";
    inline constexpr auto cyTone = "cyTone";
    
    inline constexpr auto rdLevel = "rdLevel";
    inline constexpr auto rdTone = "rdTone";
    
    inline constexpr auto cbLevel = "cbLevel";
    inline constexpr auto cbTune = "cbTune";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::masterLevel, 1}, "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    
    // BD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdLevel, 1}, "BD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTune, 1}, "BD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdDecay, 1}, "BD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTone, 1}, "BD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // SD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdLevel, 1}, "SD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdTune, 1}, "SD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdDecay, 1}, "SD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdSnappy, 1}, "SD Snappy", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // LT, MT, HT
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltLevel, 1}, "LT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltTune, 1}, "LT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltDecay, 1}, "LT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtLevel, 1}, "MT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtTune, 1}, "MT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtDecay, 1}, "MT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htLevel, 1}, "HT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htTune, 1}, "HT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htDecay, 1}, "HT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // RS, CP
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsLevel, 1}, "RS Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsTune, 1}, "RS Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpLevel, 1}, "CP Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpTone, 1}, "CP Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // CH, OH
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chLevel, 1}, "CH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chTone, 1}, "CH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohLevel, 1}, "OH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohDecay, 1}, "OH Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohTone, 1}, "OH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // CY, RD, CB
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyLevel, 1}, "CY Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyDecay, 1}, "CY Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyTone, 1}, "CY Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdLevel, 1}, "RD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdTone, 1}, "RD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbLevel, 1}, "CB Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbTune, 1}, "CB Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    
    return layout;
}
