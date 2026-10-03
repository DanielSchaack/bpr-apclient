// Fixed addresses for the inspected BPR executable. MSVC Win32/x86, C++17,
// /arch:SSE2 (not AVX: FXSAVE does not preserve upper YMM register halves).
// Call Install() after MH_Initialize(). Call AddCar() on the game thread,
// after the player's profile has loaded. A normal game save persists it.
#include "bpr/core/vehicles.hpp"
#include "bpr/core/logger.hpp"
#include "bpr/app/app.hpp"
#include "bpr/hooks/profile_diff.hpp"
#include "detours.hpp"
#include "MinHook.h"
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace CarUnlockControl
{
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

    using AddCarFn = CarData* (__thiscall*)(
        void* manager,
        std::uint64_t vehicleId,
        std::uint32_t unlockType
    );

    // FUN_06e4a510 falls back to the first car with category == 2, otherwise the game crashes on cars with a higher category, like the DLC cars
    static void EnsureBaseCar(std::uint8_t* profile)
    {
        const auto count = At<std::int32_t>(profile, 0x2A0);
        if (count <= 0 || count > 512) return;

        auto* cars = reinterpret_cast<CarData*>(profile + 0x2B8);   // maCars, from the save loader
        for (int i = 0; i < count; ++i)
            if (cars[i].category == 2) return;

        Logger::Log(std::format("EnsureBaseCar: car {:#018x} category {} -> 2", cars[0].id, cars[0].category));
        cars[0].category = 2;
    }

    // The game writes through the returned pointer in several award callers.
    // Give those writes temporary storage, never an actual profile record.
    static thread_local CarData suppressedCar{};

    static CarData* __stdcall MakeSuppressedCar(void* manager, std::uint64_t vehicleId, std::uint32_t unlockType)
    {
        auto* profile = reinterpret_cast<std::uint8_t*>(manager) + 0x170;

        // Remove Car if not granted by AP, won't do anything if car is not in profile
        if(!App::Instance->State().GetSaveData().HasCar(vehicleId))
            CarUnlockControl::RemoveCar(vehicleId);

        // Car unlocked, grant location
        if (unlockType == static_cast<uint32_t>(UnlockType::Gift) ||
            unlockType == static_cast<uint32_t>(UnlockType::ShutdownRival) ||
            unlockType == static_cast<uint32_t>(UnlockType::Sponsor) ||
            unlockType == static_cast<uint32_t>(UnlockType::IslandGift))
          App::Instance->State().SendLocation(vehicleId >> 12);

        const auto* existing = FindCar(profile, vehicleId);
        if (existing){
            Logger::Log(std::format("Car {:#018x} with unlockType {} found, suppressing using existing data", vehicleId, unlockType));
            suppressedCar = *existing;
        } else{
            Logger::Log(std::format("Car {:#018x} with unlockType {} not found, suppresing", vehicleId, unlockType));
            suppressedCar = {vehicleId, 0xFF, 0xFF, 1, 2, 0.0f, unlockType, 0};
        }

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

    bool TryAddCar( void* profile, void* vehicleList, uint64_t vehicleId, CarData*& outCar)
    {
        CarData* car = FindCar(profile, vehicleId);

        if (!car)
        {
            auto* vehicle = GetVehicleData(vehicleList, vehicleId);
            if (!vehicle) return false;

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

            Logger::Log(std::format("ProfileAddCar creating new car: flags {:#010x}, category {}", flags, category));

            car = ProfileAddCar(profile, vehicleId, 0, category, vehicleList);
            if (!car) return false;

            Logger::Log(std::format("ProfileAddCar succeeded, car at {:#018x}", reinterpret_cast<std::uintptr_t>(car)));
        }

        outCar = car;
        return true;
    }

    void AddLiveries(uint8_t* profile, void* vehicleList, const VehicleInfo* v_info)
    {
        size_t adjacentCount = 0;
        for (int i = 0; i < max_livery_amount; ++i)
            if (v_info->adjacent_ids[i] != 0)
                ++adjacentCount;

        for (int i = 0; i < max_livery_amount; i++) {
            uint64_t id = v_info->adjacent_ids[i];
            if (id == 0)
                continue;

            CarData *adjacent_car = nullptr;
            if (!TryAddCar(profile, vehicleList, id, adjacent_car))
                continue;
            adjacent_car->category = static_cast<uint32_t>(v_info->category);
            adjacent_car->unlockShown = 1;
        }
    }

    bool AddLiveries(std::uint64_t archipelagoLiveryId)
    {
        auto *game = *reinterpret_cast<std::uint8_t **>(0x013FC8E0);
        if (!game || At<std::uint32_t>(game, 0xB6D464) != 1) {
            Logger::Log(std::format("Game not ready"));
            return false;
        }

        // GameModule + 69B000 = GameStateModule; + CA10 = ProgressionManager.
        auto *manager = game + 0x6A7A10;
        auto *profile = manager + 0x170;
        auto *vehicleList = At<void *>(manager, 0x2C48C);
        if (!vehicleList || !At<void *>(manager, 0x2C418)) {
            Logger::Log(std::format("Vehicle list not ready"));
            return false;
        }

        const VehicleInfo* v_info = FindVehicleByArchipelagoLiveryID(archipelagoLiveryId);
        if(!v_info)
            return false;

        AddLiveries(profile, vehicleList, v_info);
        return true;
    }

    bool AddCar(std::uint64_t vehicleId)
    {
        Logger::Log( std::format("AddCar called for vehicleId {:#018x}", vehicleId));

        auto *game = *reinterpret_cast<std::uint8_t **>(0x013FC8E0);
        if (!game || At<std::uint32_t>(game, 0xB6D464) != 1) {
            Logger::Log(std::format("Game not ready"));
            return false;
        }

        // GameModule + 69B000 = GameStateModule; + CA10 = ProgressionManager.
        auto *manager = game + 0x6A7A10;
        auto *profile = manager + 0x170;
        auto *vehicleList = At<void *>(manager, 0x2C48C);
        if (!vehicleList || !At<void *>(manager, 0x2C418)) {
            Logger::Log(std::format("Vehicle list not ready"));
            return false;
        }

        auto *vehicle = GetVehicleData(vehicleList, vehicleId);
        if (!vehicle) {
            Logger::Log( std::format("Vehicle data not found for {:#018x}", vehicleId));
            return false;
        }

        const auto count = At<std::int32_t>(profile, 0x2A0);
        Logger::Log(std::format("AddCar profile count = {}", count));
        if (count < 0 || count > 512) {
            Logger::Log(std::format("AddCar abort: count out of bounds"));
            return false;
        }

        auto *car = FindCar(profile, vehicleId);
        if (car)
            Logger::Log( std::format("AddCar found existing car for {:#018x} at {:#018x}", vehicleId, reinterpret_cast<std::uintptr_t>(car)));
        else
            Logger::Log( std::format("AddCar no existing car for {:#018x}", vehicleId));

        if (!car && count == 512) {
            Logger::Log(std::format("Count full and car not found"));
            return false; // Native AddCar would reuse/overwrite the last slot.
        }

        bool isNew = !car;
        if (!car && !TryAddCar(profile, vehicleList, vehicleId, car))
            return false;

        Logger::Log(std::format("Before normalizing car unlockType={}, category={}, unlockShown={}, deformation={}",
                                car->unlockType, car->category, car->unlockShown, car->deformation));

        const VehicleInfo *v_info = FindVehicleByID(vehicleId);
        if (v_info) {
            car->category = static_cast<uint32_t>(v_info->category);

            if(!App::Instance->State().GetSlotData().addedLiveryItems)
                AddLiveries(profile, vehicleList, v_info);

            // Normalize the requested record to a normal unlock
            if(static_cast<uint32_t>(v_info->unlock_type) < 8)
                car->unlockType = 0;
            else
            {
                car->unlockType = static_cast<uint32_t>(v_info->unlock_type);
            }
        }

        car->unlockShown = 1;
        car->deformation = 0.0f;

        Logger::Log(std::format("After normalized car: unlockType={}, category={}, unlockShown={}, deformation={}",
                                car->unlockType, car->category, car->unlockShown, car->deformation));

        EnsureBaseCar(profile);
        RefreshCarCount(manager);
        return true;
    }

    bool RemoveCar(std::uint64_t vehicleId)
    {
        Logger::Log(std::format("Requested removal for ID: {:#018x}", vehicleId));

        auto** gamePtr = reinterpret_cast<std::uint8_t**>(0x013FC8E0);
        if (!gamePtr || !*gamePtr || At<std::uint32_t>(*gamePtr, 0xB6D464) != 1)
        {
            Logger::Log(std::format("Game not ready"));
            return false;
        }

        auto* manager = *gamePtr + 0x6A7A10;
        auto* profile = manager + 0x170;

        auto* car = FindCar(profile, vehicleId);
        if (!car)
        {
            Logger::Log(std::format("Car not found in profile, nothing to remove"));
            return false;
        }

        Logger::Log(std::format("Car found in profile, beginning removal"));

        auto& count = At<std::int32_t>(profile, 0x2A0);
        if (count <= 0 || count > 512)
        {
            Logger::Log(std::format("Count out of bounds"));
            return false;
        }

        Logger::Log(std::format("{} Cars found in profile, finding car to remove's index", count));

        std::uintptr_t carAddr = reinterpret_cast<std::uintptr_t>(car);
        std::uintptr_t profileAddr = reinterpret_cast<std::uintptr_t>(profile);

        CarData* carsArray = nullptr;
        std::ptrdiff_t index = -1;

        // step through cars in profile to find index of car that should be removed
        for (std::uintptr_t base = profileAddr + 0x2A4; base <= carAddr; ++base)
        {
            if ((carAddr - base) % sizeof(CarData) == 0)
            {
                carsArray = reinterpret_cast<CarData*>(base);
                index = (carAddr - base) / sizeof(CarData);
                break;
            }
        }

        if (!carsArray || index < 0 || index >= count)
        {
            Logger::Log(std::format("Could not deduce valid array base or index"));
            return false;
        }

        Logger::Log(std::format("Deduced array base: {:#018x} | Index to remove: {}", reinterpret_cast<std::uintptr_t>(carsArray), index));

        // if not last index, shift all following car entries one spot earlier
        if (index < count - 1)
        {
            std::memmove(car, car + 1, (count - 1 - index) * sizeof(CarData));
        }

        // car to remove is in last index, removing it by resetting to 0
        std::memset(&carsArray[count - 1], 0, sizeof(CarData));
        count--;
        EnsureBaseCar(profile);

        Logger::Log(std::format("Car {} removed", vehicleId));

        RefreshCarCount(manager);
        Logger::Log(std::format("New count: {}", count));
        return true;
    }

    // This automatic derived-car routine re-finds the car after AddCar and
    // dereferences it without a null check. Skip the routine during suppression.
    // All normal garage lookup/check functions remain untouched.
    constexpr std::uintptr_t AddCarAddress = 0x00A10BC0;
    constexpr std::uintptr_t DerivedCarsAddress = 0x06E75A80;
    static AddCarFn OriginalAddCar = nullptr;
    __declspec(naked) void BlockDerivedCars()
    {
        __asm { ret 4 }
    }

    MH_STATUS Install()
    {
        ProfileDiff::Start();
        auto* add = reinterpret_cast<void*>(AddCarAddress);
        auto* derived = reinterpret_cast<void*>(DerivedCarsAddress);
        auto status = MH_CreateHook(add, reinterpret_cast<void*>(&BlockAddCar), reinterpret_cast<void**>(&OriginalAddCar));
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
