// NAME: Game Night Quiz
// GAMES: MP3_USA
// EXECUTION: Direct

//Character Index taken from the Wiki:
//https://github.com/PartyPlanner64/PartyPlanner64/wiki/Displaying-Messages
#define TOAD_PICTURE 3  

//How many questions are defined at the bottom of the file?
#define QUESTION_COUNT 2

//How many answers does each question have?
//AI requires every question to have the same number of options
#define ANSWERS_FOR_EACH_QUESTION 4

//TODO - setup VS Code Intellisense w/ Ultra64.h include paths
//http://n64devkit.square7.ch/n64man/header.htm
#include "ultra64.h" 

void main() 
{
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

void DisplayGreetingMessage()
{
   //Display the ready message...
    char* greeting_msg = GetGreetingMessage();
    ez_ShowMessageWithConfirmation(TOAD_PICTURE, greeting_msg);
}


int AskTheQuestion()
{
    int index = PickRandomQuestionIndex();

    char *question_msg = GetQuestionMessageByNumber(index);
    char *right_msg = GetMessageForRightAnswer();


    ShowMessage(TOAD_PICTURE, question_msg, 0, 0, 0, 0, 0);

    int answer = GetCorrectAnswerToQuestionByNumber(index);

    return answer;
}


int PickRandomQuestionIndex()
{
    return GetRandomByte() % QUESTION_COUNT;
}


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
    ez_TeardownMessageBox();

    return choice;
}

int GetChoiceForCPU()
{
    //Generate a random integer that between 0 and N-1 (defined at top of file)
    //This will let the CPU pick a random answer to the quiz question.
    //Google "zero-based arrays" and "modulo operation" if this is confusing.
    //Make sure the # of answers at top of file is correctly defined!
    return GetRandomByte() % ANSWERS_FOR_EACH_QUESTION;
}


void RewardPlayerForCorrectAnswer()
{
        char *right_msg = GetMessageForRightAnswer();
        ez_ShowMessageWithConfirmation(TOAD_PICTURE, right_msg);
        return;
}

void PunishPlayerForIncorrectAnswer()
{
    char *wrong_msg = GetMessageforWrongAnswer();
    ez_ShowMessageWithConfirmation(TOAD_PICTURE, wrong_msg);       

    return;
}



//***************************************************************************//
//*******Functions that determine every message that isn't a question.*******//
//***************************************************************************//

char* GetGreetingMessage()
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

//***************************************************************************//
//*******Question definitions ***********************************************//
//***************************************************************************//

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
        default:
           result = GetFirstQuestionMessage();
           return result;
    }




    return result;

}

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
        default:
           result = GetAnswerToFirstQuestion();
           return result;
    }


    return result;
}


//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//
char* GetFirstQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who has the biggest heart"
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

//***************************************************************************//
//****************************Question***************************************//
//***************************************************************************//

char* GetSecondQuestionMessage()
{
    char *result =
    "\x0B"                          // Start the message
    "\x1A\x1A\x1A\x1A"  		    // Standard padding
    "Who has the biggest smile"
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
int GetAnswerToSecondQuestion()
{
    return 0;
}






/*****************************************************************************
**** Helper functions to hide basic Mario Party commands/help readability.****
******************************************************************************/


// Helper function that shows a message and tears the message box down
// after the player confirms the last box.  Don't use for prompt selection.
void ez_ShowMessageWithConfirmation(int characterIndex, char* message)
{
    // This function assumes you aren't using the additional arguments
    // of ShowMessage() and hardcodes them to 0.  If you want to use them,
    // add them to the wrapper function and pass through.

    ShowMessage(characterIndex, message, 0, 0, 0, 0, 0);
    ez_WaitForPlayerConfirmation();
    ez_TeardownMessageBox();

}

//just a wrapper
void ez_WaitForPlayerConfirmation()
{
    func_800EC9DC();    //Wait for confirmation
}

//Helper function that just does teardown.  
void ez_TeardownMessageBox()
{
    CloseMessage();     //Close the message
    func_800EC6EC();    //Message box teardown
}