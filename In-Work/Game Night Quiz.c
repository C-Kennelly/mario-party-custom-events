// NAME: Big Trivia Quiz
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: +Number|COIN_REWARD



//***************************************************************************//
//*********************** Description** *************************************//
//***************************************************************************//
// This event challenges a player to a quiz and asks them a random question
// from the question bank.  By default, the question bank has 6 questions in it
// but it is very simple to use up to 30 questions.
//
// If the player gets the question right, a coin reward is given.
// If the player gets the question wrong, nothing happens.
//
//
// The functionality is similar to Airsola's "Trivia Quiz" event, but written
// in C to make it easier to customize.  You can find that original quiz at
// www.mariopartylegacy.com/forum/index.php?action=downloads;sa=view;down=305
//
//
// Finally, this file is commented to make it as easy for non-programmers 
// to edit, so I've leaned on the verbose side for comments.  If you've a
// working knowledge of C, feel free to just jump to main() and see what's
// going on. 



//***************************************************************************//
//******************** Where to Customize ***********************************//
//***************************************************************************//
//
// Jump to "Quiz Configuration" to change the number of active questions in the
// quiz bank, the number of answers to each question, or the character who is
// asking the questions.
//
// Jump to "Message Configuration" to customize messages such as the greeting
// or when a player gets a correct/incorrect answer.
//
// Jump to "RewardPlayerForCorrectAnswer()" to change behavior when the player
// gets the answer right, or "PunishPlayerForIncorrectAnswer()" to change
// what happens when the player gets the answer wrong.
//
// Finally, jump to "Question definitions" to customize the question bank.



//***************************************************************************//
//******************** Quiz Configuration ***********************************//
//***************************************************************************//

// How many questions should the quiz use?
//
// Read the comments in the question config section at
// the bottom of the file for more info on how this works.
#define ACTIVE_QUESTIONS 4


// How many answer options does each quiz question have?
//
// Unless you modify it, AI selection depends on every question having 
// the same number of choices.  Human players unaffected by this.
#define ANSWERS_PER_QUESTION 4


// Index for the character portrait of the character who is giving the quiz.
// Defaults to 3, which is Toad.
#define QUIZ_GIVER_PORTRAIT 3
// Want to change the picture?  Find more on the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages

#include "ultra64.h"

//***************************************************************************//
//******************** Quiz Logic *******************************************//
//***************************************************************************//

void main() 
{
    mp3_play_idle_animation();
    DisplayGreetingMessage();

    int correctAnswer = AskTheQuestion();
    int answerChosen = GetAnswerAndTeardownMessageBox();
    
    if(answerChosen == correctAnswer)
    {   RewardPlayerForCorrectAnswer(); }
    else
    {   PunishPlayerForIncorrectAnswer(); }
    
    return;
}





// Displays a message to intro the quiz.
void DisplayGreetingMessage()
{
   // Display the ready message...
    char* greeting_msg = GetGreetingMessage();
    mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, greeting_msg);
}


// Ask a random quiz question and return the correct answer for that question
int AskTheQuestion()
{
    int index = PickARandomQuestionIndex();
    
    int correctAnswer = 0;

    // function will return the message and set the correct answer because we passed by 
    char *question_msg = GetQuestionMessageByNumber(index, &correctAnswer);


    ShowMessage(QUIZ_GIVER_PORTRAIT, question_msg, 0, 0, 0, 0, 0);

    return correctAnswer;
}

// Pick a random question from the beginning of the bank
// all the way up to the 
int PickARandomQuestionIndex()
{
    return mp3_PickARandomNumberBetween0AndN(ACTIVE_QUESTIONS);
}


// Display a message and return the answer chosen 
// Also has defined logic for CPUs.
int GetAnswerAndTeardownMessageBox()
{
    int cpuChoice = GetChoiceForCPU();

    // Get the selection, either from the player or CPU.
    // MP3 built-in function: GetBasicPromptSelection(int strategy, int index)
    // Strategy argument takes an int and behaves as follows:
    //   0 -> If CPU, always pick first (0th) option
    //   1 -> If CPU, always pick second (1th) option
    //   2 -> If CPU, pick the option that is passed in the second argument.
    // Here, we're using 2, and then passing in the cpuChoice.
    // Thus, the AI will always choose a random option for the question.
    int choice = GetBasicPromptSelection(2, cpuChoice);
    mp3_TeardownMessageBox();

    return choice;
}
// Select an answer to the quiz question for a CPU player.
int GetChoiceForCPU()
{
    //This logic will pick a random choice for the the CPU.

    //Generate a random integer that between 0 and Number_of_Answers-1
    //Google "zero-based arrays" and "modulo operation" if this is confusing.
    //Make sure the # of answers at top of file is correctly defined!
    int result = mp3_PickARandomNumberBetween0AndN(ANSWERS_PER_QUESTION);
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
    // We just punish them with disappointment by default
    // but feel free to add your own dastardly deed.
    char *wrong_msg = GetMessageforWrongAnswer();
    mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, wrong_msg);
    
    // Play the sad animation and wait 70 frames for it to complete

    mp3_play_sad_animation();
    SleepProcess(70);

    // If you want to give a coin penalty add a:
    // [+Number|COIN_PENALTY] parameter to the
    // PartyPlanner64 header at the top fo the file
    // and then uncomment the following two lines:

    //int coinPenalty = -1 * COIN_PENALTY;
    //GraduallyAdjustPlayerCoins(coinPenalty);

    return;
}

