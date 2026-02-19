#include <Textures.hpp>
#include <iostream>
#include <raylib.h>
#include <rlgl.h>
#include <unordered_map>

std::unordered_map<std::string, unsigned int> idMap;

unsigned int Textures::loadTexture(const std::string& resourceName, int mode) {
    if (idMap.contains(resourceName)) {
        return idMap.at(resourceName);
    }

    std::string path = "resources/" + resourceName;
    Image image = LoadImage(path.c_str());
    if (image.data == nullptr) {
        std::cerr << "Texture load failed: " << resourceName << std::endl;
        return 0;
    }

    auto id = rlLoadTexture(image.data, image.width, image.height, image.format, image.mipmaps);
    rlTextureParameters(id, RL_TEXTURE_MIN_FILTER, mode);
    rlTextureParameters(id, RL_TEXTURE_MAG_FILTER, mode);

    int mipmaps;
    rlGenTextureMipmaps(id, image.width, image.height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, &mipmaps);

    UnloadImage(image);
    idMap.insert(std::make_pair(resourceName, id));

    std::cout << resourceName << " -> " << id << std::endl; 

    return id;
}