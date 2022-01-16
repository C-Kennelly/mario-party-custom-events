// NAME: Turn Order Swap
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: Boolean|USES_BOARD_RAM
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
//
// If any other events on your board use these values, you will get unexpected behavior! 
//
// You can disable this event's use of board RAM by setting the "USES_BOARD_RAM" parameter to
// "False" in PartyPlanner when you place the event.
//
// This event uses board RAM to keep track of the players who will be swapping turn orders and
// allows a passive after-turn event to actually make the swaps.  This neatly handles edge cases with
// swapping turn order-mid turn, allowing the event to be set to "passing", if the board creator desires.
//
// If you disable the board RAM, then the event will just make the swaps after the player makes
// their selection.  This should be fine if the event is set to land-on.  However, if the event
// is set to passing, then there are likely unhandled edge cases where the player has movement left
// but it is no longer their turn.
//
// In short, if you can use the board RAM, do what you like, as long as you have the companion board event setup!
// If you disable the board RAM, you no longer need the companion event, but I recommend ONLY using a "land-on" trigger for this event.

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
// This land-on event allows a player to choose two players who will swap Turn Orders.
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
extern u8 D_800CD0A3;
extern u8 D_800CD0A4;

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

// TODO: Ask the player for a target and a victim. If the player accepts, setup the turn order swap.
void main() 
{
    int currentPlayerIndex = GetCurrentPlayerIndex();
    
    mp3_play_idle_animation();
    
    int targetPlayerIndex = MakeOfferToSwapTurnOrderAndGetTargetPlayer(currentPlayerIndex);

    if (targetPlayerIndex != -1)
    {
        int victimPlayerIndex = GetPlayerToChooseVicitm(targetPlayerIndex);

        PlayReadyToSwapMessage(targetPlayerIndex, victimPlayerIndex);
        SwapTurnOrderWithMessage(targetPlayerIndex, victimPlayerIndex);  //TODO - This is where If Board Ram goes
        //SwapControllersWithMessaging(currentPlayerIndex, targetPlayerIndex);
    }
    else
    {
        BeratePlayerForSayingNo(currentPlayerIndex);
    }

    return;
}

int MakeOfferToSwapTurnOrderAndGetTargetPlayer(int currentPlayerIndex)
{
    int opponentIndex = -1;

    PlayGreetingMessage();

    int answerChosen = TemptPlayerWithControllerSwap(currentPlayerIndex);
    
    if (answerChosen == 0)
    {
        int* opponents[3];
        GetOpponentsForPlayer(currentPlayerIndex, &opponents);
        
        int targetChoice = GetTargetOfSwap(currentPlayerIndex, opponents);

        if (targetChoice == 3)
        {
            targetChoice = mp3_PickARandomNumberBetween0AndN(2);
        }

        opponentIndex = opponents[targetChoice];
    }
    
    return opponentIndex;
}

int TemptPlayerWithControllerSwap(int currentPlayerIndex)
{
    char* msg = GetTemptingQuestion(currentPlayerIndex);
    ShowMessage(CHARACTER_PORTRAIT, msg, 0, 0, 0, 0, 0);
    int result = GetResponseAndTeardownMessageBox();

    return result;
}

int GetTargetOfSwap(int currentPlayerIndex, int* opponents)
{
    char* msg = GetTargetingMessage(opponents);
    ShowMessage(CHARACTER_PORTRAIT, msg, 0, 0, 0, 0, 0);
    int result = GetResponseAndTeardownMessageBox();

    return result;
}

void SwapControllersWithMessaging(int originalPlayerIndex, int targetPlayerIndex)
{
    
    SwapParametersInPlayerStructsToSwapControllers(originalPlayerIndex, targetPlayerIndex);

    return;
}

void SwapParametersInPlayerStructsToSwapControllers(int playerIndexA, int playerIndexB)
{   
    if(playerIndexA != playerIndexB)
    {
        struct Player *p_a = GetPlayerStruct(playerIndexA);
        struct Player *p_b = GetPlayerStruct(playerIndexB);

        if(p_a != NULL && p_b != NULL)  
        {
            // Move the controller
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

            //TODO - all the other counts:
            //s8 red_space_count;     
            //s8 blue_space_count;
            //s8 chance_space_count;
            //s8 bowser_space_count;  // Offset 48
            //s8 battle_space_count;
            //s8 item_space_count;
            //s8 bank_space_count;
            //s8 game_guy_space_count; //Offset  52
        }
    }
}

int BeratePlayerForSayingNo(int currentPlayerIndex)
{
    char* msg = GetMessageBeratingPlayer(currentPlayerIndex);
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);
}

