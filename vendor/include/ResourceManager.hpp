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

    // Combina RESOURCE_PATH con una ruta relativa
    static std::string getResourcePath(const std::string& relativePath);

    // Devuelve una textura cargada. Si no está, la carga y la guarda.
    Texture2D& getTexture(const std::string& path);

    // Devuelve una fuente cargada. Si no está, la carga y la guarda.
    Font& getFont(const std::string& path);

    // Sonidos cortos (SFX)
    // Obtiene (y carga si es necesario) un sonido por nombre base dentro de assets/sounds.
    // Intenta extensiones comunes: .wav, .ogg, .mp3
    Sound& getSoundByName(const std::string& baseName);

    // Música (stream)
    // Obtiene (y carga si es necesario) una música por nombre base dentro de assets/sounds.
    // Intenta extensiones comunes: .wav, .ogg, .mp3. Configura loop = true por defecto.
    Music& getMusicByName(const std::string& baseName);

    // Actualiza el stream de una música por nombre (llamar cada frame si está sonando)
    void updateMusic(const std::string& baseName);

    // Detiene un tema musical específico si está cargado
    void stopMusic(const std::string& baseName);

    // Detiene todas las músicas conocidas
    void stopAllMusic();

    // Libera todos los recursos cargados
    void unloadAll();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Font>      fonts;

    // Audio
    std::unordered_map<std::string, Sound>     sounds; // clave: nombre base (p.ej. "Salto")
    std::unordered_map<std::string, Music>     musics; // clave: nombre base

    // Helpers
    std::string resolveSoundPath(const std::string& baseName);
    std::string resolveMusicPath(const std::string& baseName);
};
//SPRITE
