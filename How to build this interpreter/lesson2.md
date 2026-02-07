# Lesson 2

## The Lexer (Tokenizer)

### Covered in this lesson:

-   What tokens are
-   Token types
-   Building a simple lexer in C++

------------------------------------------------------------------------

## What is a TOKEN?

Before the interpreter understands the code, it breaks text into pieces called tokens.
This process is called **lexical analysis** or **tokenizing**.

**Input:**

    x = 5 + 3

**Your interpreter sees:**

    [x] [=] [5] [+] [3]

These things are called **TOKENS**.

------------------------------------------------------------------------

## What is a Token Type?

| Token | Type |
| --- | --- |
| X   | IDENTIFIER |
| \=  | EQUAL |
| 5   | NUMBER |
| +   | PLUS |
| 3   | NUMBER |

------------------------------------------------------------------------

## Building a Lexer

### Libraries

These are the libraries need for building this lexer. Keep in mind that libraries will be added as the lesson progress.

``` cpp
#include <string>
#include <iostream>
#include <vector>
#include <cctype>
#include <unordered_set>
```

------------------------------------------------------------------------

## Step 1 -- Token Types

Think of the token types you want to include in your lexer. These are the things that you want you want to include in your interpreter.


Do you want a for loop? then add a __for__ token type.

``` cpp

enum class TokenType{
    NUMBER, STRING, IDENTIFIER,
    PLUS, MINUS, STAR, SLASH,
    EQUAL, LPAREN, RPAREN,
    PRINT, IF, ELSE, END, WHILE, FOR, TO,
    GREATER, LESS, NOTEQ, STRICTEQ, INPUT,
    LESSEQ, GREATEQ, LBRACKET, RBRACKET,
    EOF_TOK
};

```

------------------------------------------------------------------------

## Step 2 -- Token Structure

Define the Token Structure
Each token has a type and value:


``` cpp

struct Token{
    TokenType type;
    std::string value;
};

```

------------------------------------------------------------------------

## Step 3 -- Lexer Class

The Lexer Class - The lexer walks through your source code. This is what makes the tokens.

``` cpp

class Lexer{
    std::string text;
    int pos = 0;
    char current;

public:
    Lexer(std::string src): text(src){
        current = text[pos];
    }

    void advance();
    void skipWhiteSpace();
    Token number();
    Token string();
    Token identifier();
    Token getNextToken();

};
```
----------------------------------------------------------------------
### Functions Explanations


__advance()__ – is called everytime the current position is to be updated.


```cpp
    void Lexer::advance(){
        pos++;
        //if the end of the string is reached, make the current =  \0
        if(pos >= text.size()) current = '\0';
        else current = text[pos]; //if not, move to the next character
    }
```


__skipWhiteSpace()__ – tells the lexer to not read spaces.


```cpp

    void Lexer::skipWhiteSpace(){
        while(current == ' ') advance();
    }

```


__number()__ – is of Token type function which returns the type of the token and the value of it. This considers both integer and floating numbers.

```cpp

    Token Lexer::number(){
        std::string result;
        bool dotUsed = false;

        while(isdigit(current) || current == '.' && !dotUsed){
            if(current == '.'){
                dotUsed = true;
            }
            result += current;
            advance();
        }

        return {TokenType::NUMBER, result};
    }
```


__string()__ - this returns a string token type. this is called when the lexer encountes a double quotation and will only stop reading the _string_ once it encounters another double quotation.

```
    PRINT "Hello"

    //this will return the string "HELLO" along side the token type string.
```


```cpp

    Token string() {
        advance();
        std::string r;
        while (current && current != '"') {
            r += current;
            advance();
        }
        advance();
        return {TokenType::STRING, r};
    }

```



__Identifier()__ – returns a Token type. The function is used everytime the lexer begins to read a letter. This does not read whitespaces but reads both letters and numbers. This is also where it determines whether if it should return a token type of identifier or a keyword.

```
    Ex: Who (W->h->o), so now the result = “Who”
    Ex: x1 (x->1), result = “x1”
```

I know there is a number in the x1. Why is 1 not classified as a number?

