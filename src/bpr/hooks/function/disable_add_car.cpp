// Fixed addresses for the inspected BPR executable. MSVC Win32/x86, C++17,
// /arch:SSE2 (not AVX: FXSAVE does not preserve upper YMM register halves).
// Call Install() after MH_Initialize(). Call AddCar() on the game thread,
// after the player's profile has loaded. A normal game save persists it.
#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <cstddef>
#include "../../app/app.hpp"

namespace CarUnlockControl
{

    constexpr std::uintptr_t AddCarAddress = 0x00A10BC0;
    constexpr std::uintptr_t DerivedCarsAddress = 0x06E75A80;

    struct CarData
    {
        std::uint64_t id;               // +00
        std::uint8_t colour;         // +08
        std::uint8_t palette;        // +09
        std::uint8_t unlockShown;    // +0A
        std::uint8_t category;       // +0B
        float deformation;          // +0C
        std::uint32_t unlockType;    // +10
        std::uint32_t reserved;      // +14; not assigned a semantic name
    };
    static_assert(sizeof(CarData) == 0x18);
    static_assert(offsetof(CarData, unlockType) == 0x10);

    template<class T>
    static T& At(void* object, std::uintptr_t offset)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(object) + offset);
    }

    using FindCarFn = CarData* (__thiscall*)(void*, std::uint64_t);
    using VehicleDataFn = void* (__thiscall*)(void*, std::uint64_t);
    using ProfileAddCarFn = CarData* (__thiscall*)(
        void*, std::uint64_t, std::uint32_t, std::uint32_t, void*);
    using RefreshFn = void (__thiscall*)(void*);

    static const auto FindCar = reinterpret_cast<FindCarFn>(0x00A304A0);
    static const auto GetVehicleData = reinterpret_cast<VehicleDataFn>(0x004B7080);
    static const auto ProfileAddCar = reinterpret_cast<ProfileAddCarFn>(0x00A0A3E0);
    static const auto RefreshCarCount = reinterpret_cast<RefreshFn>(0x00A0FF10);

    // The game writes through the returned pointer in several award callers.
    // Give those writes temporary storage, never an actual profile record.
    static thread_local CarData suppressedCar{};

    static CarData* __stdcall MakeSuppressedCar(
        void* manager, std::uint64_t vehicleId, std::uint32_t unlockType)
    {
        App::Instance->State().SendLocation(vehicleId);
        auto* profile = reinterpret_cast<std::uint8_t*>(manager) + 0x170;
        const auto* existing = FindCar(profile, vehicleId);
        if (existing)
            suppressedCar = *existing;
        else
            suppressedCar = {vehicleId, 0xFF, 0xFF, 1, 2, 0.0f, unlockType, 0};

        return &suppressedCar;
    }

    __declspec(naked) void BlockAddCar()
    {
        __asm
        {
            pushfd
            pushad
            mov ebx, esp
            sub esp, 528
            and esp, -16
            fxsave [esp]

            // Saved frame: ECX at +24; original arguments at +40/+44/+48.
            push dword ptr [ebx+48] // unlock type
            push dword ptr [ebx+44] // vehicle ID high half
            push dword ptr [ebx+40] // vehicle ID low half
            push dword ptr [ebx+24] // ProgressionManager*
            call MakeSuppressedCar
            mov dword ptr [ebx+28], eax // return our temporary CarData*

            fxrstor [esp]
            mov esp, ebx
            popad
            popfd
            ret 12 // 64-bit vehicle ID + 32-bit unlock type
        }
    }

    // This automatic derived-car routine re-finds the car after AddCar and
    // dereferences it without a null check. Skip the routine during suppression.
    // All normal garage lookup/check functions remain untouched.
    __declspec(naked) void BlockDerivedCars()
    {
        __asm { ret 4 }
    }

    // true: valid vehicle now has an ordinary, repaired record in the profile.
    // false: game/resources not ready, unknown ID, or profile capacity exhausted.
    // This does not trigger a disk save or refresh an already-open garage screen.
    bool AddCar(std::uint64_t vehicleId)
    {
        auto* game = *reinterpret_cast<std::uint8_t**>(0x013FC8E0);
        if (!game || At<std::uint32_t>(game, 0xB6D464) != 1)
            return false;

        // GameModule + 69B000 = GameStateModule; + CA10 = ProgressionManager.
        auto* manager = game + 0x6A7A10;
        auto* profile = manager + 0x170;
        auto* vehicleList = At<void*>(manager, 0x2C48C);
        if (!vehicleList || !At<void*>(manager, 0x2C418))
            return false;

        auto* vehicle = GetVehicleData(vehicleList, vehicleId);
        if (!vehicle)
            return false;

        const auto count = At<std::int32_t>(profile, 0x2A0);
        if (count < 0 || count > 512)
            return false;

        auto* car = FindCar(profile, vehicleId);
        if (!car && count == 512)
            return false; // Native AddCar would reuse/overwrite the last slot.

        if (!car)
        {
            const auto flags = At<std::uint32_t>(vehicle, 0x94);
            const auto* masks = reinterpret_cast<const std::uint32_t*>(0x00EDD560);
            std::uint32_t category = 2;
            for (std::uint32_t i = 0; i < 10; ++i)
            {
                if (flags & masks[i])
                {
                    category = i;
                    break;
                }
            }

            // The same profile writer used by the original manager routine.
            // It appends the record, increments the count and sets livery data.
            car = ProfileAddCar(profile, vehicleId, 0, category, vehicleList);
            if (!car)
                return false;
        }

        // Normalize the requested record to a normal unlock. In particular,
        // special record types 8/9 are excluded by the native save exporters.
        car->unlockType = 0;
        car->unlockShown = 1;
        car->deformation = 0.0f;
        RefreshCarCount(manager);
        return true;
    }

    MH_STATUS Install()
    {
        auto* add = reinterpret_cast<void*>(AddCarAddress);
        auto* derived = reinterpret_cast<void*>(DerivedCarsAddress);
        auto status = MH_CreateHook(add, reinterpret_cast<void*>(&BlockAddCar), nullptr);
        if (status != MH_OK)
            return status;

        status = MH_CreateHook(derived, reinterpret_cast<void*>(&BlockDerivedCars), nullptr);
        if (status != MH_OK)
        {
            MH_RemoveHook(add);
            return status;
        }

        return status;
    }
}
