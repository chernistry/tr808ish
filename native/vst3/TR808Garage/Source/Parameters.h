#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    inline constexpr auto masterLevel = "masterLevel";
    
    // BD
    inline constexpr auto bdLevel = "bdLevel";
    inline constexpr auto bdTune = "bdTune";
    inline constexpr auto bdFineTune = "bdFineTune";
    inline constexpr auto bdDecay = "bdDecay";
    inline constexpr auto bdTone = "bdTone";
    inline constexpr auto bdPan = "bdPan";
    
    // SD
    inline constexpr auto sdLevel = "sdLevel";
    inline constexpr auto sdTune = "sdTune";
    inline constexpr auto sdFineTune = "sdFineTune";
    inline constexpr auto sdDecay = "sdDecay";
    inline constexpr auto sdSnappy = "sdSnappy";
    inline constexpr auto sdPan = "sdPan";
    
    // LT, MT, HT
    inline constexpr auto ltLevel = "ltLevel";
    inline constexpr auto ltTune = "ltTune";
    inline constexpr auto ltFineTune = "ltFineTune";
    inline constexpr auto ltDecay = "ltDecay";
    inline constexpr auto ltPan = "ltPan";
    
    inline constexpr auto mtLevel = "mtLevel";
    inline constexpr auto mtTune = "mtTune";
    inline constexpr auto mtFineTune = "mtFineTune";
    inline constexpr auto mtDecay = "mtDecay";
    inline constexpr auto mtPan = "mtPan";
    
    inline constexpr auto htLevel = "htLevel";
    inline constexpr auto htTune = "htTune";
    inline constexpr auto htFineTune = "htFineTune";
    inline constexpr auto htDecay = "htDecay";
    inline constexpr auto htPan = "htPan";
    
    // RS, CP
    inline constexpr auto rsLevel = "rsLevel";
    inline constexpr auto rsTune = "rsTune";
    inline constexpr auto rsPan = "rsPan";
    
    inline constexpr auto cpLevel = "cpLevel";
    inline constexpr auto cpTone = "cpTone";
    inline constexpr auto cpPan = "cpPan";
    
    // CH, OH
    inline constexpr auto chLevel = "chLevel";
    inline constexpr auto chTone = "chTone";
    inline constexpr auto chPan = "chPan";
    
    inline constexpr auto ohLevel = "ohLevel";
    inline constexpr auto ohDecay = "ohDecay";
    inline constexpr auto ohTone = "ohTone";
    inline constexpr auto ohPan = "ohPan";
    
    // CY, RD, CB
    inline constexpr auto cyLevel = "cyLevel";
    inline constexpr auto cyDecay = "cyDecay";
    inline constexpr auto cyTone = "cyTone";
    inline constexpr auto cyPan = "cyPan";
    
    inline constexpr auto rdLevel = "rdLevel";
    inline constexpr auto rdTone = "rdTone";
    inline constexpr auto rdPan = "rdPan";
    
    inline constexpr auto cbLevel = "cbLevel";
    inline constexpr auto cbTune = "cbTune";
    inline constexpr auto cbPan = "cbPan";
    
    // Hi-hat choke
    inline constexpr auto hhChoke = "hhChoke";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::masterLevel, 1}, "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    
    // BD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdLevel, 1}, "BD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTune, 1}, "BD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdFineTune, 1}, "BD Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdDecay, 1}, "BD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTone, 1}, "BD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdPan, 1}, "BD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // SD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdLevel, 1}, "SD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdTune, 1}, "SD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdFineTune, 1}, "SD Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdDecay, 1}, "SD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdSnappy, 1}, "SD Snappy", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdPan, 1}, "SD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // LT, MT, HT
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltLevel, 1}, "LT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltTune, 1}, "LT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltFineTune, 1}, "LT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltDecay, 1}, "LT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltPan, 1}, "LT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtLevel, 1}, "MT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtTune, 1}, "MT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtFineTune, 1}, "MT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtDecay, 1}, "MT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtPan, 1}, "MT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htLevel, 1}, "HT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htTune, 1}, "HT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htFineTune, 1}, "HT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htDecay, 1}, "HT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htPan, 1}, "HT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // RS, CP
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsLevel, 1}, "RS Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsTune, 1}, "RS Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsPan, 1}, "RS Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpLevel, 1}, "CP Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpTone, 1}, "CP Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpPan, 1}, "CP Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // CH, OH
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chLevel, 1}, "CH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chTone, 1}, "CH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chPan, 1}, "CH Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohLevel, 1}, "OH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohDecay, 1}, "OH Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohTone, 1}, "OH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohPan, 1}, "OH Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // CY, RD, CB
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyLevel, 1}, "CY Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyDecay, 1}, "CY Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyTone, 1}, "CY Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyPan, 1}, "CY Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdLevel, 1}, "RD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdTone, 1}, "RD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdPan, 1}, "RD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbLevel, 1}, "CB Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbTune, 1}, "CB Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbPan, 1}, "CB Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // Hi-hat choke
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::hhChoke, 1}, "HH Choke", true));
    
    return layout;
}
