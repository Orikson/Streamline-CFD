#ifndef HELPER_H
#define HELPER_H

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

#include <vector>
using std::vector;
#include <string>
using std::string;

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glshader.h"

#define MAX_BONE_INFLUENCE 4

unsigned int textureFromFile(QOpenGLFunctions* f, const char *path, const string &directory, bool gamma = false);
unsigned int loadCubemap(QOpenGLFunctions* f, vector<std::string> faces);

/**
 * @brief Defines a single vertex in OpenGL space (adapted from https://learnopengl.com/Model-Loading/Mesh)
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

/**
 * @brief Defines a single point's worth of texture data in OpenGL space (adapted from https://learnopengl.com/Model-Loading/Mesh)
 */
struct Texture {
    unsigned int id;
    string type;
    string path;
};

/**
 * @brief Defines a mesh including sets of vertices, indices, and texture structs
 */
class Mesh {
    public:
        // mesh data
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        Mesh(QOpenGLFunctions* f, vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

        void draw(Shader* shader);
    private:
        QOpenGLFunctions* f;

        // render data
        QOpenGLVertexArrayObject* VAO;
        unsigned int VBO, EBO;

        void setupMesh();
};

/**
 * @brief Defines a mesh including sets of vertices, indices, and texture structs
 */
class Model {
    public:
        // model data 
        vector<Texture> textures_loaded;
        vector<Mesh> meshes;
        string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(QOpenGLFunctions* f, string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw(Shader* shader);
    
    private:
        QOpenGLFunctions* f;

        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);

        Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

#endif