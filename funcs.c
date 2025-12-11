#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"

/* Global calculator state - properly defined */
CalculatorState calc_state;

/* ========== Stack Operations ========== */

void stack_init(Stack *s) {
    s->top = -1;
}

int stack_is_empty(Stack *s) {
    return s->top == -1;
}

int stack_is_full(Stack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

void stack_push(Stack *s, double value) {
    if (!stack_is_full(s)) {
        s->items[++(s->top)] = value;
    }
}

double stack_pop(Stack *s) {
    if (!stack_is_empty(s)) {
        return s->items[(s->top)--];
    }
    return 0.0;
}

double stack_peek(Stack *s) {
    if (!stack_is_empty(s)) {
        return s->items[s->top];
    }
    return 0.0;
}

/* ========== Expression Evaluation ========== */

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
}

int get_precedence(char op) {
    switch(op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
        case '%':
            return 2;
        case '^':
            return 3;
        default:
            return 0;
    }
}

int is_left_associative(char op) {
    return op != '^';
}

double apply_operator(char op, double a, double b, char *error) {
    switch(op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (fabs(b) < 1e-10) {
                strcpy(error, "Division by zero");
                return 0.0;
            }
            return a / b;
        case '^': return pow(a, b);
        case '%':
            if (fabs(b) < 1e-10) {
                strcpy(error, "Modulo by zero");
                return 0.0;
            }
            return fmod(a, b);
        default:
            strcpy(error, "Unknown operator");
            return 0.0;
    }
}

double evaluate_expression(const char *expr, CalculatorState *state, char *error) {
    Stack values, operators;
    stack_init(&values);
    stack_init(&operators);
    
    error[0] = '\0';
    int i = 0;
    int len = strlen(expr);
    
    while (i < len) {
        /* Skip whitespace */
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        
        /* Handle numbers */
        if (isdigit(expr[i]) || (expr[i] == '.' && i + 1 < len && isdigit(expr[i + 1]))) {
            char num_str[64];
            int j = 0;
            while (i < len && (isdigit(expr[i]) || expr[i] == '.')) {
                num_str[j++] = expr[i++];
            }
            num_str[j] = '\0';
            stack_push(&values, atof(num_str));
            continue;
        }
        
        /* Handle opening parenthesis */
        if (expr[i] == '(') {
            stack_push(&operators, '(');
            i++;
            continue;
        }
        
        /* Handle closing parenthesis */
        if (expr[i] == ')') {
            while (!stack_is_empty(&operators) && stack_peek(&operators) != '(') {
                double b = stack_pop(&values);
                double a = stack_pop(&values);
                char op = (char)stack_pop(&operators);
                double result = apply_operator(op, a, b, error);
                if (error[0] != '\0') return 0.0;
                stack_push(&values, result);
            }
            if (!stack_is_empty(&operators)) {
                stack_pop(&operators); /* Remove '(' */
            }
            i++;
            continue;
        }
        
        /* Handle operators */
        if (is_operator(expr[i])) {
            /* Handle unary minus */
            if (expr[i] == '-' && (i == 0 || expr[i-1] == '(' || is_operator(expr[i-1]))) {
                stack_push(&values, 0.0);
            }
            
            while (!stack_is_empty(&operators) && 
                   stack_peek(&operators) != '(' &&
                   ((is_left_associative(expr[i]) && 
                     get_precedence(expr[i]) <= get_precedence((char)stack_peek(&operators))) ||
                    (!is_left_associative(expr[i]) && 
                     get_precedence(expr[i]) < get_precedence((char)stack_peek(&operators))))) {
                double b = stack_pop(&values);
                double a = stack_pop(&values);
                char op = (char)stack_pop(&operators);
                double result = apply_operator(op, a, b, error);
                if (error[0] != '\0') return 0.0;
                stack_push(&values, result);
            }
            stack_push(&operators, expr[i]);
            i++;
            continue;
        }
        
        i++;
    }
    
    /* Process remaining operators */
    while (!stack_is_empty(&operators)) {
        double b = stack_pop(&values);
        double a = stack_pop(&values);
        char op = (char)stack_pop(&operators);
        double result = apply_operator(op, a, b, error);
        if (error[0] != '\0') return 0.0;
        stack_push(&values, result);
    }
    
    return stack_pop(&values);
}

