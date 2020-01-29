#include "AssetManager.h"

// Engine messages
#include "Enums/Messages.h"

// Logger
#include "Utility/Log.h"

bool AssetManager::canShutdown()
{
	return true;
}

void AssetManager::shutdown()
{
	clearAssets();
}

int AssetManager::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_TransmitAssets:
		transmitAssets(static_cast<AderAssetsSharp*>(pData));
		break;
	case Messages::msg_ClearAssets:
		clearAssets();
		break;
	}

	return 0;
}

bool AssetManager::hasAsset(const std::string& name)
{
	return m_assets.find(name) != m_assets.end();
}

void AssetManager::addAsset(const std::string& name, Asset* asset)
{
	// Check if the asset exists or not
	if (!hasAsset(name))
	{
		m_assets[name] = asset;
	}
}

Asset* AssetManager::getAsset(const std::string& name)
{
	// Check if the asset exists and return it else return nullptr
	if (hasAsset(name))
	{
		return m_assets[name];
	}
	else
	{
		return nullptr;
	}
}

void AssetManager::removeAsset(const std::string& name)
{
	// Remove asset if it exists
	if (hasAsset(name))
	{
		delete m_assets[name];
		m_assets.erase(name);
	}
}

void AssetManager::changeName(const std::string& prevName, const std::string& newName)
{
	// Check if an asset with the specified name exists
	if (hasAsset(prevName))
	{
		// Check if the name is not occupied
		if (!hasAsset(newName))
		{
			// Copy the asset to the new name slot
			m_assets[newName] = m_assets[prevName];

			// Delete the previous asset
			removeAsset(prevName);
		}
	}
}

std::string AssetManager::getName(Asset* asset)
{
	// Loop over the assets and return it's name if found
	// else return empty string
	for (auto& it : m_assets)
	{
		if (it.second == asset)
		{
			return it.first;
		}
	}

	return "";
}

void AssetManager::transmitAssets(AderAssetsSharp* pAssets)
{
	// Assign interface
	m_pSharpInterface = pAssets;

	// Set instance field to this class
	m_pSharpInterface->_CInstance->setValue(nullptr, this);
}

void AssetManager::clearAssets()
{
	// Delete memory
	for (auto& it : m_assets)
	{
		delete it.second;
	}

	// Clear map
	m_assets.clear();
}