// Display a message and return the answer chosen 
// Also has defined logic for CPUs.
int GetResponseAndTeardownMessageBox()
{
    int cpuChoice = GetChoiceForCPU();

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

    int choice = GetBasicPromptSelection(2, cpuChoice);
    mp3_TeardownMessageBox();

    return choice;
}

int GetChoiceForCPU()
{
    return 1;
}


void RestoreCoinsToSwappedCharacter(int originalPlayerIndex, int targetPlayerIndex, int coinsToRestore)
{
    PlayPostSwapMessage(originalPlayerIndex, targetPlayerIndex); 

    PlayCoinRestorationMessage();
    AdjustPlayerCoinsGradual(originalPlayerIndex, coinsToRestore);
    ShowPlayerCoinChange(originalPlayerIndex, coinsToRestore);
    TransferStarFromPlayerToPlayer(targetPlayerIndex, originalPlayerIndex);
    SleepProcess(40);
    return;
}

void PlayGreetingMessage(int currentPlayerIndex)
{
    char* msg = GetGreetingMessage(currentPlayerIndex);
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);
}

void PlayReadyToSwapMessage(int originalPlayerIndex, int targetPlayerIndex)
{
    char* msg = GetReadyToSwapMessage(targetPlayerIndex);
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);

    return;
}

void PlayPostSwapMessage(int originalPlayerIndex, int targetPlayerIndex)
{
    char* msg = GetPostSwapMessage(originalPlayerIndex, targetPlayerIndex);
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);
}

void PlayCoinRestorationMessage()
{
    char* msg = GetCoinRestoreMessage();
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);
}

// Defines the message the Quiz Giver displays first.
char* GetGreetingMessage(int currentPlayerIndex)
{
    char* playerCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(currentPlayerIndex);

    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Psst");                 
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " hey ");                 
    mplib_strncat(result, "\x06");                             // Begin blue color                 
    mplib_strncat(result, playerCharacterName);                 
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "No one seems to be watching these");
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "\x03");                             // Begin red color
    mplib_strncat(result, "controller ports");
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(result, "\x0B");                             // Feed old text away
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "I wonder if I could ");
    mplib_strncat(result, "\x03");                             // Begin red color
    mplib_strncat(result, "do something nasty ");
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "to them");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow

    return result;
}

char* GetTemptingQuestion()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Would you pay ");
    mplib_strncat(result, "\x07");                             // Begin yellow color
    //TODO - Hardcoded
    mplib_strncat(result, "60 coins ");
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "to ");
    mplib_strncat(result, "\x03");                             // Begin red color
    mplib_strncat(result, "swap controllers with");
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "another player ");
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "until the end of the game");
    mplib_strncat(result, "\xC3");                             // ?
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, "Absolutely");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, "That feels like cheating");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow

    return result;
}

char* GetReadyToSwapMessage(int targetPlayerIndex)
{
    char* targetCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(targetPlayerIndex);

    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Okay");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " I");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "m going to swap your controller with ");
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "\x03");                             // Begin red color
    mplib_strncat(result, targetCharacterName);
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "\x85");                             // .
    mplib_strncat(result, "s controller");
    mplib_strncat(result, " Here goes nothin");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
}

char* GetPostSwapMessage(int targetPlayerIndex)
{
    char* targetCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(targetPlayerIndex);

    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "There");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, " Did it work");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " ");
    mplib_strncat(result, "\x06");                             // Begin blue color        
    mplib_strncat(result, targetCharacterName);
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, "\xC3");                             // ?
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(result, "\x0B");                             // Feed old text away
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Yes");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, " You");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "ve swapped controllers now");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait    
    mplib_strncat(result, "Hey");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " hey");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " don ");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "t look so angry");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(result, "\x0B");                             // Feed old text away
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "Listen");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " I don");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "t want any trouble");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "Here");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " take these and just don");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, "t hurt me");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
}

char* GetCoinRestoreMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "Listen");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " I don");
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "t want any trouble");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "Here");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " take these and just don");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, "t hurt me");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
}

char* GetTargetingMessage(int* opponents)
{
    char* opponentNames[3];
    for (int i = 0; i < 3; i++)
    {
        opponentNames[i] = mp3_GetCharacterNameStringFromPlayerIndex(opponents[i]);
    }

    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              //start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Whose controller should I swap with yours");
    mplib_strncat(result, "\xC3");                             // ?
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, opponentNames[0]);
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, opponentNames[1]);
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, opponentNames[2]);
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    mplib_strncat(result, "\x0C");                             // Start option
    mplib_strncat(result, "You choose");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\x0D");                             // End option
    mplib_strncat(result, "\xFF");                             //Show prompt to continue

    return result;
}

