#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include <string>
#include <vector>
#include <iostream>

// Prototipo funzione intelligente
unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model {
public:
    std::vector<Texture> textures_loaded;	
    std::vector<Mesh>    meshes;
    std::string directory;

    Model(std::string const &path) {
        loadModel(path);
    }

    void Draw(unsigned int shader) {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    void loadModel(std::string const &path) {
        Assimp::Importer importer;
        // Rimuoviamo FlipUVs perché spesso crea problemi con modelli scaricati
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERRORE::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        
        // Salva la directory del file .obj (es. assets/trees)
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash != std::string::npos) directory = path.substr(0, lastSlash);
        else directory = "";

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // 1. Processa Vertici
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            if(mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }

        // 2. Processa Indici
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        // 3. Processa Materiali
        if(mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            // Cerchiamo le texture Diffuse (Colore)
            // NOTA: Alcuni modelli usano BASE_COLOR invece di DIFFUSE, proviamo entrambi
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            
            // Se non trova diffuse, prova Base Color (standard moderno PBR)
            if(diffuseMaps.empty()) {
                std::vector<Texture> baseColorMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_diffuse");
                textures.insert(textures.end(), baseColorMaps.begin(), baseColorMaps.end());
            }
        }
        
        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++) {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            if(!skip) {
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
};

// --- FUNZIONE INTELLIGENTE PER TROVARE I FILE ---
unsigned int TextureFromFile(const char *path, const std::string &directory) {
    std::string filename = std::string(path);
    
    // 1. PULIZIA: Rimuovi percorsi assoluti strani dal .mtl (es. C:\Users\Artist\...)
    // Teniamo solo il nome del file (es. "Bark.jpg")
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    // 2. COSTRUZIONE PERCORSO: Proviamo a cercare in assets/trees/Texture/
    // Assumiamo che 'directory' sia "assets/trees"
    std::string finalPath = directory + "/Texture/" + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // Forza 4 canali (RGBA) per evitare bug di allineamento
    unsigned char *data = stbi_load(finalPath.c_str(), &width, &height, &nrComponents, 4); 
    
    if (data) {
        GLenum format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "✅ CARICATA TEXTURE: " << finalPath << std::endl;
    } else {
        // Fallback: Se fallisce, prova a cercare direttamente nella cartella dell'obj senza "Texture/"
        std::string fallbackPath = directory + "/" + filename;
        data = stbi_load(fallbackPath.c_str(), &width, &height, &nrComponents, 4);
        
        if(data) {
             GLenum format = GL_RGBA;
             glBindTexture(GL_TEXTURE_2D, textureID);
             glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
             glGenerateMipmap(GL_TEXTURE_2D);
             stbi_image_free(data);
             std::cout << "✅ CARICATA (FALLBACK): " << fallbackPath << std::endl;
        } else {
            std::cout << "❌ FALLITA: Impossibile trovare " << filename << " in " << directory << "/Texture/ o root." << std::endl;
            stbi_image_free(data);
        }
    }

    return textureID;
}
#endif