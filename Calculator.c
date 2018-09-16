
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_EXPR_SIZE 255+1
#define MAX_STACK_SIZE 255+1

#define IS_NUM(c) ( c == 'P' || c == 'I' || c == 'E' || c == 'e' || c == '.' ||c >= '0' && c <= '9')

//
// Data Structure
/* Precedence */
typedef enum {lparen, rparen, // (, ),
    plus, minus, uni_minus , mult, divide, // +, -, _ , *, /,
    trans_sin, trans_cos, trans_tan, // sin, cos, tan
    trans_e, trans_ln, trans_fact, // e, ln, factorial, permutation, combination
    eos, operand
} precedence;
static int isp[] = {0, 19, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0}; // No include operand
static int icp[] = {20, 19, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 0}; // No include operand
//
// Function
void toSimpleInfix(); // infix->simple infix->postfix
void postfix();
precedence charToPrecedence(char c);
char precedenceToChar(precedence p);
double eval();
int factorial(int n);
/* Stack Double */
void pushDouble(double item);
double popDouble(void);
/* Stack Precedence */
void pushPrecedence(precedence item);
precedence popPrecedence(void);
//
// Global Variable
char infix_expr[MAX_EXPR_SIZE] = "-35.32-(-24.3+cos(-3*43))*7";
char postfix_expr[MAX_EXPR_SIZE] = {0,};
double stackDouble[MAX_STACK_SIZE];
int topDouble = -1;
precedence stackPrecedence[MAX_STACK_SIZE];
int topPrecedence = -1;


int main(void) {
    
    printf("수식 : ");
    scanf("%s",infix_expr);
    toSimpleInfix();
    postfix();
    
    
    printf("=%lf\n",eval());
}