// Gets the 3 opponents for the given player
void GetOpponentsForPlayer(int targetPlayerIndex, int* opponents[])
{
    int opponentIndex = 0;
    for(int i = 0; i < 3; i++)
    {
        if (targetPlayerIndex != opponentIndex)
        {
            opponents[i] = opponentIndex;
        }
        else //Found target player, so skip one opponent index.  Will cap at three because of the for loop.
        {
            opponentIndex++;
            opponents[i] = opponentIndex;
        }

        opponentIndex++;
    }

    return;
}

char* GetMessageBeratingPlayer(int currentPlayerIndex)
{
    int rivalCharacterIndex = GetRivalForPlayer(currentPlayerIndex);
    char* rivalCharacterName = mp3_GetCharacterNameStringFromPlayerIndex(rivalCharacterIndex);

    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");                              // Start the message
    mplib_strncat(result, "\x1A\x1A\x1A\x1A"); 	               // Standard padding for portrait
    mplib_strncat(result, "Pah");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, " I guess we");                 
    mplib_strncat(result, "\x5C");                             // '
    mplib_strncat(result, "ll see if");
    mplib_strncat(result, "\x0A");                             // Newline
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "\x03");                             // Begin red color
    mplib_strncat(result, rivalCharacterName);                 
    mplib_strncat(result, "\x08");                             // Begin white (default) color
    mplib_strncat(result, " feels the same way");
    mplib_strncat(result, "\x85\x85\x85");                     // ...
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow
    mplib_strncat(result, "\x0B");                             // Feed old text away
    mplib_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    mplib_strncat(result, "Now");
    mplib_strncat(result, "\x82");                             // ,
    mplib_strncat(result, " scram");
    mplib_strncat(result, "\xC2");                             // !
    mplib_strncat(result, "\xFF");                             // Show prompt to continue arrow

    return result;
}

int GetRivalForPlayer(int characterIndex)
{
    int rivalIndex = 0;
    int playerPlacement = GetPlayerPlacementAtEndOfGame(characterIndex);

    if (playerPlacement == 3)
    {
        // player in 4th? Return player in third.
        rivalIndex = mp3_GetPlayerInPlace(2);
    }
    else
    {
        // Else, get the player immediately behind the other.  For example:
        // 1st's rival is 2nd, 2nd's rival is 3rd, and 3rd's rival is 4th.
        rivalIndex = mp3_GetPlayerInPlace(playerPlacement + 1);
    }

    // Handle ties.
    if (rivalIndex == characterIndex)
    {
        return (characterIndex + 1) % 4;
    }
    else
    {
        return rivalIndex;
    }
}



//***************************************************************************//
//***************************************************************************//
//****************************                  *****************************//
//*************************      mplib v2.0        **************************//
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

// Returns the first playerIndex that is in 1st, 2nd, 3rd, or 4th place using zero-based placements.
// Pass '0' for 1st place, '1' for 2nd place, '2' for 3rd place, and '3' for 4th place
// If no one is in the place you asked for, find the next closest place. For example, you ask for 4th, but players
// are in 1st, 1st, 2nd, 3rd, the function will check for 3rd place.
int mp3_GetPlayerInPlace(int place) //1st, 2nd, 3rd, 4th
{
    place = mp3_clampPlace(place);

    for (int p = place; p >= 0; p--) //Step up through the places, since someone will always be in first.
    {
        for (int i = 0; i < 4; i++)
        {
            if (GetPlayerPlacementAtEndOfGame(i) == p)
            {
                return i;
            }
        }
    }

    return 0;
}

char* mplib_DebugConvertIntToString(int i)
{
    if (i <= -2)
    {
        return "Neg 2 or less";
    }
    else if (i == -1)
    {
        return "Neg 1";
    }
    else if (i == 0)
    {
        return "0";
    }
    else if (i == 1)
    {
        return "1";
    }
    else if (i == 2)
    {
        return "2";
    }
    else if (i == 3)
    {
        return "3";
    }
    else if (i == 4)
    {
        return "4";
    }
    else
    {
        return "5 or more";
    }
}

// Takes an integer representing 1st, 2nd, 3rd, and 4th place and ensures it is valid.
int mp3_clampPlace(int place)
{
    if (place <= 0)
    {
        return 0;
    }
    else if (place >= 3)
    {
        return 3;
    }
    else
    {
        return place;
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