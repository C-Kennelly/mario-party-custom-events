# Mario Party 3 Custom Events Development Guide

## Repository Overview

This repository contains custom events for Mario Party 3 that are compatible with PartyPlanner64. Each event is a self-contained C file that gets compiled and embedded directly into ROM patches.

### Directory Structure

```
mario-party-custom-events/
├── Complete/MP3/           # ✅ Finished, tested events ready for distribution
├── In-Work/MP3/           # 🔄 Work-in-progress events under development
├── Complete/MP3/Templates/ # 📋 Development templates and library functions
│   ├── mp3_Event_Template.c   # Starting template for new events
│   └── mp3lib.c              # Helper function library
└── vs-highlighting/        # N64 SDK headers for VS Code IntelliSense
    └── ultra64/            # Contains ultra64.h and PR/ headers
```

## Critical Development Constraints

### Self-Contained Files Requirement
⚠️ **CRITICAL**: Each event file must be completely self-contained because:
- **No C Standard Library**: `stdio.h`, `stdlib.h`, `string.h` etc. are not available
- **No Linking**: Events are compiled in isolation and inserted directly into ROM
- **No External Dependencies**: Cannot reference external libraries or separate object files

### Library Function Integration Strategy
Since we cannot link external libraries, use this copy-paste approach:

1. **During Development**: Reference `mp3lib.c` functions for prototyping
2. **Before Release**: Copy only the needed functions directly into your event file
3. **Optimize**: Remove unused functions to minimize ROM footprint

**Example Integration Pattern:**
```c
// At bottom of your event file - copy the mplib v2.3 header and only the functions you use:
//***************************************************************************//
//*************************      mplib v2.3        **************************//
//***************************************************************************//

void mp3_DebugMessage(char* message) { ... }
void mp3_ShowMessageWithConfirmation(int portrait, char* message) { ... }
char* mplib_strcpy(char* destination, const char* source) { ... }
```

⚠️ **NEVER implement game symbols as inline functions.** Functions like `GetCurrentPlayerIndex()`, `GetPlayerStruct()`, `ShowMessage()`, `AdjustPlayerCoinsGradual()` etc. are resolved from the MP3 symbols table at link time — defining them yourself will produce incorrect behavior. Only `mplib_*` and `mp3_*` helper functions from `mp3lib.c` should be copied in.

## Development Workflow

### 1. Start New Event
```powershell
# Copy template to In-Work directory
Copy-Item Complete/MP3/Templates/mp3_Event_Template.c In-Work/MP3/MyNewEvent.c
```

### 2. Configure Event Header
Update the required PartyPlanner64 headers:
```c
// NAME: Descriptive Event Name 
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: Boolean|USES_BOARD_RAM
// PARAM: +Number|COIN_REWARD
```

### 3. Implement Event Logic
- Use `#include "ultra64.h"` for N64 basic types (`u8`, `s16`, `u32`, etc.)
- Reference community symbols from https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym
- Call game functions directly: `func_80123456()` 
- Access RAM locations: `D_80123456`

### 4. Compile Test with CLI
Before loading into PartyPlanner64, verify compilation using the PartyPlanner64 CLI.

The CLI lives in the PartyPlanner64 repository. On this machine it is expected at:
```
C:/dev/c/PartyPlanner64/apps/cli/dist/out.js
```
If that path does not exist, ask the user where PartyPlanner64 is located before proceeding.

```powershell
node C:/dev/c/PartyPlanner64/apps/cli/dist/out.js test-event `
  --source-file In-Work/MP3/MyEvent/MyEvent.c `
  --game MP3 `
  --verbose
```
Output is JSON. A passing event looks like:
```json
{"status":"SUCCESS","file":"...","timestamp":"..."}
```
A failing event includes an error message:
```json
{"status":"FAILURE","file":"...","error":{"message":"...","type":"CompilationError"}}
```
**Always fix compilation errors before loading into PartyPlanner64.**

### 5. Testing & Finalization
- Test thoroughly in PartyPlanner64
- Copy needed `mp3lib.c` functions into event file
- Remove unused functions to save space
- Move to `Complete/MP3/` when ready

## Available Resources

### Community Symbols Table

The symbols table is the authoritative source for game function addresses and RAM locations. **Always verify a symbol exists in the correct game's symbol file before using it** — symbols are not shared across games and addresses differ between versions.

**Symbol files are at**: https://github.com/PartyPlanner64/symbols

The filename convention is `MarioParty{N}{Region}.sym`. To determine which file to use, read the `// GAMES:` directive at the top of the event file:

| `// GAMES:` directive | Symbol File | URL |
|-----------------------|-------------|-----|
| `MP1_USA` | `MarioParty1U.sym` | https://github.com/PartyPlanner64/symbols/blob/master/MarioParty1U.sym |
| `MP2_USA` | `MarioParty2U.sym` | https://github.com/PartyPlanner64/symbols/blob/master/MarioParty2U.sym |
| `MP3_USA` | `MarioParty3U.sym` | https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym |

