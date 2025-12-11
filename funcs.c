#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "funcs.h"

/* ========== Helper Functions ========== */

/* Get the name of a color from its code */
const char* get_color_name(ColorCode color) {
    switch(color) {
        case BLACK:   return "Black";
        case BROWN:   return "Brown";
        case RED:     return "Red";
        case ORANGE:  return "Orange";
        case YELLOW:  return "Yellow";
        case GREEN:   return "Green";
        case BLUE:    return "Blue";
        case VIOLET:  return "Violet";
        case GREY:    return "Grey";
        case WHITE:   return "White";
        case GOLD:    return "Gold";
        case SILVER:  return "Silver";
        case NONE:    return "None";
        default:      return "Invalid";
    }
}

/* Convert user input string to ColorCode */
ColorCode get_color_from_input(const char* input) {
    char lower_input[50];
    int i;
    
    /* Convert to lowercase for comparison */
    for(i = 0; input[i] && i < 49; i++) {
        lower_input[i] = tolower(input[i]);
    }
    lower_input[i] = '\0';
    
    if(strcmp(lower_input, "black") == 0) return BLACK;
    if(strcmp(lower_input, "brown") == 0) return BROWN;
    if(strcmp(lower_input, "red") == 0) return RED;
    if(strcmp(lower_input, "orange") == 0) return ORANGE;
    if(strcmp(lower_input, "yellow") == 0) return YELLOW;
    if(strcmp(lower_input, "green") == 0) return GREEN;
    if(strcmp(lower_input, "blue") == 0) return BLUE;
    if(strcmp(lower_input, "violet") == 0) return VIOLET;
    if(strcmp(lower_input, "grey") == 0 || strcmp(lower_input, "gray") == 0) return GREY;
    if(strcmp(lower_input, "white") == 0) return WHITE;
    if(strcmp(lower_input, "gold") == 0) return GOLD;
    if(strcmp(lower_input, "silver") == 0) return SILVER;
    if(strcmp(lower_input, "none") == 0) return NONE;
    
    return INVALID_COLOR;
}

/* Get the digit value for a color (0-9) */
int get_digit_value(ColorCode color) {
    if(color >= BLACK && color <= WHITE) {
        return (int)color;
    }
    return -1;  /* Invalid for digit */
}

/* Get the multiplier for a color */
double get_multiplier(ColorCode color) {
    switch(color) {
        case BLACK:   return 1.0;
        case BROWN:   return 10.0;
        case RED:     return 100.0;
        case ORANGE:  return 1000.0;
        case YELLOW:  return 10000.0;
        case GREEN:   return 100000.0;
        case BLUE:    return 1000000.0;
        case VIOLET:  return 10000000.0;
        case GREY:    return 100000000.0;
        case WHITE:   return 1000000000.0;
        case GOLD:    return 0.1;
        case SILVER:  return 0.01;
        default:      return -1.0;  /* Invalid */
    }
}

/* Get the tolerance percentage for a color */
double get_tolerance(ColorCode color) {
    switch(color) {
        case BROWN:   return 1.0;
        case RED:     return 2.0;
        case GREEN:   return 0.5;
        case BLUE:    return 0.25;
        case VIOLET:  return 0.1;
        case GREY:    return 0.05;
        case GOLD:    return 5.0;
        case SILVER:  return 10.0;
        case NONE:    return 20.0;
        default:      return -1.0;  /* Invalid */
    }
}

/* Get temperature coefficient in ppm/K */
int get_temp_coefficient(ColorCode color) {
    switch(color) {
        case BROWN:   return 100;
        case RED:     return 50;
        case ORANGE:  return 15;
        case YELLOW:  return 25;
        case BLUE:    return 10;
        case VIOLET:  return 5;
        default:      return -1;  /* Invalid */
    }
}

/* Format resistance value with appropriate unit prefix */
void format_resistance(double resistance, char* buffer, size_t size) {
    if(resistance >= 1e9) {
        snprintf(buffer, size, "%.2f GΩ", resistance / 1e9);
    } else if(resistance >= 1e6) {
        snprintf(buffer, size, "%.2f MΩ", resistance / 1e6);
    } else if(resistance >= 1e3) {
        snprintf(buffer, size, "%.2f kΩ", resistance / 1e3);
    } else {
        snprintf(buffer, size, "%.2f Ω", resistance);
    }
}

