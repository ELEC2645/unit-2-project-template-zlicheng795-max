// ELEC2645 Unit 2 Project: Scientific Calculator
// Command Line Application with Expression Evaluation and Scientific Functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcs.h"

/* External calculator state */
extern CalculatorState calc_state;

/* Prototypes */
static void main_menu(void);
static void print_main_menu(void);
static int  get_user_input(void);
static void select_menu_item(int input);
static void go_back_to_main(void);
static int  is_integer(const char *s);

int main(void)
{
    /* Initialize calculator state */
    init_calculator_state(&calc_state);
    
    /* Display welcome banner */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                              ║\n");
    printf("║             SCIENTIFIC CALCULATOR v1.0                       ║\n");
    printf("║                                                              ║\n");
    printf("║              ELEC2645 Unit 2 Individual Project              ║\n");
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Welcome to the Scientific Calculator!\n");
    printf("\n");
    printf("Features:\n");
    printf("  • Expression Evaluation (with operator precedence)\n");
    printf("  • Scientific Functions (trig, log, exp, roots)\n");
    printf("  • Unit Conversions (temperature, length, weight, angle)\n");
    printf("  • Programmer Mode (base conversion, bitwise operations)\n");
    printf("  • Memory & History Support\n");
    printf("\n");
    
    /* Main application loop */
    for(;;) {
        main_menu();
    }
    
    return 0;
}

static void main_menu(void)
{
    print_main_menu();
    {
        int input = get_user_input();
        select_menu_item(input);
    }
}

static int get_user_input(void)
{
    enum { MENU_ITEMS = 5 };   /* 1..4 = items, 5 = Exit */
    char buf[128];
    int valid_input = 0;
    int value = 0;

    do {
        printf("\nSelect item: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            puts("\nInput error. Exiting.");
            exit(1);
        }

        /* Strip trailing newline */
        buf[strcspn(buf, "\r\n")] = '\0';

        if (!is_integer(buf)) {
            printf("Enter an integer!\n");
            valid_input = 0;
        } else {
            value = (int)strtol(buf, NULL, 10);
            if (value >= 1 && value <= MENU_ITEMS) {
                valid_input = 1;
            } else {
                printf("Invalid menu item!\n");
                valid_input = 0;
            }
        }
    } while (!valid_input);

    return value;
}

static void select_menu_item(int input)
{
    switch (input) {
        case 1:
            menu_item_1();
            go_back_to_main();
            break;
        case 2:
            menu_item_2();
            go_back_to_main();
            break;
        case 3:
            menu_item_3();
            go_back_to_main();
            break;
        case 4:
            menu_item_4();
            go_back_to_main();
            break;
        default:
            printf("\n");
            printf("╔══════════════════════════════════════════════════════════════╗\n");
            printf("║                                                              ║\n");
            printf("║              Thank you for using the                         ║\n");
            printf("║                Scientific Calculator!                        ║\n");
            printf("║                                                              ║\n");
            printf("╚══════════════════════════════════════════════════════════════╝\n");
            printf("\n");
            exit(0);
    }
}

static void print_main_menu(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                         MAIN MENU                            ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║                                                              ║\n");
    printf("║  1. Basic Calculator                                         ║\n");
    printf("║     └─ Expression evaluation with +, -, *, /, ^, %%         ║\n");
    printf("║     └─ Memory, History, and 'ans' support                   ║\n");
    printf("║                                                              ║\n");
    printf("║  2. Scientific Functions                                     ║\n");
    printf("║     └─ Trigonometry (sin, cos, tan, arc functions)          ║\n");
    printf("║     └─ Logarithms, Exponentials, Roots, Factorial           ║\n");
    printf("║                                                              ║\n");
    printf("║  3. Unit Converter                                           ║\n");
    printf("║     └─ Temperature, Length, Weight, Angle                   ║\n");
    printf("║                                                              ║\n");
    printf("║  4. Programmer Mode                                          ║\n");
    printf("║     └─ Base conversion (Binary, Octal, Hex)                 ║\n");
    printf("║     └─ Bitwise operations (AND, OR, XOR, NOT, Shifts)       ║\n");
    printf("║                                                              ║\n");
    printf("║  5. Exit Application                                         ║\n");
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    /* Display current state */
    printf("\nCurrent Settings:\n");
    printf("  Angle Mode: %s\n", calc_state.angle_mode == RADIANS ? "RADIANS" : "DEGREES");
    printf("  Memory: %.10g\n", calc_state.memory);
    printf("  Last Result: %.10g\n", calc_state.last_result);
}

static void go_back_to_main(void)
{
    char buf[64];
    do {
        printf("\nPress 'b' or 'B' to go back to main menu: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            puts("\nInput error. Exiting.");
            exit(1);
        }
        buf[strcspn(buf, "\r\n")] = '\0';
    } while (!(buf[0] == 'b' || buf[0] == 'B') || buf[1] != '\0');
}

/* Return 1 if s is an optional [+/-] followed by one-or-more digits, else 0 */
static int is_integer(const char *s)
{
    if (!s || !*s) return 0;

    /* Optional sign */
    if (*s == '+' || *s == '-') s++;

    /* Must have at least one digit */
    if (!isdigit((unsigned char)*s)) return 0;

    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}