> Japan (`J`) and Europe (`E`) variants exist but have different addresses. Always match the region suffix in the directive (`_USA` → `U`) to the symbol file region letter.

**How to verify a symbol:** Search the `.sym` file for the function or RAM address name. Each line has the format:
```
<address>,<type>,<name>,<description>
```
For example:
```
800F2130,code,GetCurrentPlayerIndex
800F213C,code,GetPlayerStruct,A0=player_index pass -1 to get current player's struct
800EC8EC,code,ShowMessage,A0=character_index,A1=string index/pointer,...
```
If a name does not appear in the symbol file for the correct game version, do not use it.

### Key mp3lib.c Functions
These are helper functions defined in `Complete/MP3/Templates/mp3lib.c` that you copy into your event file. **Do not confuse these with game symbols** — game symbols like `GetCurrentPlayerIndex()`, `GetPlayerStruct()`, `ShowMessage()`, and `AdjustPlayerCoinsGradual()` are called directly without any definition in the event file.

**Player Helpers (mp3lib):**
- `mp3_SwapPlayerStructs(p1, p2)` - Swap two players' complete data
- `mp3_IsPlayerCertainCharacter(playerIndex, character)` - Check a player's character
- `mp3_GetCharacterNameStringFromPlayerIndex(playerIndex)` - Get character name string

**Debugging:**
- `mp3_DebugMessage(char* message)` - Display debug text (no confirmation)
- `mp3_DebugMessageWithConfirmation(char* message)` - Display debug text and wait
- `mp3_DebugPrintPlayerIndex(playerIndex)` - Show player index as a message

**Scene Control:**
- `mp3_ReloadCurrentSceneWithTransition(transitionType)` - Reload with fade effect

**Utilities:**
- `mplib_max(a, b)`, `mplib_min(a, b)` - Math functions
- `mplib_strcpy(dest, src)` - String copying (no `<string.h>` available)
- `mplib_SwapBitsForInteger(val, pos1, pos2)` - Bit manipulation

### Player Struct Definition
```c
struct Player {
    s8 unk0;
    s8 cpu_difficulty;
    s8 controller;
    u8 character;           // Character ID: Mario=0,Luigi=1,Peach=2,Yoshi=3,Wario=4,DK=5,Waluigi=6,Daisy=7
    u8 flags;               // Bit 0: CPU player flag
    s8 pad0[5];             // Reserved space
    s16 coins;              // Current coin count (offset 10)
    s16 minigame_coins;     // Mini-game coins (offset 12)
    s8 stars;               // Star count (offset 14)
    u8 cur_chain_index;     // Current board path chain (offset 15)
    u8 cur_space_index;     // Current space in chain (offset 16)
    u8 next_chain_index;    // Next chain destination (offset 17)
    u8 next_space_index;    // Next space destination (offset 18)
    u8 unk1_chain_index;    // Offset 19
    u8 unk1_space_index;    // Offset 20
    u8 reverse_chain_index; // Offset 21
    u8 reverse_space_index; // Offset 22
    u8 flags2;              // Offset 23
    u8 items[3];            // Offset 24
    u8 bowser_suit_flag;    // Offset 27
    u8 turn_color_status;   // Offset 28
    s8 pad1[7];             // Offsets 29-35
    void *obj;              // Offset 36: struct object pointer
    s16 minigame_star;      // Offset 40
    s16 coin_star;          // Offset 42
    s8 happening_space_count; // Offset 44
    s8 red_space_count;
    s8 blue_space_count;
    s8 chance_space_count;
    s8 bowser_space_count;  // Offset 48
    s8 battle_space_count;
    s8 item_space_count;
    s8 bank_space_count;
    s8 game_guy_space_count; // Offset 52
    char unk_35[3];         // Padding to reach sizeof == 56 — DO NOT OMIT
}; // sizeof == 56
```

⚠️ The trailing `char unk_35[3]` padding is required. Omitting it makes the struct 53 bytes instead of 56, which corrupts `GetPlayerStruct` for player indices 1–3.

## Board RAM Management

### Shared Memory Conflicts
⚠️ **WARNING**: Events that use Board RAM may conflict with each other!

**Available Board RAM addresses:**
```c
extern u8 D_800CD0A0;  // board_ram9
extern u8 D_800CD0A1;  // board_ram10  
extern u8 D_800CD0A2;  // board_ram11
extern u8 D_800CD0A3;  // board_ram12
extern u8 D_800CD0A4;  // board_ram13
extern u8 D_800CD0A5;  // board_ram14
extern u8 D_800CD0A6;  // board_ram15
extern u8 D_800CD0A7;  // board_ram16
extern u8 D_800CD0A8;  // board_ram17
```

