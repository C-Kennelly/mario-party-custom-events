// NAME: Turn Order Swap Triggered
// GAMES: MP3_USA
// EXECUTION: Direct


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
// This event allows a player to choose 2 players and swap their turn order.  Only
// one swap can happen per turn, so players who trigger the event a second time in a
// turn will see a flavor message telling them they cannot perform the swap.
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

void main() 
{
	int currentPlayerIndex = GetCurrentPlayerIndex();

	// Someone already used the event this turn, so return early.
	if(D_800CD0A3 == 1)
	{
		PlayMessageForTargetAlreadyChosen(currentPlayerIndex);
		return;
	}

	// Ask the player if they want to swap and bail if they don't
	int playerChoice = AskPlayerToSwapTurnOrders(currentPlayerIndex);
	if(playerChoice == 1)
	{
		PlayMessageBeratingPlayerForNotSwapping(currentPlayerIndex);
		return;
	}

	// Select swap targets
    int firstTargetPlayerIndex = AskPlayerToSelectFirstTarget(currentPlayerIndex);
	int secondTargetPlayerIndex = AskPlayerToSelectSecondTarget(firstTargetPlayerIndex, currentPlayerIndex);

	// Setup boardRAM for the passive event to handle the swap, and confirm
	D_800CD0A3 = 1;
	D_800CD0A4 = firstTargetPlayerIndex;
	D_800CD0A5 = secondTargetPlayerIndex;
	PlayMessageConfirmingTargetSelection(firstTargetPlayerIndex, secondTargetPlayerIndex);
}

int AskPlayerToSwapTurnOrders(int currentPlayerIndex)
{
	char *requestToSwapMessage = GetRequestToSwapMessage(currentPlayerIndex);
	mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, requestToSwapMessage);
	
    char* optionsMessage = GetOptionsMessage();
    ShowMessage(CHARACTER_PORTRAIT, optionsMessage, 0, 0, 0, 0, 0);
	// Get the selection, either from the player or CPU.
	// MP3 built-in function: GetBasicPromptSelection(int strategy, int index)
	// Strategy argument takes an int and behaves as follows:
	//   0 -> If CPU, always pick first (0th) option
	//   1 -> If CPU, always pick second (1th) option
	//   2 -> If CPU, pick the option that is passed in the second argument.
	//
	// Here, we're using strategy 2, and then passing in 'currentPlayerIndex'
	// which is calculated in our other function: GetChoiceForCPU(). 
	// Human players will always get to manually pick.
	int choice = GetBasicPromptSelection(2, 0);
	mp3_TeardownMessageBox();

	return choice;
}


int AskPlayerToSelectFirstTarget(int currentPlayerIndex)
{
    char *targetingMessage = GetFirstTargetingMessage(currentPlayerIndex);
	ShowMessage(CHARACTER_PORTRAIT, targetingMessage, 0, 0, 0, 0, 0);

    int cpuChoice = 0;
    if (PlayerIsCPU(currentPlayerIndex))
    {
        cpuChoice = CPUGetSelfOrSecondPlayer(currentPlayerIndex);
        SleepProcess(15); //Make the CPU wait so it doesn't go through the selection too fast
    }    
	// Get the selection, either from the player or CPU.
    // MP3 built-in function: GetBasicPromptSelection(int strategy, int index)
    // Strategy argument takes an int and behaves as follows:
    //   0 -> If CPU, always pick first (0th) option
    //   1 -> If CPU, always pick second (1th) option
    //   2 -> If CPU, pick the option that is passed in the second argument.
    //
    // Here, we're using strategy 2, and then passing in 'cpuChoice'
    // which is calculated in our other function: GetChoiceForCPU().
    // Human players will always get to manually pick.
    int firstChoice = GetBasicPromptSelection(2, cpuChoice);
    mp3_TeardownMessageBox();

	// Handle "You Choose" option by randomly selecting a player.
	if(firstChoice == 4)
	{
		firstChoice = mp3_PickARandomNumberBetween0AndN(3);
	}

    return firstChoice;
}

