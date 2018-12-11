#include <map>
#include "MaterialMap.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Bumpmap.hpp"

using namespace std;

MaterialMap::MaterialMap():materialID(1), textureID(1), bumpID(1){}

unsigned MaterialMap::addMaterial(Material * material)
{
    materialMap[materialID] = material;
    materialID++;
    return materialID-1;
}

unsigned MaterialMap::addTexture(Texture * texture)
{
    textureMap[textureID] = texture;
    textureID++;
    return textureID-1;
}

unsigned MaterialMap::addBumpmap(Bumpmap * bumpmap)
{
    bumpMap[bumpID] = bumpmap;
    bumpID++;
    return bumpID-1;
}

unsigned MaterialMap::findMaterialID(Material *material)
{
    unsigned id = 0;
    for(auto it = materialMap.begin(); it != materialMap.end(); ++it)
    {
        if(it->second == material)
        {
            id = it->first;
            break;
        }
    }
    return id;
}

unsigned MaterialMap::findTextureID(Texture * texture)
{
    unsigned id = 0;
    for(auto it = textureMap.begin(); it != textureMap.end(); ++it)
    {
        if(it->second == texture)
        {
            id = it->first;
            break;
        }
    }
    return id;
}

unsigned MaterialMap::findBumpID(Bumpmap * bumpmap)
{
    unsigned id = 0;
    for(auto it = bumpMap.begin(); it != bumpMap.end(); ++it)
    {
        if(it->second == bumpmap)
        {
            id = it->first;
            break;
        }
    }
    return id;
}

Material* MaterialMap::getMaterial(unsigned id)
{
    auto it = materialMap.find(id);
    if(it == materialMap.end()) return NULL;
    return it->second;
}

Texture* MaterialMap::getTexture(unsigned id)
{
    auto it = textureMap.find(id);
    if(it == textureMap.end()) return NULL;
    return it->second;
}

Bumpmap* MaterialMap::getBumpmap(unsigned id)
{
    auto it = bumpMap.find(id);
    if(it == bumpMap.end()) return NULL;
    return it->second;
}