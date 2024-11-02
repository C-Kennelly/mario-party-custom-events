// NAME: Reduce Hidden Blocks By Percent
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: Boolean|USES_BOARD_RAM
// PARAM: +Number|HIDDEN_BLOCK_REDUCTION_PERCENT
// PARAM: Boolean|REDUCE_ITEM_BLOCKS
// PARAM: Boolean|REDUCE_COIN_BLOCKS
// PARAM: Boolean|REDUCE_STAR_BLOCKS

// Redefining params here for vs code highlighting, not actually used in partyplanner.
#ifndef USES_BOARD_RAM
    #define USES_BOARD_RAM 1
#endif
#ifndef HIDDEN_BLOCK_REDUCTION_PERCENT
    #define HIDDEN_BLOCK_REDUCTION_PERCENT 50
#endif
#ifndef REDUCE_ITEM_BLOCKS
    #define REDUCE_ITEM_BLOCKS 1
#endif
#ifndef REDUCE_COIN_BLOCKS
    #define REDUCE_COIN_BLOCKS 1
#endif
#ifndef REDUCE_STAR_BLOCKS
    #define REDUCE_STAR_BLOCKS 1
#endif

//***************************************************************************//
//******************** BOARD RAM WARNING ************************************//
//***************************************************************************//
// This board uses the following addresses (board RAM)
//              D_800CD0A6;   //board_ram15
//              D_800CD0A7;   //board_ram16
//              D_800CD0A8;   //board_ram17
//
// If any other events on your board use these values, you will get unexpected behavior! 
//
// You can disable this events use of board RAM by setting the "USES_BOARD_RAM" parameter to
// "False" in PartyPlanner when you place the event.
//
// This board RAM is used to allow the Hidden Blocks to be disabled only a certain percentage of the time.
// If you don't enable the BoardRAM, there is no place to save the HiddenBlocks, so they'll be disabled 100% of the time.
// The True/False parameters for Item/Coin/Star blocks will stil function.
//
// If you're not sure what this parameter does, just set the "USES BOARD RAM" to true. =)
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
// This event is to reduce the frequency of hidden blocks.
//
// It should be setup to run every dice roll.
//
// This event sets the Hidden Block to an unreachable space index, 
// effectively disabling the hidden blocks on the board that turn. 
//
// However this logic does not run every turn.  A certain percent of the time,
// the logic skips, which should allow the roller to find hidden blocks naturally.
// This does NOT guarantee a hidden block - it just allows the possibility.
//
// If you set the HIDDEN_BLOCK_REDUCTION_PERCENT parameter to 100, the logic will never skip, and
// hidden blocks will be disabled completely.
// If you set the HIDDEN_BLOCK_REDUCTION_PERCENT parameter to 0, the logic will always skip, and
// this event may as well not exist.
//
// To get the most out of this event, figure out the rate of Hidden Blocks that
// are occuring naturally on your intended turn count, and then set the 
// HIDDEN_BLOCK_REDUCTION_PERCENT to the percent you want to reduce the hidden
// blocks by. For instance, if you are usually getting ~3 blocks in a 35 turn game,
// you could set the HIDDEN_BLOCK_REDUCTION_PERCENT to 66 and expect to get ~1
// blocks per game because hidden blocks would only be possible on 1/3 of the rolls.
//
// Finally, you can decide which types of hidden blocks this event affects by setting the corresponding parameter to TRUE.
// REDUCE_ITEM_BLOCKS - TRUE means the block that contains items (usually, a Skeleton Key) will be affected by this event.
// REDUCE_COIN_BLOCKS - TRUE means the block that contains coins (usually, 20 coins) will be affected by this event.
// REDUCE_STAR_BLOCKS - TRUE means the block that contains a star will be affected by this event.
//
// Leaving any of these as false will leave those blocks unmodified by this event.
// For example you might just set REDUCE_STAR_BLOCKS to TRUE and set the HIDDEN_BLOCK_REDUCTION_PERCENT to 100 to disable star blocks only.

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