/* Validate if a color is valid for a specific band position */
int validate_color_for_band(ColorCode color, int band_number, int total_bands) {
    /* First digit bands cannot be black */
    if(band_number == 1 && color == BLACK) {
        return 0;
    }
    
    /* Digit bands must be 0-9 colors */
    if(band_number <= (total_bands - 2) && (color < BLACK || color > WHITE)) {
        return 0;
    }
    
    /* Multiplier band */
    if(band_number == (total_bands - 1)) {
        return (get_multiplier(color) >= 0);
    }
    
    /* Tolerance band */
    if(band_number == total_bands && total_bands <= 5) {
        return (get_tolerance(color) >= 0);
    }
    
    /* Temperature coefficient band (6-band only) */
    if(band_number == total_bands && total_bands == 6) {
        return (get_temp_coefficient(color) >= 0);
    }
    
    return 1;
}

/* ========== Decoder Functions ========== */

/* Decode 4-band resistor */
ResistorInfo decode_4band_resistor(ColorCode band1, ColorCode band2, 
                                    ColorCode multiplier, ColorCode tolerance) {
    ResistorInfo info;
    int digit1, digit2;
    double mult;
    
    info.num_bands = 4;
    
    digit1 = get_digit_value(band1);
    digit2 = get_digit_value(band2);
    mult = get_multiplier(multiplier);
    info.tolerance = get_tolerance(tolerance);
    info.temp_coefficient = 0;  /* Not applicable for 4-band */
    
    if(digit1 < 0 || digit2 < 0 || mult < 0 || info.tolerance < 0) {
        info.resistance = -1;  /* Invalid combination */
        return info;
    }
    
    info.resistance = (digit1 * 10 + digit2) * mult;
    return info;
}

/* Decode 5-band resistor */
ResistorInfo decode_5band_resistor(ColorCode band1, ColorCode band2, ColorCode band3,
                                    ColorCode multiplier, ColorCode tolerance) {
    ResistorInfo info;
    int digit1, digit2, digit3;
    double mult;
    
    info.num_bands = 5;
    
    digit1 = get_digit_value(band1);
    digit2 = get_digit_value(band2);
    digit3 = get_digit_value(band3);
    mult = get_multiplier(multiplier);
    info.tolerance = get_tolerance(tolerance);
    info.temp_coefficient = 0;  /* Not applicable for 5-band */
    
    if(digit1 < 0 || digit2 < 0 || digit3 < 0 || mult < 0 || info.tolerance < 0) {
        info.resistance = -1;  /* Invalid combination */
        return info;
    }
    
    info.resistance = (digit1 * 100 + digit2 * 10 + digit3) * mult;
    return info;
}

/* Decode 6-band resistor */
ResistorInfo decode_6band_resistor(ColorCode band1, ColorCode band2, ColorCode band3,
                                    ColorCode multiplier, ColorCode tolerance, 
                                    ColorCode temp_coeff) {
    ResistorInfo info;
    int digit1, digit2, digit3;
    double mult;
    
    info.num_bands = 6;
    
    digit1 = get_digit_value(band1);
    digit2 = get_digit_value(band2);
    digit3 = get_digit_value(band3);
    mult = get_multiplier(multiplier);
    info.tolerance = get_tolerance(tolerance);
    info.temp_coefficient = get_temp_coefficient(temp_coeff);
    
    if(digit1 < 0 || digit2 < 0 || digit3 < 0 || mult < 0 || 
       info.tolerance < 0 || info.temp_coefficient < 0) {
        info.resistance = -1;  /* Invalid combination */
        return info;
    }
    
    info.resistance = (digit1 * 100 + digit2 * 10 + digit3) * mult;
    return info;
}

/* ========== Encoder Function ========== */

