// Refer to the learnopengl library

#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <learnopengl/mesh.h>
#include <learnopengl/shader_m.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#pragma comment(lib, "assimp.lib")

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model {
public:
    /*   Model data   */
    vector<Texture> textures_loaded; // Save all loaded textures to optimize the same textures that are reused
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    // Constructor, read model from path
    Model(string const& path, bool gamma = false)
        : gammaCorrection(gamma)
    {
        loadModel(path);
    }

    // draw all meshes of the model
    void Draw(Shader shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++) {
            meshes[i].Draw(shader);
        }
    }

private:

    // Load model from file supports ASSIMP extension and store in mesh generated mesh vector
    void loadModel(string const& path)
    {
        // Load the model file via ASSIMP
        Assimp::Importer importer;
        // parameters: transform all primitive shapes of the model into triangles | flip the texture coordinates of the y-axis during processing | calculate the soft Tangent and Bitangent vectors for each loaded vertex
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // determine if there is an error
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // Retrieve the directory path of the file path
        directory = path.substr(0, path.find_last_of('/'));

        // Recursively process the root node of ASSIMP
        processNode(scene->mRootNode, scene);
    }

    // Process nodes recursively
   // Process each individual mesh in the node and repeat this process for its children (if any)
   // scene contains all data, node is used to express the relationship between parts
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process all meshes of nodes
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // Then repeat this process for its child nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // Traverse all vertices of the mesh
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            // Because assimp uses its own vector class, this class is not directly converted to glm's vec3 class, so we first transfer the data to this placeholder glm::vec3
            glm::vec3 vector;
            // location
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normal
            if (mesh->mNormals) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            } else {
                vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            // texture coordinates
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                // Since a vertex can contain 8 different texture coordinates, we assume we won't use a model where one vertex can have multiple texture coordinates, so we always take 0
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            // tangent
            if (mesh->mTangents) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
            } else {
                vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            // bitangent
            if (mesh->mBitangents) {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            } else {
                vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            vertices.push_back(vertex);
        }

        // handle the index
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            // Retrieve the indices of all faces, store the index vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // handle the material
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Assume that the material-related variable names in the shader follow the following specifications: (N is a number from 0 to the maximum value)
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN
        // height: texture_heightN

        // diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a Mesh object
        return Mesh(vertices, indices, textures);
    }

    // load material texture
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // Check if the structure is loaded, skip if so
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // The same texture path has already been loaded and can be skipped
                    break;
                }
            }
            // not loaded
            if (!skip) {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // Add to the loaded texture
            }
        }
        return textures;
    }
};

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif