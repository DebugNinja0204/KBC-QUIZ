#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>  // For getch()
#include <unistd.h> // For sleep() on Linux/Mac
#include <windows.h>

#define MAX_QUES_LEN 300
#define MAX_OPT_LEN 100

const char *COLOUR_END = "\033[0m";
const char *RED = "\033[1;31m";
const char *GREEN = "\033[1;32m";
const char *YELLOW = "\033[1;33m";
const char *BLUE = "\033[1;34m";
const char *PINK = "\033[1;35m";
const char *AQUA = "\033[1;36m";

typedef struct KAUN_BANEGA_CROREPATI
{
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPT_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Questions;

int Mflag = 0;

// Function Prototypes
int read_question(const char *file_name, Questions **questions);
void print_formatted_question(const Questions *question);
void play_game(Questions *questions, int no_of_questions);
void use_lifeline(const Questions *question, int *money_won, int *skipped, int *lifelines_used);
void fifty_fifty(const Questions *question, int *money_won);
void audience_poll(const Questions *question, int *money_won);

int main()
{
    printf("------------------------------%sKAUN  BANEGA  CROREPATI !!!%s---------------------------------------\n", PINK, COLOUR_END);
    #ifdef _WIN32
        Sleep(3000); // Pause for 3000 ms (3 seconds) on Windows
    #else
        sleep(3); // Pause for 3 seconds on Unix/Linux
    #endif

    Questions *questions;
    int no_of_questions = read_question("Questions.txt", &questions);
    play_game(questions, no_of_questions);
    free(questions);
    return 0;
}

void play_game(Questions *questions, int no_of_questions)
{
    int money_won = 0;
    int skipped = 0;
    int lifelines_used[3] = {0, 0, 0}; // Track 50/50, Audience Poll, and Skip usage

    for (int i = 0; i < no_of_questions; i++)
    {
        system("cls");
        if (skipped)
        {
            skipped = 0;
            continue;
        }
        char ch;
        print_formatted_question(&questions[i]);
        int seconds = questions[i].timeout;
        printf("%sEnter your answer (A, B, C, D) or L for lifeline: %s\n", GREEN, COLOUR_END);
        for (int j = seconds; j >= 0; j--)
        {
            printf("\r%sHurry!! You have only %d seconds to answer...%s", RED, j, COLOUR_END);
            fflush(stdout);

            if (_kbhit())  // This function checks if a key has been pressed (Windows only)
            {
                ch = toupper(getch());  // Read the input and convert to uppercase
                // Break the loop if the user enters any valid option (A, B, C, D, or L)
                if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D' || ch == 'L')
                {
                    break;  // Exit the countdown if the user has entered a valid input
                }
            }

            if (j == 0) // If time is up
            {
                printf("\n%sTime Over!\nYou have won: Rs. %d%s\n", RED, money_won, COLOUR_END);
                return; // End the game after showing the money won
            }

#ifdef _WIN32
            Sleep(2000); // Sleep for 2000 ms (2 second) on Windows
#else
            sleep(2);  // Sleep for 2 second on Unix/Linux
#endif
        }

        // Process the user's answer after the countdown ends
        if (ch == 'L')
        {
            use_lifeline(&questions[i], &money_won, &skipped, lifelines_used);
            if (Mflag)
            {
                print_formatted_question(&questions[i]);
                printf("\nYou have no lifelines left. Please choose one of the options (A, B, C, D):\n");
                ch = toupper(getch());
            }
            continue;
        }

        if (ch == questions[i].correct_option)
        {
            printf("\n%sCorrect :)%s\n", GREEN, COLOUR_END);
            money_won = questions[i].prize_money;
            printf("%sYou have won: Rs. %d%s\n", BLUE, money_won, COLOUR_END);
#ifdef _WIN32
            Sleep(2000); // Sleep for 2 seconds on Windows
#else
            sleep(2);  // Sleep for 2 seconds on Unix/Linux
#endif
        }
        else
        {
            printf("\n%sWrong Answer! Correct answer is %c.%s\n", RED, questions[i].correct_option, COLOUR_END);
            break;  // End the game if the answer is wrong
        }
          // Clear the screen
    }

    // Print final earnings when the game is over
    printf("\n%sGame Over!! Your total earnings are: Rs. %d%s\n", AQUA, money_won, COLOUR_END);
    if (money_won == 70000000){
        printf(" ------------------------------%sSAATH CRORE !!!%s--------------------------------------- ",BLUE,COLOUR_END);
    }
}


int read_question(const char *file_name, Questions **questions)
{
    FILE *file = fopen(file_name, "r");
    if (!file)
    {
        printf("\nUnable to open questions file!\n");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_QUES_LEN];
    int line_count = 0;
    while (fgets(buffer, MAX_QUES_LEN, file))
    {
        line_count++;
    }

    int no_of_questions = line_count / 8;
    *questions = (Questions *)malloc(no_of_questions * sizeof(Questions));
    rewind(file);

    for (int i = 0; i < no_of_questions; i++)
    {
        fgets((*questions)[i].text, MAX_QUES_LEN, file);
        for (int j = 0; j < 4; j++)
        {
            fgets((*questions)[i].options[j], MAX_OPT_LEN, file);
        }

        char option[10];
        fgets(option, 10, file);
        (*questions)[i].correct_option = option[0];

        char timeout_str[10];
        fgets(timeout_str, 10, file);
        (*questions)[i].timeout = atoi(timeout_str);

        char prize_str[10];
        fgets(prize_str, 10, file);
        (*questions)[i].prize_money = atoi(prize_str);
    }

    fclose(file);
    return no_of_questions;
}

void print_formatted_question(const Questions *question)
{
    printf("\n%s%s%s\n", YELLOW, question->text, COLOUR_END);
    for (int i = 0; i < 4; i++)
    {
        if (question->options[i][0] != '\0')
        {
            printf("%s%c. %s%s\n", AQUA, 'A' + i, question->options[i], COLOUR_END);
        }
    }
}

void use_lifeline(const Questions *question, int *money_won, int *skipped, int *lifelines_used)
{
    if (!lifelines_used[0] || !lifelines_used[1] || !lifelines_used[2])
    {
        printf("\n%sChoose a Lifeline:%s\n", PINK, COLOUR_END);
    }
    else
    {
        printf("\nNo lifelines available\n");
        Mflag = 1;
        return;
    }
    if (!lifelines_used[0])
        printf("A: 50/50\n");
    if (!lifelines_used[1])
        printf("B: Audience Poll\n");
    if (!lifelines_used[2])
        printf("C: Skip the Question\n");

    char choice = toupper(getch());

    switch (choice)
    {
    case 'A':
        if (lifelines_used[0])
        {
            printf("\n%s50/50 already used!%s\n", RED, COLOUR_END);
            break;
        }
        else
        {
            lifelines_used[0] = 1;
            fifty_fifty(question, money_won);
        }
        break;
    case 'B':
        if (lifelines_used[1])
        {
            printf("\n%sAudience Poll already used!%s\n", RED, COLOUR_END);
            break;
        }
        else
        {
            lifelines_used[1] = 1;
            audience_poll(question, money_won);
        }
        break;
    case 'C':
        if (lifelines_used[2])
        {
            printf("\n%sSkip already used!%s\n", RED, COLOUR_END);
            break;
        }
        else
        {
            lifelines_used[2] = 1;
            printf("\n%sQuestion Skipped! Moving to the next one.%s\n", BLUE, COLOUR_END);
            *skipped = 1;
        }
        break;
    default:
        printf("\n%sInvalid Lifeline Choice!%s\n", RED, COLOUR_END);
        printf("Choose Again!");

        break;
    }
}

void fifty_fifty(const Questions *question, int *money_won)
{
    printf("\n%s50/50 Lifeline Activated! Two incorrect options removed.%s\n", GREEN, COLOUR_END);

    int correct_idx = question->correct_option - 'A';
    int displayed[4] = {0};
    displayed[correct_idx] = 1;

    int remaining = 1;
    while (remaining > 0)
    {
        int idx = rand() % 4;
        if (!displayed[idx] && idx != correct_idx)
        {
            displayed[idx] = 1;
            remaining--;
        }
    }

    printf("\n%s%s%s\n", YELLOW, question->text, COLOUR_END);
    for (int i = 0; i < 4; i++)
    {
        if (displayed[i])
        {
            printf("%s%c. %s%s\n", AQUA, 'A' + i, question->options[i], COLOUR_END);
        }
    }

    // Allow user input after 50/50
    printf("%sEnter your answer (A, B, C, D): %s\n", GREEN, COLOUR_END);
    char answer = toupper(getch());
    if (answer == question->correct_option)
    {
        printf("\n%sCorrect :)%s\n", GREEN, COLOUR_END);
        *money_won += question->prize_money;
        printf("%sYou have won: Rs. %d%s\n", BLUE, *money_won, COLOUR_END);
        #ifdef _WIN32
        Sleep(2000); // Sleep for 2 seconds on Windows
        #else
        sleep(2);  // Sleep for 2 seconds on Unix/Linux
        #endif
    }
    else
    {
        printf("\n%sWrong Answer! Correct answer is %c.%s\n", RED, question->correct_option, COLOUR_END);
        exit(0); // Exit game on wrong answer
    }
}

void audience_poll(const Questions *question, int *money_won)
{
    printf("\n%sAudience Poll Activated! Here are the poll results:%s\n", AQUA, COLOUR_END);
    srand(time(NULL));

    int percentages[4] = {0};
    int correct_idx = question->correct_option - 'A';

    // Assign a random percentage (between 30-70) to the correct option
    percentages[correct_idx] = (rand() % 41) + 30; // Correct option gets a higher chance (30-70%)

    int remaining = 100 - percentages[correct_idx]; // Remaining percentage for incorrect options

    // Assign random percentages to incorrect options
    for (int i = 0; i < 4; i++)
    {
        if (i != correct_idx)
        {
            percentages[i] = rand() % (remaining + 1); // Random percentage from remaining
            remaining -= percentages[i];               // Reduce the remaining percentage
        }
    }

    // If any remaining percentage is left (due to rounding), assign it to the last incorrect option
    if (remaining > 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (i != correct_idx && percentages[i] == 0)
            {
                percentages[i] = remaining;
                break;
            }
        }
    }

    // Display poll results
    for (int i = 0; i < 4; i++)
    {
        printf("%c. %s - %d%%\n", 'A' + i, question->options[i], percentages[i]);
    }

    // Get user's answer
    printf("%sEnter your answer (A, B, C, D): %s\n", GREEN, COLOUR_END);
    char answer = toupper(getch());

    // Check if the answer is correct
    if (answer == question->correct_option)
    {
        printf("\n%sCorrect :)%s\n", GREEN, COLOUR_END);
        *money_won += question->prize_money;
        printf("%sYou have won: Rs. %d%s\n", BLUE, *money_won, COLOUR_END);
        #ifdef _WIN32
        Sleep(2000); // Sleep for 2 seconds on Windows
        #else
        sleep(2);  // Sleep for 2 seconds on Unix/Linux
        #endif
    }
    else
    {
        printf("\n%sWrong Answer! Correct answer is %c.%s\n", RED, question->correct_option, COLOUR_END);
        exit(0); // Exit game on wrong answer
    }
}