//
// Function
void toSimpleInfix() {
    char expr[MAX_EXPR_SIZE];
    int infix_index = 0, n = 0, len = (int)strlen(infix_expr);
    int fact_cnt = 0, i;
    
    while (infix_index < len ) {
        switch (infix_expr[infix_index]) {
            case '-':
                if (infix_index == 0 || infix_expr[infix_index-1] == '(') {
                    expr[n++] = '_';
                    infix_index ++;
                } else {
                    expr[n++] = '-';
                    infix_index ++;
                }
                break;
            case 's': // sin()
                expr[n++] = 's';
                infix_index += 3;
                break;
            case 'c':
                expr[n++] = 'c';
                infix_index += 3;
                break;
            case 't':
                expr[n++] = 't';
                infix_index += 3;
                break;
            case 'l': // ln()
                expr[n++] = 'l';
                infix_index += 2;
                break;
            case '!':
                if (infix_expr[infix_index-1] == ')') {
                    fprintf(stdout, "!앞에는 괄호가 올 수 없습니다. (자연수만 가능)\n");
                    exit(EXIT_FAILURE);
                }
                else {
                    fact_cnt = infix_index-1;
                    while (IS_NUM(infix_expr[fact_cnt])) fact_cnt--;
                    fact_cnt++;
                    n -= infix_index-fact_cnt;
                    expr[n++] = '!';
                    while ( infix_expr[fact_cnt] != '!' ) expr[n++] = infix_expr[fact_cnt++];
                    infix_index++;
                }
                break;
            case 'P':
                expr[n++] = '3';
                expr[n++] = '.';
                expr[n++] = '1';
                expr[n++] = '4';
                expr[n++] = '1';
                expr[n++] = '5';
                expr[n++] = '9';
                expr[n++] = '2';
                infix_index += 3;
                break;
            case 'e':
                expr[n++] = '2';
                expr[n++] = '.';
                expr[n++] = '7';
                expr[n++] = '1';
                expr[n++] = '8';
                expr[n++] = '2';
                expr[n++] = '8';
                expr[n++] = '1';
                infix_index ++;
                break;
            default:
                expr[n++] = infix_expr[infix_index++];
                break;
        }
    }
    expr[n] = '\0';
    
    len = strlen(expr);
    for (i = 0 ;i < len+1 ;i ++) infix_expr[i] = expr[i];
    
}
void postfix() {
    int infix_index = 0, postfix_index = 0;
    precedence token;
    
    stackPrecedence[0] = eos;
    topPrecedence = 0;
    
    token = charToPrecedence(infix_expr[infix_index]);
    while ( token != eos ) {
        if (token == operand) {
            postfix_expr[postfix_index++] = infix_expr[infix_index++];
            while ( IS_NUM(infix_expr[infix_index]) ) postfix_expr[postfix_index++] = infix_expr[infix_index++];
            postfix_expr[postfix_index++] = ' ';
        } else if (token == rparen) {
            while (stackPrecedence[topPrecedence] != lparen)
                postfix_expr[postfix_index++] = precedenceToChar(popPrecedence());
            popPrecedence();
            infix_index++;
        } else {
            while ( isp[stackPrecedence[topPrecedence]] >= icp[token] ) {
                postfix_expr[postfix_index++] = precedenceToChar(popPrecedence());
                // Check Point
                if (topPrecedence == -1) break;
            }
            pushPrecedence(token);
            infix_index++;
        }
        token = charToPrecedence(infix_expr[infix_index]);
    }
    
    while (topPrecedence != -1 && ((token = popPrecedence()) != eos))
        postfix_expr[postfix_index++] = precedenceToChar(token);
    
    
}
precedence charToPrecedence(char c) {
    switch (c) {
        case '(': return lparen;
        case ')': return rparen;
        case '+': return plus;
        case '-': return minus;
        case '_': return uni_minus;
        case '*': return mult;
        case '/': return divide;
        case 's': return trans_sin;
        case 'c': return trans_cos;
        case 't': return trans_tan;
        case '^': return trans_e;
        case 'l': return trans_ln;
        case '!': return trans_fact;
        case '\0': return eos;
        default: return operand;
    }
}
char precedenceToChar(precedence p) {
    switch (p) {
        case plus: return '+';
        case uni_minus: return '_';
        case minus: return '-';
        case mult: return '*';
        case divide: return '/';
        case trans_sin: return 's';
        case trans_cos: return 'c';
        case trans_tan: return 't';
        case trans_e: return '^';
        case trans_fact: return '!';
        case trans_ln: return 'l';
        case eos: return '\0';
        default: return 0;
    }
}
double eval() {
    int i, j, tmp;
    char strbuf[MAX_EXPR_SIZE] = {0,};
    precedence token;
    double op1, op2;
    
    for (i = 0 ; postfix_expr[i] != '\0' ;i++) {
        token = charToPrecedence(postfix_expr[i]);
        switch (token) {
            case operand:
                for (j = 0 ; postfix_expr[i] != ' '; j++, i++)
                    strbuf[j] = postfix_expr[i];
                strbuf[j] = '\0';
                sscanf(strbuf, "%lf",&op1);
                pushDouble(op1);
                break;
            case plus:
                op2 = popDouble();
                op1 = popDouble();
                pushDouble(op1+op2);
                break;
            case minus:
                op2 = popDouble();
                op1 = popDouble();
                pushDouble(op1-op2);
                break;
            case uni_minus:
                op1 = popDouble();
                pushDouble(-1*op1);
                break;
            case mult:
                op2 = popDouble();
                op1 = popDouble();
                pushDouble(op1*op2);
                break;
            case divide:
                op2 = popDouble();
                op1 = popDouble();
                pushDouble(op1/op2);
                break;
            case trans_sin:
                op1 = popDouble();
                pushDouble(sin(op1));
                break;
            case trans_cos:
                op1 = popDouble();
                pushDouble(cos(op1));
                break;
            case trans_tan:
                op1 = popDouble();
                pushDouble(tan(op1));
                break;
            case trans_e:
                op2 = popDouble();
                op1 = popDouble();
                pushDouble(pow(op1, op2));
                break;
            case trans_ln:
                op1 = popDouble();
                pushDouble(log(op1));
                break;
            case trans_fact:
                op1 = popDouble();
                tmp = (int)op1;
                pushDouble((double)factorial(tmp));
                break;
        } // End of Switch
    }
    return popDouble();
}
int factorial(int n) {
    int i = 1;
    
    if (n == 0 || n == 1) return 1;
    else while (n > 0) i *= n--;
    
    return i;
}
/* Stack Double */
void pushDouble(double item) {
    if (topDouble == MAX_STACK_SIZE-1) {
        fprintf(stderr, "---Double push error---\n");
        exit(EXIT_FAILURE);
    }
    stackDouble[++topDouble] = item;
}
double popDouble() {
    if (topDouble == -1) {
        fprintf(stderr, "---Double pop error---\n");
        exit(EXIT_FAILURE);
    }
    return stackDouble[topDouble--];
}
/* Stack Precedence */
void pushPrecedence(precedence item) {
    if (topPrecedence == MAX_STACK_SIZE-1) {
        fprintf(stderr, "---Precedence push error---\n");
        exit(EXIT_FAILURE);
    }
    stackPrecedence[++topPrecedence] = item;
}
precedence popPrecedence() {
    if (topPrecedence == -1) {
        fprintf(stderr, "---Precedence pop error---\n");
        exit(EXIT_FAILURE);
    }
    return stackPrecedence[topPrecedence--];
}
