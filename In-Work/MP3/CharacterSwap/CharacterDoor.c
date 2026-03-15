// NAME: Character Door (Basic)
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: Number|PREFERRED_CHARACTER_CATEGORY|0|3|0
// PARAM: Number|OPEN_MODE|0|3|0
// PARAM: Number|PASSING_FEE|0|99|10
// PARAM: Number|CHANGE_MODE|0|3|0
// PARAM: Number|CHANGE_MODE_TURNS|1|10|3

// Development defines for VS Code
#ifndef PREFERRED_CHARACTER_CATEGORY
    #define PREFERRED_CHARACTER_CATEGORY 0  // 0=Plumber, 1=Princess, 2=Beast, 3=Villain
#endif

#ifndef OPEN_MODE
    #define OPEN_MODE 0  // 0=Matching Only, 1=Matching with Fees, 2=All but Matching, 3=Dynamic
#endif

#ifndef PASSING_FEE
    #define PASSING_FEE 10
#endif

#ifndef CHANGE_MODE
    #define CHANGE_MODE 0  // 0=NoChange, 1=Randomly, 2=Sequence, 3=EveryUse
#endif

#ifndef CHANGE_MODE_TURNS
    #define CHANGE_MODE_TURNS 3
#endif

//***************************************************************************//
//******************** Version Info! ****************************************//
//***************************************************************************//
// Version: 1.0 - Basic Implementation (Matching Only Mode)
// Repository: https://github.com/c-kennelly/mario-party-custom-events
//
// This is a basic implementation of the Character Door system that only
// supports "Matching Only" mode. It's structured to easily extend to the
// full system described in Spec.txt.

#include "ultra64.h"

// External declarations for future BoardRAM usage
extern u8 D_800CD0A0;  // board_ram9 - Could store door state
extern u8 D_800CD0A1;  // board_ram10 - Could store preferred category
extern u8 D_800CD0A2;  // board_ram11 - Could store change counter

// Character category constants
#define CATEGORY_PLUMBER   0
#define CATEGORY_PRINCESS  1  
#define CATEGORY_BEAST     2
#define CATEGORY_VILLAIN   3

// Open mode constants (for future expansion)
#define MODE_MATCHING_ONLY     0
#define MODE_MATCHING_WITH_FEES 1
#define MODE_ALL_BUT_MATCHING  2
#define MODE_DYNAMIC           3

// Function prototypes (for future expansion)
int GetCharacterCategory(int characterId);
int IsPassageAllowed(int currentCategory, int preferredCategory, int openMode);
void ProcessMatchingOnlyMode(int currentPlayer, int currentCategory, int preferredCategory);
void ProcessMatchingWithFeesMode(int currentPlayer, int currentCategory, int preferredCategory);
void ShowFlavorText(int preferredCategory, int isAllowed);

void main() {
    int currentPlayer = GetCurrentPlayerIndex();
    struct Player* player = GetPlayerStruct(currentPlayer);
    int characterId = player->character;
    
    // Get the character's category
    int currentCategory = GetCharacterCategory(characterId);
    int preferredCategory = PREFERRED_CHARACTER_CATEGORY;
    
    // For now, only implement Matching Only mode
    if (OPEN_MODE == MODE_MATCHING_ONLY) {
        ProcessMatchingOnlyMode(currentPlayer, currentCategory, preferredCategory);
    } else {
        // Placeholder for future modes - for now, default to matching only
        mp3_DebugMessage("Advanced modes not yet implemented");
        ProcessMatchingOnlyMode(currentPlayer, currentCategory, preferredCategory);
    }
    
    // TODO: Implement change mode logic here
    // TODO: Update BoardRAM for dynamic modes
}

// Core character categorization logic
int GetCharacterCategory(int characterId) {
    switch (characterId) {
        case 0: // Mario
        case 1: // Luigi
            return CATEGORY_PLUMBER;
        case 2: // Peach
        case 3: // Daisy
            return CATEGORY_PRINCESS;
        case 4: // Yoshi
        case 5: // DK
            return CATEGORY_BEAST;
        case 6: // Wario
        case 7: // Waluigi
            return CATEGORY_VILLAIN;
        default:
            return CATEGORY_PLUMBER; // Fallback
    }
}