/* Convert resistance value to color bands */
void encode_resistance_to_colors(double resistance, double tolerance, int num_bands) {
    int significant_digits;
    double normalized;
    int multiplier_power;
    ColorCode digit_colors[3];
    ColorCode mult_color = INVALID_COLOR;
    ColorCode tol_color = INVALID_COLOR;
    int i;
    char res_buffer[50];
    
    printf("\n>> Resistance to Color Bands Converter\n");
    printf("========================================\n\n");
    
    if(resistance <= 0) {
        printf("Error: Resistance must be positive!\n");
        return;
    }
    
    /* Determine tolerance color */
    if(fabs(tolerance - 1.0) < 0.01) tol_color = BROWN;
    else if(fabs(tolerance - 2.0) < 0.01) tol_color = RED;
    else if(fabs(tolerance - 0.5) < 0.01) tol_color = GREEN;
    else if(fabs(tolerance - 0.25) < 0.01) tol_color = BLUE;
    else if(fabs(tolerance - 0.1) < 0.01) tol_color = VIOLET;
    else if(fabs(tolerance - 0.05) < 0.01) tol_color = GREY;
    else if(fabs(tolerance - 5.0) < 0.01) tol_color = GOLD;
    else if(fabs(tolerance - 10.0) < 0.01) tol_color = SILVER;
    else if(fabs(tolerance - 20.0) < 0.01) tol_color = NONE;
    
    if(tol_color == INVALID_COLOR) {
        printf("Error: Invalid tolerance value!\n");
        printf("Valid tolerances: 20%%, 10%%, 5%%, 2%%, 1%%, 0.5%%, 0.25%%, 0.1%%, 0.05%%\n");
        return;
    }
    
    /* Calculate multiplier power */
    multiplier_power = (int)floor(log10(resistance));
    
    if(num_bands == 4) {
        multiplier_power -= 1;  /* 2 significant digits */
    } else {
        multiplier_power -= 2;  /* 3 significant digits */
    }
    
    /* Normalize resistance */
    normalized = resistance / pow(10, multiplier_power);
    
    /* Extract digits */
    if(num_bands == 4) {
        significant_digits = (int)round(normalized);
        if(significant_digits < 10) significant_digits *= 10;
        
        digit_colors[0] = (ColorCode)(significant_digits / 10);
        digit_colors[1] = (ColorCode)(significant_digits % 10);
    } else {  /* 5 or 6 bands */
        significant_digits = (int)round(normalized);
        if(significant_digits < 100) significant_digits *= 10;
        
        digit_colors[0] = (ColorCode)(significant_digits / 100);
        digit_colors[1] = (ColorCode)((significant_digits / 10) % 10);
        digit_colors[2] = (ColorCode)(significant_digits % 10);
    }
    
    /* Determine multiplier color */
    switch(multiplier_power) {
        case -2: mult_color = SILVER; break;
        case -1: mult_color = GOLD; break;
        case 0:  mult_color = BLACK; break;
        case 1:  mult_color = BROWN; break;
        case 2:  mult_color = RED; break;
        case 3:  mult_color = ORANGE; break;
        case 4:  mult_color = YELLOW; break;
        case 5:  mult_color = GREEN; break;
        case 6:  mult_color = BLUE; break;
        case 7:  mult_color = VIOLET; break;
        case 8:  mult_color = GREY; break;
        case 9:  mult_color = WHITE; break;
        default: 
            printf("Error: Resistance value out of encodable range!\n");
            return;
    }
    
    /* Display results */
    format_resistance(resistance, res_buffer, sizeof(res_buffer));
    printf("Input: %s ± %.2f%%\n\n", res_buffer, tolerance);
    
    if(num_bands == 4) {
        printf("4-Band Color Code:\n");
        printf("------------------\n");
        printf("Band 1 (1st digit):  %s\n", get_color_name(digit_colors[0]));
        printf("Band 2 (2nd digit):  %s\n", get_color_name(digit_colors[1]));
        printf("Band 3 (multiplier): %s\n", get_color_name(mult_color));
        printf("Band 4 (tolerance):  %s\n", get_color_name(tol_color));
    } else {
        printf("%d-Band Color Code:\n", num_bands);
        printf("------------------\n");
        printf("Band 1 (1st digit):  %s\n", get_color_name(digit_colors[0]));
        printf("Band 2 (2nd digit):  %s\n", get_color_name(digit_colors[1]));
        printf("Band 3 (3rd digit):  %s\n", get_color_name(digit_colors[2]));
        printf("Band 4 (multiplier): %s\n", get_color_name(mult_color));
        printf("Band 5 (tolerance):  %s\n", get_color_name(tol_color));
        if(num_bands == 6) {
            printf("Band 6 (temp coef):  (Not calculated - input required)\n");
        }
    }
    printf("\n");
}

