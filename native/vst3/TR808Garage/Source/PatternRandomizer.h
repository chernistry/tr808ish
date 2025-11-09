#pragma once

#include <juce_core/juce_core.h>
#include <random>
#include <array>

struct GenreConfig
{
    juce::String name;
    std::map<juce::String, float> voiceDensity; // 0.0-1.0 probability per step
    std::map<juce::String, std::vector<int>> allowedSteps; // empty = all allowed
    float humanizeTiming = 0.0f; // ms offset range
    float humanizeVelocity = 0.0f; // velocity variance 0-1
};

class PatternRandomizer
{
public:
    PatternRandomizer() : rng(std::random_device{}()) {}
    
    void setSeed(uint32_t seed) { rng.seed(seed); }
    
    std::array<bool, 16> generatePattern(const juce::String& voice, const juce::String& genre, float amount = 0.5f)
    {
        std::array<bool, 16> pattern{};
        const auto& config = getGenreConfig(genre);
        
        float density = config.voiceDensity.count(voice) ? config.voiceDensity.at(voice) : 0.3f;
        density *= amount;
        
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
        // Check if voice has specific allowed steps
        if (config.allowedSteps.count(voice) && !config.allowedSteps.at(voice).empty())
        {
            for (int step : config.allowedSteps.at(voice))
                if (step < 16 && dist(rng) < density)
                    pattern[step] = true;
        }
        else
        {
            for (int i = 0; i < 16; ++i)
                if (dist(rng) < density)
                    pattern[i] = true;
        }
        
        return pattern;
    }
    
    static GenreConfig getGenreConfig(const juce::String& genre)
    {
        GenreConfig config;
        config.name = genre;
        
        if (genre == "UK Garage")
        {
            config.voiceDensity["BD"] = 0.6f;
            config.voiceDensity["SD"] = 0.4f;
            config.voiceDensity["CH"] = 0.7f;
            config.voiceDensity["OH"] = 0.3f;
            config.voiceDensity["CP"] = 0.2f;
            config.allowedSteps["SD"] = {4, 12}; // Backbeat
            config.humanizeTiming = 5.0f;
            config.humanizeVelocity = 0.15f;
        }
        else if (genre == "Drum & Bass")
        {
            config.voiceDensity["BD"] = 0.5f;
            config.voiceDensity["SD"] = 0.5f;
            config.voiceDensity["CH"] = 0.8f;
            config.voiceDensity["OH"] = 0.2f;
            config.allowedSteps["SD"] = {4, 12}; // Amen break style
            config.humanizeTiming = 2.0f;
            config.humanizeVelocity = 0.1f;
        }
        else if (genre == "Techno")
        {
            config.voiceDensity["BD"] = 0.8f;
            config.voiceDensity["SD"] = 0.3f;
            config.voiceDensity["CH"] = 0.9f;
            config.voiceDensity["OH"] = 0.25f;
            config.voiceDensity["CP"] = 0.2f;
            config.allowedSteps["BD"] = {0, 4, 8, 12}; // Four-on-floor
            config.humanizeTiming = 1.0f;
            config.humanizeVelocity = 0.05f;
        }
        else if (genre == "House")
        {
            config.voiceDensity["BD"] = 0.9f;
            config.voiceDensity["SD"] = 0.4f;
            config.voiceDensity["CH"] = 0.8f;
            config.voiceDensity["OH"] = 0.4f;
            config.allowedSteps["BD"] = {0, 4, 8, 12}; // Four-on-floor
            config.allowedSteps["SD"] = {4, 12}; // Backbeat
            config.humanizeTiming = 3.0f;
            config.humanizeVelocity = 0.12f;
        }
        else if (genre == "Trap")
        {
            config.voiceDensity["BD"] = 0.5f;
            config.voiceDensity["SD"] = 0.4f;
            config.voiceDensity["CH"] = 0.9f; // Hi-hat rolls
            config.voiceDensity["OH"] = 0.2f;
            config.voiceDensity["CP"] = 0.3f;
            config.humanizeTiming = 4.0f;
            config.humanizeVelocity = 0.2f;
        }
        else if (genre == "Lo-Fi")
        {
            config.voiceDensity["BD"] = 0.5f;
            config.voiceDensity["SD"] = 0.35f;
            config.voiceDensity["CH"] = 0.4f;
            config.voiceDensity["OH"] = 0.25f;
            config.humanizeTiming = 8.0f;
            config.humanizeVelocity = 0.25f;
        }
        else // Default
        {
            config.voiceDensity["BD"] = 0.5f;
            config.voiceDensity["SD"] = 0.4f;
            config.voiceDensity["CH"] = 0.6f;
            config.voiceDensity["OH"] = 0.3f;
            config.humanizeTiming = 3.0f;
            config.humanizeVelocity = 0.15f;
        }
        
        return config;
    }
    
    static std::vector<juce::String> getAvailableGenres()
    {
        return {"UK Garage", "Drum & Bass", "Techno", "House", "Trap", "Lo-Fi"};
    }
    
private:
    std::mt19937 rng;
};