// Core passage logic (extensible for other modes)
int IsPassageAllowed(int currentCategory, int preferredCategory, int openMode) {
    switch (openMode) {
        case MODE_MATCHING_ONLY:
            return (currentCategory == preferredCategory);
        case MODE_MATCHING_WITH_FEES:
            // TODO: Implement ally logic
            return 1; // Placeholder
        case MODE_ALL_BUT_MATCHING:
            return (currentCategory != preferredCategory);
        case MODE_DYNAMIC:
            // TODO: Check BoardRAM state
            return 1; // Placeholder
        default:
            return (currentCategory == preferredCategory);
    }
}

// Handle Matching Only mode behavior
void ProcessMatchingOnlyMode(int currentPlayer, int currentCategory, int preferredCategory) {
    int isAllowed = IsPassageAllowed(currentCategory, preferredCategory, MODE_MATCHING_ONLY);
    
    // Show appropriate flavor text
    ShowFlavorText(preferredCategory, isAllowed);
    
    if (isAllowed) {
        // Allow passage - player continues normally
        mp3_DebugMessage("Character door: Access granted");
    } else {
        // Block passage - player cannot continue
        mp3_DebugMessage("Character door: Access denied");
        // TODO: Implement actual movement blocking
        // For now, just show the message
    }
}

// Placeholder for Matching with Fees mode (future implementation)
void ProcessMatchingWithFeesMode(int currentPlayer, int currentCategory, int preferredCategory) {
    // TODO: Implement ally/enemy logic
    // TODO: Implement fee payment system
    // TODO: Handle different responses based on ally/enemy/matching status
    mp3_DebugMessage("Matching with Fees mode not yet implemented");
}

// Display flavor text based on category and access result
void ShowFlavorText(int preferredCategory, int isAllowed) {
    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    // Build the message with proper formatting
    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    
    if (isAllowed) {
        mplib_strncat(message, "Ah");
        mplib_strncat(message, "\x82");                         // ,
        mplib_strncat(message, " I can tell you");
        mplib_strncat(message, "\x0A");                         // Newline
        mplib_strncat(message, "\x1A\x1A\x1A\x1A");           // Standard padding for portrait
        
        // Add category-specific flavor text from Spec.txt
        switch (preferredCategory) {
            case CATEGORY_PLUMBER:
                mplib_strncat(message, "\x06");                 // Begin yellow color
                mplib_strncat(message, "wear a hat");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_PRINCESS:
                mplib_strncat(message, "\x05");                 // Begin pink color  
                mplib_strncat(message, "are royalty");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_BEAST:
                mplib_strncat(message, "\x04");                 // Begin green color
                mplib_strncat(message, "love brightly colored fruit");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_VILLAIN:
                mplib_strncat(message, "\x03");                 // Begin red color
                mplib_strncat(message, "have names that start with W");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
        }
        
        mplib_strncat(message, "\x85");                         // .
        mplib_strncat(message, "\x0A");                         // Newline  
        mplib_strncat(message, "\x1A\x1A\x1A\x1A");           // Standard padding for portrait
        mplib_strncat(message, "You may pass");
        mplib_strncat(message, "\x81");                         // !
        
    } else {
        mplib_strncat(message, "Hmm");
        mplib_strncat(message, "\x82");                         // ,
        mplib_strncat(message, " I only let those who");
        mplib_strncat(message, "\x0A");                         // Newline
        mplib_strncat(message, "\x1A\x1A\x1A\x1A");           // Standard padding for portrait
        
        // Add category-specific requirement text
        switch (preferredCategory) {
            case CATEGORY_PLUMBER:
                mplib_strncat(message, "\x06");                 // Begin yellow color
                mplib_strncat(message, "wear a hat");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_PRINCESS:
                mplib_strncat(message, "\x05");                 // Begin pink color
                mplib_strncat(message, "are royalty");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_BEAST:
                mplib_strncat(message, "\x04");                 // Begin green color
                mplib_strncat(message, "love brightly colored fruit");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
            case CATEGORY_VILLAIN:
                mplib_strncat(message, "\x03");                 // Begin red color
                mplib_strncat(message, "have names that start with W");
                mplib_strncat(message, "\x08");                 // Begin white (default) color
                break;
        }
        
        mplib_strncat(message, " pass through");
        mplib_strncat(message, "\x85");                         // .
        mplib_strncat(message, "\x0A");                         // Newline
        mplib_strncat(message, "\x1A\x1A\x1A\x1A");           // Standard padding for portrait
        mplib_strncat(message, "You shall not pass");
        mplib_strncat(message, "\x81");                         // !
    }
    
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

    mp3_ShowMessageWithConfirmation(5, message);  // Use DK portrait (character 5)
}

