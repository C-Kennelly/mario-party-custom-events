// NAME: Nintendo Trivia Quiz
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: Boolean|USES_BOARD_RAM
// PARAM: +Number|COIN_REWARD
// PARAM: +Number|EASY_CPU_ACCURACY_PERCENT
// PARAM: +Number|NORMAL_CPU_ACCURACY_PERCENT
// PARAM: +Number|HARD_CPU_ACCURACY_PERCENT

#ifndef USES_BOARD_RAM
#define USES_BOARD_RAM 1
#endif

#ifndef COIN_REWARD
#define COIN_REWARD 10
#endif

#ifndef EASY_CPU_ACCURACY_PERCENT
#define EASY_CPU_ACCURACY_PERCENT 25
#endif

#ifndef NORMAL_CPU_ACCURACY_PERCENT
#define NORMAL_CPU_ACCURACY_PERCENT 50
#endif

#ifndef HARD_CPU_ACCURACY_PERCENT
#define HARD_CPU_ACCURACY_PERCENT 75
#endif

//***************************************************************************//
//******************** BOARD RAM WARNING ************************************//
//***************************************************************************//
// This board uses the following addresses (board RAM)
//               D_800CD0A0;    //board_ram9
//               D_800CD0A1;    //board_ram10
//               D_800CD098;    //board_ram11
//
// If any other events on your board use these values, you will get unexpected behavior! 
//
// You can disable this quiz's use of board RAM by setting the "USES_BOARD_RAM" parameter to
// "False" in PartyPlanner when you place the event.
//
// The board RAM is used to ensure questions are asked without repeats.  So, if you disable
// the use of board RAM, the questions will still be randomly selected, but they may repeat.
//
// If you're not sure what this parameter does, just set the "USES BOARD RAM" to true. =)

//***************************************************************************//
//******************** Version Info! ****************************************//
//***************************************************************************//


// This is version: 1.2
//
// There may be an update available at:
// https://github.com/c-kennelly/mario-party-custom-events
// You can also report a bug as an issue (and maybe a PR that fixes it?)


//***************************************************************************//
//*********************** Description ***************************************//
//***************************************************************************//
// This event challenges a player to a quiz and asks them a random question
// from the question bank.
//  
// The event design is based on Airsola's original "Trivia Quiz" event, with
// some enchancments I hope you'll enjoy.  You can find that original quiz at:
// www.mariopartylegacy.com/forum/index.php?action=downloads;sa=view;down=305
//
//                                 Features:
//
//  ***** 64 Questions ****
// There are 64 default questions in the question bank, all based on N64-era 
// Nintendo trivia.  You can customize the entire question bank.
//
// ***** Get coins for correct answers ****
// If the player gets the question right, a coin reward is given.
// If the player gets the question wrong, nothing happens.
// This behavior is customizable - see "Where to Customize" below.
//
// ***** Randomized choice ordering ****
// Each time a question appears, the choices will appear in a different order
// so that the quiz is fresh on repeated plays.
//
// ***** Smart CPU's ****
// CPU's have different accuracy rates based on their difficulty, which is
// set in the event parameters.  Some recommended defaults:
// Easy     --  25% chance of getting a question right  (value = 25)
// Normal   --  50% chance of getting a question right  (value = 50)
// Hard     --  75% chance of getting a question right. (value = 75)
//
// ***** No repeats until all questions have been asked ****
// The v1.1 update ensured that you'll see all the questions in the question bank
// before the quiz asks a repeat question.  The quiz starts on a different random question
// each game, and then asks the questions in sequence.  If the quiz has exactly 64 active questions
// the sequence the questions are asked in will also change game to game.  v1.15 allows you to disable
// this behavior in case you want to prioritize another event that uses the same addresses.
// 
//
// Finally, this file is commented to make it as easy as possible for 
// non-programmers to edit and beginners to pull code samples, so I've 
// leaned on the verbose side for comments.  If you've a working knowledge 
// of C, feel free to just jump to main() and see what's going on. 


//***************************************************************************//
//***********************     Changelist      *******************************//
//***************************************************************************//
//   Version 1.2    - Brought number of questions up to 64 for release.
//   Version 1.15   - Added ability to disable the event's use of Board RAM. 
//                    Reorganized comments a bit.
//   Version 1.1    - Questions no longer repeat unless all questions have been asked!
//   Version 1.0    - First version of the event!


//***************************************************************************//
//******************** Where to Customize ***********************************//
//***************************************************************************//
// Jump to "Quiz Configuration" to change the number of active questions 
// in the quiz bank, or the character who is giving the quiz the questions.
//
// Jump to "Message Configuration" to customize messages such as the greeting
// or when a player gets a correct/incorrect answer.
//
// Jump to "RewardPlayerForCorrectAnswer()" to change behavior when the player
// gets the answer right, or "PunishPlayerForIncorrectAnswer()" to change
// what happens when the player gets the answer wrong.
//
// Finally, jump to "Question Definitions" to customize the question bank.


//***************************************************************************//
//******************** Quiz Configuration ***********************************//
//***************************************************************************//

// How many questions should the quiz use?
#define ACTIVE_QUESTIONS 64

// The character portrait of the character who is giving the quiz.
#define QUIZ_GIVER_PORTRAIT 3
// Want to change the picture?  Find options on the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*************************** Declarations **********************************//
//***************************************************************************//

// Used for the data types used in the player struct:
// Header file: http://n64devkit.square7.ch/header/ultra64.htm
// Ultratypes: http://n64devkit.square7.ch/header/ultratypes.htm
// For more exploration: http://n64devkit.square7.ch/header/
#include "ultra64.h"

//Event parameter can disable these:
extern u8 D_800CD0A0;
extern u8 D_800CD0A1;
extern u8 D_800CD098;

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

    void *obj;              // Offset 36:  struct object pointer
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
//*************************** Quiz Logic ************************************//
//***************************************************************************//

void main() 
{
    mp3_play_idle_animation();
    DisplayGreetingMessage();

    int correctAnswer = AskTheQuestion();
    int answerChosen = GetResponseAndTeardownMessageBox(correctAnswer);
    
    if(answerChosen == correctAnswer)
    {   
        RewardPlayerForCorrectAnswer(); 
    }
    else
    {   
        PunishPlayerForIncorrectAnswer(); 
    }
    
    return;
}


// Displays a message to intro the quiz.
void DisplayGreetingMessage()
{
   // Display the ready message...
    char* greeting_msg = GetGreetingMessage();
    mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, greeting_msg);
}

// Selects the next question to be asked and returns the correct answer for that question
int AskTheQuestion()
{
    int index = PickQuestionIndex();

    int correctAnswer = 0;
    char *question_msg = GetQuestionByNumber(index, &correctAnswer);

    ShowMessage(QUIZ_GIVER_PORTRAIT, question_msg, 0, 0, 0, 0, 0);

    return correctAnswer;
}

