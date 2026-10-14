/*
This File can be deleted in the future, CMAKE ENABLE_SCRIPTING is disabled and not needed for the compilation
enabling it requires adding cryptography for .c mod support, this file let's use libraries instead of .c mods
*/

#include "ModApi.h"

#include <spdlog/spdlog.h>
#include <exception>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <ship/Context.h>
#include <ship/resource/archive/Archive.h>
#include <ship/resource/archive/ArchiveManager.h>

#include "2s2h/ShipInit.hpp"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static constexpr const char* GetPlatformKey() {
#if defined(_WIN32)
    return "windows_x64";
#elif defined(__APPLE__)
    return "darwin";
#else
    return "linux_x64";
#endif
}

class ModLibrary {
  public:
    bool Load(const std::vector<char>& libraryImage) {
        const std::string path = WriteToTemporaryFile(libraryImage);
        if (path.empty()) {
            return false;
        }
#if defined(_WIN32)
        mHandle = LoadLibraryA(path.c_str());
        mFileToDelete = path;
#else
        mHandle = dlopen(path.c_str(), RTLD_NOW);
        unlink(path.c_str());
#endif
        return mHandle != nullptr;
    }

    void* GetFunction(const char* name) const {
#if defined(_WIN32)
        return (void*)GetProcAddress((HMODULE)mHandle, name);
#else
        return dlsym(mHandle, name);
#endif
    }

    void Unload() {
#if defined(_WIN32)
        FreeLibrary((HMODULE)mHandle);
        DeleteFileA(mFileToDelete.c_str());
#else
        dlclose(mHandle);
#endif
    }

  private:
    static std::string WriteToTemporaryFile(const std::vector<char>& libraryImage) {
        std::string path;

#if defined(_WIN32)
        char temporaryDirectory[MAX_PATH];
        char temporaryFile[MAX_PATH];
        if (GetTempPathA(MAX_PATH, temporaryDirectory) == 0 ||
            GetTempFileNameA(temporaryDirectory, "s2h", 0, temporaryFile) == 0) {
            return "";
        }
        path = temporaryFile;
#else
        char pathTemplate[] = "/tmp/s2h_mod_XXXXXX";
        int descriptor = mkstemp(pathTemplate);
        if (descriptor == -1) {
            return "";
        }
        fchmod(descriptor, 0755);
        close(descriptor);
        path = pathTemplate;
#endif

        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        output.write(libraryImage.data(), static_cast<std::streamsize>(libraryImage.size()));
        output.close();

        return output ? path : "";
    }

    void* mHandle = nullptr;
    std::string mFileToDelete;
};

class LoadedMods {
  public:
    ~LoadedMods() {
        for (auto& library : mLibraries) {
            library.Unload();
        }
    }

    void Add(const ModLibrary& library) {
        mLibraries.push_back(library);
    }

  private:
    std::vector<ModLibrary> mLibraries;
};

static LoadedMods sLoadedMods;

static void LoadMod(const std::shared_ptr<Ship::Archive>& archive, const std::string& binaryPath,
                    const std::string& name) {
    auto file = archive->LoadFile(binaryPath);
    if (file == nullptr || !file->IsLoaded) {
        SPDLOG_ERROR("[ModApi] Could not read '{}' from mod '{}'", binaryPath, name);
        return;
    }

    ModLibrary library;
    if (!library.Load(*file->Buffer)) {
        SPDLOG_ERROR("[ModApi] Mod '{}' could not be loaded", name);
        return;
    }

    auto setApi = (S2HModSetApiFunc)library.GetFunction("ModSetApi");
    if (setApi == nullptr) {
        SPDLOG_ERROR("[ModApi] Mod '{}' exports no ModSetApi, skipping it", name);
        library.Unload();
        return;
    }

    setApi(ModApi_Get());

    auto init = (S2HModInitFunc)library.GetFunction("ModInit");
    if (init != nullptr) {
        init();
    }

    sLoadedMods.Add(library);
    SPDLOG_INFO("[ModApi] Loaded mod '{}'", name);
}

static void LoadMods() {
    const std::string platform = GetPlatformKey();
    auto archives = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetArchives();

    for (const auto& archive : *archives) {
        const auto& manifest = archive->GetManifest();

        auto binaryEntry = manifest.Binaries.find(platform);
        if (binaryEntry == manifest.Binaries.end()) {
            continue;
        }

        try {
            LoadMod(archive, binaryEntry->second, manifest.Name);
        } catch (const std::exception& e) {
            SPDLOG_ERROR("[ModApi] Mod '{}' failed to load: {}", manifest.Name, e.what());
        }
    }
}

static RegisterShipInitFunc initFunc(LoadMods);
