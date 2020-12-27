// NAME: Big Trivia Quiz
// GAMES: MP3_USA
// EXECUTION: Direct
// PARAM: +Number|COIN_REWARD

#include "ultra64.h"

// This is version 1.0
//
// There may be an update available at:
// https://github.com/c-kennelly/mario-party-custom-events
// You can also report a bug as an iusse (and maybe a PR that fixes it?)

//***************************************************************************//
//******************** Quiz Configuration ***********************************//
//***************************************************************************//

// How many questions should the quiz use?
#define ACTIVE_QUESTIONS 6

// The character portrait of the character who is giving the quiz.
#define QUIZ_GIVER_PORTRAIT 3
// Want to change the picture?  Find options the PartyPlanner64 wiki:
// https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages


//***************************************************************************//
//*********************** Description** *************************************//
//***************************************************************************//
// This event challenges a player to a quiz and asks them a random question
// from the question bank.  Each time a question appears, the choices will
// appear in a different order.  
//
// By default, the question bank has 6 questions active,
// but it is very simple to use up to 48 questions.
//
// By default, if the player gets the question right, a coin reward is given.
// If the player gets the question wrong, nothing happens.
// 
//
//
// The functionality is very similar to Airsola's "Trivia Quiz" event, 
// but written in C. You can find that original quiz at
// www.mariopartylegacy.com/forum/index.php?action=downloads;sa=view;down=305
//
//
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
//******************** Where to Customize ***********************************//
//***************************************************************************//
// Jump to "Quiz Configuration" to change the number of active questions in the
// quiz bank, or the character who is giving the quiz the questions.
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
//*************************** Quiz Logic ************************************//
//***************************************************************************//

void main() 
{
    mp3_play_idle_animation();
    DisplayGreetingMessage();

    int correctAnswer = AskTheQuestion();
    int answerChosen = GetAnswerAndTeardownMessageBox();
    
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

// Ask a random quiz question and return the correct answer for that question
int AskTheQuestion()
{
    int index = PickARandomQuestionIndex();

    int correctAnswer = 0;
    char *question_msg = GetQuestionByNumber(index, &correctAnswer);

    ShowMessage(QUIZ_GIVER_PORTRAIT, question_msg, 0, 0, 0, 0, 0);

    return correctAnswer;
}

// Pick a random question from the beginning of the bank
// to the number defined by ACTIVE_QUESTIONS
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
    //
    // Here, we're using strategy 2, and then passing in the cpuChoice.
    // Thus, the AI will always choose a random option for the question.
    // Human players will always get to manually pick.

    int choice = GetBasicPromptSelection(2, cpuChoice);
    mp3_TeardownMessageBox();

    return choice;
}

