#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ANS_FILE "ANS.txt"  

void save_ans(double ans) {
    FILE *file = fopen(ANS_FILE, "w");
    if (file) {
        fprintf(file, "%.2f", ans);
        fclose(file);
    } else {
        printf("FILE ERROR\n");
    }
}

double load_ans() {
    double ans = 0;
    FILE *file = fopen(ANS_FILE, "r");
    if (file) {
        fscanf(file, "%lf", &ans);
        fclose(file);
    } else {
        printf("FILE ERROR => ANS = 0\n");
    }
    return ans;
}

void clear_ans() {
    if (remove(ANS_FILE) == 0) {
        printf("0\n");
    } else {
        printf("SYNTAX ERROR\n");
    }
}

int main(){
    double a, b, result;
    double ANS = load_ans();  
    char op;
    char expression[40];

    while (1) {
        printf("Enter an expression: ");
        fgets(expression, sizeof(expression), stdin);

        if (strncmp(expression, "EXIT", 4) == 0) {
            save_ans(ANS);  
            break;
        }

        if (strncmp(expression, "CLEAR", 5) == 0) {
            clear_ans();
            ANS = 0;  
            continue; 
        }
   
        if (strncmp(expression, "ANS", 3) == 0) {
            if (sscanf(expression, "ANS %c %lf", &op, &b) == 2) {
                a = ANS;  
            } else {
                printf("SYNTAX ERROR\n");
                continue;  
            }
        } else if (sscanf(expression, "%lf %c %lf", &a, &op, &b) != 3) {
            printf("SYNTAX ERROR\n");
            continue;  
        }
        
        switch(op) {
            case '+':
                result = a + b;
                printf("RESULT: %.2f\n", result);
                break;
            case '-':
                result = a - b;
                printf("RESULT: %.2f\n", result);
                break;
            case '*':
                result = a * b;
                printf("RESULT: %.2f\n", result);
                break;
            case '/':
                if (b != 0) {
                    result = a / b;
                    printf("RESULT: %.2f\n", result);
                } else {
                    printf("MATH ERROR\n");
                    continue;  
                }
                break;
            default:
                printf("SYNTAX ERROR\n");
                continue;  
        }
        ANS = result;
    }
    return 0;
}
