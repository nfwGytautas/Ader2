#include "InternalCalls.h"

#include "MonoWrap/MonoManager.h"
#include "Modules/AssetManager.h"
#include "Utility/Log.h"

VAO* VAOnew(AssetManager* assetManager, MonoObject* name)
{
	std::string assetName = SharpUtility::toString(name);
	return assetManager->newAsset<VAO>(assetName);
}

void VAOsetVertices(VAO* vao, MonoArray* vertices)
{
	// Get the size of the array
	int size = mono_array_length(vertices);

	// Get address to the first element
	float* start = mono_array_addr(vertices, float, 0);

	// Create vector from the array and create vertices buffer
	std::vector<float> verts(start, start + size);
	vao->bind();
	vao->createVerticesBuffer(verts, false);
}

void VAOsetIndices(VAO* vao, MonoArray* indices)
{
	// Get the size of the array
	int size = mono_array_length(indices);

	// Get address to the first element
	unsigned int* start = mono_array_addr(indices, unsigned int, 0);

	// Create vector from the array and create indices buffer
	std::vector<unsigned int> indc(start, start + size);
	vao->bind();
	vao->createIndiceBuffer(indc, false);
}

void VAOsetUV(VAO* vao, MonoArray* texCoords)
{
	// Get the size of the array
	int size = mono_array_length(texCoords);

	// Get address to the first element
	float* start = mono_array_addr(texCoords, float, 0);

	// Create vector from the array and create indices buffer
	std::vector<float> tex(start, start + size);
	vao->bind();
	vao->createUVBuffer(tex, false);
}



Visual* Visualnew(AssetManager* assetManager, MonoObject* name)
{
	std::string assetName = SharpUtility::toString(name);
	return assetManager->newAsset<Visual>(assetName);
}

void VisualsetVAO(Visual* visual, VAO* vao)
{
	visual->VAO = vao;
}

void VisualsetShader(Visual* visual, Shader* shader)
{
	visual->Shader = shader;
}

void VisualsetTexture(Visual* visual, int slot, Texture* texture)
{
	// Set the texture
	visual->Textures[slot] = texture;
}

VAO* VisualgetVAO(Visual* visual)
{
	return visual->VAO;
}

Shader* VisualgetShader(Visual* visual)
{
	return visual->Shader;
}

Texture* VisualgetTexture(Visual* visual, int slot)
{
	return visual->Textures[slot];
}



Shader* Shadernew(AssetManager* assetManager, MonoObject* name)
{
	std::string assetName = SharpUtility::toString(name);
	return assetManager->newAsset<Shader>(assetName);
}

void Shaderload(Shader* shader, MonoObject* vertex, MonoObject* fragment)
{
	shader->VertexSource = SharpUtility::toString(vertex);
	shader->FragmentSource = SharpUtility::toString(fragment);

	shader->load();
}



Texture* Texturenew(AssetManager* assetManager, MonoObject* name)
{
	std::string assetName = SharpUtility::toString(name);
	return assetManager->newAsset<Texture>(assetName);
}

void Textureload(Texture* texture, MonoObject* source)
{
	texture->Source = SharpUtility::toString(source);

	texture->load();
}



Asset* AssetManagerget(AssetManager* manager, MonoObject* name)
{
	return manager->getAsset(SharpUtility::toString(name));
}

bool AssetManagerhas(AssetManager* manager, MonoObject* name)
{
	return manager->hasAsset(SharpUtility::toString(name));
}



GameObject* ScenenewGameObject(AderScene* scene)
{
	return scene->newGameObject();
}



Visual* GOgetVisual(GameObject* gObject)
{
	return gObject->getVisual();
}

void GOsetVisual(GameObject* gObject, Visual* visual)
{
	return gObject->setVisual(visual);
}



void AderInternals::addInternals()
{
	// Add visual internals
	mono_add_internal_call("Ader2.Visual::__new(intptr,string)", Visualnew);
	mono_add_internal_call("Ader2.Visual::__setVAO(intptr,intptr)", VisualsetVAO);
	mono_add_internal_call("Ader2.Visual::__setShader(intptr,intptr)", VisualsetShader);
	mono_add_internal_call("Ader2.Visual::__setTexture(intptr,int,intptr)", VisualsetTexture);
	mono_add_internal_call("Ader2.Visual::__getVAO(intptr)", VisualgetVAO);
	mono_add_internal_call("Ader2.Visual::__getShader(intptr)", VisualgetShader);
	mono_add_internal_call("Ader2.Visual::__getTexture(intptr,int)", VisualgetTexture);

	// Add VAO internals
	mono_add_internal_call("Ader2.Core.VAO::__new(intptr,string)", VAOnew);
	mono_add_internal_call("Ader2.Core.VAO::__setIndices(intptr,uint[])", VAOsetIndices);
	mono_add_internal_call("Ader2.Core.VAO::__setVertices(intptr,single[])", VAOsetVertices);
	mono_add_internal_call("Ader2.Core.VAO::__setUV(intptr,single[])", VAOsetUV);

	// Add Shader internals
	mono_add_internal_call("Ader2.Core.Shader::__new(intptr,string)", Shadernew);
	mono_add_internal_call("Ader2.Core.Shader::__load(intptr,string,string)", Shaderload);

	// Add texture internals
	mono_add_internal_call("Ader2.Core.Texture::__new(intptr,string)", Texturenew);
	mono_add_internal_call("Ader2.Core.Texture::__load(intptr,string)", Textureload);

	// Add asset manager internals
	mono_add_internal_call("Ader2.Core.AderAssets::__get(intptr,string)", AssetManagerget);
	mono_add_internal_call("Ader2.Core.AderAssets::__has(intptr,string)", AssetManagerhas);

	// Add scene internals
	mono_add_internal_call("Ader2.AderScene::__newGameObject(intptr)", ScenenewGameObject);

	// Add game object internals
	mono_add_internal_call("Ader2.GameObject::__getVisual(intptr)", GOgetVisual);
	mono_add_internal_call("Ader2.GameObject::__setVisual(intptr,intptr)", GOsetVisual);
}