int AskPlayerToSelectSecondTarget(int firstTargetPlayerIndex, int currentPlayerIndex)
{
    // Generate options and keep track of them so we can parse the response.
    // Need to handle this dynamically here because it is dependent on the first
    int playerIndexTargets[3];

    int playerIndex = 0;
    for(int arrayIndex = 0; arrayIndex < 3; arrayIndex++)
    {
        if(firstTargetPlayerIndex == arrayIndex)
        {
            playerIndex++;
        }

        playerIndexTargets[arrayIndex] = playerIndex;

        playerIndex++;
    }

    char *targetingMessage = GetSecondTargetingMessage(firstTargetPlayerIndex, playerIndexTargets[0], playerIndexTargets[1], playerIndexTargets[2]);
	ShowMessage(CHARACTER_PORTRAIT, targetingMessage, 0, 0, 0, 0, 0);
    
    int cpuChoice = 0;
    if (PlayerIsCPU(currentPlayerIndex))
    {
        cpuChoice = CPUGetFirstOr4thPlayer(firstTargetPlayerIndex, currentPlayerIndex);
        SleepProcess(15); // Make the CPU wait so it doesn't go through the selection too fast
    }
	// Get the selection, either from the player or CPU.
    // MP3 built-in function: GetBasicPromptSelection(int strategy, int index)
    // Strategy argument takes an int and behaves as follows:
    //   0 -> If CPU, always pick first (0th) option
    //   1 -> If CPU, always pick second (1th) option
    //   2 -> If CPU, pick the option that is passed in the second argument.
    //
    // Here, we're using strategy 2, and then passing in 'cpuChoice'
    // which is calculated in our other function: GetChoiceForCPU().
    // Human players will always get to manually pick.
    int secondChoice = GetBasicPromptSelection(2, cpuChoice);
    mp3_TeardownMessageBox();


	// Handle "You Choose" option by randomly selecting a player and ensuring there is no overlap with the first choice.
	if(secondChoice >= 3)
	{
		secondChoice = mp3_PickARandomNumberBetween0AndN(3);
		while (secondChoice == firstTargetPlayerIndex)
		{
			secondChoice = mp3_PickARandomNumberBetween0AndN(3);
		}
	}
    else // Choice must have been one of the three remaining players (0, 1, or 2)
    {
        // So, we can look up which player index that was using the targets array we were already keeping track of.
        secondChoice = playerIndexTargets[secondChoice];
    }

    return secondChoice;
}

// CPU Logic
//		If CPU is not rolling first (playerIndex 0), make them swap with index 0.
//		Else, make them swap 2nd and 4th players instead.
int CPUGetSelfOrSecondPlayer(int currentPlayerIndex)
{
    if(currentPlayerIndex == 0)
    {
        return 1;
    }
    else
    {
        return currentPlayerIndex;
    }
}

int CPUGetFirstOr4thPlayer(int otherTarget, int currentPlayerIndex)
{
    if(otherTarget == 1)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

// Messages
void PlayMessageForTargetAlreadyChosen(int currentPlayerIndex)
{
	char* playerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(currentPlayerIndex);

    char* message = func_80035934(256);         // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                        // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(message, "Can");
    mplib_strncat(message, "\x5C");                             // '
    mplib_strncat(message, "t you see I");
    mplib_strncat(message, "\x5C");                             // '
    mplib_strncat(message, "m busy");
 	mplib_strncat(message, "\xC3");                             // ?
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(message, "\x0B");                             // Feed old text away
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "Now");
    mplib_strncat(message, "\x82");                             // ,
    mplib_strncat(message, " scram");
    mplib_strncat(message, "\xC2");                             // !
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);
}

char* GetRequestToSwapMessage(int currentPlayerIndex)
{
	char* playerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(currentPlayerIndex);

    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    mplib_strncat(message, "Psst");                 
    mplib_strncat(message, "\x82");                             // ,
    mplib_strncat(message, " hey ");                 
    mplib_strncat(message, "\x06");                             // Begin blue color                 
    mplib_strncat(message, playerCharacterName);                 
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, "\xC2");                             // !
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "No one seems to be watching these");
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "\x03");                             // Begin red color
    mplib_strncat(message, "controller ports");
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, "\x85\x85\x85");                     // ...
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

	return message;
}

char* GetOptionsMessage()
{
    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "Since you were nice enough to");
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "drop by");
    mplib_strncat(message, "\x82");                             // ,
    mplib_strncat(message, " do you want me to");
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "\x03");                             // Begin red color
    mplib_strncat(message, "swap two of them");
    mplib_strncat(message, "\x08");                             // Begin white (default) color
 	mplib_strncat(message, "\xC3");                             // ?
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, "Yes");
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, "That sounds like cheating");
    mplib_strncat(message, "\x85\x85\x85");                     // ...
	mplib_strncat(message, "\x0D");                             // End option
	mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

    return message;
}

