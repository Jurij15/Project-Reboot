#include <MinHook.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "patterns.h"
#include "server.h"
#include "helper.h"
#include "processevent.h"
#include "inventory.h"
#include "abilities.h"
#include "build.h"
#include "gui.h"
#include "edit.h"
#include "loot.h"
#include "interaction.h"

// DEFINE_LOG_CATEGORY_STATIC(LogInit, ELogLevel::All)

DWORD WINAPI Input(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            Defines::bLogProcessEvent = !Defines::bLogProcessEvent;
            std::cout << "Set Defines::bLogProcessEvent to " << Defines::bLogProcessEvent << '\n';
        }

        else if (GetAsyncKeyState(VK_F3) & 1)
        {
            static auto BuildingItemCollectorClass = FindObject("/Script/FortniteGame.BuildingItemCollectorActor");

            std::cout << "BuildingItemCollectorClass: " << BuildingItemCollectorClass << '\n';

            auto BuildingItemCollectorActorActors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);

            std::cout << "Skid: " << BuildingItemCollectorActorActors.size() << '\n';

            // for (auto BuildingItemCollectorActor : BuildingItemCollectorActorActors)
            for (int i = 0; i < BuildingItemCollectorActorActors.size(); i++)
            {
                auto BuildingItemCollectorActor = BuildingItemCollectorActorActors.At(i);

                std::cout << "A!\n";

                static auto CollectorUnitInfoClassName = FindObject("/Script/FortniteGame.CollectorUnitInfo") ? "/Script/FortniteGame.CollectorUnitInfo" :
                    "/Script/FortniteGame.ColletorUnitInfo";

                static auto CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName);

                static auto CollectorUnitInfoClassSize = Helper::GetSizeOfClass(CollectorUnitInfoClass);

                static auto ItemCollectionsOffset = BuildingItemCollectorActor->GetOffset("ItemCollections");

                TArray<__int64>* ItemCollections = Get<TArray<__int64>>(BuildingItemCollectorActor, ItemCollectionsOffset); // CollectorUnitInfo

                static auto OutputItemOffset = FindOffsetStruct2(CollectorUnitInfoClassName, "OutputItem", false, true);

                static auto Def = FindObject("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03");

                *Get<UObject*>(ItemCollections->AtPtr(0, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
                *Get<UObject*>(ItemCollections->AtPtr(1, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
                *Get<UObject*>(ItemCollections->AtPtr(2, CollectorUnitInfoClassSize), OutputItemOffset) = Def;
            }

            BuildingItemCollectorActorActors.Free();
        }

        /* else if (GetAsyncKeyState(VK_F4) & 1)
        {
            auto LootDrops = Looting::PickLootDrops("Loot_AthenaFloorLoot");

            std::cout << "LootDrops: " << LootDrops.size() << '\n';

            for (auto& LootDrop : LootDrops)
            {
                auto LootDropDef = LootDrop.first;
            }
        } */

        else if (GetAsyncKeyState(VK_F5) & 1)
        {
            std::cout << MH_StatusToString(MH_CreateHook((PVOID)KickPlayerAddress, Server::Hooks::KickPlayer, (PVOID*)&Defines::KickPlayer)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)KickPlayerAddress)) << '\n';

            std::cout << MH_StatusToString(MH_CreateHook((PVOID)ValidationFailureAddress, Server::Hooks::ValidationFailure, (PVOID*)&Defines::ValidationFailure)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)ValidationFailureAddress)) << '\n';

            std::cout << MH_StatusToString(MH_CreateHook((PVOID)NoReserveAddress, Server::Hooks::NoReservation, (PVOID*)&Defines::NoReservation)) << '\n';
            std::cout << MH_StatusToString(MH_EnableHook((PVOID)NoReserveAddress)) << '\n';
        }

        else if (GetAsyncKeyState(VK_F6) & 1)
        {
            auto before = Defines::bShouldSpawnFloorLoot;
            Defines::bShouldSpawnFloorLoot = !Defines::bShouldSpawnFloorLoot;
            std::cout << "Set Defines::bShouldSpawnFloorLoot to " << !before << '\n';
        }

        else if (GetAsyncKeyState(VK_F7) & 1)
        {
            MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
            MH_EnableHook((PVOID)ProcessEventAddress);
        }

        else if (GetAsyncKeyState(VK_F8) & 1)
        {
            auto GameState = Helper::GetGameState();
            static auto WarmupCountdownEndTimeOffset = GameState->GetOffset("WarmupCountdownEndTime");
            static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
            auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);

            std::cout << "WarmupCountdownEndTime: " << *Get<float>(GameState, WarmupCountdownEndTimeOffset);

            *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::Warmup;

            std::cout << "WarmupCountdownEndTime: " << *Get<float>(GameState, WarmupCountdownEndTimeOffset);

            static auto OnRepGamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");

            GameState->ProcessEvent(OnRepGamePhase, &OldPhase);

            std::cout << "WarmupCountdownEndTime: " << *Get<float>(GameState, WarmupCountdownEndTimeOffset);
            std::cout << "TimeSeconds: " << Helper::GetTimeSeconds() << '\n';
        }

        else if (GetAsyncKeyState(VK_F9) & 1)
        {
            Defines::bShouldSpawnVehicles = !Defines::bShouldSpawnVehicles;
            std::cout << "Defines::bShouldSpawnVehicles: " << Defines::bShouldSpawnVehicles << '\n';
        }

        else if (GetAsyncKeyState(VK_F10) & 1)
        {
            Defines::bShouldSpawnForagedItems = !Defines::bShouldSpawnForagedItems;
            std::cout << "Defines::bShouldSpawnForagedItems: " << Defines::bShouldSpawnForagedItems << '\n';

            /* auto GameState = Helper::GetGameState();
            static auto GamePhaseOffset = GameState->GetOffset("GamePhase");
            auto OldPhase = *Get<EAthenaGamePhase>(GameState, GamePhaseOffset);
            *Get<EAthenaGamePhase>(GameState, GamePhaseOffset) = EAthenaGamePhase::None;

            static auto OnRepGamePhase = FindObject<UFunction>("/Script/FortniteGame.FortGameStateAthena.OnRep_GamePhase");

            GameState->ProcessEvent(OnRepGamePhase, &OldPhase); */
        }

        else if (GetAsyncKeyState(VK_F11) & 1)
        {
            auto GameMode = Helper::GetGameMode();

            static auto WarmupRequiredPlayerCountOffset = GameMode->GetOffset("WarmupRequiredPlayerCount");
            std::cout << "WarmupRequiredPlayerCountOffset: " << *(int*)(__int64(GameMode) + WarmupRequiredPlayerCountOffset) << '\n';

            auto GameState = Helper::GetGameState();
            static auto WarmupCountdownEndTimeOffset = GameState->GetOffset("WarmupCountdownEndTime");
            std::cout << "WarmupCountdownEndTime: " << *Get<float>(GameState, WarmupCountdownEndTimeOffset);    
        }

        else if (GetAsyncKeyState(VK_F12) & 1)
        {
            // dumps objects

            if (true)
            {
                auto ObjectNum = OldObjects ? OldObjects->Num() : NewObjects->Num();

                std::ofstream obj("ObjectsDump.txt");

                for (int i = 0; i < ObjectNum; i++)
                {
                    auto CurrentObject = GetObjectByIndex(i);

                    if (!CurrentObject)
                        continue;

                    obj << CurrentObject->GetFullName() << '\n';
                }
            }
            else
            {
                /* auto Loot_AthenaFloorLootDrops = Looting::PickLootDrops("Loot_AthenaFloorLoot");

                std::cout << "Size: " << Loot_AthenaFloorLootDrops.size() << '\n';

                for (auto& Drop : Loot_AthenaFloorLootDrops)
                {
                    auto Def = Drop.first;

                    std::cout << std::format("[{}] {}\n", Drop.second, Def ? Def->GetFullName() : "NULL");
                } */
            }
        }

        Sleep(1000 / 30);
    }
}

