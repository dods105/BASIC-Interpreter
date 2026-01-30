#include "lexer.h"

struct AST { virtual ~AST() = default; };

struct NumberNode : AST {
    double v;
    NumberNode(double x):v(x){}
};
struct StringNode : AST {
    std::string v;
    StringNode(std::string x): v(x) {}
};
struct VarNode : AST {
    std::string n;
    VarNode(std::string x):n(x){}
};
struct AssignNode : AST {
    std::string n;
    AST* v;
    AssignNode(std::string a,AST*b):n(a),v(b){}
};
struct BinOpNode : AST {
    AST*l;Token o;
    AST*r;
    BinOpNode(AST*a,Token b,AST*c):l(a),o(b),r(c){}
};
struct PrintNode : AST {
    AST* e; PrintNode(AST*x) : e(x){}
};
struct BlockNode : AST {
    std::vector<AST*> s;
    BlockNode(std::vector<AST*>x):s(x){}
};
struct IfNode : AST {
    AST*c,*t,*e;
    IfNode(AST*a,AST*b,AST*d=nullptr):c(a),t(b),e(d){}
};
struct WhileNode : AST {
    AST*c,*b;
    WhileNode(AST*a,AST*d):c(a),b(d){}
};
struct ForNode : AST {
    std::string v;
    AST*start,*end,*body;
    ForNode(std::string a,AST*b,AST*c,AST*d):v(a),start(b),end(c),body(d){}
};
struct InputNode : AST{
    AST* text;

    InputNode(AST* str) : text(str) {}
};

struct ArrayAccessNode : AST {
    std::string name;
    AST *index;

    ArrayAccessNode(std::string n, AST* i) : name(n), index(i) {}
};

struct ArrayAssignNode : AST {
    std::string name;
    AST *index;
    AST *value;
    ArrayAssignNode(std::string n, AST* i, AST* v) : name(n), index(i), value(v) {}
};

// ================= PARSER =================
class Parser {
    Lexer& l;
    Token cur;

public:
    Parser(Lexer& x):l(x){
        cur=l.getNextToken();
    }

    void eat(TokenType t){
        if(cur.type==t){
            cur=l.getNextToken();
        }
        else{
            std::string error = "Unexpected Token: " + tokenNames(cur.type) + " Value: " + cur.value;
            throw std::runtime_error(error);
        }
    }

    AST* factor(){
        if (cur.type == TokenType::INPUT) {
            std::cout << "Input\n";
            eat(TokenType::INPUT);
            return new InputNode(expr());
        }
        if(cur.type==TokenType::NUMBER){
            auto v=cur.value;
            eat(TokenType::NUMBER);
            return new NumberNode(std::stod(v));
        }
        if(cur.type==TokenType::STRING){
            auto v = cur.value;
            eat(TokenType::STRING);
            return new StringNode(v);
        }
        if(cur.type==TokenType::IDENTIFIER){
            auto name = cur.value;
            eat(TokenType::IDENTIFIER);

            if(cur.type == TokenType::LBRACKET){
                std::cout << "Accessing. LBRACKET prob\n";
                eat(TokenType::LBRACKET);
                AST *index = expr();
                eat(TokenType::RBRACKET);
                return new ArrayAccessNode(name, index);
            }

            return new VarNode(name);
        }
        if(cur.type==TokenType::LPAREN){
            eat(TokenType::LPAREN);
            auto n=expr();
            eat(TokenType::RPAREN);
            return n;
        }
        std::string exp = "TokenType " + tokenNames(cur.type);
        throw std::runtime_error("Invalid factor " + exp);
    }

    AST* term(){
        auto n = factor();
        while(cur.type == TokenType::STAR||cur.type == TokenType::SLASH){
            auto o = cur; eat(cur.type);
            n = new BinOpNode(n,o,factor());
        }
        return n;
    }

    AST* expr(){
        auto n=term();
        while(cur.type==TokenType::PLUS||cur.type==TokenType::MINUS){
            auto o=cur; eat(cur.type);
            n=new BinOpNode(n,o,term());
        }
        return n;
    }

    AST* comparison(){
        auto n = expr();
        while(cur.type==TokenType::GREATER||cur.type==TokenType::LESS||cur.type==TokenType::STRICTEQ || cur.type == TokenType::NOTEQ
            || cur.type == TokenType::GREATEQ || cur.type == TokenType::LESSEQ
        ){
            auto o = cur;
            std::cout << "Cur.type is: " + tokenNames(cur.type);
            eat(cur.type);
            n=new BinOpNode(n,o,expr());
        }
        return n;
    }

    AST* statement(){
        if(cur.type==TokenType::PRINT){
            eat(TokenType::PRINT);
            return new PrintNode(expr());
        }

        if(cur.type==TokenType::IDENTIFIER){
            auto name = cur.value;

            eat(TokenType::IDENTIFIER);
            if(cur.type == TokenType::LBRACKET){
                std::cout << "Assigning. LBRACKET prob\n";
                eat(TokenType::LBRACKET);
                AST *index = expr();
                eat(TokenType::RBRACKET);
                eat(TokenType::EQUAL);

                return new ArrayAssignNode(name, index, expr());
            }

            eat(TokenType::EQUAL);
            std::cout << "Input is now Token\n";
            return new AssignNode(name,expr());
        }

        if(cur.type==TokenType::IF){
            std::cout << "If Problem\n";
            eat(TokenType::IF);
            auto c = comparison();
            auto t = block();
            AST* e = nullptr;
            if(cur.type == TokenType::ELSE){
                std::cout << "Else Problem\n";
                eat(TokenType::ELSE);
                e = block();
            }
            eat(TokenType::END);
            return new IfNode(c,t,e);
        }

        if(cur.type==TokenType::WHILE){
            std::cout << "While Problem\n";
            eat(TokenType::WHILE);
            auto c=comparison();
            auto b=block();
            std::cout << "End Problem\n";
            eat(TokenType::END);
            return new WhileNode(c,b);
        }

        if(cur.type==TokenType::FOR){
            eat(TokenType::FOR);
            auto v = cur.value; //identifier
            eat(TokenType::IDENTIFIER);
            eat(TokenType::EQUAL);
            auto s = expr(); //start value of loop
            eat(TokenType::TO);
            auto e = expr(); //end value of loop
            auto b = block();
            eat(TokenType::END);
            return new ForNode(v,s,e,b);
        }
        std::string error = "Invalid Statement. TokenType is: " + tokenNames(cur.type) + " Token Value is " + cur.value;
        throw std::runtime_error(error);
    }

    AST* block(){
        std::vector<AST*> r;
        while(cur.type!=TokenType::END && cur.type!=TokenType::ELSE && cur.type!=TokenType::EOF_TOK)
            r.push_back(statement());
        return new BlockNode(r);
    }

    AST* parse(){
        std::vector<AST*> r;
        while(cur.type!=TokenType::EOF_TOK){
            r.push_back(statement());
        }
        return new BlockNode(r);
    }
};
