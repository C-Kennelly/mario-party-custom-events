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
// At bottom of your event file:
//***************************************************************************//
//************************ mp3lib Functions *******************************//
//***************************************************************************//
// NOTE - this instance of mp3lib is pared down just to the functions used

// Copy only the functions you actually use:
int GetCurrentPlayerIndex() { /* implementation */ }
void AdjustPlayerCoinsGradual(int playerIndex, int coinAmount) { /* implementation */ }
struct Player * GetPlayerStruct(s32 playerIndex) { /* implementation */ }
```

## Development Workflow

### 1. Start New Event
```bash
# Copy template to In-Work directory
cp Complete/MP3/Templates/mp3_Event_Template.c In-Work/MP3/MyNewEvent.c
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

### 4. Testing & Finalization
- Test thoroughly in PartyPlanner64
- Copy needed `mp3lib.c` functions into event file
- Remove unused functions to save space
- Move to `Complete/MP3/` when ready

## Available Resources

### Community Symbols Table
**URL**: https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym

This contains discovered function addresses and RAM locations you can use:
```c
// Examples from symbols table:
extern void func_800F2304(s32, s32, s32);  // Animation function
extern void SleepProcess(s32);              // Process sleep
extern struct Player *GetPlayerStruct(s32); // Get player data
```

### Key mp3lib.c Functions
Most commonly used library functions:

**Player Management:**
- `GetCurrentPlayerIndex()` - Get the active player (0-3)  
- `AdjustPlayerCoinsGradual(playerIndex, amount)` - Animate coin changes
- `mp3_SwapPlayerStructs(p1, p2)` - Swap two players' complete data

**Debugging:**
- `mp3_DebugMessage(char* message)` - Display debug text
- `mp3_DebugPlayerIndex(playerIndex)` - Show player index

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
    u8 character;           // Character ID (0-7)
    u8 flags;               // Bit 0: CPU player flag
    s8 pad0[5];             // Reserved space
    s16 coins;              // Current coin count (offset 10)
    s16 minigame_coins;     // Mini-game coins (offset 12)
    s8 stars;               // Star count (offset 14)
    u8 cur_chain_index;     // Current board path chain (offset 15)
    u8 cur_space_index;     // Current space in chain (offset 16)
    u8 next_chain_index;    // Next chain destination (offset 17)
    u8 next_space_index;    // Next space destination (offset 18)
    // ... additional navigation fields
};
```

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
- Embedded in board `.json` files for complete board packages
- Uploaded to Mario Party Legacy forums or GitHub repositories

## Resources

- **PartyPlanner64 Wiki**: https://github.com/PartyPlanner64/PartyPlanner64/wiki
- **Symbols Database**: https://github.com/PartyPlanner64/symbols
- **Event Repository**: https://github.com/PartyPlanner64/events  
- **Community Events**: https://www.mariopartylegacy.com/forum/index.php?action=downloads;cat=4
- **This Repository**: https://github.com/c-kennelly/mario-party-custom-events