/* ========== I/O Helper Functions ========== */

/* Print resistor information */
void print_resistor_info(ResistorInfo info) {
    char res_buffer[50];
    
    if(info.resistance < 0) {
        printf("\n✗ Error: Invalid color combination!\n");
        return;
    }
    
    printf("\n✓ Decoded Resistor Information:\n");
    printf("================================\n");
    
    format_resistance(info.resistance, res_buffer, sizeof(res_buffer));
    printf("Resistance: %s\n", res_buffer);
    printf("Tolerance:  ±%.2f%%\n", info.tolerance);
    
    if(info.num_bands == 6 && info.temp_coefficient > 0) {
        printf("Temp Coef:  %d ppm/K\n", info.temp_coefficient);
    }
    
    /* Calculate and display resistance range */
    double min_resistance = info.resistance * (1 - info.tolerance / 100.0);
    double max_resistance = info.resistance * (1 + info.tolerance / 100.0);
    char min_buffer[50], max_buffer[50];
    
    format_resistance(min_resistance, min_buffer, sizeof(min_buffer));
    format_resistance(max_resistance, max_buffer, sizeof(max_buffer));
    
    printf("\nResistance Range:\n");
    printf("  Minimum: %s\n", min_buffer);
    printf("  Maximum: %s\n", max_buffer);
    printf("\n");
}

