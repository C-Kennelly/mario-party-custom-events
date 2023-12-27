// NAME: Turn Order Swap
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: +Number|BASE_PRICE


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
//******************** BOARD RAM WARNING ************************************//
//***************************************************************************//
// This board uses the following addresses (board RAM)
//              D_800CD0A3;   //board_ram12
//              D_800CD0A4;   //board_ram13
//              D_800CD0A5;   //board_ram14
//
// If any other events on your board use these values, you will get unexpected behavior! 
//
// This event uses board RAM to keep track of the players who will be swapping turn orders and
// allows a passive after-turn event to actually make the swaps.  This neatly handles edge cases with
// swapping turn order-mid turn, allowing the event to be set to "passing", if the board creator desires.

//***************************************************************************//
//******************** Event Configuration ***********************************//
//***************************************************************************//

// This is the portrait of the character giving the messages.
#define CHARACTER_PORTRAIT 5
// Want to change the picture?  Find options the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*********************** Description ***************************************//
//***************************************************************************//
// This land-on event allows a player advance their own turn order by 1.
// So a player who goes third will swap with the player who is going second.
//
// This is really a two part event, as this event simply logs the two players into
// boardRAM, and relies on a passive after-turn event to check the boardRAM and actually
// make the swaps.  So you can think of this mostly as presentation and choice selection.
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

// Event parameter can disable this:
extern u8 D_800CD0A3;	//IsInitialized - 1 == yes.
extern u8 D_800CD0A4;	//firstPlayerIndex
extern u8 D_800CD0A5;	//secondPlayerIndex

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

//This event is intended to run on After Turn timing. It may work on other timings, but is untested.
void main() 
{
	if(D_800CD0A3 == 1)
	{
		int firstPlayerIndex = D_800CD0A4;
		int secondPlayerIndex = D_800CD0A5;

		SwapContentsOfPlayerStructs(firstPlayerIndex, secondPlayerIndex);

		// Refresh the HUD for the swapped players
		RefreshHUD(firstPlayerIndex);
		RefreshHUD(secondPlayerIndex);

		// Mark the event an uninitialized and then remove the players from boardRAM
		D_800CD0A3 = 0;
		D_800CD0A4 = -1;
		D_800CD0A5 = -1;

		PlayMessageConfirmingSwap();
	}
	
	return;
}

void PlayMessageConfirmingSwap()
{
	char* message = "Heh heh heh";
	mp3_DebugMessage(message);
}

void SwapContentsOfPlayerStructs(int playerIndexA, int playerIndexB)
{   
	if(playerIndexA != playerIndexB)
	{
		struct Player *p_a = GetPlayerStruct(playerIndexA);
		struct Player *p_b = GetPlayerStruct(playerIndexB);

		if(p_a != NULL && p_b != NULL)  
		{
			// Swap the controller parameters
			s8 s8SwapValue = p_a->controller;
			p_a->controller = p_b->controller;
			p_b->controller = s8SwapValue;

			// Make the bonus star counts follow the player (controller), not the character
			// This ensures rewards for player decisions stay with the player who made them.
			s8SwapValue = p_a->happening_space_count;
			p_a->happening_space_count = p_b->happening_space_count;
			p_b->happening_space_count = s8SwapValue;

			s16 s16SwapValue = p_a->minigame_star;
			p_a->minigame_star = p_b->minigame_star;
			p_b->minigame_star = s16SwapValue;

			s16SwapValue = p_a->coin_star;
			p_a->coin_star = p_b->coin_star;
			p_b->coin_star = s16SwapValue;

			// Swap all space counts so they follow the player as well. This will keep stats intact for end of game.
			s8SwapValue = p_a->red_space_count;
			p_a->red_space_count = p_b->red_space_count;
			p_b->red_space_count = s8SwapValue;

			s8SwapValue = p_a->blue_space_count;
			p_a->blue_space_count = p_b->blue_space_count;
			p_b->blue_space_count = s8SwapValue;

			s8SwapValue = p_a->chance_space_count;
			p_a->chance_space_count = p_b->chance_space_count;
			p_b->chance_space_count = s8SwapValue;

			s8SwapValue = p_a->bowser_space_count;
			p_a->bowser_space_count = p_b->bowser_space_count;
			p_b->bowser_space_count = s8SwapValue;

			s8SwapValue = p_a->battle_space_count;
			p_a->battle_space_count = p_b->battle_space_count;
			p_b->battle_space_count = s8SwapValue;

			s8SwapValue = p_a->item_space_count;
			p_a->item_space_count = p_b->item_space_count;
			p_b->item_space_count = s8SwapValue;

			s8SwapValue = p_a->bank_space_count;
			p_a->bank_space_count = p_b->bank_space_count;
			p_b->bank_space_count = s8SwapValue;

			s8SwapValue = p_a->game_guy_space_count;
			p_a->game_guy_space_count = p_b->game_guy_space_count;
			p_b->game_guy_space_count = s8SwapValue;
		}
	}
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
	result = mplib_max(result, a4);
	
	return result;
}

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************      /end mplib       ******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//