/* ========== Basic Arithmetic Operations ========== */

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (fabs(b) < 1e-10) {
        return NAN;
    }
    return a / b;
}

double power(double base, double exponent) {
    return pow(base, exponent);
}

double modulo(double a, double b) {
    if (fabs(b) < 1e-10) {
        return NAN;
    }
    return fmod(a, b);
}

/* ========== Scientific Functions ========== */

double factorial(int n) {
    if (n < 0) return NAN;
    if (n == 0 || n == 1) return 1.0;
    if (n > 170) return INFINITY; /* Prevent overflow */
    
    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double sine(double x, AngleMode mode) {
    if (mode == DEGREES) {
        x = to_radians(x);
    }
    return sin(x);
}

double cosine(double x, AngleMode mode) {
    if (mode == DEGREES) {
        x = to_radians(x);
    }
    return cos(x);
}

double tangent(double x, AngleMode mode) {
    if (mode == DEGREES) {
        x = to_radians(x);
    }
    return tan(x);
}

double arcsine(double x, AngleMode mode) {
    if (x < -1.0 || x > 1.0) return NAN;
    double result = asin(x);
    return (mode == DEGREES) ? to_degrees(result) : result;
}

double arccosine(double x, AngleMode mode) {
    if (x < -1.0 || x > 1.0) return NAN;
    double result = acos(x);
    return (mode == DEGREES) ? to_degrees(result) : result;
}

double arctangent(double x, AngleMode mode) {
    double result = atan(x);
    return (mode == DEGREES) ? to_degrees(result) : result;
}

double logarithm_base10(double x) {
    if (x <= 0) return NAN;
    return log10(x);
}

double logarithm_natural(double x) {
    if (x <= 0) return NAN;
    return log(x);
}

double logarithm_base(double x, double base) {
    if (x <= 0 || base <= 0 || base == 1) return NAN;
    return log(x) / log(base);
}

double exponential(double x) {
    return exp(x);
}

double square_root(double x) {
    if (x < 0) return NAN;
    return sqrt(x);
}

double cube_root(double x) {
    return cbrt(x);
}

double nth_root(double x, double n) {
    if (n == 0) return NAN;
    if (x < 0 && fmod(n, 2.0) == 0) return NAN; /* Even root of negative */
    return pow(x, 1.0 / n);
}

double absolute_value(double x) {
    return fabs(x);
}

/* ========== Unit Conversion Functions ========== */

double celsius_to_fahrenheit(double c) {
    return (c * 9.0 / 5.0) + 32.0;
}

double fahrenheit_to_celsius(double f) {
    return (f - 32.0) * 5.0 / 9.0;
}

double celsius_to_kelvin(double c) {
    return c + 273.15;
}

double kelvin_to_celsius(double k) {
    return k - 273.15;
}

double meters_to_feet(double m) {
    return m * 3.28084;
}

double feet_to_meters(double f) {
    return f / 3.28084;
}

double kilometers_to_miles(double km) {
    return km * 0.621371;
}

double miles_to_kilometers(double mi) {
    return mi / 0.621371;
}

double kilograms_to_pounds(double kg) {
    return kg * 2.20462;
}

double pounds_to_kilograms(double lb) {
    return lb / 2.20462;
}

double degrees_to_radians(double deg) {
    return deg * PI / 180.0;
}

double radians_to_degrees(double rad) {
    return rad * 180.0 / PI;
}

/* ========== Programmer Mode Functions ========== */

void decimal_to_binary(long num, char *result) {
    if (num == 0) {
        strcpy(result, "0");
        return;
    }
    
    int negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    char temp[65];
    int i = 0;
    
    while (num > 0) {
        temp[i++] = (num % 2) + '0';
        num /= 2;
    }
    
    int j = 0;
    if (negative) result[j++] = '-';
    
    while (i > 0) {
        result[j++] = temp[--i];
    }
    result[j] = '\0';
}

void decimal_to_octal(long num, char *result) {
    if (num == 0) {
        strcpy(result, "0");
        return;
    }
    
    int negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    sprintf(result, "%s%lo", negative ? "-" : "", num);
}

void decimal_to_hexadecimal(long num, char *result) {
    if (num == 0) {
        strcpy(result, "0");
        return;
    }
    
    int negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }
    
    sprintf(result, "%s%lX", negative ? "-" : "", num);
}

