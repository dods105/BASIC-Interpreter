#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <fstream>
#include <sstream>
#include <variant>


// ================= TOKEN =================
enum class TokenType {
    NUMBER, STRING, IDENTIFIER,
    PLUS, MINUS, STAR, SLASH,
    EQUAL, LPAREN, RPAREN,
    PRINT, IF, ELSE, END, WHILE, FOR, TO,
    GREATER, LESS, NOTEQ, STRICTEQ, INPUT,
    LESSEQ, GREATEQ, LBRACKET, RBRACKET,
    EOF_TOK
};

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

struct Token {
    TokenType type;
    std::string value;
};

// ================= LEXER =================
class Lexer {
    std::string text;
    size_t pos = 0;
    char current;

public:
    Lexer(const std::string& src) : text(src) {
        current = pos < text.size() ? text[pos] : '\0';
    }

    void advance() {
        pos++;
        current = pos < text.size() ? text[pos] : '\0';
    }

    void skipWhitespace() {
        while (isspace(current)) advance();
    }

    Token number() {
        std::string r;
        while (isdigit(current)){
            r += current;
            advance();
        }
        return {TokenType::NUMBER, r};
    }

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

    Token getNextToken() {
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
        return {TokenType::EOF_TOK,""};
    }
};
