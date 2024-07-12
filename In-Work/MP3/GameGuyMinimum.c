// NAME: Game Guy Minimum
// GAMES: MP3_USA
// EXECUTION: Direct
 // PARAM: +Number|COIN_MINIMUM


// Redefining params here for vs code highlighting, not actually used in partyplanner.
#ifndef COIN_MINIMUM
    #define COIN_MINIMUM 5
#endif

//***************************************************************************//
//******************** Version Info! ****************************************//
//***************************************************************************//
//
// This is version: 1.0
//
// There may be an update available at:
// https://github.com/c-kennelly/mario-party-custom-events
// You can also report a bug as an issue (and maybe a PR that fixes it?)


//***************************************************************************//
//******************** Event Configuration ***********************************//
//***************************************************************************//

// This is the portrait of the character giving the messages.
#define CHARACTER_PORTRAIT 0xA
// Want to change the picture?  Find options the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*********************** Description ***************************************//
//***************************************************************************//
// Setup as a passing event on Game Guy, this event gives coins to the player
// if the player is below the COIN_MINIMUM.  Use to modestly buff Game Guy utility
// as a comeback mechanism without affecting players with any significant number of coins.
// 
// Finally, this file is commented to make it as easy as possible for 
// non-programmers to edit and beginners to pull code samples, so I've 
// leaned on the verbose side for comments.  If you've a working knowledge 
// of C, feel free to just jump to main() and see what's going on.


//***************************************************************************//
//***********************     Changelist      *******************************//
//***************************************************************************//
//   Version 1.0 - First version of the event!


//***************************************************************************//
//*************************** Declarations **********************************//
//***************************************************************************//

// Used for the data types used in the player struct.
// Header file: http://n64devkit.square7.ch/header/ultra64.htm
// Ultratypes: http://n64devkit.square7.ch/header/ultratypes.htm
// For more exploration: http://n64devkit.square7.ch/header/
#include "ultra64.h"

// The Player struct for Mario Party 3, used for player manipulation.
// Reference wiki article can be found here:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Player-Structs
struct Player {
    s8 unk0;
    s8 cpu_difficulty;
    s8 controller;
    u8 character;
    u8 flags;               // Miscellaneous flags. "1" is CPU player
    s8 pad0[5];             // Skip undocumented offset 5-9
    s16 coins;              // Offset 10: Current coin count.
    s16 minigame_coins;     // Offset 12: Coins obtained during a Mini-Game.
    s8 stars;               // Offset 14
    
    u8 cur_chain_index;     // Offset 15
    u8 cur_space_index;     // Offset 16
    u8 next_chain_index;    // Offset 17
    u8 next_space_index;    // Offset 18
    u8 unk1_chain_index;    // Offset 19
    u8 unk1_space_index;    // Offset 20
    u8 reverse_chain_index; // Offset 21
    u8 reverse_space_index; // Offset 22

    u8 flags2;              // Offset 23
    u8 items[3];            // Offset 24
    u8 bowser_suit_flag;    // Offset 27
    u8 turn_color_status;   // Offset 28

    s8 pad1[7];             // Offsets: 29 - 35

    void *obj;              // Offset 36:  struct object *
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
    s8 game_guy_space_count; //Offset  52

    // s8 pad2[3];
}; // sizeof == 56


//***************************************************************************//
//*************************** Event Logic ************************************//
//***************************************************************************//

void main() 
{   
    int currentPlayerIndex = GetCurrentPlayerIndex();

    int coinReward = CalculateCoinReward(currentPlayerIndex);

    if(coinReward > 0)
    {
		char* message = GetFlavorMessage();
        mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);
        GraduallyAdjustPlayerCoins(currentPlayerIndex, coinReward);
    }
}

int CalculateCoinReward(int currentPlayerIndex)
{
    int currentCoins = 0;

    struct Player *p = GetPlayerStruct(currentPlayerIndex);
	currentCoins = p->coins;

	// If the player has less than the minimum coins, return the difference.
	if(currentCoins < COIN_MINIMUM)
	{
		return COIN_MINIMUM - currentCoins;
	}

	// Otherwise, return 0.
	return 0;
}

void GraduallyAdjustPlayerCoins(int playerIndex, int adjustmentAmount)
{   
    AdjustPlayerCoinsGradual(playerIndex, adjustmentAmount);
    ShowPlayerCoinChange(playerIndex, adjustmentAmount);

    // Sleep for 30 frames to let the coin change take effect.
    // Also syncs up nicely with the joy animation.
    SleepProcess(30);
}

char* GetFlavorMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                                   // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	                // Standard padding for portrait
    mplib_strncat(result, "Aw");                 
    mplib_strncat(result, "\x82");                                  // ,
    mplib_strncat(result, " you look a little down on your");
    mplib_strncat(result, "\x0A");                                  // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                      // Standard padding for portrait
    mplib_strncat(result, "luck");
    mplib_strncat(result, "\x85");                                  // .
    mplib_strncat(result, " Here are a few coins on");
    mplib_strncat(result, "\x0A");                                  // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                      // Standard padding for portrait
    mplib_strncat(result, "the house to keep it fun");
    mplib_strncat(result, "\xC2");                                  // !
    mplib_strncat(result, "\xFF");                                  //Show prompt to continue arrow

    return result;
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