// Pick a question from the bank.  If it's the first question of the game,
// start at a random question index so that there's a different question every game.
// After we ask a question, increment the question index that will be returned next time by the StepValue.
// This will step through the question list so that we never ask a repeat question until all questions in
// the bank have been asked.
//
// ******** Perisistent board RAM values used.
// ******** If any other events on your board use these values, you will get unexpected behavior!!!***
// D_800CD0A0 -> FirstQuestionIndex;        The question index for the first question
// D_800CD0A1 -> NumQuestionsAsked;         The number of questions asked so far
// D_800CD098 -> StepValue;                 The index we step by for each successive question.
//
// If board RAM is disabled - presumably because the board creator is using another event that clashes and didn't know
// how to edit this event to use different addresses - fallback on selecting a random question from the question bank each time.
int PickQuestionIndex()
{
    int result = 0;

    // If the event is allowed to use the persistent memory addresses, prevent repeat questions.
    // This behavior is set by event parameter in PartyPlanner.
    if(USES_BOARD_RAM == 1)
    {

        // If we haven't set a StepValue, it's the first time the event is called.
        // So we need to initialize the quiz data.
        if(D_800CD098 == 0)
        {
            InitializeQuizData();
        }

        // Take the initial question index, and roll foward by the step counter to get the index
        // of the next question we should ask. Mod by total active questions so we always have a valid index.
        //
        // result = ((FirstQuestionIndex + (NumQuestionsAsked * StepValue)) % ACTIVE_QUESTIONS);
        result = ( (D_800CD0A0 + (D_800CD0A1 * D_800CD098) ) % ACTIVE_QUESTIONS);

        //increment NumQuestionsAsked counter.
        D_800CD0A1 = (D_800CD0A1 + 1);
    }
    // If not allowed to use persistent memory addresses, best we can do is select a random question each time.
    else
    {
        result = mp3_PickARandomNumberBetween0AndN(ACTIVE_QUESTIONS);
    }

    return result;
}

// The first time the event is called, decide what question we're starting on and what sequence
// the questions will be asked in.  Not called if we've disabled board RAM.
void InitializeQuizData()
{
    // Pick a random first question.
    D_800CD0A0 = mp3_PickARandomNumberBetween0AndN(ACTIVE_QUESTIONS);

    // When we have a question bank size that is divisible by 16, stepping through the questions in increments of 1, 5, or 7
    // will guarantee we don't repeat a question until all questions have been asked once.  Randomly selecting a different StepValue
    // each game means the question order will vary game to game.
    //
    // The rest of this function is the logic to randomly choose a value of 1, 5, or 7 
    // and initialize the StepValue with it.
    //
    // If you've modified ACTIVE_QUESTIONS to not be divisible by 16, the quiz defaults
    // to using a step of "1" every time.  That means your quiz will still start in a random place,
    // but questions will be asked in the same order every game.  (For example, Question 26 will ALWAYS
    // be asked after Question 25).   Feel free to edit this function if you'd like, but you'll need to 
    // recalculate good step values and fill them in by hand.
    //
    // Set the StepValue:
    if((ACTIVE_QUESTIONS % 16 == 0) && 
        (ACTIVE_QUESTIONS % 5 != 0) && 
        (ACTIVE_QUESTIONS % 7 != 0))  //These values have been tested for question banks at size: 16, 32, 48, 64, and 96.
    {
        int stepIndex = mp3_PickARandomNumberBetween0AndN(3);

        switch (stepIndex)
        {
        case (0):
            D_800CD098 = 1;
            break;
        case (1):
            D_800CD098 = 5;
            break;
        case (2):
            D_800CD098 = 7;
            break;
        default:
            D_800CD098 = 1;
            break;
        }
    }
    else
    {
        D_800CD098 = 1;
    }
}

// Display a message and return the answer chosen 
// Also has defined logic for CPUs.
int GetResponseAndTeardownMessageBox(int correctAnswer)
{
    int cpuChoice = GetChoiceForCPU(correctAnswer);

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

// Select an answer to the quiz question for a CPU player based on difficulty level
// Each difficulty level has a chance of automatically getting the question right.
// If that roll fails, then they select an incorrect answer.
// Recommended settings:
// 25%  -- Easy
// 50%  -- Normal
// 75%  -- Hard
// 100% -- Hard & Waluigi (since he is a sneaky bastard, of course)
int GetChoiceForCPU(int correctAnswer)
{
    int maxValidIndex = 3;  // 4 answers to questions mean max valid index is 3 (0, 1, 2, and 3).
    int cpuChoice = 0;


    int playerIndex = GetCurrentPlayerIndex();
    if (PlayerIsCPU(playerIndex))
    {
        PauseCPUToConsiderAnswer(15);
        
        u8 cpuDifficulty = GetCPUDifficulty(playerIndex);

        if (cpuDifficulty == 0)  // Easy
        {
            if(mp3_ReturnTruePercentOfTime(EASY_CPU_ACCURACY_PERCENT))
            {
                cpuChoice = correctAnswer;
            }   
            else 
            {
                cpuChoice = (correctAnswer + 1) % maxValidIndex;        //Incorrect answer clamped to valid index.
            }
        }
        else if (cpuDifficulty == 1)  // Normal
        {
            if(mp3_ReturnTruePercentOfTime(NORMAL_CPU_ACCURACY_PERCENT))
            {
                cpuChoice = correctAnswer;
            }   
            else 
            {
                cpuChoice = (correctAnswer + 1) % maxValidIndex;        //Incorrect answer clamped to valid index.
            }
        }
        else // Hard
        {
            // I am contractually obligated to make Waluigi the hardest CPU, and he's always wahhtching...
            if( mp3_ReturnTruePercentOfTime(HARD_CPU_ACCURACY_PERCENT) || IsWaluigi(playerIndex) )
            {    
                cpuChoice = correctAnswer;
            }   
            else
            {
                cpuChoice = (correctAnswer + 1) % maxValidIndex;        //Incorrect answer clamped to valid index.
            }
        }
    }
    
    return cpuChoice;
}

// Pausing to let question be displayed when CPU player is looking at answer.
// Without this, CPU's blaze through the questions and players don't have time to read the longer ones.
void PauseCPUToConsiderAnswer(int msToPause)
{
        SleepProcess(msToPause);
}

// Returns true if the character wahs romantically.
int IsWaluigi(int playerIndex)
{
    struct Player *p = GetPlayerStruct(playerIndex);
    if(p != NULL && p->character == 6)  // Mario=0, Luigi=1, Peach=2, Yoshi=3, Wario=4, DK=5, Waluigi=6, Daisy=7
    {
        return 1;
    }
    else
    {
        return 0; 
    }
}

// Returns the difficulty of the CPU.  Uses ultra.64 types
u8 GetCPUDifficulty(int playerIndex)
{
    struct Player *p = GetPlayerStruct(playerIndex);

    if (p != NULL )
    {
        return p->cpu_difficulty;       //0:Easy; 1:Normal; 2:Hard
    }
    else
    {
        return 1;       //Couldn't get player index, which means things are borked.  Assume normal difficulty.
    }
    
}




// When the player gets a correct answer, run this logic.
void RewardPlayerForCorrectAnswer()
{
        char *right_msg = GetMessageForRightAnswer();
        mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, right_msg);
        GraduallyAdjustPlayerCoins(COIN_REWARD);

        return;
}

