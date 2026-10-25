# Burnout Paradise Remastered

## What does randomization do to this game?

In the Archipelago Randomizer, all Cars and Non-Burning Route Race Events are items shuffled in the multiworld. All Events, Car unlocks, license upgrades are locations. Sanity location include Smashes, Billboards, and super jumps.

## What are the goals of Burnout Paradise Remastered Archipelago?

- Reach a specified license

## Is there a tracker pack?

BPR AP Supports Universal Tracker. However, there is no map tracker. If anyone wished to make one, I would be happy to support them.

### Known Issues and how to report

- If you find a logic bug or mod bug issue, create an issue on the [AP World Issue Tracker](https://github.com/DanielSchaack/Archipelago-BurnoutParadiseRemastered/issues).

## Why not a Rom?

Burnout Paradise Remastered was the most accessible to develop for. There is a decomp being done on the XBOX 360 ROM which this mod could be updated to support once finished.

## Installing

## Required Software

- [Archipelago](https://github.com/ArchipelagoMW/Archipelago/releases).
- A Legal copy of Burnout Paradise Remastered

## Installation Procedures

## **WARNING THIS MOD LOADER IS CLOSED SOURCE. USE AT YOUR OWN RISK. I AM NOT RESPONSIBLE FOR IT OR ITS CODE**

### Install Mod Loader

1. Download and run BPR Modder from [https://bpr.bo98.uk/](https://bpr.bo98.uk/)
2. Run the .exe and hit install.
3. The window will say completed, hit the "Launch" Button
4. Enter your BPR Install Directory and hit OK
   - For steam, its STEAM_INSTALL\steamapps\common\BurnoutPR
   - For Origin, I have no Idea, and I have not tested it
5. Click Core bugfixes and install it
6. Close the Window
7. Navigate to your BurnoutPR\mods folder and delete the core-bugfixes.bprmod file

### Install Mod

**Archipelago mod is INCOMPATIBLE with any other mod that runs a .dll REMOVE ANY FILES IN YOUR MOD DIRECTORY**
1. Download the [Latest Mod Release](https://github.com/DanielSchaack/bpr-apclient/releases/latest).
2. Create a folder named "mods" in the games Install directory it does not already exist
    - For steam, its STEAM_INSTALL\steamapps\common\BurnoutPR
    - For Origin, I have no Idea, and I have not tested it
3. put the mod `bpr_apclient.dll` inside into the "mods" folder
4. Launch the game

## Installation Procedures on Linux

### Prerequisite

This setup has been written based on running the game directly through Steam using its built-in Proton 11.0-2/Proton Experimental. This setup may differ for other launchers.

### Installation Guide

1. Download mod installer [BPRModderInstaller.exe](https://bpr.bo98.uk/) and the [bpr_apclient.dll](https://github.com/DanielSchaack/bpr-apclient/releases/)
2. Execute and click through the installer running the command`protontricks-launch --appid 1238080 ~/Downloads/BPRModderInstaller.exe`
-> This will fail when clicking on 'Launch', but this prepares everything for the follow-up
3. Execute `protontricks-launch --appid 1238080   "~/.local/share/Steam/steamapps/compatdata/1238080/pfx/drive_c/Program Files (x86)/BPR Modder/BPR Modder.exe"`
4. Set games folder to (default) `S:\steamapps\common\BurnoutPR`
5. Install CoreFixes
6. Remove CoreFixes bprmod file from the mods folder using the command `rm ~/.local/share/Steam/steamapps/common/BurnoutPR/mods/core-bugfixes.bprmod`
7. Place the dll in the mods folder, i.e. by using the command `cp ~/Downloads/bpr_apclient.dll ~/.local/share/Steam/steamapps/common/BurnoutPR/mods/`

## How to Uninstall

Uninstalling is as simple as removing the dll file from the mods directory

