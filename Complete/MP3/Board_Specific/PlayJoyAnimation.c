// NAME: Play Joy Animation
// GAMES: MP3_USA
// EXECUTION: Direct

#include "ultra64.h"

void main() {
    func_800F2304(-1, 5, 0); 	// joy animation, no loop
    SleepProcess(35);			// sleep for animation
}