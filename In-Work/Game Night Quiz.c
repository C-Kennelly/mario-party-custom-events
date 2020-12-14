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
    int index = PickRandomQuestionIndex();

    char *question_msg = GetQuestionMessageByNumber(index);

    ShowMessage(QUIZ_GIVER_PORTRAIT, question_msg, 0, 0, 0, 0, 0);

    int correctAnswer = GetCorrectAnswerToQuestionByNumber(index);

    return correctAnswer;
}
// Generate a random number to pick an Active Question.
int PickRandomQuestionIndex()
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
        mp3_play_joy_animation();
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

// Picks a random number between 0 and N, avoiding modulo bias
// More on modulo bias, how this function solves it, and why it might 
// produce not-truly-random numbers here:
// https://research.kudelskisecurity.com/2020/07/28/the-definitive-guide-to-modulo-bias-and-how-to-avoid-it/
int mp3_PickARandomNumberBetween0AndN(int n)
{
    //TODO - actually implement
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

//(-1, 3, 0)
//Plays the sad animation, with no loop
void mp3_play_sad_animation() 
{
    func_800F2304(-1, 3, 0); // Sad animation, no loop
}

//(-1, 5, 0)
//Plays the joy animation, with no loop
void mp3_play_joy_animation() 
{
    func_800F2304(-1, 5, 0); // joy animation, no loop
}

//(-1, -1, 0)
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

//***************************************************************************//
//***************************************************************************//
//*************************                  ********************************//
//**********************         mplib        *******************************//
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
char* GetQuestionMessageByNumber(int question)
{
    char *result;

    
    switch (question)
    {
        case (0):
            result = GetFirstQuestionMessage();
            break;
        case (1):
            result = GetSecondQuestionMessage();
            break;
        case (2):
            result = GetThirdQuestionMessage();
            break;
        case (3):
            result = GetFourthQuestionMessage();
            break;
        case (4):
            result = GetFifthQuestionMessage();
            break;
        case (5):
            result = GetSixthQuestionMessage();
            break;
        case (6):
            result = GetSeventhQuestionMessage();
            break;
        case (7):
            result = GetEigthQuestionMessage();
            break;
        case (8):
            result = GetNinthQuestionMessage();
            break;
        case (9):
            result = GetTenthQuestionMessage();
            break;
        case (10):
            result = GetEleventhQuestionMessage();
            break;
        case (11):
            result = GetTwelfthQuestionMessage();
            break;
        case (12):
            result = GetThirteenthQuestionMessage();
            break;
        case (13):
            result = GetFourteenthQuestionMessage();
            break;
        case (14):
            result = GetFifteenthQuestionMessage();
            break;
        case (15):
            result = GetSixteenthQuestionMessage();
            break;
        case (16):
            result = GetSeventeenthQuestionMessage();
            break;
        case (17):
            result = GetEighteenthQuestionMessage();
            break;
        case (18):
            result = GetNinteenthQuestionMessage();
            break;
        case (19):
            result = GetTwentiethQuestionMessage();
            break;
        default:
           result = GetFirstQuestionMessage();
           return result;
    }

    // Want to test a sepcific question?  Edit and uncomment 
    // the following line to ensure it always picks a specifc question.
    // Don't forget to override the answer selection in the next 
    //function as well!
    
    //result = GetFirstQuestionMessage();

    return result;

}
// Returns a answer that corresponds to the number passed in.
//
// If the number doesn't have a question assigned, it will default
// to the first question.
int GetCorrectAnswerToQuestionByNumber(int question)
{
    int *result;

    switch (question)
    {
        case (0):
            result = GetAnswerToFirstQuestion();
            break;
        case (1):
            result = GetAnswerToSecondQuestion();
            break;
        case (2):
            result = GetAnswerToThirdQuestion();
            break;
        case (3):
            result = GetAnswerToFourthQuestion();
            break;
        case (4):
            result = GetAnswerToFifthQuestion();
            break;
        case (5):
            result = GetAnswerToSixthQuestion();
            break;
        case (6):
            result = GetAnswerToSeventhQuestion();
            break;
        case (7):
            result = GetAnswerToEigthQuestion();
            break;
        case (8):
            result = GetAnswerToNinthQuestion();
            break;
        case (9):
            result = GetAnswerToTenthQuestion();
            break;
        case (10):
            result = GetAnswerToEleventhQuestion();
            break;
        case (11):
            result = GetAnswerToTwelfthQuestion();
            break;
        case (12):
            result = GetAnswerToThirteenthQuestion();
            break;
        case (13):
            result = GetAnswerToFourteenthQuestion();
            break;
        case (14):
            result = GetAnswerToFifteenthQuestion();
            break;
        case (15):
            result = GetAnswerToSixteenthQuestion();
            break;
        case (16):
            result = GetAnswerToSeventeenthQuestion();
            break;
        case (17):
            result = GetAnswerToEighteenthQuestion();
            break;
        case (18):
            result = GetAnswerToNinteenthQuestion();
            break;
        case (19):
            result = GetAnswerToTwentiethQuestion();
            break;
        default:
           result = GetAnswerToFirstQuestion();
           return result;
    }

    // Want to test a sepcific question?  Edit and uncomment 
    // the following line to ensure it always picks a specifc answer.
    
    //result = GetAnswerToFirstQuestion();

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

char* GenerateMessageForQuestionWithFourOptions(char* question, char* randomizedOption0, char* randomizedOption1, char* randomizedOption2, char* randomizedOption3)
{
    char result[1024];

    my_strcpy(result, "\x0B");                              //start the message
    my_strncat(result, question);
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOption0);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOption1);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOption2);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\x0A");                             // Newline
    my_strncat(result, "\x1A\x1A\x1A\x1A\x1A\x1A");         // Little more padding for option indent    
    my_strncat(result, "\x0C");                             // Start option
    my_strncat(result, randomizedOption3);
    my_strncat(result, "\x0D");                             // End option
    my_strncat(result, "\xFF");                             //Show prompt to continue

    return result;
}



//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//





// Question for index 0
char* GetFirstQuestionMessage()
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
    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    return result;
}
// Answer to the question at index 0.
int GetAnswerToFirstQuestion()
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

// Question for index 1
char* GetSecondQuestionMessage()
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a green hat");

    char* correctAnswer = "Luigi";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    return result;

}
// Answer to the question at index 1.
int GetAnswerToSecondQuestion()
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

// Question for index 2
char* GetThirdQuestionMessage()
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char* correctAnswer = "Wario";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Luigi";
    char* wrongAnswer3 = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    return result;
}
// Answer to the question at index 2.
int GetAnswerToThirdQuestion()
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

// Question for index 3
char* GetFourthQuestionMessage()
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    char* correctAnswer = "Walugi";
    char* wrongAnswer1 = "Mario";
    char* wrongAnswer2 = "Luigi";
    char* wrongAnswer3 = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);
    return result;
}
// Answer to the question at index 3.
int GetAnswerToFourthQuestion()
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