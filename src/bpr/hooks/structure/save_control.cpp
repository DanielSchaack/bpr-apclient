#include "detours.hpp"
#include "MinHook.h"
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "../../app/app.hpp"


namespace RedirectSave
{
    constexpr uintptr_t Address = 0x070c6a90;

    static void* Original = nullptr;

    using FunctionType = void(__thiscall*)(char* outputbuffer, const char* suffix);
    static std::string path;
    extern "C" const char* __cdecl GetSuffix(const char* suffix)
    {
        if (suffix != nullptr &&
            std::strcmp(suffix, "Save\\") == 0)
        {
            if (App::Instance->State().isDisconnected()){
                std::cout << "Loading with no connection to AP" << std::endl;
                return suffix;
            }
            path = std::format("AP_Save_{}_{}_\\", App::Instance->State().GetSeed(), App::Instance->State().GetSlot());
            return path.c_str();
        }

        return suffix;
    }


    extern "C" void __cdecl WriteOwnFile(const char* directory)
    {
        if (!directory)
            return;

        auto& state = App::Instance->State();

        const std::string filename = std::format(
            "apsave.json",
            state.GetSeed(),
            state.GetSlot()
        );
        const std::string expectedFolder = std::format("AP_Save_{}_{}_\\", App::Instance->State().GetSeed(), App::Instance->State().GetSlot());

        const std::filesystem::path dirPath(directory);

        if (dirPath.filename() != expectedFolder)
        {
            return;
        }

        const std::filesystem::path filePath = std::filesystem::path(directory) / filename;

        const bool file_exists = std::filesystem::exists(filePath);
        if (!state.HasApSaveData()){

            if(!file_exists){
                state.MarkSaveAsInitialized();
                std::cout << "Creating new AP save: "  << filePath << '\n';
                return;
            }

            std::ifstream file(filePath);

            if (!file)
            {
                std::cerr  << "Failed to open AP save for reading: " << filePath << '\n';
                return;
            }

            try
            {
                nlohmann::json json;
                file >> json;

                auto& data = state.GetSaveData();

                data = json.get<bpr::SaveData>();

                state.MarkSaveAsInitialized();

                std::cout << "Loaded AP save: " << filePath << '\n';
            }
            catch (const nlohmann::json::exception& e)
            {
                std::cerr
                    << "Failed to parse AP save "
                    << filePath
                    << ": "
                    << e.what()
                    << '\n';
            }

            return;
        }

         const auto& data = state.GetSaveData();

        std::ofstream file(
            filePath,
            std::ios::trunc
        );

        if (!file)
        {
            std::cerr << "Failed to open AP save for writing: " << filePath << '\n';
            return;
        }

        try
        {
            nlohmann::json json = data;

            file << json.dump(4);

            if (!file)
            {
                std::cerr << "Failed to write AP save: " << filePath << '\n';
                return;
            }

            std::cout << "Saved AP save: " << filePath << '\n';
        }
        catch (const nlohmann::json::exception& e)
        {
            std::cerr
                << "Failed to serialize AP save: "
                << e.what()
                << '\n';
        }
    }
    
    __declspec(naked) void Detour()
    {
        __asm
        {
            // Entry:
            //
            // ECX     = outputBuffer
            // [ESP]   = return address
            // [ESP+4] = original suffix

            // Save things we need.
            push ecx
            push dword ptr [esp + 8]   // original suffix

            // Get redirected suffix.
            push dword ptr [esp]       // original suffix
            call GetSuffix
            add esp, 4

            // Stack:
            // [esp]   = original suffix
            // [esp+4] = saved ECX
            // [esp+8] = return address
            // [esp+12]= caller's original suffix

            mov ecx, [esp + 4]         // restore outputBuffer

            // We need Original to see our new suffix as its [ESP+4].
            //
            // Call creates a new return address, so push the desired
            // suffix first.
            push eax
            call dword ptr [Original]

            // Original uses plain RET, so our pushed suffix remains.
            add esp, 4

            push dword ptr [esp + 4]   // saved outputBuffer
            call WriteOwnFile
            add esp, 4

            // Remove saved original suffix + ECX.
            add esp, 8

            // Caller stack is now exactly as it was when Detour entered.
            ret
        }
    }

    constexpr uintptr_t SaveStringAddress = 0x00DC8030;

    void RedirectSaveDirectory()
    {
        constexpr char replacement[] = "APSV\\";

        DWORD oldProtect{};

        VirtualProtect(
            reinterpret_cast<void*>(SaveStringAddress),
            sizeof(replacement),
            PAGE_READWRITE,
            &oldProtect
        );

        std::memcpy(
            reinterpret_cast<void*>(SaveStringAddress),
            replacement,
            sizeof(replacement)
        );

        VirtualProtect(
            reinterpret_cast<void*>(SaveStringAddress),
            sizeof(replacement),
            oldProtect,
            &oldProtect
        );
    }

    MH_STATUS Install()
    {
        // RedirectSaveDirectory();
        // return MH_OK;
        return MH_CreateHook(
            reinterpret_cast<void*>(Address),
            reinterpret_cast<void*>(&Detour),
            &Original
        );
    }
}