long binary_to_decimal(const char *bin) {
    long result = 0;
    int negative = 0;
    int i = 0;
    
    if (bin[0] == '-') {
        negative = 1;
        i = 1;
    }
    
    while (bin[i]) {
        result = result * 2 + (bin[i] - '0');
        i++;
    }
    
    return negative ? -result : result;
}

long octal_to_decimal(const char *oct) {
    long result = 0;
    int negative = 0;
    int i = 0;
    
    if (oct[0] == '-') {
        negative = 1;
        i = 1;
    }
    
    while (oct[i]) {
        result = result * 8 + (oct[i] - '0');
        i++;
    }
    
    return negative ? -result : result;
}

long hexadecimal_to_decimal(const char *hex) {
    long result = 0;
    int negative = 0;
    int i = 0;
    
    if (hex[0] == '-') {
        negative = 1;
        i = 1;
    }
    
    while (hex[i]) {
        char c = toupper(hex[i]);
        if (c >= '0' && c <= '9') {
            result = result * 16 + (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            result = result * 16 + (c - 'A' + 10);
        }
        i++;
    }
    
    return negative ? -result : result;
}

long bitwise_and(long a, long b) {
    return a & b;
}

long bitwise_or(long a, long b) {
    return a | b;
}

long bitwise_xor(long a, long b) {
    return a ^ b;
}

long bitwise_not(long a) {
    return ~a;
}

long left_shift(long num, int shift) {
    return num << shift;
}

long right_shift(long num, int shift) {
    return num >> shift;
}

/* ========== Helper Functions ========== */

double to_radians(double degrees) {
    return degrees * PI / 180.0;
}

double to_degrees(double radians) {
    return radians * 180.0 / PI;
}

void add_to_history(CalculatorState *state, const char *expr, double result) {
    if (state->history_count >= MAX_HISTORY) {
        /* Shift history up */
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(state->history[i], state->history[i + 1]);
        }
        state->history_count = MAX_HISTORY - 1;
    }
    
    char entry[MAX_EXPR_LENGTH];
    snprintf(entry, MAX_EXPR_LENGTH, "%s = %.10g", expr, result);
    strcpy(state->history[state->history_count++], entry);
}

void show_history(CalculatorState *state) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    CALCULATION HISTORY                     ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    if (state->history_count == 0) {
        printf("  No calculations in history.\n\n");
    } else {
        for (int i = 0; i < state->history_count; i++) {
            printf("  %2d. %s\n", i + 1, state->history[i]);
        }
        printf("\n");
    }
}

void clear_history(CalculatorState *state) {
    state->history_count = 0;
    printf("\n✓ History cleared.\n\n");
}

int is_valid_number(const char *str) {
    int has_digit = 0;
    int has_dot = 0;
    int i = 0;
    
    if (str[0] == '-' || str[0] == '+') i++;
    
    for (; str[i]; i++) {
        if (isdigit(str[i])) {
            has_digit = 1;
        } else if (str[i] == '.') {
            if (has_dot) return 0;
            has_dot = 1;
        } else {
            return 0;
        }
    }
    
    return has_digit;
}

void format_result(double result, char *buffer, size_t size) {
    if (isnan(result)) {
        snprintf(buffer, size, "Error: Invalid input");
    } else if (isinf(result)) {
        snprintf(buffer, size, result > 0 ? "∞" : "-∞");
    } else if (fabs(result) < 1e-10 && result != 0) {
        snprintf(buffer, size, "%.10e", result);
    } else if (fabs(result) >= 1e10) {
        snprintf(buffer, size, "%.10e", result);
    } else {
        snprintf(buffer, size, "%.10g", result);
    }
}

/* ========== Calculator State Management ========== */

