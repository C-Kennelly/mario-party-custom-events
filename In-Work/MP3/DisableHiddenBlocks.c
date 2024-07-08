// NAME: Disable Hidden Blocks
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
//*********************** Description ***************************************//
//***************************************************************************//
// This event sets the Hidden Block space index every turn, effectively
// disabling the hidden blocks on the board. This is meant to be paired with

//***************************************************************************//
//***********************     Changelist      *******************************//
//***************************************************************************//
//   Version 1.0 - First version of the event!

//***************************************************************************//
//*************************** Declarations **********************************//
//***************************************************************************//

// Header file: http://n64devkit.square7.ch/header/ultra64.htm
// Ultratypes: http://n64devkit.square7.ch/header/ultratypes.htm
// For more exploration: http://n64devkit.square7.ch/header/
#include "ultra64.h"

extern u8 D_800CC4E5;	// Hidden block space index

//***************************************************************************//
//*************************** Event Logic ************************************//
//***************************************************************************//

void main() 
{   
	D_800CC4E5 = 0xFF;	// Disable hidden blocks
}    