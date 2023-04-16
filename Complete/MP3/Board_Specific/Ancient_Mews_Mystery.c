// NAME: Ancient Mew's Mystery
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: +Number|HAPPENING_COIN_MULTIPLIER
// PARAM: +Number|MIN_HAPPENING_SPACES
// PARAM: +Number|MAX_HAPPENING_SPACES

// The step-value for happening spaces - the happening space count is multiplied (with a little fuzz) by this number
// For example, if this value is 3, each "step" becomes 3.  So the possible rewards are:
// 1 | 1-5
// 2 | 4-7
// 3 | 7-11
// 4 | 10-14
// 5 | 13-17
#ifndef HAPPENING_COIN_MULTIPLIER
#define HAPPENING_COIN_MULTIPLIER 0
#endif

// If the player has landed on less happening spaces than this number, they won't get any coins.
#ifndef MIN_HAPPENING_SPACES
#define MIN_HAPPENING_SPACES 0
#endif

// Caps the number of happening spaces the player can get coins for to keep the economy in check.  
// Player receives the flat HAPPENING_COIN_MULTIPLIER * Happening Spaces when they hit this cap.
#ifndef MAX_HAPPENING_SPACES
#define MAX_HAPPENING_SPACES 0
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
#define CHARACTER_PORTRAIT -1
// Want to change the picture?  Find options the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*********************** Description ***************************************//
//***************************************************************************//
// Intended as a passing event near the start of a baord, Ancient Mew's Mystery 
// gives a a flavorful "get coins for passing go" event, giving you more coins 
// if the player's landed on happening spaces.
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
    int happeningSpaceCount = GetHappeningSpaceCount(currentPlayerIndex);

    if(happeningSpaceCount >= MIN_HAPPENING_SPACES)
    {
        int coinReward = CalculateCoinReward(happeningSpaceCount);

        PlayFlavorMessage(happeningSpaceCount);
        GraduallyAdjustPlayerCoins(currentPlayerIndex, coinReward);
    }
    else
    {
        PlayMessageForNoCoins();
    }
    
    return;
}

int GetHappeningSpaceCount(int currentPlayerIndex)
{
    int happeningCount = 0;
    struct Player *currentPlayerStruct = GetPlayerStruct(currentPlayerIndex);

    if (currentPlayerStruct != NULL)
    {
        happeningCount = currentPlayerStruct->happening_space_count;
    }

    return happeningCount;
}

int CalculateCoinReward(int happeningSpaceCount)
{
    if( (MAX_HAPPENING_SPACES > 0) && (happeningSpaceCount >= MAX_HAPPENING_SPACES) )
    {
        return (HAPPENING_COIN_MULTIPLIER *happeningSpaceCount);
    }

    int fuzzFactor = mp3_PickARandomNumberBetween0AndN(HAPPENING_COIN_MULTIPLIER);

    if (mp3_ReturnTruePercentOfTime(50))
    {
        fuzzFactor = fuzzFactor * -1;
    }

    int coinReward = (happeningSpaceCount * HAPPENING_COIN_MULTIPLIER) + fuzzFactor;

    return coinReward;
}

void PlayFlavorMessage(int happeningSpaceCount)
{
    char* message = NULL;

    if(happeningSpaceCount < 5)
    {
        message = GetFlavorMessage();
    }
    else
    {
        message = GetFinalFlavorMessage();
    }

    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);

    return;
}

void GraduallyAdjustPlayerCoins(int playerIndex, int adjustmentAmount)
{   
    // Some mp3-defined functions to change the coins
    // Find more at https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym
    AdjustPlayerCoinsGradual(playerIndex, adjustmentAmount);
    ShowPlayerCoinChange(playerIndex, adjustmentAmount);

    // Sleep for 30 frames to let the coin change take effect.
    // Also syncs up nicely with the joy animation.
    SleepProcess(30);
}

void PlayMessageForNoCoins()
{
    char* message = GetNoCoinMessage();
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);
    return;
}

// Anicent Mews eyes glitter mysteriously. You follow its gaze to a pile of coins.
char* GetFlavorMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                                   // Start the message
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	                // Standard padding for portrait
    mplib_strncat(result, "Ancient Mew");                 
    mplib_strncat(result, "\x5C");                                  // '
    mplib_strncat(result, "s eyes glitter mysteriously");
    mplib_strncat(result, "\x85");                                  // .
    mplib_strncat(result, "\x0A");                                  // Newline
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A");                    // Standard padding for portrait
    mplib_strncat(result, "You follow its gaze to a pile of ");
    mplib_strncat(result, "\x06");                                  //Blue font start
    mplib_strncat(result, "coins");
    mplib_strncat(result, "\x06");                                  //White font start
    mplib_strncat(result, "\x85");                                  //.
    mplib_strncat(result, "\xFF");                                  //Show prompt to continue arrow

    return result;
}

// You have explored many mysteries.  Ancient Mew is pleased.
char* GetFinalFlavorMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                               // Start the message
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	                // Standard padding for portrait
    mplib_strncat(result, "You have explored many mysteries");
    mplib_strncat(result, "\x85");                              //.
    mplib_strncat(result, "\x0A");                              // Newline
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A");                  // Standard padding for portrait
    mplib_strncat(result, "Ancient Mew is pleased");   
    mplib_strncat(result, "\x85");                              //.
    mplib_strncat(result, "\xFF");                              //Show prompt to continue arrow

    return result;
}

