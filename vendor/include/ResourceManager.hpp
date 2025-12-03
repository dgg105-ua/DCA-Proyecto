#pragma once

#include <string>
#include <unordered_map>

extern "C" {
    #include <raylib.h>
}

//SPRITE
class ResourceManager {
public:
    // Acceso único al gestor (singleton)
    static ResourceManager& instance();

    // Devuelve una textura cargada. Si no está, la carga y la guarda.
    Texture2D& getTexture(const std::string& path);

    // Devuelve una fuente cargada. Si no está, la carga y la guarda.
    Font& getFont(const std::string& path);

    // Libera todos los recursos cargados
    void unloadAll();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Font>      fonts;
};
//SPRITE
