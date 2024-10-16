#include "Object3D.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AudioObject.h"
#include "Visualizer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform.hpp> // For rotation matrix
#include <glm/gtc/matrix_transform.hpp>	// For matrix transformation functions
#include <unordered_map>

using namespace tinyobj;

// Override the hash function for packed vertices
// This lets us do VBO indexing with a hashmap
// Hashing function follows recommendations at: https://en.cppreference.com/w/cpp/utility/hash
namespace std
{
	template<> struct hash<Object3D::PackedVertex>
	{
		size_t operator()(Object3D::PackedVertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
		}
	};
}

Object3D::Object3D(string file, string texture)
{
	filePath = file;
	texturePath = texture;
}

Object3D::~Object3D()
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &elementBuffer);
	glDeleteTextures(1, &texture);
	glDeleteProgram(shader);
}

bool Object3D::Init()
{
	if (!LoadModel())
	{
		return false;
	}

	if (!InitShaders())
	{
		return false;
	}

	if (!InitTextures())
	{
		return false;
	}
	InitBuffers();
	return true;
}

bool Object3D::LoadModel()
{
	ObjReader reader;
	ObjReaderConfig config;

	// Ignore vertex coloring for now
	config.vertex_color = false;

	if (!reader.ParseFromFile(filePath, config))
	{
		cout << reader.Error() << endl;
		return false;
	}

	std::unordered_map<PackedVertex, uint32_t> uniqueVertices = { };

	const attrib_t& attrib = reader.GetAttrib();

	for (const auto& shape : reader.GetShapes())
	{
		for (const auto& index : shape.mesh.indices)
		{
			PackedVertex vertex = { };

			if (index.vertex_index >= 0)
			{
				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
			}

			if (index.texcoord_index >= 0)
			{
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					-attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (index.normal_index >= 0)
			{
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());

				vertices.push_back(vertex.pos);
				uvs.push_back(vertex.uv);
				normals.push_back(vertex.normal);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	return true;
}

void Object3D::InitBuffers()
{
	// The classics
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

	// Element buffer for VBO indexing
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
}

bool Object3D::InitShaders()
{
	shader = LoadShaders("Shaders/TransformVertexShader.vertexshader", "Shaders/TextureFragmentShader.fragmentshader");

	if (shader != 0)
	{
		matrixID = glGetUniformLocation(shader, "MVP");
		return true;
	}

	cout << "Object shader initialization failed" << endl;
	return false;
}

bool Object3D::InitTextures()
{
	texture = loadDDS(texturePath.c_str());
	if (texture != 0)
	{
		textureID = glGetUniformLocation(shader, "myTextureSampler");
		return true;
	}
	cout << "Object texture initialization failed" << endl;
	return false;
}

void Object3D::Draw(const AudioObject& audioObject, const Visualizer& visualizer)
{
	// Use our shader
	glUseProgram(shader);

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 6), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	GLfloat scalefactor = *(audioObject.GetOutputBuckets().begin()) / 15.0f;
	Model *= scale(vec3(scalefactor, scalefactor, scalefactor));

	// Just rotate in place for now
	Model *= rotate((GLfloat)(ROTATION_SPEED * visualizer.GetDeltaTime()), vec3(0, 1.0f, 0));

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Make some funky background colors based on trebbles, mids and highs!
	glClearColor(
		(audioObject.GetOutputBuckets()[1]) / 5.0f,
		(audioObject.GetOutputBuckets()[2]) / 5.0f,
		(audioObject.GetOutputBuckets()[3]) / 5.0f,
		0.0f);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(textureID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type - PROBABLY UNSAFE, but this method doesnt support proper 4 byte types
		(void*)0           // element array buffer offset
	);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}