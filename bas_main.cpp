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
    GREATER, LESS, NOTEQ, STRICTEQ,
    EOF_TOK
};

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
                return {TokenType::GREATER,">"};
            }
            if (current == '<') { 
                advance(); 
                return {TokenType::LESS,"<"}; 
            }

            throw std::runtime_error("Invalid character");
        }
        return {TokenType::EOF_TOK,""};
    }
};

// ================= AST =================
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
    ForNode(std::string a,AST*b,AST*c,AST*d):v(a),start(b),end(c),body(d){} };

// ================= PARSER =================
class Parser {
    Lexer& l;
    Token cur;

public:
    Parser(Lexer& x):l(x){ 
        cur=l.getNextToken(); 
    }

    void eat(TokenType t){
        if(cur.type==t) cur=l.getNextToken();
        else throw std::runtime_error("Unexpected token");
    }

    AST* factor(){
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
            auto v=cur.value; 
            eat(TokenType::IDENTIFIER); 
            return new VarNode(v); 
        }
        if(cur.type==TokenType::LPAREN){ 
            eat(TokenType::LPAREN); 
            auto n=expr(); 
            eat(TokenType::RPAREN); 
            return n; }
        throw std::runtime_error("Invalid factor");
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
        while(cur.type==TokenType::GREATER||cur.type==TokenType::LESS||cur.type==TokenType::STRICTEQ){
            auto o = cur; 
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
            auto v = cur.value; 
            eat(TokenType::IDENTIFIER); 
            eat(TokenType::EQUAL);
            return new AssignNode(v,expr());
        }

        if(cur.type==TokenType::IF){
            eat(TokenType::IF);
            auto c = comparison();
            auto t = block();
            AST* e = nullptr;
            if(cur.type == TokenType::ELSE){ 
                eat(TokenType::ELSE); 
                e = block(); 
            }
            eat(TokenType::END);
            return new IfNode(c,t,e);
        }

        if(cur.type==TokenType::WHILE){
            eat(TokenType::WHILE);
            auto c=comparison();
            auto b=block();
            eat(TokenType::END);
            return new WhileNode(c,b);
        }

        if(cur.type==TokenType::FOR){
            eat(TokenType::FOR);
            auto v=cur.value; eat(TokenType::IDENTIFIER);
            eat(TokenType::EQUAL);
            auto s=expr();
            eat(TokenType::TO);
            auto e=expr();
            auto b=block();
            eat(TokenType::END);
            return new ForNode(v,s,e,b);
        }

        throw std::runtime_error("Invalid statement");
    }

    AST* block(){
        std::vector<AST*> r;
        while(cur.type!=TokenType::END&&cur.type!=TokenType::ELSE&&cur.type!=TokenType::EOF_TOK)
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

// ================= INTERPRETER =================
class Interpreter {
    std::unordered_map<std::string,double> vars;

public:
    using NodeVal = std::variant<double, std::string, bool>;
    NodeVal visit(AST* n){
        if(auto x = dynamic_cast<NumberNode*>(n)) return x->v;
        if(auto x = dynamic_cast<StringNode*>(n)){ 
            return x->v;; 
        }
        if(auto x =dynamic_cast<VarNode*>(n)) return vars[x->n];
        if(auto x =dynamic_cast<AssignNode*>(n)) return vars[x->n] = std::get<double>(visit(x->v));
        if(auto x =dynamic_cast<BinOpNode*>(n)){
            double a = std::get<double>(visit(x->l));
            double b = std::get<double>(visit(x->r));
            if(x->o.type == TokenType ::PLUS){
                return a+b;
            }
            if(x->o.type == TokenType ::MINUS) {
                return a-b;
            }
            if(x->o.type == TokenType ::STAR) return a*b;
            if(x->o.type == TokenType ::SLASH) return a/b;
            if(x->o.type == TokenType ::GREATER) return a > b;
            if(x->o.type == TokenType ::LESS) return a < b;
            if(x->o.type == TokenType::STRICTEQ) return a == b;
        }
        if(auto p = dynamic_cast<PrintNode*>(n)){
            NodeVal val = visit(p->e);
            std::string str;
            if(std::holds_alternative<double>(val)){
                std::cout << std::get<double>(val);
            }else{
                str = std::get<std::string>(val);
                if(str == "_e_l"){
                    std::cout << std::endl;
                }else{
                    std::cout << str << std::endl;
                }
            }
        }

        if(auto x= dynamic_cast<BlockNode*>(n)){ 
            for(auto s:x->s){
                visit(s); 
            }
        }
        if(auto x=dynamic_cast<IfNode*>(n)){ 
            if(std::get<bool>(visit(x->c))){
                visit(x->t);
            }else if(x->e) {
                visit(x->e);
            } 
        }
        if(auto x = dynamic_cast<WhileNode*>(n)){ 
            while(std::get<bool>(visit(x->c))){
                visit(x->b); 
            }
        }
        if(auto x = dynamic_cast<ForNode*>(n)){
            double s = std::get<double>(visit(x->start));
            double e = std::get<double>(visit(x->end));
            for(vars[x->v] = s; vars[x->v] <= e; vars[x->v]++){ 
                visit(x->body);
            }
        }
        return 0.0;
    }
};

//to run this in a .bas file
//cd [file location]
//g++ bas_main.cpp -o basic
//basic [bas_file_name.bas]

// ================= MAIN =================
int main(int argc, char* argv[]) {
    std::cout << "=== BASIC Interpreter ===\n";

    if (argc < 2) {
        std::cerr << "Usage: interpreter <file.bas>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        Parser parser(lexer);
        AST* program = parser.parse();

        Interpreter interpreter;
        interpreter.visit(program);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}