__int64 rettrue() { return 1; }

DWORD WINAPI Initialize(LPVOID)
{
    // if (FALSE && !TRUE)
    {
        AllocConsole();

        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w+", stdout);

        SetConsoleTitleA("Project Reboot V2");
    }

    // Log::Init();

    if (MH_Initialize() != MH_OK)
    {
        MessageBoxA(0, "MinHook failed to initialize", "Project Reboot V2", MB_ICONERROR);
        return 1;
    }

    if (!InitializePatterns())
    {
        MessageBoxA(0, "Failed to setup patterns", "Project Reboot V2", MB_ICONERROR);
        return 1;
    }

    CreateThread(0, 0, GuiThread, 0, 0, 0);

    MH_CreateHook((PVOID)CanActivateAbilityAddress, rettrue, nullptr); // TODO: Find a better fix
    MH_EnableHook((PVOID)CanActivateAbilityAddress);

    MH_CreateHook((PVOID)HandleReloadCostAddress, Inventory::HandleReloadCost, (PVOID*)&Defines::HandleReloadCost);
    MH_EnableHook((PVOID)HandleReloadCostAddress);

    MH_CreateHook((PVOID)ProcessEventAddress, ProcessEventDetour, (PVOID*)&ProcessEventO);
    MH_EnableHook((PVOID)ProcessEventAddress);

    if (WorldGetNetModeAddress && NoMCPAddress)
    {
        MH_CreateHook((PVOID)WorldGetNetModeAddress, rettrue, nullptr);
        MH_EnableHook((PVOID)WorldGetNetModeAddress);

        MH_CreateHook((PVOID)NoMCPAddress, rettrue, nullptr);
        MH_EnableHook((PVOID)NoMCPAddress);
    }
    else
    {
        std::cout << "Unable to enable GetNetMode fix!\n";
        std::cout << "NoMCPAddress: " << NoMCPAddress << '\n';
        std::cout << "WorldGetNetModeAddress: " << WorldGetNetModeAddress << '\n';
    }
    
    std::cout << "Initialized\n";
    // LOG(LogInit, All, L"Initialized");
    std::cout << "Fortnite_Season: " << Fortnite_Season << '\n';
    std::cout << "GiveAbilityS14ANDS15: " << Defines::GiveAbilityS14ANDS15 << '\n';
    std::cout << "GiveAbilityAddress: " << GiveAbilityAddress << '\n';
    std::cout << std::format("Base Address 0x{:x}\n", (uintptr_t)GetModuleHandleW(0));

    FFortItemEntry::ItemEntryStruct = FindObjectSlow("ScriptStruct /Script/FortniteGame.FortItemEntry", false);
    FastTArray::FastArraySerializerStruct = FindObjectSlow("ScriptStruct /Script/Engine.FastArraySerializer", false);
    Abilities::GameplayAbilitySpecClass = FindObjectSlow("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", false);
    Editing::EditToolDefinition = FindObject("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");

    auto PC = Helper::GetLocalPlayerController();

    std::cout << "PC: " << PC << '\n';

    static auto SwitchLevel = FindObject<UFunction>("/Script/Engine.PlayerController.SwitchLevel");

    FString Level = Defines::GetMapName();

    PC->ProcessEvent(SwitchLevel, &Level);
    
    CreateThread(0, 0, Input, 0, 0, 0);

    AddHook("/Script/Engine.GameModeBase.HandleStartingNewPlayer", HandleStartingNewPlayer);
    AddHook("/Script/Engine.GameMode.ReadyToStartMatch", ReadyToStartMatch);
    AddHook("/Script/Engine.PlayerController.ServerAcknowledgePossession", ServerAcknowledgePossession);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", Inventory::ServerExecuteInventoryItem);
    AddHook(Engine_Version >= 420 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"
        : "/Script/FortniteGame.FortPlayerController.ServerSpawnInventoryDrop", Inventory::ServerAttemptInventoryDrop);
    AddHook(Fortnite_Season < 13 ? "/Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"
        : "/Script/FortniteGame.FortPlayerPawn.ServerHandlePickupInfo", Inventory::ServerHandlePickup);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon", Inventory::ServerExecuteInventoryWeapon);

    AddHook("/Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor", Build::ServerCreateBuildingActor);
    AddHook("/Script/FortniteGame.FortDecoTool.ServerSpawnDeco", Build::ServerSpawnDeco);

    AddHook(Engine_Version < 423 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptInteract" :
        "/Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract", Interaction::ServerAttemptInteract);

    // if (false)
    {
        AddHook("/Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor", Editing::ServerBeginEditingBuildingActorHook);
        AddHook("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", Editing::ServerEditBuildingActorHook);
        AddHook("/Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor", Editing::ServerEndEditingBuildingActorHook);
    }
    
    AddHook("/Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied", ClientOnPawnDied);

    if (InternalTryActivateAbilityAddress && GiveAbilityAddress)
    {
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility", Abilities::ServerTryActivateAbility);
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch", Abilities::ServerAbilityRPCBatch);
        AddHook("/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData", Abilities::ServerTryActivateAbilityWithEventData);
    }

    AddHook(Fortnite_Version < 9 ? "/Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"
        : (Engine_Version < 424 ? "/Script/FortniteGame.FortPlayerController.ServerAttemptAircraftJump" 
            : "/Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump"), ServerAttemptAircraftJump);

    // Level.Free();

    auto matchmaking = Memory::FindPattern("83 BD ? ? ? ? 01 7F 18 49 8D 4D D8 48 8B D6 E8 ? ? ? ? 48");

    matchmaking = matchmaking ? matchmaking : Memory::FindPattern("83 7D 88 01 7F 0D 48 8B CE E8");

    Defines::bMatchmakingSupported = matchmaking;
    int idx = 0;

    if (Defines::bMatchmakingSupported) // now check if it leads to the right place and where the jg is at
    {
        for (int i = 0; i < 9; i++)
        {
            auto byte = (uint8_t*)(matchmaking + i);

            if (IsBadReadPtr(byte))
                continue;

            // std::cout << std::format("[{}] 0x{:x}\n", i, (int)*byte);

            if (*byte == 0x7F)
            {
                Defines::bMatchmakingSupported = true;
                idx = i;
                break;
            }

            Defines::bMatchmakingSupported = false;
        }
    }

    std::cout << "Matchmaking will " << (Defines::bMatchmakingSupported ? "be supported\n" : "not be supported\n");

    if (Defines::bMatchmakingSupported)
    {
        std::cout << "idx: " << idx << '\n';

        auto before = (uint8_t*)(matchmaking + idx);

        std::cout << "before byte: " << (int)*before << '\n';

        *before = 0x74;
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Initialize, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        MH_DisableHook(MH_ALL_HOOKS);
        // Log::Shutdown();
        break;
    }
    return TRUE;
}