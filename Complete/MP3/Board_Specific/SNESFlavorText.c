// NAME: SNES Flavor Text
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
//******************** Event Configuration ***********************************//
//***************************************************************************//

// This is the portrait of the character giving the messages.
#define CHARACTER_PORTRAIT -1
// Want to change the picture?  Find options the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*********************** Description ***************************************//
//***************************************************************************//
// This event plays some flavor text and is meant to be paired with Gwonam
// Get Random Item event on the Game Night board.
// https://www.mariopartylegacy.com/forum/index.php?action=downloads;sa=view;down=272

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

//***************************************************************************//
//*************************** Event Logic ************************************//
//***************************************************************************//

// TODO: Ask the player for a target and a victim. If the player accepts, setup the turn order swap.
void main() 
{   
    mp3_play_idle_animation();

    char* msg = GetFlavorMessage();
    mp3_ShowMessageWithConfirmation(CHARACTER_PORTRAIT, msg);
}    

// Defines the flavor message the player sees before the effect is applied.
char* GetFlavorMessage()
{
    char* result = func_80035934(256);		// First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
											// transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);						// Second, zero out the memory allocated above so we don't get unexpected behavior.

    mplib_strcpy(result, "\x0B");								// Start the message
    mplib_strncat(result, "You rummage around your old box of games");
    mplib_strncat(result, "\x0A");								// Newline
    mplib_strncat(result, "looking for an old favorite");
    mplib_strncat(result, "\x85");								// .
    mplib_strncat(result, " You don");
    mplib_strncat(result, "\x5C");								// '
    mplib_strncat(result, "t"); 
    mplib_strncat(result, "\x0A");								// Newline
	mplib_strncat(result, "find it");
	mplib_strncat(result, "\x82");								// ,	
	mplib_strncat(result, " but you do find something nice");
    mplib_strncat(result, "\x85");								// .
    mplib_strncat(result, "\xFF");								// Show prompt to continue arrow

    return result;
}


//Stripped-down mplib for space since this event is so simple.

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

// Plays the idle animation with no loop.
void mp3_play_idle_animation() 
{
    func_800F2304(-1, -1, 0);
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