void GraduallyAdjustPlayerCoins(int adjustmentAmount)
{
    int currentPlayerIndex = GetCurrentPlayerIndex();
    mp3_play_joy_animation();
    
    AdjustPlayerCoinsGradual(currentPlayerIndex, adjustmentAmount);
    ShowPlayerCoinChange(currentPlayerIndex, adjustmentAmount);

    // Sleep for 30 frames to let the coin change take effect.
    // Also syncs up nicely with the joy animation.
    SleepProcess(30);
}



//***************************************************************************//
//******************** Message Configuration ********************************//
//***************************************************************************//

// Defines the message the Quiz Giver displays first.
char* GetGreetingMessage()
{
    char *result = 
    "\x0B"                      // Start the message
    "\x1A\x1A\x1A\x1A" 			//Standard padding for portrait
    "Ready for a quiz"          //
    "\xC3" 						// ?
    "\x0A" 						//Newline
    "\x1A\x1A\x1A\x1A" 			//Standard padding for portrait
    "If you get the question right"
    "\x82"                      // ,
    "\x0A" 						//Newline
    "\x1A\x1A\x1A\x1A" 			//Standard padding for portrait
    "you can earn some coins"   //
    "\xC2"                      // !
    "\xFF";						//Show prompt to continue arrow

    return result;
}

// Defines the message shown if the player gives the correct answer.
char* GetMessageForRightAnswer()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    //Standard padding
    "Yes"
    "\x82"                          // ,
    " that\x5Cs right"               //  \x5C ->  '
    "\xC2"                          // !
    "\xFF";						    //Show prompt to continue arrow

    return result;
}

// Defines the message shown if the player gets the answer wrong.
char* GetMessageforWrongAnswer()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    //Standard padding
    "I\x5Cm so sorry"
    "\x82"                          // ,
    " that\x5Cs not correct"        //  \x5C ->  ' 
    "\x85\x85\x85"                  //...
    "\xFF";						    //Show prompt to continue arrow
    
    return result;
}


//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************         mplib        *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//

void mp3_DebugMessage(char* message)
{
    mp3_ShowMessageWithConfirmation(0x16, message);
}