void PlayMessageBeratingPlayerForNotSwapping()
{
    char* message = func_80035934(256);         // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                        // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");					// Standard padding for portrait
    mplib_strncat(message, "Ugh");
    mplib_strncat(message, "\x82");                             // ,
    mplib_strncat(message, " you goodey two shoes");
	mplib_strncat(message, "\x0A");                             // Newline
	mplib_strncat(message, "\x1A\x1A\x1A\x1A");					// Standard padding for portrait
    mplib_strncat(message, "NEVER want to have any fun");
    mplib_strncat(message, "\xC2");                             // !
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);
}

int GetFirstTargetingMessage()
{
    char* firstPlayerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(0);
    char* secondPlayerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(1);
    char* thirdPlayerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(2);
    char* fourthPlayerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(3);

    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    mplib_strncat(message, "Whose turn order are we swapping");
    mplib_strncat(message, "\xC3");                             // ?
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, firstPlayerCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, secondPlayerCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, thirdPlayerCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, fourthPlayerCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, "You Choose");
    mplib_strncat(message, "\x0D");                             // End option
	mplib_strncat(message, "\xFF");                             // Show prompt to continue

    return message;
}

int GetSecondTargetingMessage(int firstTargetPlayerIndex, int firstOptionPlayerIndex, int secondOptionPlayerIndex, int thirdOptionPlayerIndex)
{

    char* firstTargetCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(firstTargetPlayerIndex);

    char* firstOptionCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(firstOptionPlayerIndex);
    char* secondOptionCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(secondOptionPlayerIndex);
    char* thirdOptionCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(thirdOptionPlayerIndex);

    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    mplib_strncat(message, "And who are we swapping ");
    mplib_strncat(message, "\x03");                             // Begin red color
    mplib_strncat(message, firstTargetCharacterName);
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, " with");
    mplib_strncat(message, "\xC3");                             // ?
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, firstOptionCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, secondOptionCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, thirdOptionCharacterName);
    mplib_strncat(message, "\x0D");                             // End option
    mplib_strncat(message, "\x0A");                             // Newline
    mplib_strncat(message, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(message, "\x0C");                             // Start option
    mplib_strncat(message, "You Choose");
    mplib_strncat(message, "\x0D");                             // End option
	mplib_strncat(message, "\xFF");                             // Show prompt to continue

    return message;

}

void PlayMessageConfirmingTargetSelection(int firstTargetPlayerIndex, int secondTargetPlayerIndex)
{
	char* firstTargetCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(firstTargetPlayerIndex);
	char* secondTargetCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(secondTargetPlayerIndex);

    char* message = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(message, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(message, "\x0B");                              // Start the message
    mplib_strncat(message, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    mplib_strncat(message, "All right then");                 
    mplib_strncat(message, "\x82");                             // ,
    mplib_strncat(message, " when the coast is");
	mplib_strncat(message, "\x0A");                             // Newline
	mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "clear I"); 	
	mplib_strncat(message, "\x5C");                             // '
    mplib_strncat(message, "ll swap ");
    mplib_strncat(message, "\x03");                             // Begin red color
    mplib_strncat(message, firstTargetCharacterName);
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, " and ");                 
	mplib_strncat(message, "\x0A");                             // Newline
	mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "\x03");                             // Begin red color
    mplib_strncat(message, secondTargetCharacterName);
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, "\x5C");                             // '
    mplib_strncat(message, "s turn order");
    mplib_strncat(message, "\x85");                             // .
    mplib_strncat(message, "\x08");                             // Begin white (default) color
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(message, "\x0B");                             // Feed old text away
    mplib_strncat(message, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(message, "I love getting up to no good");
	mplib_strncat(message, "\xC2");                             // !
    mplib_strncat(message, "\xFF");                             // Show prompt to continue arrow

    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, message);
}

// How to use:
// Copy and paste everything in the library block into the your 
// bottom of any Mario Party 3 event to get some helpful functions!

//***************************************************************************//
//***************************************************************************//
//****************************                  *****************************//
//*************************      mplib v2.4        **************************//
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
        mplib_strcpy(characterName, "champ");
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

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************      /end mplib       ******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//