// NAME: Game Night Quiz
// GAMES: MP3_USA
// EXECUTION: Direct

//Character Index taken from the Wiki:
//https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages
#define TOAD_PICTURE 3  

//How many questions are defined at the bottom of the file?
#define QUESTION_COUNT 1

//How many answers does each question have?
//AI requires every question to have the same number of options
#define ANSWERS_FOR_EACH_QUESTION 4

//TODO - setup VS Code Intellisense w/ Ultra64.h include paths
//http://n64devkit.square7.ch/n64man/header.htm
#include "ultra64.h" 

void main() 
{
    DisplayReadyMessage();    
    int correctAnswer = AskTheQuestion();
    int randomOptionForCPUToPick = GetChoiceForCPU();
    int answerChosen = GetAnswerAndTeardownMessageBox(randomOptionForCPUToPick);
    CompareAnswer(answerChosen, correctAnswer);
    
    return;
}

void DisplayReadyMessage()
{
   //Display the ready message...
    char* ready_msg = GetMessageForReadyLine();
    ShowMessageWithConfirmationAndTeardown(TOAD_PICTURE, ready_msg);

//    ShowMessage(TOAD_PICTURE, ready_msg, 0, 0, 0, 0, 0);
//	func_800EC9DC(); // Wait for confirmation
//    CloseMessage();  //Close message
    
}

int AskTheQuestion()
{

    int index = 0;
    //load all the questions into an array
    //load correct answer array
    //questionIndex = random integer between 0 and length of array
    //switch case on the index
    //if choice == answer[questionIndex]
    char *questions[1];
    int answers[1];

    questions[index] = GetFirstQuestion();
    answers[index] = GetAnswerToFirstQuestion();        


    ShowMessage(TOAD_PICTURE, questions[0], 0, 0, 0, 0, 0);

    return answers[index];
}

void ShowMessageWithConfirmationAndTeardown(int characterIndex, char* message)
{
    // Helper function that shows a message and tears the message box down
    // after the player confirms the last box.  Don't use for prompt selection.
    
    // This function assumes you aren't using the additional arguments
    // of ShowMessage() and hardcodes them to 0.  If you want to use them,
    // add them to the wrapper function and pass through.

    ShowMessage(characterIndex, message, 0, 0, 0, 0, 0);
    func_800EC9DC();    //Wait for confirmation
    CloseMessage();     //Close the message
    func_800EC6EC();    //Message box teardown
}

int GetChoiceForCPU()
{
    //Generate a random integer that between 0 and N-1 (defined at top of file)
    //This will let the CPU pick a random answer to the quiz question.
    //Google "zero-based arrays" and "modulo operation" if this is confusing.
    //Make sure the # of answers at top of file is correctly defined!
    return GetRandomByte() % ANSWERS_FOR_EACH_QUESTION;
}

int GetAnswerAndTeardownMessageBox(int optionForCPUToPick)
{
    // Get the selection, either from the player or CPU.
    // GetBasicPromptSelection(int strategy, int index)
    // Strategy options:
    //   0 -> If CPU, always pick first (0th) option
    //   1 -> If CPU, always pick second (1th) option
    //   2 -> If CPU, pick the option that is passed in the second argument.
    // Here, we're using 2, and then passing in the random index from above.
    // Thus, the AI will always choose a random option for the question.
    s32 choice = GetBasicPromptSelection(2, optionForCPUToPick);
    CloseMessage();         //Close the message
    func_800EC6EC();        //Message box teardown

    return choice;
}

void CompareAnswer(int choice, int answer)
{
    //If the answer matches what we defined at the bottom of the file...
    if(choice == answer)
    {
        char *right_msg = GetMessageForRightAnswer();
        ShowMessageWithConfirmationAndTeardown(TOAD_PICTURE, right_msg);
    }
    //If the answer doesn't match, it's wrong)
    else 
    {
        char *wrong_msg = GetMessageforWrongAnswer();
        ShowMessageWithConfirmationAndTeardown(TOAD_PICTURE, wrong_msg);       
    }

    return;
}

//***************************************************************************//
//*******Functions that determine every message that isn't a question.*******//
//***************************************************************************//

char* GetMessageForReadyLine()
{
    char *result = 
    "\x0B" // Start the message
    "\x1A\x1A\x1A\x1A" 			//Standard padding for portrait
    "Ready for a quiz"          //
    "\x11"                      // Player Character's Name 
    "\xC3" 						// ?
    "\x0A" 						//Newline
    "\x1A\x1A\x1A\x1A" 			//Standard padding for portrait
    "Here\x5Cs your question"
    "\xC2"                      // !
    "\xFF";						//Show prompt to continue

    return result;
}


char* GetMessageforWrongAnswer()
{
    char *result =
    "\x1A\x1A\x1A\x1A"  		    //Standard padding
    "That is wrongish"
    "\xC2"                          // !
    "\xFF";						    //Show prompt to continue
    
    return result;
}

char* GetMessageForRightAnswer()
{
    char *result =
    "\x1A\x1A\x1A\x1A"  		    //Standard padding
    "That is right"
    "\xC2"                          // !
    "\xFF";						    //Show prompt to continue

    return result;
}

char* GetFirstQuestion()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who has the biggest dick"
    "\xC3" 						    // ?
    "\x0A" 						    //Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Connor"
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Chad"
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent    
    "\x0C"                          // Start option
    "Randy"
    "\x0D"                          // End option
    "\x0A"                          // Newline
    "\x1A\x1A\x1A\x1A\x1A\x1A"      // Little more for option indent 
    "\x0C"                          // Start option
    "Eric"
    "\x0D"                          // End option
    "\xFF";						    //Show prompt to continue

    return result;
}

int GetAnswerToFirstQuestion()
{
    return 1;
}
    