// When the player gets an answer wrong, run this logic.
void PunishPlayerForIncorrectAnswer()
{
    // By default we just punish them with disappointment,
    // but feel free to add your own dastardly deed.
    char *wrong_msg = GetMessageforWrongAnswer();
    mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, wrong_msg);
    

    // If you want to give a coin penalty add a:
    // [+Number|COIN_PENALTY] parameter to the
    // PartyPlanner64 header at the top of the file
    // and then uncomment the following two lines below.
    // You'll also want to change the SleepProcess line to
    // SleepProcess(40) since adjust will wait 30 on its own.
    
    // Play the sad animation and wait 70 frames for it to complete
    mp3_play_sad_animation();
    //int coinPenalty = -1 * COIN_PENALTY;
    //GraduallyAdjustPlayerCoins(coinPenalty);
    SleepProcess(70);

    return;
}

// Adjust the current player's coins by the specified amount.
// Takes negative numbers.
void GraduallyAdjustPlayerCoins(int adjustmentAmount)
{
    // Get the player whos turn it is.
    int currentPlayerIndex = GetCurrentPlayerIndex();
    // play the joy animation for the current player
    mp3_play_joy_animation();
    
    //Some mp3-defined functions to change the coins
    AdjustPlayerCoinsGradual(currentPlayerIndex, adjustmentAmount);
    ShowPlayerCoinChange(currentPlayerIndex, adjustmentAmount);

    // Sleep for 30 frames to let the coin change take effect.
    // Also syncs up nicely with the joy animation.
    SleepProcess(30);
}


//***************************************************************************//
//***************************************************************************//
//***************************************************************************//
//***********************   Message Configuration   *************************//
//***************************************************************************//
//***************************************************************************//
//***************************************************************************//

