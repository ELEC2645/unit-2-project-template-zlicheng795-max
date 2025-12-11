#ifndef FUNCS_H
#define FUNCS_H

#include <math.h>

/* Constants */
#define MAX_EXPR_LENGTH 256
#define MAX_STACK_SIZE 100
#define MAX_HISTORY 10
#define PI M_PI
#define E M_E

/* Angle mode for trigonometric functions */
typedef enum {
    RADIANS,
    DEGREES
} AngleMode;

/* Operation types for expression evaluation */
typedef enum {
    OP_ADD = '+',
    OP_SUB = '-',
    OP_MUL = '*',
    OP_DIV = '/',
    OP_POW = '^',
    OP_MOD = '%'
} Operator;

/* Stack structure for expression evaluation */
typedef struct {
    double items[MAX_STACK_SIZE];
    int top;
} Stack;

/* Calculator state */
typedef struct {
    AngleMode angle_mode;
    double memory;
    double last_result;
    char history[MAX_HISTORY][MAX_EXPR_LENGTH];
    int history_count;
} CalculatorState;

/* Menu item functions */
void menu_item_1(void);  /* Basic Calculator */
void menu_item_2(void);  /* Scientific Functions */
void menu_item_3(void);  /* Unit Converter */
void menu_item_4(void);  /* Programmer Mode */

/* Stack operations */
void stack_init(Stack *s);
int stack_is_empty(Stack *s);
int stack_is_full(Stack *s);
void stack_push(Stack *s, double value);
double stack_pop(Stack *s);
double stack_peek(Stack *s);

/* Expression evaluation */
double evaluate_expression(const char *expr, CalculatorState *state, char *error);
int is_operator(char c);
int get_precedence(char op);
int is_left_associative(char op);
double apply_operator(char op, double a, double b, char *error);

/* Basic arithmetic operations */
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double base, double exponent);
double modulo(double a, double b);

/* Scientific functions */
double factorial(int n);
double sine(double x, AngleMode mode);
double cosine(double x, AngleMode mode);
double tangent(double x, AngleMode mode);
double arcsine(double x, AngleMode mode);
double arccosine(double x, AngleMode mode);
double arctangent(double x, AngleMode mode);
double logarithm_base10(double x);
double logarithm_natural(double x);
double logarithm_base(double x, double base);
double exponential(double x);
double square_root(double x);
double cube_root(double x);
double nth_root(double x, double n);
double absolute_value(double x);

/* Unit conversion functions */
double celsius_to_fahrenheit(double c);
double fahrenheit_to_celsius(double f);
double celsius_to_kelvin(double c);
double kelvin_to_celsius(double k);
double meters_to_feet(double m);
double feet_to_meters(double f);
double kilometers_to_miles(double km);
double miles_to_kilometers(double mi);
double kilograms_to_pounds(double kg);
double pounds_to_kilograms(double lb);
double degrees_to_radians(double deg);
double radians_to_degrees(double rad);

/* Programmer mode functions */
void decimal_to_binary(long num, char *result);
void decimal_to_octal(long num, char *result);
void decimal_to_hexadecimal(long num, char *result);
long binary_to_decimal(const char *bin);
long octal_to_decimal(const char *oct);
long hexadecimal_to_decimal(const char *hex);
long bitwise_and(long a, long b);
long bitwise_or(long a, long b);
long bitwise_xor(long a, long b);
long bitwise_not(long a);
long left_shift(long num, int shift);
long right_shift(long num, int shift);

/* Helper functions */
void print_scientific_menu(void);
void print_unit_converter_menu(void);
void print_programmer_menu(void);
double to_radians(double degrees);
double to_degrees(double radians);
void add_to_history(CalculatorState *state, const char *expr, double result);
void show_history(CalculatorState *state);
void clear_history(CalculatorState *state);
int is_valid_number(const char *str);
void format_result(double result, char *buffer, size_t size);

/* Calculator state management */
void init_calculator_state(CalculatorState *state);
void set_angle_mode(CalculatorState *state, AngleMode mode);
void store_memory(CalculatorState *state, double value);
double recall_memory(CalculatorState *state);
void clear_memory(CalculatorState *state);

#endif