//***************************************************************************//
//************************ mp3lib Functions *******************************//
//***************************************************************************//
// NOTE - this instance of mp3lib is pared down just to the functions used

// These core functions are not in mp3lib.c but are available from symbols table
int GetCurrentPlayerIndex() {
    // Returns the index of the player whose turn it currently is
    return *(int*)0x800F32D4; // Current player index from symbols table
}

struct Player * GetPlayerStruct(s32 playerIndex) {
    // Returns a pointer to the Player struct for the given player index (0-3)
    struct Player* players = (struct Player*)0x800F6C84; // Player array base address
    return &players[playerIndex];
}

// Prints a message in game with the Millenium Star portrait.
// Does not wait for player confirmation.
void mp3_DebugMessage(char* message)
{
    mp3_ShowMessageWithConfirmation(-1, message);
}

// Helper function that shows a message and then tears the message box down
// after the player confirms the last box.  Don't use for prompt selection.
void mp3_ShowMessageWithConfirmation(int characterPortraitIndex, char* message)
{
    // This function assumes you aren't using the additional arguments
    // of ShowMessage() and hardcodes them to 0.  If you want to use them,
    // add them to the wrapper function and pass through.

    ShowMessage(characterPortraitIndex, message, 0, 0, 0, 0, 0);
    mp3_WaitForPlayerConfirmation();
    mp3_TeardownMessageBox();

}

// A wrapper for the "wait for confimration" command in MP3
void mp3_WaitForPlayerConfirmation()
{
    func_800EC9DC();    //Wait for confirmation
}

// Helper function that just does teardown of a message box.
// Split out so it can be used after waiting for confirmation 
// or getting a selection choice.
void mp3_TeardownMessageBox()
{
    CloseMessage();     //Close the message
    func_800EC6EC();    //Message box teardown
}

// Long-form implementation from:
// https://www.techiedelight.com/implement-strcpy-function-c/
//
// Function to implement strcpy() function
char* mplib_strcpy(char* destination, const char* source)
{
    // return if no memory is allocated to the destination
    if (destination == NULL)
        return NULL;
 
    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;
 
    // copy the C-string pointed by source into the array
    // pointed to by destination
    while (*source != '\0')
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    // include the terminating null character
    *destination = '\0';
 
    // destination is returned by standard strcpy()
    return ptr;
}

// Second implementation from this site:
// https://www.techiedelight.com/implement-strncat-function-c/#:~:text=The%20strncat()%20function%20appends,pointer%20to%20the%20destination%20string
// Stripped out the "num" prototype so that strncat always appends the full string passed, instead of a defined subset.

// Function to implement strncat() function in C
char* mplib_strncat(char* destination, const char* source)
{
    int i, j;
 
    // move to the end of destination string
    for (i = 0; destination[i] != '\0'; i++);
 
    // i now points to terminating null character in destination
 
    // Appends num characters of source to the destination string
    for (j = 0; source[j] != '\0'; j++)
        destination[i + j] = source[j];
 
    // null terminate destination string
    destination[i + j] = '\0';
 
    // destination is returned by standard strncat()
    return destination;
}

// Player struct definition for reference
struct Player {
    s8 unk0;
    s8 cpu_difficulty;
    s8 controller;
    u8 character;           // Character ID (0-7) - offset 3
    u8 flags;               // Bit 0: CPU player flag
    s8 pad0[5];             // Reserved space
    s16 coins;              // Current coin count - offset 10
    s16 minigame_coins;     // Mini-game coins - offset 12
    s8 stars;               // Star count - offset 14
    u8 cur_chain_index;     // Current board path chain - offset 15
    u8 cur_space_index;     // Current space in chain - offset 16
    u8 next_chain_index;    // Next chain destination - offset 17
    u8 next_space_index;    // Next space destination - offset 18
    // ... additional navigation fields
};
