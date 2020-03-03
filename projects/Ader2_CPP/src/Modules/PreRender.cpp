#include "PreRender.h"

// Engine messages
#include "Enums/Messages.h"

// Logging
#include "Utility/Log.h"

bool PreRender::canShutdown()
{
	return false;
}

void PreRender::shutdown()
{
}

int PreRender::onMessage(MessageBus::MessageType msg, MessageBus::DataType pData)
{
	switch (msg)
	{
	case Messages::msg_SceneChanged:
		sceneChanged(pData);
		return 0;
	case Messages::msg_SystemPreRender:
		preRender();
		return 0;
	}

	return 0;
}

void PreRender::sceneChanged(MessageBus::DataType pData)
{
	m_currentScene = *static_cast<Memory::reference<AderScene>*>(pData);
}

void PreRender::preRender()
{
	// Iterate over each visual
	for (Visual* visual : m_currentScene->getVisuals())
	{
		// For now
		visual->RenderCount = visual->Objects.size();

		// Update transforms
		updateTransforms(visual);

		// Update texture offsets
		updateTexOffsets(visual);
	}
}

void PreRender::updateTransforms(Visual* visual)
{
	if (visual->Transforms.size() <= visual->Objects.size())
	{
		visual->Transforms.resize(visual->Objects.size());
	}

	// Iterate over each game object
	for (size_t i = 0; i < visual->Objects.size(); i++)
	{
		// Check if it is in need of updating
		if (visual->Render[i])
		{
			// Check if the transformation needs recalculating
			if (visual->Objects[i]->transformChanged())
			{
				// Update transformation
				visual->Transforms[i] = visual->Objects[i]->getTransformation();
			}
		}
	}
}

void PreRender::updateTexOffsets(Visual* visual)
{
	if (visual->Offsets.size() <= visual->Objects.size())
	{
		visual->Offsets.resize(visual->Objects.size());
	}

	// Iterate over each game object
	for (size_t i = 0; i < visual->Objects.size(); i++)
	{
		// Check if it is in need of updating
		if (visual->Render[i])
		{
			// Check if the transformation needs recalculating
			if (visual->Objects[i]->offsetChanged())
			{
				// Update offset
				visual->Offsets[i] = visual->Objects[i]->getOffset(visual->AtlasDims);
			}
		}
	}
}
