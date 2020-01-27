#include "InternalCalls.h"

#include "MonoWrap/MonoManager.h"
#include "Utility/Log.h"

void AderInternals::addInternals()
{
	sceneInternals();
	visualInternals();
	engineInternals();
}




Visual* visualNew(AderScene* scene)
{
	return scene->newVisual();
}

Memory::reference<VAO>* VAOget(Visual* visual)
{
	return &visual->VAO;
}

Memory::reference<Shader>* Shaderget(Visual* visual)
{
	return &visual->Shader;
}

void Shaderload(Memory::reference<Shader>* shader, MonoObject* vertex, MonoObject* fragment)
{
	(*shader)->VertexSource = SharpUtility::toString(vertex);
	(*shader)->FragmentSource = SharpUtility::toString(fragment);

	(*shader)->load();
}

void VAOsetVertices(Memory::reference<VAO>* vao, MonoArray* vertices)
{
	// Get the size of the array
	int size = mono_array_length(vertices);

	// Get address to the first element
	float* start = mono_array_addr(vertices, float, 0);

	// Create vector from the array and create vertices buffer
	std::vector<float> verts(start, start + size);
	(*vao)->bind();
	(*vao)->createVerticesBuffer(verts, false);
}

void VAOsetIndices(Memory::reference<VAO>* vao, MonoArray* indices)
{
	// Get the size of the array
	int size = mono_array_length(indices);

	// Get address to the first element
	unsigned int* start = mono_array_addr(indices, unsigned int, 0);

	// Create vector from the array and create indices buffer
	std::vector<unsigned int> indc(start, start + size);
	(*vao)->bind();
	(*vao)->createIndiceBuffer(indc, false);
}

void AderInternals::visualInternals()
{
	// Consts
	const std::string nSpace = "Ader2";
	const std::string nSpaceCore = "Ader2.Core";
	const std::string klass = "Visual";

	// Create signatures
	std::string newSignature = 
		SharpUtility::methodSignature(
			nSpace,
			klass,
			"__new",
			"intptr",
			true);

	// Create signatures
	std::string VAOgetSignature =
		SharpUtility::methodSignature(
			nSpaceCore,
			"VAO",
			"__get",
			"intptr",
			true);

	std::string ShadergetSignature =
		SharpUtility::methodSignature(
			nSpaceCore,
			"Shader",
			"__get",
			"intptr",
			true);

	std::string ShaderloadSignature =
		SharpUtility::methodSignature(
			nSpaceCore,
			"Shader",
			"__load",
			"intptr,string,string",
			true);

	std::string VAOsetVerticesSignature =
		SharpUtility::methodSignature(
			nSpaceCore,
			"VAO",
			"__setVertices",
			"intptr,single[]",
			true);

	std::string VAOsetIndicesSignature =
		SharpUtility::methodSignature(
			nSpaceCore,
			"VAO",
			"__setIndices",
			"intptr,uint[]",
			true);

	mono_add_internal_call(newSignature.c_str(), visualNew);

	mono_add_internal_call(VAOgetSignature.c_str(), VAOget);
	mono_add_internal_call(ShadergetSignature.c_str(), Shaderget);
	mono_add_internal_call(ShaderloadSignature.c_str(), Shaderload);

	mono_add_internal_call(VAOsetVerticesSignature.c_str(), VAOsetVertices);
	mono_add_internal_call(VAOsetIndicesSignature.c_str(), VAOsetIndices);
}




void AderInternals::sceneInternals()
{
	//mono_add_internal_call("");
}

void AderInternals::engineInternals()
{
	
}
