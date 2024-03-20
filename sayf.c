#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>

void sayf(int delay,const char* restrict Format, ...) {
    char* Formatted_str = (char*) calloc(strlen(Format) + 1, sizeof(char));
    size_t size = strlen(Format) + 1;

    // Launching Argument list
    va_list args;
    va_start(args, Format);

    int k = 0; // Progress on final string
    for (int i = 0; i < strlen(Format); i++) {

        if (Format[i] != '\\') {

            if (Format[i] == '%') {

                switch (Format[i+1]) {

                    case 'd': {
                        int arg_int = va_arg(args, int);

                        // Getting int in string
                        char arg_int_str[16];
                        sprintf(arg_int_str, "%d", arg_int);

                        // Reallocating memory
                        size += strlen(arg_int_str) + 1;
                        Formatted_str = (char*) realloc(Formatted_str, size);

                        // Adding to the string
                        strcat(Formatted_str, arg_int_str);
                        k += strlen(arg_int_str);
                        memset(Formatted_str+k,0,strlen(arg_int_str)+1);
                    
                        i++;
                        break;
                    }
                    
                    case 'f': {
                        double arg_double = va_arg(args, double);

                        // Getting double in string
                        char arg_double_str[16];
                        sprintf(arg_double_str, "%f", arg_double);

                        // Reallocating memory
                        size += strlen(arg_double_str) + 1;
                        Formatted_str = (char*) realloc(Formatted_str, size);

                        // Adding to the string
                        strcat(Formatted_str, arg_double_str);
                        k += strlen(arg_double_str);
                        memset(Formatted_str+k,0,strlen(arg_double_str)+1);
                        
                    
                        i++;
                        break;
                    }

                    case 'c': {
                        char arg_char = va_arg(args, int);

                        // Reallocating memory
                        size += 2;
                        Formatted_str = (char*) realloc(Formatted_str, size);

                        // Adding to the string
                        Formatted_str[k++] = arg_char;
                        memset(Formatted_str+k,0,2);
                        
                    
                        i++;
                        break;
                    }

                    case 's': {
                        char* arg_str = va_arg(args, char*);

                        // Reallocating memory
                        size += strlen(arg_str) + 1;
                        Formatted_str = (char*) realloc(Formatted_str, size);

                        // Adding to the string
                        strcat(Formatted_str, arg_str);
                        k += strlen(arg_str);
                        memset(Formatted_str+k,0,strlen(arg_str)+1);
                        
                    
                        i++;
                        break;
                    }

                    default: {
                        Formatted_str[k++] = Format[i];
                        Formatted_str[k++] = Format[i + 1];
                        i++;
                        break;
                    }

                }
            } else {
                Formatted_str[k++] = Format[i];
            }

        } else {
            Formatted_str[k++] = Format[i];
        }

    }

    Formatted_str[k] = '\0';

    for (int i=0; Formatted_str[i] != '\0'; i++) {
        printf("%c",Formatted_str[i]);
        Sleep(delay);
    }


    // Ending the argument list
    va_end(args);
    free(Formatted_str);
}