// Select an answer to the quiz question for a CPU player.
int GetChoiceForCPU()
{
    // Generate a random integer that between 0 and 3 because there
    // are 4 answers to every qeustion.
    int result = mp3_PickARandomNumberBetween0AndN(3);

    return result;
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
    char* result = func_80035934(256);      // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                            // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(result, 256);                     // Thenm zero out the memory allocated above so we don't get unexpected behavior.

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
// 1. This quiz has 48 questions slots;  
//    6 of them are active by default.
//   
// 2. Questions get used **in order**, up to whatever number is
//    defined by ACTIVE QUESTIONS at the top of the file.
//    
// 3. You can control how many questions are active by editing
//    the definition "ACTIVE QUESTIONS" at the top of the file.
// 
// 4. To customize one of the 48 questions, simply edit the
//    corresponding message and ensure the answer index is
//    defined correctly.  You don't have to do anything else.
//    
// 5. If 48 questions aren't enough for you, you're a monster.
//    But it can be done.  You'll have to define the new question 
//    AND edit the switch(case) in "GetQuestionByNumber()" to make 
//    sure the question finder can randomly pick your question.  
//    Follow the pattern and you'll be fine...




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
    // This sample question demonstrates how to make a 1-line simple question.
    // Each line is its own argument to the function.
    // This will display as:
    //  
    //      "Who wears a red hat?"
    //
    // (final question mark and newline will be added automatically)
    //
    // If you want special characters or punctuation, either concatenate the
    // strings you send in, or look below for the custom message example.
    
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    // Answers are defined here.
    // Correct answer gets defined first, followed by the the three wrong answers.  
    // Order here doesn't matter; They will be displayed in a 
    // random order each time the question is selected.

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 1
char* GetSecondQuestion(int* correctAnswerIndexPtr)
{
    // This sample question demonstrates how to make a 2-line simple question.
    // Each line is its own argument to the function.
    // This will display as:
    //  
    //      "Who wears a purple hat"
    //      "and has a moustache?"
    //
    // (final question mark and newline will be added automatically)
    
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 2
char* GetThirdQuestion(int* correctAnswerIndexPtr)
{
    // This sample question demonstrates how to make a 3-line simple question.
    // Each line is its own argument to the function.
    // This will display as: 
    //
    //      "Who wears a purple hat"
    //      "and has a moustache"
    //      "and likes power?"
    //
    // (final question mark and newline will be added automatically)

    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 3
char* GetFourthQuestion(int* correctAnswerIndexPtr)
{
    // This sample question demonstrates how to make a complex question that uses special characters.
    // You need to add your own question mark to close.
    // However, a newline before the answers is automatically inserted.
    //
    // Mario Party 3 special characters documented at:
    // https://github.com/PartyPlanner64/PartyPlanner64/wiki/String-Encoding
    //    
    char* question = func_80035934(256);        // malloc() to reserve memory from the heap.  Heap is cleared during any MP3 scene 
                                                // transition, such as a minigame.  Or, you can call free() with func_80035958(ptr)
    bzero(question, 256);                       // Then zero out the memory allocated above so we don't get unexpected behavior.

    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 4
char* GetFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a green hat", "and is a coward");

    char correctAnswer[16] = "Luigi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

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

    char correctAnswer[16] = "Cake";
    char wrongAnswer1[16] = "Mushrooms";
    char wrongAnswer2[16] = "Lonely";
    char wrongAnswer3[16] = "Steak";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 6
char* GetSeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 7
char* GetEigthQuestion(int* correctAnswerIndexPtr)
{   
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 8
char* GetNinthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 9
char* GetTenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 10
char* GetEleventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 11
char* GetTwelfthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 12
char* GetThirteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 13
char* GetFourteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 14
char* GetFifteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;   
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 15
char* GetSixteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
     my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 16
char* GetSeventeenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 17
char* GetEighteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 18
char* GetNinteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 19
char* GetTwentiethQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 20
char* GetTwentyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 21
char* GetTwentySecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 22
char* GetTwentyThirdQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 23
char* GetTwentyFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 24
char* GetTwentyFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 25
char* GetTwentySixthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 26
char* GetTwentySeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 27
char* GetTwentyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 28
char* GetTwentyNinthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 29
char* GetThirtiethQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 30
char* GetThirtyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 31
char* GetThirtySecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 32
char* GetThirtyThirdQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 33
char* GetThirtyFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 34
char* GetThirtyFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 35
char* GetThirtySixthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 36
char* GetThirtySeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//


// Question for index 37
char* GetThirtyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 38
char* GetThirtyNinthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 39
char* GetFourtiethQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 40
char* GetFourtyFirstQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 41
char* GetFourtySecondQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 42
char* GetFourtyThirdQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 43
char* GetFourtyFourthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 44
char* GetFourtyFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char correctAnswer[16] = "Mario";
    char wrongAnswer1[16] = "Luigi";
    char wrongAnswer2[16] = "Wario";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 45
char* GetFourtySixthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleTwoLineQuestionMessage("Who wears a yellow hat", "and has a moustache");

    char correctAnswer[16] = "Wario";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Waluigi";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
   
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 46
char* GetFourtySeventhQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleThreeLineQuestionMessage("Who wears a purple hat", "and has a moustache", "and likes power");
    
    char correctAnswer[16] = "Walugi";
    char wrongAnswer1[16] = "Mario";
    char wrongAnswer2[16] = "Luigi";
    char wrongAnswer3[16] = "Wario";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 47
char* GetFourtyEighthQuestion(int* correctAnswerIndexPtr)
{
    char* question = func_80035934(256);      
    bzero(question, 256);                      
    
    my_strcpy(question, "\x0B");                            // Start the message
    my_strncat(question, "\x1A\x1A\x1A\x1A"); 	            // Standard padding for portrait
    my_strncat(question, "Which item is");                        
    my_strncat(question, "\x03");                           // Red Font
    my_strncat(question, " NOT");                              
    my_strncat(question, "\x08");                           // White Font
    my_strncat(question, " used in the Biggoron");
    my_strncat(question, "\x5C");                           // '
    my_strncat(question, "s Sword");
    my_strncat(question, "\x0A");                           // Newline
    my_strncat(question, "\x1A\x1A\x1A\x1A");               // Standard padding for portrait
    my_strncat(question, "Quest in The Legend of Zelda");
    my_strncat(question, "\x7B");                           // :
    my_strncat(question, "Ocarina of Time");
    my_strncat(question, "\xC3");                          // ?

    char correctAnswer[16] = "Weird Egg";
    char wrongAnswer1[16] = "Odd Mushroom";
    char wrongAnswer2[16] = "Prescription";
    char wrongAnswer3[16] = "Eyeball Frog";

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

// Takes a question message, a correct anser, and three incorrect answers, and returns a full message for Toad to read.
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

// Prints a message in game with the Millenium Star portrait.
// Does not wait for player confirmation.
void mp3_DebugMessage(char* message)
{
    mp3_ShowMessageWithConfirmation(0x16, message);
}

// Picks a random number between 0 and N, using rejection sampling to avoid modulo bias.
// IMPORTANT.  Maximum random number we could generate here would be 255 (max value of a byte).
// Don't use this!!! -->  { return GetRandomByte() % n; } or you may bias your random generation!
// https://zuttobenkyou.wordpress.com/2012/10/18/generating-random-numbers-without-modulo-bias/
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