void init_calculator_state(CalculatorState *state) {
    state->angle_mode = RADIANS;
    state->memory = 0.0;
    state->last_result = 0.0;
    state->history_count = 0;
}

void set_angle_mode(CalculatorState *state, AngleMode mode) {
    state->angle_mode = mode;
}

void store_memory(CalculatorState *state, double value) {
    state->memory = value;
}

double recall_memory(CalculatorState *state) {
    return state->memory;
}

void clear_memory(CalculatorState *state) {
    state->memory = 0.0;
}

/* ========== Menu Item Functions ========== */

/* Menu Item 1: Basic Calculator with Expression Evaluation */
void menu_item_1(void) {
    char input[MAX_EXPR_LENGTH];
    char error[256];
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    BASIC CALCULATOR                        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Enter a mathematical expression to evaluate.\n");
    printf("Supported operators: + - * / ^ (power) %% (modulo)\n");
    printf("You can use parentheses: (2+3)*4\n");
    printf("Special commands:\n");
    printf("  'ans' - Use last result\n");
    printf("  'mem' - Recall memory\n");
    printf("  'history' - Show calculation history\n");
    printf("  'clear' - Clear history\n");
    printf("  'q' - Return to main menu\n\n");
    
    while (1) {
        printf("Expression: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        
        /* Remove newline */
        input[strcspn(input, "\r\n")] = '\0';
        
        /* Check for quit */
        if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
            break;
        }
        
        /* Check for history command */
        if (strcmp(input, "history") == 0) {
            show_history(&calc_state);
            continue;
        }
        
        /* Check for clear command */
        if (strcmp(input, "clear") == 0) {
            clear_history(&calc_state);
            continue;
        }
        
        /* Replace 'ans' with last result */
        char expr[MAX_EXPR_LENGTH];
        strcpy(expr, input);
        char *ans_pos = strstr(expr, "ans");
        if (ans_pos != NULL) {
            char temp[MAX_EXPR_LENGTH];
            int pos = ans_pos - expr;
            strncpy(temp, expr, pos);
            temp[pos] = '\0';
            char num_str[32];
            snprintf(num_str, sizeof(num_str), "%.10g", calc_state.last_result);
            strcat(temp, num_str);
            strcat(temp, ans_pos + 3);
            strcpy(expr, temp);
        }
        
        /* Replace 'mem' with memory value */
        char *mem_pos = strstr(expr, "mem");
        if (mem_pos != NULL) {
            char temp[MAX_EXPR_LENGTH];
            int pos = mem_pos - expr;
            strncpy(temp, expr, pos);
            temp[pos] = '\0';
            char num_str[32];
            snprintf(num_str, sizeof(num_str), "%.10g", calc_state.memory);
            strcat(temp, num_str);
            strcat(temp, mem_pos + 3);
            strcpy(expr, temp);
        }
        
        /* Evaluate expression */
        double result = evaluate_expression(expr, &calc_state, error);
        
        if (error[0] != '\0') {
            printf("  ✗ Error: %s\n\n", error);
        } else {
            char result_str[64];
            format_result(result, result_str, sizeof(result_str));
            printf("  ➜ Result: %s\n\n", result_str);
            
            calc_state.last_result = result;
            add_to_history(&calc_state, input, result);
            
            /* Ask if user wants to store in memory */
            printf("Store result in memory? (y/n): ");
            char choice[10];
            if (fgets(choice, sizeof(choice), stdin)) {
                if (choice[0] == 'y' || choice[0] == 'Y') {
                    store_memory(&calc_state, result);
                    printf("✓ Stored in memory\n\n");
                }
            }
        }
    }
}

