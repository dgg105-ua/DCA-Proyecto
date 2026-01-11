#include <ResourceManager.hpp>
#include <iostream>

// Define RESOURCE_PATH si no está definida (fallback)
#ifndef RESOURCE_PATH
    #define RESOURCE_PATH "assets/"
#endif

//SPRITE
ResourceManager& ResourceManager::instance() {
    static ResourceManager rm;
    return rm;
}

std::string ResourceManager::getResourcePath(const std::string& relativePath) {
    std::string fullPath = std::string(RESOURCE_PATH) + relativePath;
    return fullPath;
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

// --- Audio helpers ---
std::string ResourceManager::resolveSoundPath(const std::string& baseName) {
    // Busca en assets/sounds con extensiones comunes
    const char* exts[] = { ".wav", ".ogg", ".mp3" };
    for (auto ext : exts) {
        std::string rel = std::string("sounds/") + baseName + ext;
        std::string full = getResourcePath(rel);
        // No tenemos acceso directo a filesystem portable aquí, raylib carga y valida
        // Intentamos cargar más adelante; aquí devolvemos el primero que funcione
        // Para consistencia, devolvemos la ruta completa candidata
        // La validación real se hará al cargar.
        // Preferimos .wav -> .ogg -> .mp3
        // Nota: Si no existe, LoadSound/LoadMusicStream fallará y lo manejamos.
        return full; // intentaremos la primera; si falla, probaremos la siguiente en carga
    }
    return getResourcePath(std::string("sounds/") + baseName + ".wav");
}

std::string ResourceManager::resolveMusicPath(const std::string& baseName) {
    const char* exts[] = { ".ogg", ".mp3", ".wav" };
    for (auto ext : exts) {
        std::string rel = std::string("sounds/") + baseName + ext;
        std::string full = getResourcePath(rel);
        return full;
    }
    return getResourcePath(std::string("sounds/") + baseName + ".ogg");
}

Sound& ResourceManager::getSoundByName(const std::string& baseName) {
    auto it = sounds.find(baseName);
    if (it != sounds.end()) {
        return it->second;
    }

    // Intentar cargar con diferentes extensiones hasta lograr un Sound válido
    const char* exts[] = { ".wav", ".ogg", ".mp3" };
    Sound loaded = {0};
    std::string chosenPath;
    for (auto ext : exts) {
        std::string rel = std::string("sounds/") + baseName + ext;
        std::string full = getResourcePath(rel);
        loaded = LoadSound(full.c_str());
        if (IsSoundValid(loaded)) {
            chosenPath = full;
            std::cout << "[ResourceManager] Sonido cargado: " << full << std::endl;
            break;
        }
    }

    if (!IsSoundValid(loaded)) {
        std::cerr << "[ResourceManager] ERROR cargando sonido: " << baseName << std::endl;
        // Crear un alias inválido para evitar crash al devolver referencia; seguirá siendo no válido.
        loaded.frameCount = 0;
    }

    auto [itInserted, _] = sounds.emplace(baseName, loaded);
    return itInserted->second;
}

Music& ResourceManager::getMusicByName(const std::string& baseName) {
    auto it = musics.find(baseName);
    if (it != musics.end()) {
        return it->second;
    }

    const char* exts[] = { ".ogg", ".mp3", ".wav" };
    Music loaded = {0};
    for (auto ext : exts) {
        std::string rel = std::string("sounds/") + baseName + ext;
        std::string full = getResourcePath(rel);
        loaded = LoadMusicStream(full.c_str());
        if (IsMusicValid(loaded)) {
            loaded.looping = true; // loop por defecto
            std::cout << "[ResourceManager] Música cargada: " << full << std::endl;
            break;
        }
    }

    if (!IsMusicValid(loaded)) {
        std::cerr << "[ResourceManager] ERROR cargando música: " << baseName << std::endl;
    }

    auto [itInserted, _] = musics.emplace(baseName, loaded);
    return itInserted->second;
}

void ResourceManager::updateMusic(const std::string& baseName) {
    auto it = musics.find(baseName);
    if (it != musics.end() && IsMusicValid(it->second) && IsMusicStreamPlaying(it->second)) {
        UpdateMusicStream(it->second);
    }
}

void ResourceManager::stopMusic(const std::string& baseName) {
    auto it = musics.find(baseName);
    if (it != musics.end() && IsMusicValid(it->second)) {
        StopMusicStream(it->second);
    }
}

void ResourceManager::stopAllMusic() {
    for (auto& kv : musics) {
        if (IsMusicValid(kv.second)) StopMusicStream(kv.second);
    }
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

    for (auto& kv : sounds) {
        if (IsSoundValid(kv.second)) {
            UnloadSound(kv.second);
        }
    }
    sounds.clear();

    for (auto& kv : musics) {
        if (IsMusicValid(kv.second)) {
            StopMusicStream(kv.second);
            UnloadMusicStream(kv.second);
        }
    }
    musics.clear();

    std::cout << "[ResourceManager] Recursos liberados" << std::endl;
}
//SPRITE
