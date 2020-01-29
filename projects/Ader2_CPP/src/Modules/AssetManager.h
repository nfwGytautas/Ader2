#pragma once

// AssetManager is part of the module system
#include "ModuleSystem/ModuleSystem.h"

// AssetManager manages asset instances
#include "CommonTypes/Asset.h"
#include "OpenGLModules/GLContext.h"

// AssetManager C# interface
#include "MonoWrap/GLUE/AderEngineSharp.h"

// Map of assets
#include <unordered_map>

/**
 * AssetManager provides the engine with the ability to store and get
 * assets from any part of a script
 *
 * MODULE
 * Messages:
 *  -TransmitAssets
 *  -ClearAssets
 *
 * Posts:
 * 
 */
class AssetManager : public Module
{
public:
    // Inherited via Module
    virtual bool canShutdown() override;

    // Inherited via Module
    virtual void shutdown() override;

    // Inherited via Module
    virtual int onMessage(MessageBus::MessageType msg, MessageBus::DataType pData) override;

    /**
     * Returns true if an asset with the specified name exists
     */
    bool hasAsset(const std::string& name);

    /**
     * Add asset with the specified name to the asset map
     */
    void addAsset(const std::string& name, Asset* asset);

    /** 
     * Returns the asset of the specified name, nullptr if invalid
     */
    Asset* getAsset(const std::string& name);

    /**
     * Remove asset from the asset manager
     */
    void removeAsset(const std::string& name);

    /**
     * Changes the name of the asset to a new one if it's free otherwise leaves
     * the previous name
     */
    void changeName(const std::string& prevName, const std::string& newName);

    /**
     * Gets the name of the asset if the asset doesn't exist then an empty string
     * will be returned
     */
    std::string getName(Asset* asset);

    /**
     * Creates a new asset with the specified name of the specified type
     */
    template<class T>
    T* newAsset(const std::string& name)
    {
        // Check if an asset exists, if it does return nullptr 
        // and don't do anything
        if (hasAsset(name))
        {
            return nullptr;
        }

        // Create new asset and add it
        T* asset = new T();

        // Add asset
        addAsset(name, asset);

        // Return the asset
        return asset;
    }
private:
    /**
     * Sets the asset manager sharp interface to this instance
     */
    void transmitAssets(AderAssetsSharp* pAssets);

    /**
     * Clears all assets
     */
    void clearAssets();
private:
    /// Pointer to the assets interface
    AderAssetsSharp* m_pSharpInterface = nullptr;

    /// Asset map
    std::unordered_map<std::string, Asset*> m_assets;
};
