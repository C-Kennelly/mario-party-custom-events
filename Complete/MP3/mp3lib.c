// How to use:
// Copy and paste everything in the library block into the your 
// bottom of any Mario Party 3 event to get some helpful functions!  Requires the contents of mplib.c pasted underneath it.

//***************************************************************************//
//***************************************************************************//
//****************************                  *****************************//
//*************************      mp3lib v1.0       **************************//
//****************************                  *****************************//
//***************************************************************************//
//***************************************************************************//
//***      Some helper functions to make up for lack of stdlib.io; and    ***//
//***     to hide the complexity of some Mario Party-specific functions   ***//
//***************************************************************************//
//***************************************************************************//
// Paste this at the bottom of an event file to get access to helpful functions during development!
// Then, when you are ready to ship, delete the functions you don't use to save space and remove clutter.
//
// Get the latest version or submit changes at: 
// https://github.com/C-Kennelly/mario-party-custom-events
//***************************************************************************//
//
// Looking for another function?  
// Have you checked the PartyPlanner64 symbols table yet?
// https://github.com/PartyPlanner64/symbols/blob/master/MarioParty3U.sym
//***************************************************************************//


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

	char unk_35[3]; 		//likely padding
}; // sizeof == 56

// Prints a message in game with the Millenium Star portrait.
// Does not wait for player confirmation.
void mp3_DebugMessage(char* message)
{
    mp3_ShowMessageWithConfirmation(-1, message);
}

// As mp3_DebugMessage, but appends the prompt arrow to the end of the message so the message doesn't flash by.
void mp3_DebugMessageWithConfirmation(char* message)
{
	char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

	mplib_strncat(result, message);                                 //Store the passed message in the buffer
	mplib_strncat(result, "\xFF");                                  //Append the prompt to continue arrow so the message doesn't flash by
	mp3_ShowMessageWithConfirmation(-1, result);
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

char* mp3_GetCharacterNameStringFromPlayerIndex(int playerIndex)
{
    char* characterName = func_80035934(8);         // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                    // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(characterName, 8);                        // Zero out the memory allocated above so we don't get unexpected behavior.
    
    int characterInt = -1;     

    struct Player *p = GetPlayerStruct(playerIndex);
    if(p != NULL)
    {
        characterInt = p->character;
    }


    if(characterInt == 0)
    {
        mplib_strcpy(characterName, "MARIO");
    }
    else if(characterInt == 1)
    {
        mplib_strcpy(characterName, "LUIGI");
    }
    else if(characterInt == 2)
    {
        mplib_strcpy(characterName, "PEACH");
    }
    else if(characterInt == 3)
    {
        mplib_strcpy(characterName, "YOSHI");
    }
    else if(characterInt == 4)
    {
        mplib_strcpy(characterName, "WARIO");
    }
    else if(characterInt == 5)
    {
        mplib_strcpy(characterName, "DK");
    }
    else if(characterInt == 6)
    {
        mplib_strcpy(characterName, "WALUIGI");
    }
    else if(characterInt == 7)
    {
        mplib_strcpy(characterName, "DAISY");
    }
    else
    {
        mplib_strcpy(characterName, "IndexOutOfRange");
    }
    
    return characterName;
}

void mp3_DebugPrintPlayerIndex(int playerIndex)
{
	char* message = mp3_PlayerIndexToString(playerIndex);
	mp3_DebugMessageWithConfirmation(message);
}

char* mp3_PlayerIndexToString(int playerIndex)
{
	char *message = "10";
	if(playerIndex == 0)
	{
		message = "0";
	}
	else if(playerIndex == 1)
	{
		message = "1";
	}
	else if(playerIndex == 2)
	{
		message = "2";
	}
	else if(playerIndex == 3)
	{
		message = "3";
	}

	return message;
}

void mp3_SwapPlayerStructs(s32 player1Index, s32 player2Index) { //swap player structs
    void* temp_s0;
    
	struct Player* temp_s1;
    struct Player* temp_s2;

    temp_s1 = GetPlayerStruct(player1Index);
    temp_s2 = GetPlayerStruct(player2Index);
    temp_s0 = MallocPerm(sizeof(struct Player));


    bcopy(temp_s1, temp_s0, sizeof(struct Player));
    bcopy(temp_s2, temp_s1, sizeof(struct Player));
    bcopy(temp_s0, temp_s2, sizeof(struct Player));
    FreePerm(temp_s0);

	// This cursed mushroom bit isn't necessary for the swap, but it is tied to the player struct and should be swapped to avoid bugs.
	mp3_SwapCursedMushroomBits(player1Index, player2Index);
}

extern u8 D_800CD0AF;	//Cursed Muschroom bit - slow_dice_flags, bit0 is P1, bit1 is P2, bit2 is P3, bit3 is P4
void mp3_SwapCursedMushroomBits(int firstPlayerIndex, int secondPlayerIndex)
{
	D_800CD0AF = mplib_SwapBitsForInteger(D_800CD0AF, firstPlayerIndex, secondPlayerIndex);
    //TODO - should rename? - we're actually not swapping bits for an integer, we're swapping
    //bits in a byte from two positions - doesn't matter if it's an int or not.
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
    result = mplib_max(result, a4);
    
    return result;
}

// Reloads the scene with the given transition type. Big credit to Rain for figuring out how this works.
// These are required to define the functions we use in ReloadScene below().
extern s32 D_800A12D4;
void func_8004F010(s32);
void func_800F8C74(void);
void func_8004819C(s32);
void func_8004849C(void);
void func_8004F074(void);

void mp3_ReloadCurrentSceneWithTransition(int transitionType)
{
	InitFadeOut(transitionType, 0x10);
	SleepProcess(0x11);
	D_800A12D4 = 1;
	func_800F8C74();
	func_8004819C(1);
	func_8004849C();
	func_8004F074();
}
// First argument to initFade out is fade_out type. Per Airsola, types are:
	// 0 = Bar Code
	// 1 = Circle
	// 2 = Star
	// 3 = Bowser
	// 4 = ?
	// 5 = !
	// 6 = Toad
	// 7 = Koopa
	// 8 = Goomba
	// 9 = Game Guy
	// 10 = Tumble
	// 11 = Generic
	// 12 = Boo

// Takes an integer and two bit positions, and swaps the bits at those positions, returning the bit-swapped integer.
// For mario party values, you you'll want to assign the return value to the value you pulled it from. For instance, if
// you wanted to swap cursed mushroom bits in Mario Party 3, you'd do something like:
// D_800CD0AF = SwapBitsForInteger(D_800CD0AF, firstPlayerIndex, secondPlayerIndex);
int mplib_SwapBitsForInteger(int targetInteger, int position1, int position2)
{
    // Get the bits at the target position using bit shifting
    int bit1 = (targetInteger >> position1) & 1;
    int bit2 = (targetInteger >> position2) & 1;

    // If the bits already match, we don't need to do anything.
    // If the bits do NOT match, we just need to toggle each of them to perform the swap
    if(bit1 != bit2)
    {
        targetInteger ^= (1 << position1);
        targetInteger ^= (1 << position2);
    }

    return targetInteger;
}

//***************************************************************************//