// Your search of the ancient tomb turns up nothing, but you can't shake the feeling there are more mysteries to discover.
char* GetNoCoinMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                               // Start the message
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	                // Standard padding for portrait
    mplib_strncat(result, "Your search of the ancient tomb turns up");                 
    mplib_strncat(result, "\x0A");                              // Newline
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A");                  // Standard padding for portrait
    mplib_strncat(result, "nothing");
    mplib_strncat(result, "\x82");                              // ,
    mplib_strncat(result, " but you can");
    mplib_strncat(result, "\x5C");                              // '
    mplib_strncat(result, "t shake the feeling");
    mplib_strncat(result, "\x0A");                              // Newline
    //mplib_strncat(result, "\x1A\x1A\x1A\x1A");                  //Standard padding for portrait
    mplib_strncat(result, "there are more mysteries to discover");          
    mplib_strncat(result, "\x85");                              //.
    mplib_strncat(result, "\xFF");                              //Show prompt to continue arrow

    return result;
}


//***************************************************************************//
//***************************************************************************//
//****************************                  *****************************//
//*************************      mplib v2.1        **************************//
//****************************                  *****************************//
//***************************************************************************//
//***************************************************************************//
//***      Some helper functions to make up for lack of stdlib.io; and    ***//
//***     to hide the complexity of some Mario Party-specific functions   ***//
//***************************************************************************//
//***************************************************************************//
// Paste this at the bottom of an event file to get access to helpful functions!
// Get the latest version or submit changes at: 
// https://github.com/C-Kennelly/mario-party-custom-events
//***************************************************************************//

// Looking for another function?  
// Have you checked the PartyPlanner64 symbols table yet?
// https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym


// Prints a message in game with the Millenium Star portrait.
// Does not wait for player confirmation.
void mp3_DebugMessage(char* message)
{
    mp3_ShowMessageWithConfirmation(-1, message);
}

// Picks a random number between 0 and N, using rejection sampling to avoid modulo bias.
// Don't use this!!! -->  { return GetRandomByte() % n; } or you may bias your random generation!
// https://zuttobenkyou.wordpress.com/2012/10/18/generating-random-numbers-without-modulo-bias/
//
// IMPORTANT.  Maximum random number we could generate here would be 255 (max value of a byte).
int mp3_PickARandomNumberBetween0AndN(int n)
{
    int result = GetRandomByte();                   //Get a random number by picking a random byte.
    
    int randMax = 255;                              // 255 is the maximum value of a byte.
    int randExcess = (randMax % n) + 1;             // Caluclate the biased remainder.  1/256 edge case when (randMax == n) but the result is just some wasted cycles, so acceptable.
    int randLimit = randMax - randExcess;           // Anything above randLimit would create modulo bias, so...
    while (result > randLimit)                      // Reject any random bytes with values above randLimit...
    {
        result = GetRandomByte();                   // and roll again by selecting a new byte.
    }    

    return result % n;                              //Since we rejected the excess samples, we've guaranteed an unbiased result.
}


// RNGChance from the built-in functions doesn't seem to re-roll within a turn
// So this function provides the same functionality of returning true a certain 
// percent of the time but uses GetRandomByte() to guarantee different rolls
// within a turn.
//
// Values <= 0 always return false.  
// Values >= 100 always return true.
int mp3_ReturnTruePercentOfTime(int percentChanceOfTrue)
{
    if(percentChanceOfTrue <= 0)
    {
        return 0;    //negative chance always returns false
    }
    else if(percentChanceOfTrue >= 100)
    {
        return 1;    //greater than 100% chance always true
    }
    else
    {
        int randValue = mp3_PickARandomNumberBetween0AndN(99);
        if (randValue < percentChanceOfTrue)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
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

//Plays the sad animation, with no loop
void mp3_play_sad_animation() 
{
    func_800F2304(-1, 3, 0); // Sad animation, no loop
}

//Plays the joy animation, with no loop
void mp3_play_joy_animation() 
{
    func_800F2304(-1, 5, 0); // joy animation, no loop
}

// Plays the idle animation with no loop.
void mp3_play_idle_animation() 
{
    func_800F2304(-1, -1, 0);
}

enum mp3_Character {Mario, Luigi, Peach, Yoshi, Wario, DK, Waluigi, Daisy};

int mp3_IsPlayerCertainCharacter(int playerIndex, enum mp3_Character character)
{
    struct Player *p = GetPlayerStruct(playerIndex);
    if(p != NULL && p->character == character)  
    {
        return 1;
    }
    else
    {
        return 0; 
    }
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

// Returns the smaller of two numbers.  Ties go to the first argument.
int mplib_min(int a1, int a2)
{
    if (a1 <= a2) { return a1; }
    else { return a2; }
}

// Returns the largest of two numbers.  Ties go to the first argument.
int mplib_max(int a1, int a2)
{
    if (a1 >= a2) { return a1; }
    else { return a2; }
}

// Returns the largest of three numbers
// C doesn't support overloading, don't hate me
int mplib_max3(int a1, int a2, int a3)
{
    int result = a1;
    result = mplib_max(result, a2);
    result = mplib_max(result, a3);

    return result;
}

// Returns the largest of four numbers
// C doesn't support overloading, don't hate me
int mplib_max4(int a1, int a2, int a3, int a4)
{
    int result = a1;
    result = mplib_max(result, a2);
    result = mplib_max(result, a3);
    result = mplib_max(result, a3);
    
    return result;
}

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************      /end mplib       ******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//