/* Menu Item 2: Scientific Functions */
void menu_item_2(void) {
    char input[MAX_EXPR_LENGTH];
    int choice;
    
    while (1) {
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║                  SCIENTIFIC FUNCTIONS                      ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║  1. Trigonometric Functions                                ║\n");
        printf("║  2. Inverse Trigonometric Functions                        ║\n");
        printf("║  3. Logarithmic Functions                                  ║\n");
        printf("║  4. Exponential & Power Functions                          ║\n");
        printf("║  5. Root Functions                                         ║\n");
        printf("║  6. Factorial & Absolute Value                             ║\n");
        printf("║  7. Toggle Angle Mode (Current: %-8s)                  ║\n",
               calc_state.angle_mode == RADIANS ? "RADIANS" : "DEGREES");
        printf("║  8. Back to Main Menu                                      ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        
        printf("\nSelect function: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        choice = atoi(input);
        
        if (choice == 8) break;
        
        if (choice == 7) {
            calc_state.angle_mode = (calc_state.angle_mode == RADIANS) ? DEGREES : RADIANS;
            printf("\n✓ Angle mode set to %s\n", 
                   calc_state.angle_mode == RADIANS ? "RADIANS" : "DEGREES");
            continue;
        }
        
        double x, result, base;
        int n;
        char result_str[64];
        
        switch (choice) {
            case 1: /* Trigonometric */
                printf("\n1. sin  2. cos  3. tan\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int trig_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                x = atof(input);
                
                switch (trig_choice) {
                    case 1: result = sine(x, calc_state.angle_mode); break;
                    case 2: result = cosine(x, calc_state.angle_mode); break;
                    case 3: result = tangent(x, calc_state.angle_mode); break;
                    default: printf("Invalid choice\n"); continue;
                }
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            case 2: /* Inverse Trigonometric */
                printf("\n1. arcsin  2. arccos  3. arctan\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int inv_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                x = atof(input);
                
                switch (inv_choice) {
                    case 1: result = arcsine(x, calc_state.angle_mode); break;
                    case 2: result = arccosine(x, calc_state.angle_mode); break;
                    case 3: result = arctangent(x, calc_state.angle_mode); break;
                    default: printf("Invalid choice\n"); continue;
                }
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            case 3: /* Logarithmic */
                printf("\n1. log10  2. ln  3. log (custom base)\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int log_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                x = atof(input);
                
                if (log_choice == 3) {
                    printf("Enter base: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    base = atof(input);
                    result = logarithm_base(x, base);
                } else if (log_choice == 1) {
                    result = logarithm_base10(x);
                } else if (log_choice == 2) {
                    result = logarithm_natural(x);
                } else {
                    printf("Invalid choice\n");
                    continue;
                }
                
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            case 4: /* Exponential & Power */
                printf("\n1. e^x  2. x^y\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int exp_choice = atoi(input);
                
                if (exp_choice == 1) {
                    printf("Enter exponent: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    x = atof(input);
                    result = exponential(x);
                } else if (exp_choice == 2) {
                    printf("Enter base: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    x = atof(input);
                    printf("Enter exponent: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    double y = atof(input);
                    result = power(x, y);
                } else {
                    printf("Invalid choice\n");
                    continue;
                }
                
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            case 5: /* Roots */
                printf("\n1. Square root  2. Cube root  3. nth root\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int root_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                x = atof(input);
                
                if (root_choice == 1) {
                    result = square_root(x);
                } else if (root_choice == 2) {
                    result = cube_root(x);
                } else if (root_choice == 3) {
                    printf("Enter root degree: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    double n_val = atof(input);
                    result = nth_root(x, n_val);
                } else {
                    printf("Invalid choice\n");
                    continue;
                }
                
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            case 6: /* Factorial & Absolute */
                printf("\n1. Factorial  2. Absolute value\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                int misc_choice = atoi(input);
                
                if (misc_choice == 1) {
                    printf("Enter integer: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    n = atoi(input);
                    result = factorial(n);
                } else if (misc_choice == 2) {
                    printf("Enter value: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    x = atof(input);
                    result = absolute_value(x);
                } else {
                    printf("Invalid choice\n");
                    continue;
                }
                
                format_result(result, result_str, sizeof(result_str));
                printf("➜ Result: %s\n", result_str);
                calc_state.last_result = result;
                break;
                
            default:
                printf("Invalid choice!\n");
        }
    }
}

/* Menu Item 3: Unit Converter */
void menu_item_3(void) {
    char input[MAX_EXPR_LENGTH];
    int choice;
    
    while (1) {
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║                     UNIT CONVERTER                         ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║  1. Temperature Conversion                                 ║\n");
        printf("║  2. Length Conversion                                      ║\n");
        printf("║  3. Weight Conversion                                      ║\n");
        printf("║  4. Angle Conversion                                       ║\n");
        printf("║  5. Back to Main Menu                                      ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        
        printf("\nSelect category: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        choice = atoi(input);
        
        if (choice == 5) break;
        
        double value, result;
        int sub_choice;
        
        switch (choice) {
            case 1: /* Temperature */
                printf("\n1. °C → °F  2. °F → °C  3. °C → K  4. K → °C\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                value = atof(input);
                
                switch (sub_choice) {
                    case 1: 
                        result = celsius_to_fahrenheit(value);
                        printf("➜ %.2f°C = %.2f°F\n", value, result);
                        break;
                    case 2:
                        result = fahrenheit_to_celsius(value);
                        printf("➜ %.2f°F = %.2f°C\n", value, result);
                        break;
                    case 3:
                        result = celsius_to_kelvin(value);
                        printf("➜ %.2f°C = %.2fK\n", value, result);
                        break;
                    case 4:
                        result = kelvin_to_celsius(value);
                        printf("➜ %.2fK = %.2f°C\n", value, result);
                        break;
                    default:
                        printf("Invalid choice\n");
                        continue;
                }
                calc_state.last_result = result;
                break;
                
            case 2: /* Length */
                printf("\n1. m → ft  2. ft → m  3. km → mi  4. mi → km\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                value = atof(input);
                
                switch (sub_choice) {
                    case 1:
                        result = meters_to_feet(value);
                        printf("➜ %.2f m = %.2f ft\n", value, result);
                        break;
                    case 2:
                        result = feet_to_meters(value);
                        printf("➜ %.2f ft = %.2f m\n", value, result);
                        break;
                    case 3:
                        result = kilometers_to_miles(value);
                        printf("➜ %.2f km = %.2f mi\n", value, result);
                        break;
                    case 4:
                        result = miles_to_kilometers(value);
                        printf("➜ %.2f mi = %.2f km\n", value, result);
                        break;
                    default:
                        printf("Invalid choice\n");
                        continue;
                }
                calc_state.last_result = result;
                break;
                
            case 3: /* Weight */
                printf("\n1. kg → lb  2. lb → kg\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                value = atof(input);
                
                switch (sub_choice) {
                    case 1:
                        result = kilograms_to_pounds(value);
                        printf("➜ %.2f kg = %.2f lb\n", value, result);
                        break;
                    case 2:
                        result = pounds_to_kilograms(value);
                        printf("➜ %.2f lb = %.2f kg\n", value, result);
                        break;
                    default:
                        printf("Invalid choice\n");
                        continue;
                }
                calc_state.last_result = result;
                break;
                
            case 4: /* Angle */
                printf("\n1. deg → rad  2. rad → deg\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter value: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                value = atof(input);
                
                switch (sub_choice) {
                    case 1:
                        result = degrees_to_radians(value);
                        printf("➜ %.4f° = %.6f rad\n", value, result);
                        break;
                    case 2:
                        result = radians_to_degrees(value);
                        printf("➜ %.6f rad = %.4f°\n", value, result);
                        break;
                    default:
                        printf("Invalid choice\n");
                        continue;
                }
                calc_state.last_result = result;
                break;
                
            default:
                printf("Invalid choice!\n");
        }
    }
}

/* Menu Item 4: Programmer Mode */
void menu_item_4(void) {
    char input[MAX_EXPR_LENGTH];
    int choice;
    
    while (1) {
        printf("\n╔════════════════════════════════════════════════════════════╗\n");
        printf("║                     PROGRAMMER MODE                        ║\n");
        printf("╠════════════════════════════════════════════════════════════╣\n");
        printf("║  1. Number Base Conversion                                 ║\n");
        printf("║  2. Bitwise Operations                                     ║\n");
        printf("║  3. Bit Shifting                                           ║\n");
        printf("║  4. Back to Main Menu                                      ║\n");
        printf("╚════════════════════════════════════════════════════════════╝\n");
        
        printf("\nSelect function: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        choice = atoi(input);
        
        if (choice == 4) break;
        
        long num, num2;
        char result[128];
        int sub_choice, shift_amount;
        
        switch (choice) {
            case 1: /* Base Conversion */
                printf("\n1. Dec→Bin  2. Dec→Oct  3. Dec→Hex\n");
                printf("4. Bin→Dec  5. Oct→Dec  6. Hex→Dec\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                if (sub_choice <= 3) {
                    printf("Enter decimal number: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    num = atol(input);
                    
                    switch (sub_choice) {
                        case 1:
                            decimal_to_binary(num, result);
                            printf("➜ Decimal %ld = Binary %s\n", num, result);
                            break;
                        case 2:
                            decimal_to_octal(num, result);
                            printf("➜ Decimal %ld = Octal %s\n", num, result);
                            break;
                        case 3:
                            decimal_to_hexadecimal(num, result);
                            printf("➜ Decimal %ld = Hexadecimal %s\n", num, result);
                            break;
                    }
                } else {
                    switch (sub_choice) {
                        case 4:
                            printf("Enter binary number: ");
                            if (!fgets(input, sizeof(input), stdin)) break;
                            input[strcspn(input, "\r\n")] = '\0';
                            num = binary_to_decimal(input);
                            printf("➜ Binary %s = Decimal %ld\n", input, num);
                            break;
                        case 5:
                            printf("Enter octal number: ");
                            if (!fgets(input, sizeof(input), stdin)) break;
                            input[strcspn(input, "\r\n")] = '\0';
                            num = octal_to_decimal(input);
                            printf("➜ Octal %s = Decimal %ld\n", input, num);
                            break;
                        case 6:
                            printf("Enter hexadecimal number: ");
                            if (!fgets(input, sizeof(input), stdin)) break;
                            input[strcspn(input, "\r\n")] = '\0';
                            num = hexadecimal_to_decimal(input);
                            printf("➜ Hexadecimal %s = Decimal %ld\n", input, num);
                            break;
                        default:
                            printf("Invalid choice\n");
                    }
                }
                break;
                
            case 2: /* Bitwise Operations */
                printf("\n1. AND  2. OR  3. XOR  4. NOT\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter first number: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                num = atol(input);
                
                if (sub_choice == 4) {
                    long res = bitwise_not(num);
                    printf("➜ NOT %ld = %ld (0x%lX)\n", num, res, res);
                } else {
                    printf("Enter second number: ");
                    if (!fgets(input, sizeof(input), stdin)) break;
                    num2 = atol(input);
                    
                    long res;
                    switch (sub_choice) {
                        case 1:
                            res = bitwise_and(num, num2);
                            printf("➜ %ld AND %ld = %ld (0x%lX)\n", num, num2, res, res);
                            break;
                        case 2:
                            res = bitwise_or(num, num2);
                            printf("➜ %ld OR %ld = %ld (0x%lX)\n", num, num2, res, res);
                            break;
                        case 3:
                            res = bitwise_xor(num, num2);
                            printf("➜ %ld XOR %ld = %ld (0x%lX)\n", num, num2, res, res);
                            break;
                        default:
                            printf("Invalid choice\n");
                    }
                }
                break;
                
            case 3: /* Bit Shifting */
                printf("\n1. Left Shift  2. Right Shift\n");
                printf("Select: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                sub_choice = atoi(input);
                
                printf("Enter number: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                num = atol(input);
                
                printf("Enter shift amount: ");
                if (!fgets(input, sizeof(input), stdin)) break;
                shift_amount = atoi(input);
                
                long res;
                if (sub_choice == 1) {
                    res = left_shift(num, shift_amount);
                    printf("➜ %ld << %d = %ld (0x%lX)\n", num, shift_amount, res, res);
                } else if (sub_choice == 2) {
                    res = right_shift(num, shift_amount);
                    printf("➜ %ld >> %d = %ld (0x%lX)\n", num, shift_amount, res, res);
                } else {
                    printf("Invalid choice\n");
                }
                break;
                
            default:
                printf("Invalid choice!\n");
        }
    }
}
