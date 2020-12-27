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
    

    mp3_DebugMessage("Done");
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
    // We just punish them with disappointment by default
    // but feel free to add your own dastardly deed.
    char *wrong_msg = GetMessageforWrongAnswer();
    mp3_ShowMessageWithConfirmation(QUIZ_GIVER_PORTRAIT, wrong_msg);
    
    // Play the sad animation and wait 70 frames for it to complete
    mp3_play_sad_animation();
    SleepProcess(70);

    // If you want to give a coin penalty add a:
    // [+Number|COIN_PENALTY] parameter to the
    // PartyPlanner64 header at the top of the file
    // and then uncomment the following two lines:

    //int coinPenalty = -1 * COIN_PENALTY;
    //GraduallyAdjustPlayerCoins(coinPenalty);

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
// 5. If 30 questions aren't enough for you, you're a monster.
//    But it can be done.  You'll have to define the new question 
//    AND edit the switch(case) below to make sure the question 
//    finder can randomly pick your  question.  Follow the pattern 
//    and you'll be fine..




// Returns a question that corresponds to the number passed in.
// 
// If the number doesn't have a question assigned, it will default
// to the first question.
char* GetQuestionByNumber(int question, int* correctAnswerPtr)
{
    char *result;
 /*
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
        default:
           result = GetFirstQuestion(correctAnswerPtr);
           return result;
    }
*/
    // Want to test a sepcific question?  Edit and uncomment 
    // the following line to ensure it always picks a specifc question.
    // Don't forget to recomment when you are done testing!
    
    result = GetFirstQuestion(correctAnswerPtr);

    return result;
}




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

    // Answers are defined here:
    // Correct answer gets defined first.  Then the three wrong answers.  
    // Order here doesn't matter; They will be displayed in a 
    // random order each time the question is selected.

    char* correctAnswer = "Mario";
    char* wrongAnswer1 = "Luigi";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";

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

// Question for index 3
char* GetFourthQuestion(int* correctAnswerIndexPtr)
{
    // This sample question demonstrates how to make a question that uses special characters.
    //
    // Mario Party 3 special characters documented at:
    // https://github.com/PartyPlanner64/PartyPlanner64/wiki/String-Encoding
    //
    // By default, questions have a 200 character limit due to 
    // the array size defined in "GenerateMessageForQuestionWithFourOptions()
    // This one comes to 155 characters.
    
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which item is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3"                              // ?
    "\x0A";                             // Newline

    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);
    
    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 4
char* GetFifthQuestion(int* correctAnswerIndexPtr)
{
    char* question = CreateSimpleOneLineQuestionMessage("Who wears a red hat");

    char* correctAnswer = "Mario";
    char* wrongAnswer1 = "Luigi";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";


    char *result = GenerateMessageForQuestionWithFourOptions(question, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3, correctAnswerIndexPtr);

    return result;
}

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

// Question for index 5
char* GetSixthQuestion(int* correctAnswerIndexPtr)
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

// Question for index 6
char* GetSeventhQuestion(int* correctAnswerIndexPtr)
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

// Question for index 7
char* GetEigthQuestion(int* correctAnswerIndexPtr)
{
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which item is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3"                              // ?
    "\x0A";                             // Newline

    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

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


    // Answers are defined here:
    // Correct answer gets defined first.  Then the three wrong answers.  
    // Order here doesn't matter; They will be displayed in a 
    // random order each time the question is selected.

    char* correctAnswer = "Mario";
    char* wrongAnswer1 = "Luigi";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";


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

// Question for index 10
char* GetEleventhQuestion(int* correctAnswerIndexPtr)
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

// Question for index 11
char* GetTwelfthQuestion(int* correctAnswerIndexPtr)
{
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which item is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3"                              // ?
    "\x0A";                             // Newline

    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

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


    // Answers are defined here:
    // Correct answer gets defined first.  Then the three wrong answers.  
    // Order here doesn't matter; They will be displayed in a 
    // random order each time the question is selected.

    char* correctAnswer = "Mario";
    char* wrongAnswer1 = "Luigi";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";


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

// Question for index 14
char* GetFifteenthQuestion(int* correctAnswerIndexPtr)
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

// Question for index 15
char* GetSixteenthQuestion(int* correctAnswerIndexPtr)
{
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which item is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3"                              // ?
    "\x0A";                             // Newline

    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

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


    // Answers are defined here:
    // Correct answer gets defined first.  Then the three wrong answers.  
    // Order here doesn't matter; They will be displayed in a 
    // random order each time the question is selected.

    char* correctAnswer = "Mario";
    char* wrongAnswer1 = "Luigi";
    char* wrongAnswer2 = "Wario";
    char* wrongAnswer3 = "Waluigi";


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

// Question for index 18
char* GetNinteenthQuestion(int* correctAnswerIndexPtr)
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

// Question for index 19
char* GetTwentiethQuestion(int* correctAnswerIndexPtr)
{
    char* question =
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Which item is"                        
    "\x03"                              // Red Font
    " NOT"                              
    "\x08"                              // White Font
    " used in the Biggoron\x5Cs Sword"
    "\x0A"                              // Newline
    "\x1A\x1A\x1A\x1A"                  // Standard padding for portrait
    "Quest in The Legend of Zelda\x7B Ocarina of Time"
    "\xC3"                              // ?
    "\x0A";                             // Newline

    char* correctAnswer = "Weird Egg";
    char* wrongAnswer1 = "Odd Mushroom";
    char* wrongAnswer2 = "Prescription";
    char* wrongAnswer3 = "Eyeball Frog";

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

// Takes a question message, a correct anser, and three incorrect answers, and returns a full message for Toad to read.
// Answer order will be randomized and the index of the correct answer will be returned via the correctAnswerIndexPtr parameter.
char* GenerateMessageForQuestionWithFourOptions(char* question, char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3, int* correctAnswerIndexPtr)
{
    //56 characters for boilerplate here.
    //No promises on what happens if user-provided message + answer is greater than 512 characters
    char result[2048];

    char* randomizedOptions[4];
    *correctAnswerIndexPtr = RandomizeOptionOrderTest(&randomizedOptions, correctAnswer, wrongAnswer1, wrongAnswer2, wrongAnswer3);


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

int RandomizeOptionOrderTest(char* randomizedOptions[], char* correctAnswer, char* wrongAnswer1, char* wrongAnswer2, char* wrongAnswer3)
{
    int correctAnswerIndex = 0;
    randomizedOptions[0] = correctAnswer;
    randomizedOptions[1] = wrongAnswer1;
    randomizedOptions[2] = wrongAnswer2;
    randomizedOptions[3] = wrongAnswer3;

    return correctAnswerIndex;
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
    int correctAnswerIndex;

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


// Prints a message in game with the Millenium Star portrait.
// Does not wait for player confirmation.
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
//**********************      end mplib       *******************************//
//************************                  *********************************//
//***************************************************************************//
//***************************************************************************//