// Picks a random number between 0 and N, avoiding modulo bias via rejection sampling
// Edge case: 1/256 chance this gives a biased result, so fine for mp3 terms.
// Don't use this!!! -->  { return GetRandomByte() % n; } 
// https://zuttobenkyou.wordpress.com/2012/10/18/generating-random-numbers-without-modulo-bias/
int mp3_PickARandomNumberBetween0AndN(int n)
{
    //todo - why does this always produce ?
    //int result;
    //int randMax = 255;                            // 255 is the maximum value of a random byte, which is how mp3 gets random numbers
    //int randExcess = (randMax + 1) % n;           // Caluclate the biased remainder
    //int randLimit = randMax - randExcess;         // Anything above randLimit would create modulo bias, so...
    //while (result = GetRandomByte() > randLimit) {};    // Reject any random bytes with values above randLimit, and roll again.
    //return result % n;                                  //Since we rejected the excess samples, we've guaranteed an unbiased result.
    return GetRandomByte() % n;
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

/*
  -1 - idle, stop immediately
  0 - idle, finish last animation (doesn't look good)
  1 - Sprint/run animation
  2 - "Get Ready" pose
  3 - Sad/disappointed animation
  4 - Fall over dizzy with sorrow (Bowser stole a star bad)
  5 - Jump for joy (full joy)
  6 - Do a dance
*/

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


// Long-form implementation from:
// https://www.techiedelight.com/implement-strcpy-function-c/

// Function to implement strcpy() function
char* my_strcpy(char* destination, const char* source)
{
    // return if no memory is allocated to the destination
    if (destination == NULL)
        return NULL;
 
    // take a pointer pointing to the beginning of destination string
    char *ptr = destination;
 
    // copy the C-string pointed by source into the array
    // pointed by destination
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
//**********************         /mplib       *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//




//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//********************** Question definitions *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//




/******** How to Configure Questions ***********/
// 1. This quiz has 30 questions slots;  
//    6 of them are active by default.
//   
// 2. Questions get used in order, up to whatever number is
//    defined by ACTIVE QUESTIONS at the top of the file.
//    
// 3. You can control how many questions are active by editing
//    the definition "ACTIVE QUESTIONS" at the top of the file.
// 
// 4. To customize one of the 20 questions, simply edit the
//    corresponding message and ensure the answer index is
//    defined correctly.  You don't have to do anything else.
//
//    
// 5. If 30 questions aren't enough for you, you're a monster.
//    But it can be done.  You'll have to edit the switch(case)'s
//    below to make sure the question finder can randomly pick your
//    question.  Follow the pattern for BOTH the question and answer picker
//    and don't accientally overwrite the default case in either.






// Returns a question that corresponds to the number passed in.
// 
// If the number doesn't have a question assigned, it will default
// to the first question.
char* GetQuestionMessageByNumber(int question, int* correctAnswerPtr)
{
    char *result;

    switch (question)
    {
        case (0):
            result = GetFirstQuestionMessage(correctAnswerPtr);
            break;
        case (1):
            result = GetSecondQuestionMessage(correctAnswerPtr);
            break;
        case (2):
            result = GetThirdQuestionMessage(correctAnswerPtr);
            break;
        case (3):
            result = GetFourthQuestionMessage(correctAnswerPtr);
            break;
    /*
        case (4):
            result = GetFifthQuestionMessage(&correctAnswer);
            break;
        case (5):
            result = GetSixthQuestionMessage(&correctAnswer);
            break;
        case (6):
            result = GetSeventhQuestionMessage(&correctAnswer);
            break;
        case (7):
            result = GetEigthQuestionMessage(&correctAnswer);
            break;
        case (8):
            result = GetNinthQuestionMessage(&correctAnswer);
            break;
        case (9):
            result = GetTenthQuestionMessage(&correctAnswer);
            break;
        case (10):
            result = GetEleventhQuestionMessage(&correctAnswer);
            break;
        case (11):
            result = GetTwelfthQuestionMessage(&correctAnswer();
            break;
        case (12):
            result = GetThirteenthQuestionMessage(&correctAnswer);
            break;
        case (13):
            result = GetFourteenthQuestionMessage(&correctAnswer);
            break;
        case (14):
            result = GetFifteenthQuestionMessage(&correctAnswer);
            break;
        case (15):
            result = GetSixteenthQuestionMessage(&correctAnswer);
            break;
        case (16):
            result = GetSeventeenthQuestionMessage(&correctAnswer);
            break;
        case (17):
            result = GetEighteenthQuestionMessage(&correctAnswer);
            break;
        case (18):
            result = GetNinteenthQuestionMessage(&correctAnswer);
            break;
        case (19):
            result = GetTwentiethQuestionMessage(&correctAnswer);
            break;
    */
        default:
           result = GetFirstQuestionMessage(correctAnswerPtr);
           return result;
    }

    // Want to test a sepcific question?  Edit and uncomment 
    // the following line to ensure it always picks a specifc question.
    // Don't forget to override the answer selection in the next 
    //function as well!
    
    //result = GetFirstQuestionMessage();

    return result;

}

char* CreateSimpleOneLineQuestionMessage(char* questionLineOne)
{
    char* result =
    my_strcpy(result, "\x1A\x1A\x1A\x1A");  // Standard padding for portrait
    my_strncat(result, questionLineOne);
    my_strncat(result, "\xC3");             // ?   

    return result;
}

char* CreateSimpleTwoLineQuestionMessage(char* questionLineOne, char* questionLineTwo)
{
    char* result =
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
    char* result =
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



char* GenerateMessageForQuestionWithFourOptions(char* question, char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3, int* correctAnswerIndexPtr)
{
    char result[1024];

    char* randomizedOptions[4];


    //*correctAnswerIndexPtr = RandomizeOptionOrder(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    *correctAnswerIndexPtr = RandomizedOptionOrderTest(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);

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

int RandomizedOptionOrderTest(char* randomizedOptions[], char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3)
{
    int result = 0;

    randomizedOptions[0] = wrongAnswer1;
    randomizedOptions[1] = correctAnswer;
    result = 1;
    randomizedOptions[2] = wrongAnswer2;
    randomizedOptions[3] = wrongAnswer3;

    return result;
}

// Generate a random number for each option.  Sort the numbers in ascending order.
// When the correct answer gets assigned, keep track of the index it was set to.
// randomizedOptions is being modified and will have the 4 options randomized for the calling function.
int RandomizeOptionOrder(char* randomizedOptions[], char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3)
{
    int correctAnswerSortingNumber = GetRandomByte();
    int wrongAnswer1SortingNumber = GetRandomByte();
    int wrongAnswer2SortingNumber = GetRandomByte();
    int wrongAnswer3SortingNumber = GetRandomByte();

    int biggestSortingNumber = my_max4(correctAnswerSortingNumber, wrongAnswer1SortingNumber, wrongAnswer2SortingNumber, wrongAnswer3SortingNumber);
    int correctAnswerIndex;

    //switch(case) must have constant expression =/
    if( biggestSortingNumber == correctAnswerSortingNumber)
    {
        randomizedOptions[0] = correctAnswer;
        correctAnswerIndex = 0;
        RandomizeRemainingThreeIncorrectOptions(&randomizedOptions, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    } 
    else if (biggestSortingNumber == wrongAnswer1SortingNumber)
    {
        randomizedOptions[0] = wrongAnswer1;
        correctAnswerIndex = RandomizeRemainingThreeOptions(&randomizedOptions, correctAnswer, wrongAnswer2, wrongAnswer3);
    } 
    else if (biggestSortingNumber == wrongAnswer2SortingNumber)
    {
        randomizedOptions[0] = wrongAnswer2;
        correctAnswerIndex = RandomizeRemainingThreeOptions(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer3);
    }
    else //biggestSortingNumber == wrongAnswer3SortingNumber
    {
        randomizedOptions[0] = wrongAnswer3;
        correctAnswerIndex = RandomizeRemainingThreeOptions(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2);
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
    int correctAnswerIndex;

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
//****************************Question***************************************//
//***************************************************************************//

// Question for index 0
char* GetFirstQuestionMessage(int* correctAnswerIndexPtr)
{
    // Mario Party 3 special characters documented at:
    // https://github.com/PartyPlanner64/PartyPlanner64/wiki/String-Encoding
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which items is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3";                             // ?


    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

    //when ready to do answers, have the same function that returns the correct answer display the question 
    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 1
char* GetSecondQuestionMessage(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a green hat");

    char* correctAnswer = "Luigi";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 2
char* GetThirdQuestionMessage(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char* correctAnswer = "Wario";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Luigi";
    char* wrongAnswer3 = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 3
char* GetFourthQuestionMessage(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    char* correctAnswer = "Walugi";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Luigi";
    char* wrongAnswer3 = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;

}




//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 4
char* GetFifthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "What do you do when a stranger wahs" // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Nod head"                      // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Scratch chin"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wah romantically back"         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Run away"                      // **** First option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 4.
int GetAnswerToFifthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 2;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 5
char* GetSixthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "What does Peach make Mario"    // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mushrooms"                     // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Hamburgers"                    // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Cake"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Lonely"                        // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 5.
int GetAnswerToSixthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 2;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 6
char* GetSeventhQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a red hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                          // **** First option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 6.
int GetAnswerToSeventhQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 0;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 7
char* GetEigthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a green hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 7.
int GetAnswerToEigthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 1;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 8
char* GetNinthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a yellow hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 8.
int GetAnswerToNinthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 2;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 9
char* GetTenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a purple hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 9.
int GetAnswerToTenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 3;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 10
char* GetEleventhQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a red hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                          // **** First option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 10.
int GetAnswerToEleventhQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 0;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 11
char* GetTwelfthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a green hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 11.
int GetAnswerToTwelfthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 1;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 12
char* GetThirteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a yellow hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 12.
int GetAnswerToThirteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 2;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 13
char* GetFourteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a purple hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 13.
int GetAnswerToFourteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 3;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 14
char* GetFifteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a red hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                          // **** First option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 14.
int GetAnswerToFifteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 0;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 15
char* GetSixteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a green hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 15.
int GetAnswerToSixteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 1;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 16
char* GetSeventeenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a yellow hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 16.
int GetAnswerToSeventeenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 2;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 17
char* GetEighteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a purple hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 17.
int GetAnswerToEighteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 3;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 18
char* GetNinteenthQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a red hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                          // **** First option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 18.
int GetAnswerToNinteenthQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 0;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 19
char* GetTwentiethQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who wears a green hat"          // **** Question text**** //
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Mario"                        // **** First option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Luigi"                          // **** Second option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Wario"                         //// **** Third option**** //
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Waluigi"                       // **** Fourth option**** //
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}
// Answer to the question at index 19.
int GetAnswerToTwentiethQuestion()
{
    // Remember, the answer choice is 0-based:
    //    return 0 = First option is correct
    //    return 1 = Second option is correct
    //    return 2 = Third option is correct
    //    return 3 = Fourth option is correct

    return 1;
}