extern s8 D_800CC4E5;   // The hidden_block_item space index from the mp3 sym table
extern s8 D_800CD0A6;   // BoardRAM to store the hidden_block_item space value

extern s8 D_800CE1C5;   // The hidden_block_coins_space index from the mp3 sym table
extern s8 D_800CD0A7;   // BoardRAM to store the hidden_block_coins space value

extern s8 D_800D124F;   // The hidden_block_star index from the mp3 sym table
extern s8 D_800CD0A8;   // BoardRAM to store the hidden_block_star value


//***************************************************************************//
//*************************** Event Logic ************************************//
//***************************************************************************//

void main() 
{   
    InitializeBoardRAMOnFirstPassIfEnabled();

	if (mp3_ReturnTruePercentOfTime(HIDDEN_BLOCK_REDUCTION_PERCENT))
	{
        // Logic has hit, blocks will be unreachable this turn if BoardRAM is enabled.
        StoreHiddenBlocksInBoardRAMIfEnabled();
        MakeHiddenBlocksUnreachable();
	}
	else
	{
        // Logic has skipped, blocks will be reachable this turn if BoardRAM is enabled.
        RestoreHiddenBlocksFromBoardRAMIfEnabled();
	}
}

void MakeHiddenBlocksUnreachable()
{
    s8 unreachableHiddenBlockSpaceIndex = 0x00;
    if(REDUCE_ITEM_BLOCKS == 1)
    {
        D_800CC4E5 = unreachableHiddenBlockSpaceIndex;
    }
    
    if(REDUCE_COIN_BLOCKS == 1)
    {
        D_800CE1C5 = unreachableHiddenBlockSpaceIndex;
    }

    if(REDUCE_STAR_BLOCKS == 1)
    {
        D_800D124F = unreachableHiddenBlockSpaceIndex;
    }
}

void StoreHiddenBlocksInBoardRAMIfEnabled()
{
    if(USES_BOARD_RAM != 1)
    {
        return;
    }

    if(REDUCE_ITEM_BLOCKS == 1)
    {
        // Store the item block space in BoardRAM.
        D_800CD0A6 = D_800CC4E5;
    }
    
    if(REDUCE_COIN_BLOCKS == 1)
    {
        // Store the coin block space in BoardRAM.
        D_800CD0A7 = D_800CE1C5;
    }

    if(REDUCE_STAR_BLOCKS == 1)
    {
        // Store the star block space in BoardRAM.
        D_800CD0A8 = D_800D124F;
    }
}

void RestoreHiddenBlocksFromBoardRAMIfEnabled()
{
    if(USES_BOARD_RAM != 1)
    {
        return;
    }

    if(REDUCE_ITEM_BLOCKS == 1)
    {
        // Restore the item block space value from BoardRAM so it can be found again.
        D_800CC4E5 = D_800CD0A6;
    }
    
    if(REDUCE_COIN_BLOCKS == 1)
    {
        // Restore the coin block space value from BoardRAM so it can be found again
        D_800CE1C5 = D_800CD0A7;
    }

    if(REDUCE_STAR_BLOCKS == 1)
    {
        // Restore the star block space value from BoardRAM so it can be found again.
        D_800D124F = D_800CD0A8;
    }
}

void InitializeBoardRAMOnFirstPassIfEnabled()
{
    if(USES_BOARD_RAM != 1)
    {
        return;
    }

    if(D_800CC4E5 != 0)
    {
        return;
    }
        
    D_800CD0A6 = -1;
    D_800CD0A7 = -1;
    D_800CD0A8 = -1;
}


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
// NOTE - this instance of mp3lib is pared down just to the functions used in this event
//
// Get the latest version or submit changes at: 
// https://github.com/C-Kennelly/mario-party-custom-events
//***************************************************************************//


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