// Defines the message the Quiz Giver displays first.
char* GetGreetingMessage()
{
    char* result = func_80035934(256);      // First, malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Second, zero out the memory allocated above so we don't get unexpected behavior.

    my_strcpy(result, "\x0B");                              // Start the message
    my_strncat(result, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(result, "Ready for a quiz");                 
    my_strncat(result, "\xC3");                             // ?
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A");                 // Standard padding for portrait
    my_strncat(result, "If you get the question right");    
    my_strncat(result, "\x82");                             // ,
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A");                 //Standard padding for portrait
    my_strncat(result, "you can earn some coins");          
    my_strncat(result, "\xC2");                             // !
    my_strncat(result, "\xFF");                             //Show prompt to continue arrow

    return result;
}

// Defines the message shown if the player gives the correct answer.
char* GetMessageForRightAnswer()
{
    char* result = func_80035934(256);      // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Zero out the memory allocated above so we don't get unexpected behavior.
    
    my_strcpy(result, "\x0B");                              // Start the message
    my_strncat(result, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(result, "Yes");                 
    my_strncat(result, "\x82");                             // ,
    my_strncat(result, " that");
    my_strncat(result, "\x5C");                             // ' 
    my_strncat(result, "s right");
    my_strncat(result, "\xC2");                             // !
    my_strncat(result, "\xFF");                             //Show prompt to continue arrows

    return result;
}

// Defines the message shown if the player gets the answer wrong.
char* GetMessageforWrongAnswer()
{
    char* result = func_80035934(256);      // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Zero out the memory allocated above so we don't get unexpected behavior.
    
    my_strcpy(result, "\x0B");                              // Start the message
    my_strncat(result, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(result, "I");                 
    my_strncat(result, "\x5C");                             // ' 
    my_strncat(result, "m so sorry");
    my_strncat(result, "\x82");                             // ,
    my_strncat(result, " that");
    my_strncat(result, "\x5C");                             // ' 
    my_strncat(result, "s not correct");
    my_strncat(result, "\x85\x85\x85");                     //...
    my_strncat(result, "\xFF");                             //Show prompt to continue arrows

    return result;
}


//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//********************** Question Definitions *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//


/******** How to Configure Questions ***********/
// 1. This quiz has 64 questions slots, all customizable.
//     
// 2. You can control how many questions are active by editing
//    the definition "ACTIVE QUESTIONS" at the top of the file.
//    So, if you wanted to populate the question bank with your
//    own questions, but only wanted to write 20 of them, you
//    would set the "ACTIVE_QUESTIONS" definition to 20.
//
// 3. Questions get used **in order**, up to whatever number is
//    defined by ACTIVE QUESTIONS at the top of the file.  So if
//    you limited ACTIVE_QUESTIONS to 20, then questions 1-20 would
//    be active, and questions 21-64 would be inactive.
// 
// 4. To customize one of the 64 questions, simply edit the
//    corresponding message and ensure the answers are assigned
//    correctly.  You don't have to do anything else.
//
// 5. If you want to increase array sizes for answers to support more characters, 
//    go for it; just know the quiz has only been tested with the default sizes.
//    
// 6. If 64 questions aren't enough for you, you're a monster.
//    But it can be done.  You'll have to define the new question 
//    AND edit the switch(case) in "GetQuestionByNumber()" to make 
//    sure the question finder can randomly pick your question.  
//    Follow the pattern and you'll be fine!




// Returns a question that corresponds to the number passed in.
// 
// If the number doesn't have a question assigned, it will default
// to the first question.
char* GetQuestionByNumber(int question, int* correctAnswerPtr)
{
    char* result = func_80035934(1024);         // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 1024);                        // Zero out the memory allocated above so we don't get unexpected behavior.

    switch (question)
    {
        case (0):
            result = GetFirstQuestion(correctAnswerPtr);
            break;
        case (1):
            result = GetSecondQuestion(correctAnswerPtr);
            break;
        case (2):
            result = GetThirdQuestion(correctAnswerPtr);
            break;
        case (3):
            result = GetFourthQuestion(correctAnswerPtr);
            break;
        case (4):
            result = GetFifthQuestion(correctAnswerPtr);
            break;
        case (5):
            result = GetSixthQuestion(correctAnswerPtr);
            break;
        case (6):
            result = GetSeventhQuestion(correctAnswerPtr);
            break;
        case (7):
            result = GetEigthQuestion(correctAnswerPtr);
            break;
        case (8):
            result = GetNinthQuestion(correctAnswerPtr);
            break;
        case (9):
            result = GetTenthQuestion(correctAnswerPtr);
            break;
        case (10):
            result = GetEleventhQuestion(correctAnswerPtr);
            break;
        case (11):
            result = GetTwelfthQuestion(correctAnswerPtr);
            break;
        case (12):
            result = GetThirteenthQuestion(correctAnswerPtr);
            break;
        case (13):
            result = GetFourteenthQuestion(correctAnswerPtr);
            break;
        case (14):
            result = GetFifteenthQuestion(correctAnswerPtr);
            break;
        case (15):
            result = GetSixteenthQuestion(correctAnswerPtr);
            break;
        case (16):
            result = GetSeventeenthQuestion(correctAnswerPtr);
            break;
        case (17):
            result = GetEighteenthQuestion(correctAnswerPtr);
            break;
        case (18):
            result = GetNinteenthQuestion(correctAnswerPtr);
            break;
        case (19):
            result = GetTwentiethQuestion(correctAnswerPtr);
            break;
        case (20):
            result = GetTwentyFirstQuestion(correctAnswerPtr);
            break;
        case (21):
            result = GetTwentySecondQuestion(correctAnswerPtr);
            break;
        case (22):
            result = GetTwentyThirdQuestion(correctAnswerPtr);
            break;
        case (23):
            result = GetTwentyFourthQuestion(correctAnswerPtr);
            break;
        case (24):
            result = GetTwentyFifthQuestion(correctAnswerPtr);
            break;
        case (25):
            result = GetTwentySixthQuestion(correctAnswerPtr);
            break;
        case (26):
            result = GetTwentySeventhQuestion(correctAnswerPtr);
            break;
        case (27):
            result = GetTwentyEighthQuestion(correctAnswerPtr);
            break;
        case (28):
            result = GetTwentyNinthQuestion(correctAnswerPtr);
            break;
        case (29):
            result = GetThirtiethQuestion(correctAnswerPtr);
            break;
        case (30):
            result = GetThirtyFirstQuestion(correctAnswerPtr);
            break;
        case (31):
            result = GetThirtySecondQuestion(correctAnswerPtr);
            break;
        case (32):
            result = GetThirtyThirdQuestion(correctAnswerPtr);
            break;
        case (33):
            result = GetThirtyFourthQuestion(correctAnswerPtr);
            break;
        case (34):
            result = GetThirtyFifthQuestion(correctAnswerPtr);
            break;
        case (35):
            result = GetThirtySixthQuestion(correctAnswerPtr);
            break;
        case (36):
            result = GetThirtySeventhQuestion(correctAnswerPtr);
            break;
        case (37):
            result = GetThirtyEighthQuestion(correctAnswerPtr);
            break;
        case (38):
            result = GetThirtyNinthQuestion(correctAnswerPtr);
            break;
        case (39):
            result = GetFourtiethQuestion(correctAnswerPtr);
            break;
        case (40):
            result = GetFourtyFirstQuestion(correctAnswerPtr);
            break;
        case (41):
            result = GetFourtySecondQuestion(correctAnswerPtr);
            break;
        case (42):
            result = GetFourtyThirdQuestion(correctAnswerPtr);
            break;
        case (43):
            result = GetFourtyFourthQuestion(correctAnswerPtr);
            break;
        case (44):
            result = GetFourtyFifthQuestion(correctAnswerPtr);
            break;
        case (45):
            result = GetFourtySixthQuestion(correctAnswerPtr);
            break;
        case (46):
            result = GetFourtySeventhQuestion(correctAnswerPtr);
            break;
        case (47):
            result = GetFourtyEighthQuestion(correctAnswerPtr);
            break;
        case (48):
            result = GetFourtyNinthQuestion(correctAnswerPtr);
            break;
        case (49):
            result = GetFifteithQuestion(correctAnswerPtr);
            break;
        case (50):
            result = GetFiftyFirstQuestion(correctAnswerPtr);
            break;
        case (51):
            result = GetFiftySecondQuestion(correctAnswerPtr);
            break;
        case (52):
            result = GetFiftyThirdQuestion(correctAnswerPtr);
            break;
        case (53):
            result = GetFiftyFourthQuestion(correctAnswerPtr);
            break;
        case (54):
            result = GetFiftyFifthQuestion(correctAnswerPtr);
            break;
        case (55):
            result = GetFiftySixthQuestion(correctAnswerPtr);
            break;
        case (56):
            result = GetFiftySeventhQuestion(correctAnswerPtr);
            break;
        case (57):
            result = GetFiftyEighthQuestion(correctAnswerPtr);
            break;
        case (58):
            result = GetFiftyNinthQuestion(correctAnswerPtr);
            break;
        case (59):
            result = GetSixteithQuestion(correctAnswerPtr);
            break;
        case (60):
            result = GetSixtyFirstQuestion(correctAnswerPtr);
            break;
        case (61):
            result = GetSixtySecondQuestion(correctAnswerPtr);
            break;
        case (62):
            result = GetSixtyThirdQuestion(correctAnswerPtr);
            break;
        case (63):
            result = GetSixtyFourthQuestion(correctAnswerPtr);
            break;
        default:
           result = GetFirstQuestion(correctAnswerPtr);
           return result;
    }

    // Want to test a sepcific question?  Edit and uncomment 
    // the following line to ensure it always gets picked.
    // Don't forget to recomment when you are done testing!
    
    //result = GetFirstQuestion(correctAnswerPtr);

    return result;
}

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************     Question Bank    *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 0
char* GetFirstQuestion(int* correctAnswerIndexPtr)
{
    // This sample question demonstrates how to make a 2-line simple question.
    // Each line is its own argument to the function. 
    // This question will display as:
    //  
    //      According to Shigeru Miyamoto,
    //      who is Bowser Jr.'s mother?
    //
    // (final question mark and newline will be added automatically)
    // As you can see, special characters or punctuation can be used inline.
    //    Find special characters at this community resource:
    //    https://github.com/PartyPlanner64/PartyPlanner64/wiki/String-Encoding

    char* question = CreateSimpleTwoLineQuestionMessage("According to Shigeru Miyamoto\x82", "who is Bowser Jr\x85\x5Cs mother");

    // After you have defined the question, you define the answers.
    //
    // In this case, correct answer is "Shigeru Miyamoto" so we assign it to the "correctAnswer variable."
    // You then fill in three incorrect answers (note, you can still  use special characters inline here)
    // and pass them to the GenerateMessageForQuestionWithFourOptions() function.
    //
    // The Generate...() function will scramble the order of the possible answers every single time.

    char correctAnswer[48] = "Shigeru Miyamoto";
    char wrongAnswer1[48] = "Bowsette";
    char wrongAnswer2[48] = "Peach";
    char wrongAnswer3[48] = "Wendy O\x85 Koopa";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 1
char* GetSecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 1", "released in Japan");

    char correctAnswer[48] = "1998";
    char wrongAnswer1[48] = "1997";
    char wrongAnswer2[48] = "1999";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 2
char* GetThirdQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 1", "released in the US");

    char correctAnswer[48] = "1999";
    char wrongAnswer1[48] = "1997";
    char wrongAnswer2[48] = "1998";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 3
char* GetFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 2", "released in Japan");

    char correctAnswer[48] = "1999";
    char wrongAnswer1[48] = "1998";
    char wrongAnswer2[48] = "2001";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 4
char* GetFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 2", "released in the US");

    char correctAnswer[48] = "2000";
    char wrongAnswer1[48] = "1998";
    char wrongAnswer2[48] = "2001";
    char wrongAnswer3[48] = "1999";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 5
char* GetSixthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What does Peach make Mario");

    char correctAnswer[48] = "Cake";
    char wrongAnswer1[48] = "Mushrooms";
    char wrongAnswer2[48] = "Lonely";
    char wrongAnswer3[48] = "Spaghetti";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 6
char* GetSeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What year was Pokemon Snap released");

    char correctAnswer[48] = "1999";
    char wrongAnswer1[48] = "1998";
    char wrongAnswer2[48] = "1997";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 7
char* GetEigthQuestion(int* correctAnswerIndexPtr)
{   
    char* question = CreateSimpleTwoLineQuestionMessage("What was the name of the vehicle", "that carries Todd through Pokemon Snap");

    char correctAnswer[48] = "Zero\x3DOne";
    char wrongAnswer1[48] = "It didn\x5Ct have a name";
    char wrongAnswer2[48] = "Poke\x3DTrekker";
    char wrongAnswer3[48] = "V\x3DZero";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 8
char* GetNinthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What game did Conker the Squirrel", "first appear in");

    char correctAnswer[48] = "Diddy Kong Racing";
    char wrongAnswer1[48] = "Conker\x5Cs Bad Fur Day";
    char wrongAnswer2[48] = "Conker\x5Cs Pocket Talesg";
    char wrongAnswer3[48] = "Twelve Tales\x7B Conker 64";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 9
char* GetTenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Pokemon Snap started development at HAL", "with what fairy tale theme");

    char correctAnswer[48] = "Jack and the Beanstalk";
    char wrongAnswer1[48] = "Hansel and Gretel";
    char wrongAnswer2[48] = "Goldilocks and the Three Bears";
    char wrongAnswer3[48] = "Little Red Riding Hood";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 10
char* GetEleventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What is the theme of the first level in Pokemon Snap");

    char correctAnswer[48] = "Beach";
    char wrongAnswer1[48] = "River";
    char wrongAnswer2[48] = "Jungle";
    char wrongAnswer3[48] = "Cave";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 11
char* GetTwelfthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("How many pictures can be taken per course in Pokemon Snap");

    char correctAnswer[48] = "60";
    char wrongAnswer1[48] = "30";
    char wrongAnswer2[48] = "45";
    char wrongAnswer3[48] = "Unlimited";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 12
char* GetThirteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What is the only Nintendo 64 game", "that sold more than ten million units");

    char correctAnswer[48] = "Super Mario 64";
    char wrongAnswer1[48] = "The Legend of Zelda\x7B Ocarina of Time";
    char wrongAnswer2[48] = "Mario Kart 64";
    char wrongAnswer3[48] = "Super Smash Bros";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 13
char* GetFourteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("About how many Nintendo 64 games", "sold more than one million units");

    char correctAnswer[48] = "40 \x3D 49";
    char wrongAnswer1[48] = "20 \x3D 29";
    char wrongAnswer2[48] = "30 \x3D 39";
    char wrongAnswer3[48] = "50 \x3D 59";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 14
char* GetFifteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Other than the Mario titles\x82 what", "was the most popular franchise on the N64 by combined sales");
    
    char correctAnswer[48] = "Pokemon";
    char wrongAnswer1[48] = "The Legend of Zelda";
    char wrongAnswer2[48] = "Donkey Kong";
    char wrongAnswer3[48] = "Star Wars";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;   
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 15
char* GetSixteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What was the first game that Banjo", "appeared in");

    char correctAnswer[48] = "Diddy Kong Racing";
    char wrongAnswer1[48] = "Banjo Kazooie";
    char wrongAnswer2[48] = "Conker\x5Cs Pocket Tales";
    char wrongAnswer3[48] = "Battletoads";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 48
char* GetSeventeenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What color are Banjo\x5Cs shorts");

    char correctAnswer[48] = "Yellow";
    char wrongAnswer1[48] = "Blue";
    char wrongAnswer2[48] = "Orange";
    char wrongAnswer3[48] = "He does not wear shorts\x85";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 17
char* GetEighteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What color is Grunty\x5Cs scarf");

    char correctAnswer[48] = "Purple";
    char wrongAnswer1[48] = "Red";
    char wrongAnswer2[48] = "Green";
    char wrongAnswer3[48] = "Blue";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 18
char* GetNinteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("What number am I thinking of\x82 Banjo");
    
    char correctAnswer[48] = "Two";
    char wrongAnswer1[48] = "One";
    char wrongAnswer2[48] = "Three";
    char wrongAnswer3[48] = "Four";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 19
char* GetTwentiethQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What was the Nintendo 64 named when", "it was first revealed to the public");

    char correctAnswer[48] = "Ultra 64";
    char wrongAnswer1[48] = "Super 64";
    char wrongAnswer2[48] = "Mega 64";
    char wrongAnswer3[48] = "Plus Ultra 64";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 20
char* GetTwentyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What was the GameCube codenamed", "during development");

    char correctAnswer[48] = "Project Dolphin";
    char wrongAnswer1[48] = "Project Cubed";
    char wrongAnswer2[48] = "Project Revolution";
    char wrongAnswer3[48] = "Project Ultra";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 21
char* GetTwentySecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What was the Wii U codenamed", "during development");

    char correctAnswer[48] = "Project Cafe";
    char wrongAnswer1[48] = "Project Nitro";
    char wrongAnswer2[48] = "Project Revolution";
    char wrongAnswer3[48] = "Project Wiiquel";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 22
char* GetTwentyThirdQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("What is the name of the rabbit found", "in the basement of Peach\x5Cs Castle", "in Super Mario 64");
    
    char correctAnswer[48] = "MIPS";
    char wrongAnswer1[48] = "Bink";
    char wrongAnswer2[48] = "Fleep";
    char wrongAnswer3[48] = "Lola";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 23
char* GetTwentyFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Where did Waluigi first", "make his appearance");

    char correctAnswer[48] = "Mario Tennis";
    char wrongAnswer1[48] = "Mario Golf";
    char wrongAnswer2[48] = "Mario Party 2";
    char wrongAnswer3[48] = "Book of Genesis";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 24
char* GetTwentyFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Which of these Masks is \x03NOT\x08 found", "in The Legend of Zelda\x7B Ocarina of Time");

    char correctAnswer[48] = "Gibdo Mask";
    char wrongAnswer1[48] = "Mask of Truth";
    char wrongAnswer2[48] = "Zora Mask";
    char wrongAnswer3[48] = "Skull Mask";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 25
char* GetTwentySixthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("What is the name of the sword", "Link starts the game with in", "The Legend of Zelda\x7B Majoras Mask");

    char correctAnswer[48] = "Kokiri Sword";
    char wrongAnswer1[48] = "Razor Sword";
    char wrongAnswer2[48] = "Noble Sword";
    char wrongAnswer3[48] = "Hero Sword";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 26
char* GetTwentySeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("According to the DK Rap\x82 which Kong", "is the last member of the DK Crew");
    
    char correctAnswer[48] = "Chunky Kong";
    char wrongAnswer1[48] = "Lanky Kong";
    char wrongAnswer2[48] = "Tiny Kong";
    char wrongAnswer3[48] = "Coconut Gun";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 27
char* GetTwentyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Donkey Kong 64", "released worldwide");

    char correctAnswer[48] = "1999";
    char wrongAnswer1[48] = "1997";
    char wrongAnswer2[48] = "1998";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 28
char* GetTwentyNinthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Banjo Kazooie", "released worldwide");

    char correctAnswer[48] = "1998";
    char wrongAnswer1[48] = "1997";
    char wrongAnswer2[48] = "1999";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 29
char* GetThirtiethQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 3", "released in Japan");

    char correctAnswer[48] = "2000";
    char wrongAnswer1[48] = "2002";
    char wrongAnswer2[48] = "1999";
    char wrongAnswer3[48] = "2001";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 30
char* GetThirtyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Party 3", "released in the US");

    char correctAnswer[48] = "2001";
    char wrongAnswer1[48] = "2002";
    char wrongAnswer2[48] = "1999";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 31
char* GetThirtySecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("What is the name of the unreleased", "Rareware RPG that went on to", "become Banjo Kazooie");

    char correctAnswer[48] = "Project Dream";
    char wrongAnswer1[48] = "Echo Delta";
    char wrongAnswer2[48] = "Bird Bear 64";
    char wrongAnswer3[48] = "40 Winks";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 32
char* GetThirtyThirdQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("In Super Smash Bros\x85\x82 who is unlocked", "after completing Break the Targets", "with all 8 starter characters");

    char correctAnswer[48] = "Luigi";
    char wrongAnswer1[48] = "Captain Falcon";
    char wrongAnswer2[48] = "Jigglypuff";
    char wrongAnswer3[48] = "Ness";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 33
char* GetThirtyFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("What is unlocked in the Nintendo 64", "game Super Smash Bros\x85 when you play ", "100 multiplayer matches");

    char correctAnswer[48] = "Item Switch";
    char wrongAnswer1[48] = "Sound Test";
    char wrongAnswer2[48] = "The Mushroom Kindom Stage";
    char wrongAnswer3[48] = "Captain Falcon";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 34
char* GetThirtyFifthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("In The Legend of Zelda\x7B Majora\x5Cs Mask\x82",  "what mask do you earn after completing", "the Anju and Kafei Questline");
    
    char correctAnswer[48] = "The Couples Mask";
    char wrongAnswer1[48] = "The All Night Mask";
    char wrongAnswer2[48] = "Kafei\x5Cs Mask";
    char wrongAnswer3[48] = "The Mask of Scents";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 35
char* GetThirtySixthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("In The Legend of Zelda\x7B Ocarina of Time\x82", "which item is not a part of the", "Biggoron\x5Cs Sword Quest");

    char correctAnswer[48] = "Weird Egg";
    char wrongAnswer1[48] = "Odd Mushroom";
    char wrongAnswer2[48] = "Prescription";
    char wrongAnswer3[48] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 36
char* GetThirtySeventhQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("In The Legend of Zelda\x7B Majora\x5Cs Mask\x82 what", "mask does the Old Lady give you after", "stopping the man who robbed her");

    char correctAnswer[48] = "The Blast Mask";
    char wrongAnswer1[48] = "The All Night Mask";
    char wrongAnswer2[48] = "The Mask of Scents";
    char wrongAnswer3[48] = "The Stone Mask";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//


// Question for index 37
char* GetThirtyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("What year was Mario Kart 64", "released in the US");

    char correctAnswer[48] = "1997";
    char wrongAnswer1[48] = "1998";
    char wrongAnswer2[48] = "1999";
    char wrongAnswer3[48] = "2000";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 38
char* GetThirtyNinthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleOneLineQuestionMessage("In what game did Mario make his debut");

	char correctAnswer[48] = "Donkey Kong";
	char wrongAnswer1[48] = "Mario Bros\x85";
	char wrongAnswer2[48] = "Super Mario Bros\x85";
	char wrongAnswer3[48] = "Punch Out\xC2\xC2";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 39
char* GetFourtiethQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleOneLineQuestionMessage("In what game did Luigi make his debut");

	char correctAnswer[48] = "Mario Bros\x85";
	char wrongAnswer1[48] = "Super Mario Bros\x85";
	char wrongAnswer2[48] = "Vs\x85 Super Mario Bros\x85";
	char wrongAnswer3[48] = "Luigi\x5Cs Hammer Toss";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 40
char* GetFourtyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("In what game did Princess Peach", "make her debut");

    char correctAnswer[48] = "Super Mario Bros\x85";
    char wrongAnswer1[48] = "Donkey Kong";
    char wrongAnswer2[48] = "Super Mario Bros\x85 2";
    char wrongAnswer3[48] = "Super Mario Land";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 41
char* GetFourtySecondQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("In what game did Princess Daisy", "make her debut");

    char correctAnswer[48] = "Super Mario Land";
    char wrongAnswer1[48] = "Mario Party 2";
    char wrongAnswer2[48] = "Mario Tennis";
    char wrongAnswer3[48] = "Mario Golf";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 42
char* GetFourtyThirdQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleOneLineQuestionMessage("In what game did Wario make his debut");
    
    char correctAnswer[48] = "Super Mario Land 2\x7B 6 Golden Coins";
    char wrongAnswer1[48] = "Wario Land";
    char wrongAnswer2[48] = "Mario Kart 64";
    char wrongAnswer3[48] = "Mario Tennis";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 43
char* GetFourtyFourthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these games was never bundled", "with the Nintendo 64 console during its lifetime");

	char correctAnswer[48] = "The Legend of Zelda\x7B Ocarina of Time";
	char wrongAnswer1[48] = "Pokemon Stadium";
	char wrongAnswer2[48] = "Star Wars Episode 1\x7B Racer";
	char wrongAnswer3[48] = "GoldenEye 007";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 44
char* GetFourtyFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("In what game did Donkey Kong", "make his debut");

    char correctAnswer[48] = "Donkey Kong Country";
    char wrongAnswer1[48] = "Donkey Kong";
    char wrongAnswer2[48] = "Donkey Kong Jr\x85";
    char wrongAnswer3[48] = "Donkey Konga";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 45
char* GetFourtySixthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleOneLineQuestionMessage("In what game did Yoshi make his debut");

    char correctAnswer[48] = "Super Mario World";
    char wrongAnswer1[48] = "Yoshi";
    char wrongAnswer2[48] = "Yoshi\x5Cs Cookie";
    char wrongAnswer3[48] = "Super Mario Bros\x85 3";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 46
char* GetFourtySeventhQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleThreeLineQuestionMessage("What was the in\x3Dgame name of the", "device that let you communicate with Pokemon", "in Hey You\x82 Pikachu");
    
    char correctAnswer[48] = "PokeHelper";
    char wrongAnswer1[48] = "Voice Recognition Unit";
    char wrongAnswer2[48] = "PokeTalker";
    char wrongAnswer3[48] = "PikaPhone";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 47
char* GetFourtyEighthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games did", "\x03NOT\x08 require the Expansion Pack");


	char correctAnswer[48] = "Pokemon Stadium 2";
	char wrongAnswer1[48] = "Donkey Kong 64";
	char wrongAnswer2[48] = "Perfect Dark";
	char wrongAnswer3[48] = "The Legend of Zelda\x7B Majora\x5Cs Mask";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 48
char* GetFourtyNinthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Pokemon Stadium";
	char wrongAnswer1[48] = "Donkey Kong 64";
	char wrongAnswer2[48] = "Diddy Kong Racing";
	char wrongAnswer3[48] = "Star Fox 64";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 49
char* GetFifteithQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Mario Kart 64";
	char wrongAnswer1[48] = "GoldenEye 007";
	char wrongAnswer2[48] = "The Legend of Zelda\x7B Ocarina of Time";
	char wrongAnswer3[48] = "Super Smash Bros";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 50
char* GetFiftyFirstQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Banjo Kazooie";
	char wrongAnswer1[48] = "Pokemon Snap";
	char wrongAnswer2[48] = "The Legend of Zelda\x7B Majora\x5Cs Mask";
	char wrongAnswer3[48] = "Pokemon Stadium";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 51
char* GetFiftySecondQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Mario Party";
	char wrongAnswer1[48] = "Mario Party 2";
	char wrongAnswer2[48] = "Mario Party 3";
	char wrongAnswer3[48] = "Mario Tennis";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 52
char* GetFiftyThirdQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Star Wars Episode 1\x7B Racer";
	char wrongAnswer1[48] = "Wave Race 64";
	char wrongAnswer2[48] = "Yoshi\x5Cs Story";
	char wrongAnswer3[48] = "Perfect Dark";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 53
char* GetFiftyFourthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Excitebike 64";
	char wrongAnswer1[48] = "Kirby 64\x7B The Crystal Shards";
	char wrongAnswer2[48] = "Mario Party 3";
	char wrongAnswer3[48] = "Tony Hawk\x5Cs Pro Skater";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 54
char* GetFiftyFifthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games sold", "the \x03MOST\x08 copies");

	char correctAnswer[48] = "Mario Golf";
	char wrongAnswer1[48] = "Paper Mario";
	char wrongAnswer2[48] = "F-Zero X";
	char wrongAnswer3[48] = "Bomberman 64";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 55
char* GetFiftySixthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Besides Mario titles\x82 which N64 series", "sold the \x03MOST\x08 copies");

	char correctAnswer[48] = "Pokemon";
	char wrongAnswer1[48] = "The Legend of Zelda";
	char wrongAnswer2[48] = "Donkey Kong";
	char wrongAnswer3[48] = "Star Wars";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 56
char* GetFiftySeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Which third party \x03DEVELOPER\x08 had the most N64", "games that sold more than one million units");

    char correctAnswer[48] = "Rare";
    char wrongAnswer1[48] = "AKI Corporation";
    char wrongAnswer2[48] = "Iquana Entertainment";
    char wrongAnswer3[48] = "LucasArts";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 57
char* GetFiftyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Which third party \PUBLISHER\x08 had the most N64", "games that sold more than one million units");

    char correctAnswer[48] = "THQ";
    char wrongAnswer1[48] = "AKI Corporation";
    char wrongAnswer2[48] = "Acclaim Entertainment";
    char wrongAnswer3[48] = "Activision";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 58
char* GetFiftyNinthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which of these Nintendo 64 games were", "\x03NOT\x08 published by Rare");

	char correctAnswer[48] = "Donkey Kong 64";
	char wrongAnswer1[48] = "Diddy Kong Racing";
	char wrongAnswer2[48] = "Perfect Dark";
	char wrongAnswer3[48] = "Jet Force Gemini";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 59
char* GetSixteithQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("What is the \x03STANDARD\x08 difficulty", "in GoldenEye 007");

	char correctAnswer[48] = "Secret Agent";
	char wrongAnswer1[48] = "Agent";
	char wrongAnswer2[48] = "00 Agent";
	char wrongAnswer3[48] = "007";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 60
char* GetSixtyFirstQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which one of these game modes was", "\x03NOT\x08 available in Wave Race 64");

	char correctAnswer[48] = "Grand Prix";
	char wrongAnswer1[48] = "Championship";
	char wrongAnswer2[48] = "Time Trials";
	char wrongAnswer3[48] = "Stunt Mode";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 61
char* GetSixtySecondQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which one of these minigames was", "\x03NOT\x08 available in Pokemon Stadium");

	char correctAnswer[48] = "Eager Eevee";
	char wrongAnswer1[48] = "Run Rattata Run";
	char wrongAnswer2[48] = "Ekans Hoop Hurl";
	char wrongAnswer3[48] = "Clefairy Says";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 62
char* GetSixtyThirdQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which one of these minigames was", "\x03NOT\x08 available in Pokemon Stadium 2");

	char correctAnswer[48] = "Rock Harden";
	char wrongAnswer1[48] = "Clear Cut Challenge";
	char wrongAnswer2[48] = "Topsy Turvy";
	char wrongAnswer3[48] = "Egg Emergency";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 63
char* GetSixtyFourthQuestion(int* correctAnswerIndexPtr)
{
	char* question = CreateSimpleTwoLineQuestionMessage("Which one of these game modes was", "\x03NOT\x08 available in Star Fox 64 multiplayer");

	char correctAnswer[48] = "Land Masters";
	char wrongAnswer1[48] = "Point Match";
	char wrongAnswer2[48] = "Battle Royale";
	char wrongAnswer3[48] = "Time Trial";

	char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

	return result;
}



//***************************************************************************//
//***************************************************************************//
//*********************                        ******************************//
//*************   Question generation and randomization    ******************//
//*********************                        ******************************//
//***************************************************************************//
//***************************************************************************//


char* CreateSimpleOneLineQuestionMessage(char* questionLineOne)
{
    char* result = func_80035934(256);      // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Zero out the memory allocated above so we don't get unexpected behavior.

    my_strcpy(result, "\x1A\x1A\x1A\x1A");  // Standard padding for portrait
    my_strncat(result, questionLineOne);
    my_strncat(result, "\xC3");             // ?   

    return result;
}

char* CreateSimpleTwoLineQuestionMessage(char* questionLineOne, char* questionLineTwo)
{
    char* result = func_80035934(256);          // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                         // Zero out the memory allocated above so we don't get unexpected behavior.
    
    my_strcpy(result, "\x1A\x1A\x1A\x1A");      // Standard padding for portrait
    my_strncat(result, questionLineOne);
    my_strncat(result, "\x0A");                 // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A");     // Standard padding for portrait
    my_strncat(result, questionLineTwo);
    my_strncat(result, "\xC3");                 // ?   

    return result;
}

char* CreateSimpleThreeLineQuestionMessage(char* questionLineOne, char* questionLineTwo, char* questionLineThree)
{
    char* result = func_80035934(256);          // malloc() to reserve memory from the heap for character pointer named "result" size 256.  
                                                // Heap is cleared during any MP3 scene transition, such as a minigame.  
                                                // Or, you can call free() with func_80035958(ptr) to release memory early.
    bzero(result, 256);                         // Zero out the memory allocated above so we don't get unexpected behavior.
    
    my_strcpy(result, "\x1A\x1A\x1A\x1A");      // Standard padding for portrait
    my_strncat(result, questionLineOne);
    my_strncat(result, "\x0A");                 // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A");     // Standard padding for portrait
    my_strncat(result, questionLineTwo);
    my_strncat(result, "\x0A");                 // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A");     // Standard padding for portrait
    my_strncat(result, questionLineThree);
    my_strncat(result, "\xC3");                 // ?   

    return result;
}

// Takes a question message, a correct answer, and three incorrect answers, and returns a full message for the quiz giver to read.
// Answer order will be randomized and the index of the correct answer will be returned via the correctAnswerIndexPtr parameter.
char* GenerateMessageForQuestionWithFourOptions(char* question, char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3, int* correctAnswerIndexPtr)
{
    char result[1024];
    bzero(result, 1024);                                     //Zero out result for safety.

    char* randomizedOptions[4];
    *correctAnswerIndexPtr = RandomizeOptionOrder(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);

    my_strcpy(result, "\x0B");                              //start the message
    my_strncat(result, question);
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOptions[0]);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOptions[1]);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOptions[2]);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOptions[3]);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\xFF");                             //Show prompt to continue

    return result;
}



// Generate a random number for each option.  Sort the numbers in ascending order.
// When the correct answer gets assigned, keep track of the index it was set to.
// randomizedOptions is being modified and will have return 4 options in 
// randomized order for the calling function.
int RandomizeOptionOrder(char* randomizedOptions[], char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3)
{
    int correctAnswerSortingNumber = GetRandomByte();
    int wrongAnswer1SortingNumber = GetRandomByte();
    int wrongAnswer2SortingNumber = GetRandomByte();
    int wrongAnswer3SortingNumber = GetRandomByte();

    int biggestSortingNumber = my_max4(correctAnswerSortingNumber, wrongAnswer1SortingNumber, wrongAnswer2SortingNumber, wrongAnswer3SortingNumber);
    int correctAnswerIndex = 0;

    //switch(case) must have constant expression =/
    if( biggestSortingNumber == correctAnswerSortingNumber)
    {
        randomizedOptions[0] = correctAnswer;
        correctAnswerIndex = 0;
        RandomizeRemainingThreeIncorrectOptions(randomizedOptions, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    } 
    else if (biggestSortingNumber == wrongAnswer1SortingNumber)
    {
        randomizedOptions[0] = wrongAnswer1;
        correctAnswerIndex = RandomizeRemainingThreeOptions(randomizedOptions, correctAnswer, wrongAnswer2, wrongAnswer3);
    } 
    else if (biggestSortingNumber == wrongAnswer2SortingNumber)
    {
        randomizedOptions[0] = wrongAnswer2;
        correctAnswerIndex = RandomizeRemainingThreeOptions(randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer3);
    }
    else //biggestSortingNumber == wrongAnswer3SortingNumber
    {
        randomizedOptions[0] = wrongAnswer3;
        correctAnswerIndex = RandomizeRemainingThreeOptions(randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2);
    }

    return correctAnswerIndex;
}

void RandomizeRemainingThreeIncorrectOptions(char* randomizedOptions[], char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3)
{
    int wrongAnswer1SortingNumber = GetRandomByte();
    int wrongAnswer2SortingNumber = GetRandomByte();
    int wrongAnswer3SortingNumber = GetRandomByte();

    int biggestSortingNumber = my_max3(wrongAnswer1SortingNumber, wrongAnswer2SortingNumber, wrongAnswer3SortingNumber);

    if(biggestSortingNumber == wrongAnswer1SortingNumber)
    {
        randomizedOptions[1] = wrongAnswer1;
        if(wrongAnswer2SortingNumber > wrongAnswer3SortingNumber) {
            randomizedOptions[2] = wrongAnswer2;
            randomizedOptions[3] = wrongAnswer3;
        } else { 
            randomizedOptions[2] = wrongAnswer3;
            randomizedOptions[3] = wrongAnswer2;
        }
    }
    else if (biggestSortingNumber == wrongAnswer2SortingNumber)
    {
        randomizedOptions[1] = wrongAnswer2;
        if(wrongAnswer1SortingNumber > wrongAnswer3SortingNumber) {
            randomizedOptions[2] = wrongAnswer1;
            randomizedOptions[3] = wrongAnswer3;
        } else { 
            randomizedOptions[2] = wrongAnswer3;
            randomizedOptions[3] = wrongAnswer1;
        }
    }
    else // biggestSortingNumber == wrongAnswer3SortingNumber
    {
        randomizedOptions[1] = wrongAnswer3;
        if(wrongAnswer1SortingNumber > wrongAnswer2SortingNumber) {
            randomizedOptions[2] = wrongAnswer1;
            randomizedOptions[3] = wrongAnswer2;
        } else { 
            randomizedOptions[2] = wrongAnswer2;
            randomizedOptions[3] = wrongAnswer1;
        }
    }

    return;
}

int RandomizeRemainingThreeOptions(char* randomizedOptions[], char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2)
{
    int correctAnswerSortingNumber = GetRandomByte();
    int wrongAnswer1SortingNumber = GetRandomByte();
    int wrongAnswer2SortingNumber = GetRandomByte();

    int biggestSortingNumber = my_max3(correctAnswerSortingNumber, wrongAnswer1SortingNumber, wrongAnswer2SortingNumber);
    int correctAnswerIndex = 0;

    if(biggestSortingNumber == correctAnswerSortingNumber)
    {
        randomizedOptions[1] = correctAnswer;
        correctAnswerIndex = 1;
        if(wrongAnswer1SortingNumber > wrongAnswer2SortingNumber) {
            randomizedOptions[2] = wrongAnswer1;
            randomizedOptions[3] = wrongAnswer2;
        } else { 
            randomizedOptions[2] = wrongAnswer2;
            randomizedOptions[3] = wrongAnswer1;
        }
    }
    else if(biggestSortingNumber == wrongAnswer1SortingNumber)
    {
        randomizedOptions[1] = wrongAnswer1;
        if(correctAnswerSortingNumber > wrongAnswer2SortingNumber) {
            randomizedOptions[2] = correctAnswer;
            randomizedOptions[3] = wrongAnswer2;
            correctAnswerIndex = 2;
        } else { 
            randomizedOptions[2] = wrongAnswer2;
            randomizedOptions[3] = correctAnswer;
            correctAnswerIndex = 3;
        }
    }
    else // biggestSortingNumber == wrongAnswer2SortingNumber
    {
        randomizedOptions[1] = wrongAnswer2;
        if(correctAnswerIndex > wrongAnswer1SortingNumber) {
            randomizedOptions[2] = correctAnswer;
            randomizedOptions[3] = wrongAnswer1;
            correctAnswerIndex = 2;
        } else { 
            randomizedOptions[2] = wrongAnswer1;
            randomizedOptions[3] = correctAnswer;
            correctAnswerIndex = 3;
        }
    }
    
    return correctAnswerIndex;
}



//***************************************************************************//
//***************************************************************************//
//****************************                  *****************************//
//*************************      mplib v1.0        **************************//
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
    mp3_ShowMessageWithConfirmation(0x16, message);
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

int IsPlayerCertainCharacter(int playerIndex, enum mp3_Character character)
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
char* my_strcpy(char* destination, const char* source)
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
char* my_strncat(char* destination, const char* source)
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
int my_max(int a1, int a2)
{
    if (a1 >= a2) { return a1; }
    else { return a2; }
}


// Returns the largest of three numbers
// C doesn't support overloading, don't hate me
int my_max3(int a1, int a2, int a3)
{
    int result = a1;
    result = my_max(result, a2);
    result = my_max(result, a3);

    return result;
}

// Returns the largest of four numbers
// C doesn't support overloading, don't hate me
int my_max4(int a1, int a2, int a3, int a4)
{
    int result = a1;
    result = my_max(result, a2);
    result = my_max(result, a3);
    result = my_max(result, a3);
    
    return result;
}

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************      /end mplib       ******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//