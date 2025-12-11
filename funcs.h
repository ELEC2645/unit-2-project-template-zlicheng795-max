#ifndef FUNCS_H
#define FUNCS_H

/* Color definitions for resistor bands */
typedef enum {
    BLACK = 0,
    BROWN,
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    VIOLET,
    GREY,
    WHITE,
    GOLD,
    SILVER,
    NONE,
    INVALID_COLOR
} ColorCode;

/* Temperature coefficient codes (6-band resistors) */
typedef enum {
    BROWN_TEMP = 100,
    RED_TEMP = 50,
    ORANGE_TEMP = 15,
    YELLOW_TEMP = 25,
    BLUE_TEMP = 10,
    VIOLET_TEMP = 5
} TempCoefficient;

/* Structure to hold resistor information */
typedef struct {
    double resistance;      /* Resistance value in ohms */
    double tolerance;       /* Tolerance percentage */
    int temp_coefficient;   /* Temperature coefficient (ppm/K) */
    int num_bands;         /* Number of color bands */
} ResistorInfo;

/* Menu item functions */
void menu_item_1(void);  /* 4-band resistor decoder */
void menu_item_2(void);  /* 5-band resistor decoder */
void menu_item_3(void);  /* 6-band resistor decoder */
void menu_item_4(void);  /* Resistance to color bands converter */

/* Helper functions */
const char* get_color_name(ColorCode color);
ColorCode get_color_from_input(const char* input);
int get_digit_value(ColorCode color);
double get_multiplier(ColorCode color);
double get_tolerance(ColorCode color);
int get_temp_coefficient(ColorCode color);
void format_resistance(double resistance, char* buffer, size_t size);
int validate_color_for_band(ColorCode color, int band_number, int total_bands);

/* Decoder functions */
ResistorInfo decode_4band_resistor(ColorCode band1, ColorCode band2, 
                                    ColorCode multiplier, ColorCode tolerance);
ResistorInfo decode_5band_resistor(ColorCode band1, ColorCode band2, ColorCode band3,
                                    ColorCode multiplier, ColorCode tolerance);
ResistorInfo decode_6band_resistor(ColorCode band1, ColorCode band2, ColorCode band3,
                                    ColorCode multiplier, ColorCode tolerance, 
                                    ColorCode temp_coeff);

/* Encoder function (resistance to colors) */
void encode_resistance_to_colors(double resistance, double tolerance, int num_bands);

/* Input/Output helper functions */
void print_resistor_info(ResistorInfo info);
void print_color_table(void);
int get_color_input(const char* prompt, ColorCode* color, int band_num, int total_bands);

#endif

