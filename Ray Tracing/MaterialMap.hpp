#pragma once

#include <map>
#include "Material.hpp"
#include "Texture.hpp"
#include "Bumpmap.hpp"

class MaterialMap
{
    unsigned materialID;
    unsigned textureID; 
    unsigned bumpID;

    std::map<unsigned, Material*> materialMap;
    std::map<unsigned, Texture*> textureMap;
    std::map<unsigned, Bumpmap*> bumpMap;
public:
    MaterialMap();
    unsigned addMaterial(Material * material);
    unsigned addTexture(Texture * texture);
    unsigned addBumpmap(Bumpmap * bumpmap);
    unsigned findMaterialID(Material *material);
    unsigned findTextureID(Texture * texture);
    unsigned findBumpID(Bumpmap * bumpmap);
    Material* getMaterial(unsigned id);
    Texture* getTexture(unsigned id);
    Bumpmap* getBumpmap(unsigned id);
};