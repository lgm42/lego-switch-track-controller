#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class ConfigurationProvider
{
    public:
        typedef struct
        {
            String hostname;
        } Parameters;
        
        ConfigurationProvider();
        virtual ~ConfigurationProvider();

        void setup();

        void loadFromFlash();
        void saveToFlash();
        void load(const String & data);

        void createDefaultConfiguration();

        const Parameters & parameters() const;
        Parameters & parameters();
    private:
        void parseJson(const String & data);
        
        static const String ConfigurationFilename;
        Parameters _parameters;

        static const int DynamicJsonDocumentMaxSize = 3 * 1024;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern ConfigurationProvider Configuration;
#endif