Like I said, the function is called everytime it encounters a letter and ends when it sees a whitespace. So x1 is different from x 1


```cpp
 Token identifier() {
        std::string r;
        while (isalnum(current)) {
            std::cout << current << " Identifier" << "\n";
            r += current;
            advance();
        }

        if (r == "PRINT") return {TokenType::PRINT, r};
        if (r == "IF") return {TokenType::IF, r};
        if (r == "ELSE") return {TokenType::ELSE, r};
        if (r == "END") return {TokenType::END, r};
        if (r == "WHILE") return {TokenType::WHILE, r};
        if (r == "FOR") return {TokenType::FOR, r};
        if (r == "TO") return {TokenType::TO, r};
        if (r == "INPUT") return { TokenType::INPUT, r };

        return {TokenType::IDENTIFIER, r};
    }
```



__getNextToken()__ – this is where the lexer begins to walk through your source code. This is cliché but, this is the heart of your Lexer Class. This is where your code is read and where the rest of the functions are called.


If you would look at it, you would notice that we begin with a while loop. This while loop will continue running as long as it has not reach the end of the string.


```cpp
    while (current) {
            if (isspace(current)){
                skipWhitespace();
                continue;
            }
            if (isdigit(current)) return number();
            if (isalpha(current)) return identifier();
            if (current == '"') return string();

            if(current == '['){
                std::cout << current << " Left" << "\n";
                advance();
                return {TokenType::LBRACKET, "["};
            }
            if(current == ']'){
                advance();
                return {TokenType::RBRACKET, "]"};
            }

            if (current == '+') {
                advance();
                return {TokenType::PLUS,"+"};
            }
            if (current == '-') {
                advance();
                return {TokenType::MINUS,"-"};
            }
            if (current == '*') {
                advance();
                return {TokenType::STAR,"*"};
            }
            if (current == '/') {
                advance();
                if (current == '/'){
                    advance();
                    std::string com;
                    while (current && current != '/'){
                        com += current;
                        advance();
                    }
                    advance();
                    continue;
                }
                return {TokenType::SLASH,"/"};
            }
            if (current == '(') {
                advance();
                return {TokenType::LPAREN,"("};
            }
            if (current == ')') {
                advance();
                return {TokenType::RPAREN,")"};
            }

            if (current == '=') {
                advance();
                if (current == '=') {
                    advance();
                    return {TokenType::STRICTEQ,"=="}; }
                return {TokenType::EQUAL,"="};
            }

            if (current == '>') {
                advance();
                if (current == '='){
                    advance();
                    return {TokenType::GREATEQ, ">="};

                }
                return {TokenType::GREATER,">"};
            }
            if (current == '<') {
                advance();
                if (current == '='){
                    advance();
                    return {TokenType::LESSEQ, "<="};

                }
                return {TokenType::LESS,"<"};
            }

            if (current == '!') {
                advance();
                advance();
                return {TokenType::NOTEQ,"!="};
            }

            throw std::runtime_error("Invalid character");
        }

```


(optional)
You can add this to get your token type, if you want. This is also helpful if you are gonna debug. This is a function separate from the Lexer class.
//token name getter

```cpp
    std::string tokenNames(TokenType t){
        switch (t)
        {
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::STRING: return "STRING";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::PLUS: return "PLUS";
            case TokenType::MINUS: return "MINUS";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::STAR: return "STAR";
            case TokenType::SLASH: return "SLASH";
            case TokenType::PRINT: return "PRINT";
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::IF: return "IF";
            case TokenType::ELSE: return "ELSE";
            case TokenType::WHILE: return "WHILE";
            case TokenType::FOR: return "FOR";
            case TokenType::TO: return "TO";
            case TokenType::GREATER: return "GREATER";
            case TokenType::LESS: return "LESS";
            case TokenType::NOTEQ: return "NOTEQ";
            case TokenType::STRICTEQ: return "STRICTEQ";
            case TokenType::LBRACKET: return "LBRACKET";
            case TokenType::RBRACKET: return "RBRACKET";
            case TokenType::END: return "END";
            default:
                return "UNKNOWN";
        }
    }

```