/* Print color reference table */
void print_color_table(void) {
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║            RESISTOR COLOR CODE REFERENCE TABLE            ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Color   │ Digit │ Multiplier │ Tolerance │ Temp Coef      ║\n");
    printf("╠═════════╪═══════╪════════════╪═══════════╪════════════════╣\n");
    printf("║ Black   │   0   │    ×1      │     -     │       -        ║\n");
    printf("║ Brown   │   1   │    ×10     │    ±1%%    │   100 ppm/K    ║\n");
    printf("║ Red     │   2   │    ×100    │    ±2%%    │    50 ppm/K    ║\n");
    printf("║ Orange  │   3   │    ×1K     │     -     │    15 ppm/K    ║\n");
    printf("║ Yellow  │   4   │    ×10K    │     -     │    25 ppm/K    ║\n");
    printf("║ Green   │   5   │    ×100K   │   ±0.5%%   │       -        ║\n");
    printf("║ Blue    │   6   │    ×1M     │  ±0.25%%   │    10 ppm/K    ║\n");
    printf("║ Violet  │   7   │    ×10M    │   ±0.1%%   │     5 ppm/K    ║\n");
    printf("║ Grey    │   8   │    ×100M   │  ±0.05%%   │       -        ║\n");
    printf("║ White   │   9   │    ×1G     │     -     │       -        ║\n");
    printf("║ Gold    │   -   │    ×0.1    │    ±5%%    │       -        ║\n");
    printf("║ Silver  │   -   │    ×0.01   │   ±10%%    │       -        ║\n");
    printf("║ None    │   -   │     -      │   ±20%%    │       -        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

/* Get color input from user with validation */
int get_color_input(const char* prompt, ColorCode* color, int band_num, int total_bands) {
    char input[50];
    
    do {
        printf("%s", prompt);
        if(!fgets(input, sizeof(input), stdin)) {
            return 0;  /* Input error */
        }
        
        /* Remove newline */
        input[strcspn(input, "\r\n")] = '\0';
        
        *color = get_color_from_input(input);
        
        if(*color == INVALID_COLOR) {
            printf("Invalid color! Please try again.\n");
        } else if(!validate_color_for_band(*color, band_num, total_bands)) {
            printf("Invalid color for this band position! Please try again.\n");
            *color = INVALID_COLOR;
        }
    } while(*color == INVALID_COLOR);
    
    return 1;
}

/* ========== Menu Item Functions ========== */

/* Menu Item 1: 4-Band Resistor Decoder */
void menu_item_1(void) {
    ColorCode band1, band2, multiplier, tolerance;
    ResistorInfo info;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              4-BAND RESISTOR COLOR DECODER                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    print_color_table();
    
    printf("Enter the color of each band:\n");
    printf("-----------------------------\n\n");
    
    if(!get_color_input("Band 1 (1st digit): ", &band1, 1, 4)) return;
    if(!get_color_input("Band 2 (2nd digit): ", &band2, 2, 4)) return;
    if(!get_color_input("Band 3 (multiplier): ", &multiplier, 3, 4)) return;
    if(!get_color_input("Band 4 (tolerance): ", &tolerance, 4, 4)) return;
    
    info = decode_4band_resistor(band1, band2, multiplier, tolerance);
    print_resistor_info(info);
}

/* Menu Item 2: 5-Band Resistor Decoder */
void menu_item_2(void) {
    ColorCode band1, band2, band3, multiplier, tolerance;
    ResistorInfo info;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              5-BAND RESISTOR COLOR DECODER                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    print_color_table();
    
    printf("Enter the color of each band:\n");
    printf("-----------------------------\n\n");
    
    if(!get_color_input("Band 1 (1st digit): ", &band1, 1, 5)) return;
    if(!get_color_input("Band 2 (2nd digit): ", &band2, 2, 5)) return;
    if(!get_color_input("Band 3 (3rd digit): ", &band3, 3, 5)) return;
    if(!get_color_input("Band 4 (multiplier): ", &multiplier, 4, 5)) return;
    if(!get_color_input("Band 5 (tolerance): ", &tolerance, 5, 5)) return;
    
    info = decode_5band_resistor(band1, band2, band3, multiplier, tolerance);
    print_resistor_info(info);
}

/* Menu Item 3: 6-Band Resistor Decoder */
void menu_item_3(void) {
    ColorCode band1, band2, band3, multiplier, tolerance, temp_coeff;
    ResistorInfo info;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              6-BAND RESISTOR COLOR DECODER                 ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    
    print_color_table();
    
    printf("Enter the color of each band:\n");
    printf("-----------------------------\n\n");
    
    if(!get_color_input("Band 1 (1st digit): ", &band1, 1, 6)) return;
    if(!get_color_input("Band 2 (2nd digit): ", &band2, 2, 6)) return;
    if(!get_color_input("Band 3 (3rd digit): ", &band3, 3, 6)) return;
    if(!get_color_input("Band 4 (multiplier): ", &multiplier, 4, 6)) return;
    if(!get_color_input("Band 5 (tolerance): ", &tolerance, 5, 6)) return;
    if(!get_color_input("Band 6 (temp coef): ", &temp_coeff, 6, 6)) return;
    
    info = decode_6band_resistor(band1, band2, band3, multiplier, tolerance, temp_coeff);
    print_resistor_info(info);
}

/* Menu Item 4: Resistance to Color Bands Converter */
void menu_item_4(void) {
    char input[100];
    double resistance, tolerance;
    int num_bands;
    
    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║         RESISTANCE TO COLOR BANDS CONVERTER                ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    printf("This tool converts a resistance value to color bands.\n\n");
    
    /* Get resistance value */
    printf("Enter resistance value in ohms (e.g., 4700, 10000): ");
    if(!fgets(input, sizeof(input), stdin)) return;
    resistance = atof(input);
    
    if(resistance <= 0) {
        printf("Error: Invalid resistance value!\n");
        return;
    }
    
    /* Get tolerance */
    printf("Enter tolerance (%%) [0.05, 0.1, 0.25, 0.5, 1, 2, 5, 10, 20]: ");
    if(!fgets(input, sizeof(input), stdin)) return;
    tolerance = atof(input);
    
    /* Get number of bands */
    printf("Enter number of bands (4 or 5): ");
    if(!fgets(input, sizeof(input), stdin)) return;
    num_bands = atoi(input);
    
    if(num_bands != 4 && num_bands != 5) {
        printf("Error: Number of bands must be 4 or 5!\n");
        return;
    }
    
    encode_resistance_to_colors(resistance, tolerance, num_bands);
}