**Best Practice**: Document which Board RAM addresses your event uses and provide a fallback parameter to disable Board RAM usage.

## Template Structure

### Required Event File Format
```c
// PartyPlanner64 headers (REQUIRED)
// NAME: Event Description
// GAMES: MP3_USA  
// EXECUTION: Direct
// PARAM: Boolean|EnableFeature

// Development defines for VS Code (optional)
#ifndef COIN_REWARD
    #define COIN_REWARD 10
#endif

// Version and documentation
//***************************************************************************//
//******************** Version Info! ****************************************//
//***************************************************************************//
// Version: 1.0
// Repository: https://github.com/c-kennelly/mario-party-custom-events

// Include N64 types
#include "ultra64.h"

// External declarations for RAM access
extern u8 D_800CD0A0;  // Board RAM if needed

// Event implementation
void main() {
    // Your event logic here
    int currentPlayer = GetCurrentPlayerIndex();
    // ... rest of implementation
}

//***************************************************************************//
//************************ Library Functions *******************************//
//***************************************************************************//
// Copy needed functions from mp3lib.c here before release
```

## Common Patterns

### Simple Events
For basic animations or single actions:
```c
void main() {
    func_800F2304(-1, 5, 0);  // Joy animation
    SleepProcess(35);         // Wait for animation
}
```

### Interactive Events  
For events with player choices:
```c
void main() {
    int currentPlayer = GetCurrentPlayerIndex();
    
    // Show message and get user input
    int choice = DisplayMessage(/* parameters */);
    
    if (choice == 1) {
        AdjustPlayerCoinsGradual(currentPlayer, COIN_REWARD);
    }
}
```

### Multi-Turn Events
For events that persist across turns using Board RAM:
```c
void main() {
    if (D_800CD0A0 == 0) {
        // First activation - initialize
        D_800CD0A0 = 1;
        D_800CD0A1 = GetCurrentPlayerIndex();
    }
    
    // Check if conditions met for later turns
    if (/* some condition */) {
        // Trigger final event
        D_800CD0A0 = 0;  // Reset for next time
    }
}
```

## Testing & Debugging

### Compile Testing with the PartyPlanner64 CLI
The PartyPlanner64 CLI at `C:/dev/c/PartyPlanner64/apps/cli/dist/out.js` (in the separate PartyPlanner64 repository — ask the user for the path if it does not exist) provides a `test-event` command that compiles a `.c` event file and reports success or failure without needing to open the UI.

```powershell
# Basic usage
node C:/dev/c/PartyPlanner64/apps/cli/dist/out.js test-event --source-file <path-to-event.c> --game MP3

# With verbose output (shows game version and parameter info)
node C:/dev/c/PartyPlanner64/apps/cli/dist/out.js test-event --source-file <path-to-event.c> --game MP3 --verbose
```

**Arguments:**
- `--source-file` (required): Absolute or relative path to the `.c` event file
- `--game` (optional): Game version to target. Accepts `MP1`, `MP2`, `MP3` (or `_USA` suffix). Defaults to the first game listed in the event's `// GAMES:` header.
- `--verbose` (optional): Prints the target game and detected parameters before the result JSON.

**Output format** (always JSON on stdout):
```json
// Success
{"status":"SUCCESS","file":"path/to/event.c","timestamp":"2026-01-01T00:00:00.000Z"}

// Failure
{"status":"FAILURE","file":"path/to/event.c","timestamp":"...","error":{"message":"Line 42: ...","type":"CompilationError"}}
```

Run this after every edit. Fix all `FAILURE` results before loading into PartyPlanner64.

### PartyPlanner64 Integration
1. Import your `.c` file into PartyPlanner64
2. Place event on a board space
3. Test in Project64 emulator with debugger enabled

### Debug Techniques
- Use `mp3_DebugMessage()` for runtime logging  
- Set breakpoints at `807B0000` + offset in Project64 debugger
- Memory viewer to inspect Board RAM values during execution
- Enable Advanced/Debug UI in PartyPlanner64 to see space indices

## Distribution

### Before Release Checklist
- [ ] Remove unused `mp3lib.c` functions to save ROM space
- [ ] Document Board RAM usage and conflicts  
- [ ] Test on actual hardware or accurate emulator
- [ ] Include version information and changelog
- [ ] Verify event works with USES_BOARD_RAM = False

### Sharing Events
Events can be shared as:
- Individual `.c` files for import into PartyPlanner64

## Resources

- **PartyPlanner64 Wiki**: https://github.com/PartyPlanner64/PartyPlanner64/wiki
- **Symbols Database**: https://github.com/PartyPlanner64/symbols
- **Event Repository**: https://github.com/PartyPlanner64/events  
- **Community Events**: https://www.mariopartylegacy.com/forum/index.php?action=downloads;cat=4
- **This Repository**: https://github.com/c-kennelly/mario-party-custom-events
