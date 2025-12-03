#include <ResourceManager.hpp>
#include <iostream>

//SPRITE
ResourceManager& ResourceManager::instance() {
    static ResourceManager rm;
    return rm;
}

Texture2D& ResourceManager::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second; // ya estaba cargada
    }

    Texture2D tex = LoadTexture(path.c_str());
    if (tex.id == 0) {
        std::cerr << "[ResourceManager] ERROR cargando textura: " << path << std::endl;
        // Textura de emergencia 1x1 blanca para no crashear
        Image img = GenImageColor(1, 1, WHITE);
        tex = LoadTextureFromImage(img);
        UnloadImage(img);
    } else {
        std::cout << "[ResourceManager] Textura cargada: " << path << std::endl;
    }

    auto [itInserted, _] = textures.emplace(path, tex);
    return itInserted->second;
}

Font& ResourceManager::getFont(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end()) {
        return it->second;
    }

    Font font = LoadFont(path.c_str());
    if (font.texture.id == 0) {
        std::cerr << "[ResourceManager] ERROR cargando fuente: " << path << std::endl;
        static Font defaultFont = GetFontDefault();
        return defaultFont;
    } else {
        std::cout << "[ResourceManager] Fuente cargada: " << path << std::endl;
    }

    auto [itInserted, _] = fonts.emplace(path, font);
    return itInserted->second;
}

void ResourceManager::unloadAll() {
    for (auto& kv : textures) {
        if (kv.second.id != 0) {
            UnloadTexture(kv.second);
        }
    }
    textures.clear();

    for (auto& kv : fonts) {
        if (kv.second.texture.id != 0) {
            UnloadFont(kv.second);
        }
    }
    fonts.clear();

    std::cout << "[ResourceManager] Recursos liberados" << std::endl;
}
//SPRITE
