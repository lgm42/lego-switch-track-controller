#include <FS.h>
#include "SPIFFS.h"
#include "ConfigurationProvider.h"

const String ConfigurationProvider::ConfigurationFilename = "/configuration.json";

ConfigurationProvider::ConfigurationProvider()
{
}

ConfigurationProvider::~ConfigurationProvider()
{
}

void ConfigurationProvider::setup()
{
    if (!SPIFFS.begin(true)) 
    {
        Serial.println("Unable to begin SPIFFS");
    }
    else
    {
        Serial.println("SPIFFS initialized.");
    }
}

void ConfigurationProvider::loadFromFlash()
{
    //TODO : restore when wifi ok and validated
    createDefaultConfiguration();
    saveToFlash();
    if(SPIFFS.exists(ConfigurationFilename))
    {
        File file = SPIFFS.open(ConfigurationFilename, "r");
        if (!file)
        {
            Serial.println("Exception during opening system configuration, resetting to factory settings");
            createDefaultConfiguration();
            saveToFlash();
        }
        else
        {
            Serial.println("Configuration file opened.");
        }
        String configurationFileAsString = "";
        while (file.available())
        {
            configurationFileAsString +=(char)file.read();
        }
        Serial.println("Configuration file read.");
        parseJson(configurationFileAsString);
    }
    else
    {
        Serial.println("Configuration file doesn't exists, resetting to factory settings");
        createDefaultConfiguration();
        saveToFlash();
    }
}

void ConfigurationProvider::createDefaultConfiguration()
{
    _parameters.hostname = "switch-track";
}

void ConfigurationProvider::saveToFlash()
{
    // Delete existing file, otherwise the configuration is appended to the file
    if (SPIFFS.exists(ConfigurationFilename))
        SPIFFS.remove(ConfigurationFilename);

    // Open file for writing
    File file = SPIFFS.open(ConfigurationFilename, FILE_WRITE);
    if (!file) {
        Serial.println(F("Failed to create file"));
        return;
    }
    
    //we browse the tree to generate the json
    DynamicJsonDocument doc(DynamicJsonDocumentMaxSize);

    JsonObject parameters = doc.createNestedObject("parameters");
    parameters["hostname"] = _parameters.hostname;
    
    serializeJson(doc, Serial);
    serializeJson(doc, file);
    file.close();
    Serial.println("Configuration file saved");
}

void ConfigurationProvider::load(const String & data)
{
    parseJson(data);
}

void ConfigurationProvider::parseJson(const String & data)
{
    //deserializeJson
    DynamicJsonDocument doc(DynamicJsonDocumentMaxSize);
    Serial.print("parseJson : ");
    Serial.println(data);
    DeserializationError error = deserializeJson(doc, data);
    if (error) 
    {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    //iterate and create structure
    
    JsonObject parameters = doc["parameters"];
    _parameters.hostname = parameters["hostname"] | "switch-track";
}

const ConfigurationProvider::Parameters & ConfigurationProvider::parameters() const
{
    return _parameters;
}

ConfigurationProvider::Parameters & ConfigurationProvider::parameters()
{
    return _parameters;
}

#if !defined(NO_GLOBAL_INSTANCES)
ConfigurationProvider Configuration;
#endif