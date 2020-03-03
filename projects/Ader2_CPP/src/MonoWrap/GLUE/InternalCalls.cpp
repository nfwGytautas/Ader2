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

void VisualgetSize(Visual* visual, glm::vec2* value)
{
	*value = visual->AtlasDims;
}

void VisualsetSize(Visual* visual, glm::vec2* value)
{
	visual->AtlasDims = *value;
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

Camera* ScenenewCamera(AderScene* scene)
{
	return scene->newCamera();
}

Camera* ScenegetActiveCamera(AderScene* scene)
{
	return scene->getActiveCamera();
}

void ScenesetActiveCamera(AderScene* scene, Camera* camera)
{
	scene->setActiveCamera(camera);
}


Visual* GOgetVisual(GameObject* gObject)
{
	return gObject->getVisual();
}

void GOsetVisual(GameObject* gObject, Visual* visual)
{
	return gObject->setVisual(visual);
}

void GOgetPosition(GameObject* gObject, glm::vec3* value)
{
	*value = gObject->getTransform().Position;
}

void GOsetPosition(GameObject* gObject, glm::vec3* value)
{
	gObject->setTransform().Position = *value;
}

void GOgetRotation(GameObject* gObject, glm::vec3* value)
{
	*value = gObject->getTransform().Rotation;
}

void GOsetRotation(GameObject* gObject, glm::vec3* value)
{
	gObject->setTransform().Rotation = *value;
}

void GOgetScale(GameObject* gObject, glm::vec3* value)
{
	*value = gObject->getTransform().Scale;
}

void GOsetScale(GameObject* gObject, glm::vec3* value)
{
	gObject->setTransform().Scale = *value;
}

void GOgetTexOffset(GameObject* gObject, glm::vec2* value)
{
	*value = gObject->getTexOffset();
}

void GOsetTexOffset(GameObject* gObject, glm::vec2* value)
{
	gObject->setTexOffset() = *value;
}


void CameragetPosition(Camera* camera, glm::vec3* value)
{
	*value = camera->getPosition();
}

void CamerasetPosition(Camera* camera, glm::vec3* value)
{
	camera->setPosition(*value);
}

void CameragetRotation(Camera* camera, glm::vec3* value)
{
	*value = camera->getRotation();
}

void CamerasetRotation(Camera* camera, glm::vec3* value)
{
	camera->setRotation(*value);
}


void AudioListenergetPosition(AderScene* scene, glm::vec3* value)
{
	*value = scene->getAudioListener()->Position;
}

void AudioListenersetPosition(AderScene* scene, glm::vec3* value)
{
	scene->setAudioListener()->Position = *value;
}

void AudioListenergetVolume(AderScene* scene, float* value)
{
	*value = scene->getAudioListener()->Volume;
}

void AudioListenersetVolume(AderScene* scene, float* value)
{
	scene->setAudioListener()->Volume = *value;
}

void AudioListenergetOrientationAt(AderScene* scene, glm::vec3* value)
{
	*value = scene->getAudioListener()->Orientation.At;
}

void AudioListenersetOrientationAt(AderScene* scene, glm::vec3* value)
{
	scene->setAudioListener()->Orientation.At = *value;
}

void AudioListenergetOrientationUp(AderScene* scene, glm::vec3* value)
{
	*value = scene->getAudioListener()->Orientation.Up;
}

void AudioListenersetOrientationUp(AderScene* scene, glm::vec3* value)
{
	scene->setAudioListener()->Orientation.Up = *value;
}


Audio* Audionew(AssetManager* assetManager, MonoObject* name)
{
	std::string assetName = SharpUtility::toString(name);
	return assetManager->newAsset<Audio>(assetName);
}

void Audioload(Audio* audio, MonoObject* source)
{
	audio->Source = SharpUtility::toString(source);
	audio->load();
}

void Audioplay(Audio* audio)
{
	audio->start();
}

void Audiopause(Audio* audio)
{
	audio->pause();
}

void Audiostop(Audio* audio)
{
	audio->stop();
}

void AudiogetPitch(Audio* audio, float* value)
{
	*value = audio->getPitch();
}

void AudiosetPitch(Audio* audio, float* value)
{
	audio->setPitch(*value);
}

void AudiogetVolume(Audio* audio, float* value)
{
	*value = audio->getVolume();
}

void AudiosetVolume(Audio* audio, float* value)
{
	audio->setVolume(*value);
}

void AudiogetPosition(Audio* audio, glm::vec3* value)
{
	*value = audio->getPosition();
}

void AudiosetPosition(Audio* audio, glm::vec3* value)
{
	audio->setPosition(*value);
}

void AudiogetVelocity(Audio* audio, glm::vec3* value)
{
	*value = audio->getVelocity();
}

void AudiosetVelocity(Audio* audio, glm::vec3* value)
{
	audio->setVelocity(*value);
}

void AudiogetLooping(Audio* audio, bool* value)
{
	*value = audio->getLooping();
}

void AudiosetLooping(Audio* audio, bool* value)
{
	audio->setLooping(*value);
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
	mono_add_internal_call("Ader2.Visual::__getSize(intptr,Ader2.Core.Vector2&)", VisualgetSize);
	mono_add_internal_call("Ader2.Visual::__setSize(intptr,Ader2.Core.Vector2&)", VisualsetSize);

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
	mono_add_internal_call("Ader2.AderScene::__newCamera(intptr)", ScenenewCamera);
	mono_add_internal_call("Ader2.AderScene::__getActiveCamera(intptr)", ScenegetActiveCamera);
	mono_add_internal_call("Ader2.AderScene::__setActiveCamera(intptr,intptr)", ScenesetActiveCamera);

	// Add game object internals
	mono_add_internal_call("Ader2.GameObject::__getVisual(intptr)", GOgetVisual);
	mono_add_internal_call("Ader2.GameObject::__setVisual(intptr,intptr)", GOsetVisual);
	mono_add_internal_call("Ader2.GameObject::__getPosition(intptr,Ader2.Core.Vector3&)", GOgetPosition);
	mono_add_internal_call("Ader2.GameObject::__setPosition(intptr,Ader2.Core.Vector3&)", GOsetPosition);
	mono_add_internal_call("Ader2.GameObject::__getRotation(intptr,Ader2.Core.Vector3&)", GOgetRotation);
	mono_add_internal_call("Ader2.GameObject::__setRotation(intptr,Ader2.Core.Vector3&)", GOsetRotation);
	mono_add_internal_call("Ader2.GameObject::__getScale(intptr,Ader2.Core.Vector3&)", GOgetScale);
	mono_add_internal_call("Ader2.GameObject::__setScale(intptr,Ader2.Core.Vector3&)", GOsetScale);
	mono_add_internal_call("Ader2.GameObject::__getTexOffset(intptr,Ader2.Core.Vector2&)", GOgetTexOffset);
	mono_add_internal_call("Ader2.GameObject::__setTexOffset(intptr,Ader2.Core.Vector2&)", GOsetTexOffset);

	// Add camera internals
	mono_add_internal_call("Ader2.Camera::__getPosition(intptr,Ader2.Core.Vector3&)", CameragetPosition);
	mono_add_internal_call("Ader2.Camera::__setPosition(intptr,Ader2.Core.Vector3&)", CamerasetPosition);
	mono_add_internal_call("Ader2.Camera::__getRotation(intptr,Ader2.Core.Vector3&)", CameragetRotation);
	mono_add_internal_call("Ader2.Camera::__setRotation(intptr,Ader2.Core.Vector3&)", CamerasetRotation);

	// Add audio listener internals
	mono_add_internal_call("Ader2.Core.AudioListener::__getPosition(intptr,Ader2.Core.Vector3&)", AudioListenergetPosition);
	mono_add_internal_call("Ader2.Core.AudioListener::__setPosition(intptr,Ader2.Core.Vector3&)", AudioListenersetPosition);
	mono_add_internal_call("Ader2.Core.AudioListener::__getVolume(intptr,single&)", AudioListenergetVolume);
	mono_add_internal_call("Ader2.Core.AudioListener::__setVolume(intptr,single&)", AudioListenersetVolume);
	mono_add_internal_call("Ader2.Core.AudioListener::__getOrientationAt(intptr,Ader2.Core.Vector3&)", AudioListenergetOrientationAt);
	mono_add_internal_call("Ader2.Core.AudioListener::__setOrientationAt(intptr,Ader2.Core.Vector3&)", AudioListenersetOrientationAt);
	mono_add_internal_call("Ader2.Core.AudioListener::__getOrientationUp(intptr,Ader2.Core.Vector3&)", AudioListenergetOrientationUp);
	mono_add_internal_call("Ader2.Core.AudioListener::__setOrientationUp(intptr,Ader2.Core.Vector3&)", AudioListenersetOrientationUp);

	// Add audio internals
	mono_add_internal_call("Ader2.Core.Audio::__new(intptr,string)", Audionew);
	mono_add_internal_call("Ader2.Core.Audio::__load(intptr,string)", Audioload);
	mono_add_internal_call("Ader2.Core.Audio::__play(intptr)", Audioplay);
	mono_add_internal_call("Ader2.Core.Audio::__pause(intptr)", Audiopause);
	mono_add_internal_call("Ader2.Core.Audio::__stop(intptr)", Audiostop);
	mono_add_internal_call("Ader2.Core.Audio::__getPitch(intptr,single&)", AudiogetPitch);
	mono_add_internal_call("Ader2.Core.Audio::__setPitch(intptr,single&)", AudiosetPitch);
	mono_add_internal_call("Ader2.Core.Audio::__getVolume(intptr,single&)", AudiogetVolume);
	mono_add_internal_call("Ader2.Core.Audio::__setVolume(intptr,single&)", AudiosetVolume);
	mono_add_internal_call("Ader2.Core.Audio::__getPosition(intptr,Ader2.Core.Vector3&)", AudiogetPosition);
	mono_add_internal_call("Ader2.Core.Audio::__setPosition(intptr,Ader2.Core.Vector3&)", AudiosetPosition);
	mono_add_internal_call("Ader2.Core.Audio::__getVelocity(intptr,Ader2.Core.Vector3&)", AudiogetVelocity);
	mono_add_internal_call("Ader2.Core.Audio::__setVelocity(intptr,Ader2.Core.Vector3&)", AudiosetVelocity);
	mono_add_internal_call("Ader2.Core.Audio::__getLooping(intptr,bool&)", AudiogetLooping);
	mono_add_internal_call("Ader2.Core.Audio::__setLooping(intptr,bool&)", AudiosetLooping);
}
