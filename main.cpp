#include "parser.h"

class Interpreter {
    using NodeVal = std::variant<double, std::string, bool, TokenType, int, std::vector<int>>;
    std::unordered_map<std::string, NodeVal> vars;
    std::unordered_map<std::string, std::vector<NodeVal>> arrays;

public:
    NodeVal visit(AST* n){

        if(auto ar = dynamic_cast<ArrayAccessNode*>(n)){
            double idx = std::get<double>(visit(ar->index));
            return arrays[ar->name][idx];
        }

        if(auto ar = dynamic_cast<ArrayAssignNode*>(n)){
            double idx = std::get<double>(visit(ar->index));
            NodeVal v = visit(ar->value);
            if(arrays[ar->name].size() <= idx){
                arrays[ar->name].resize(idx+1);
            }
            arrays[ar->name][idx] = v;
            return v;
        }

        if(auto x = dynamic_cast<InputNode*>(n)){
            std::cout << std::get<std::string>(visit(x->text));
            std::variant<double, std::string> inputVar;
            std::string inpt;
            getline(std::cin, inpt);

            try{
                double value = std::stod(inpt);
                inputVar = value;
                return std::get<double>(inputVar);
            }
            catch(const std::exception& e){
                inputVar = inpt;
                return std::get<std::string>(inputVar);
            }
        }

        if(auto x = dynamic_cast<NumberNode*>(n)) return x->value;
        if(auto x = dynamic_cast<StringNode*>(n)){
            return x->value;
        }
        if(auto x =dynamic_cast<VarNode*>(n)) return vars[x->n];
        if(auto x =dynamic_cast<AssignNode*>(n)){
            NodeVal val = visit(x->value);
            vars[x->n] = val;
            return val;
        }
        if(auto x =dynamic_cast<BinOpNode*>(n)){
            double a = std::get<double>(visit(x->l));
            double b = std::get<double>(visit(x->r));
            if(x->op.type == TokenType ::PLUS){
                return a+b;
            }
            if(x->op.type == TokenType ::MINUS) {
                return a-b;
            }
            if(x->op.type == TokenType ::STAR) return a*b;
            if(x->op.type == TokenType ::SLASH) return a/b;
            if(x->op.type == TokenType ::GREATER) return a > b;
            if(x->op.type == TokenType ::LESS) return a < b;
            if(x->op.type == TokenType::STRICTEQ) return a == b;
            if (x->op.type == TokenType::NOTEQ) return a != b;
            if (x->op.type == TokenType::LESSEQ) return a <= b;
            if (x->op.type == TokenType::GREATEQ) return a >= b;
        }
        if(auto p = dynamic_cast<PrintNode*>(n)){
            NodeVal val = visit(p->e);
            std::string str;
            if(std::holds_alternative<double>(val)){
                std::cout << std::get<double>(val);
            }else{
                str = std::get<std::string>(val);
                if(str == "\\n"){
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
            if(std::get<bool>(visit(x->condition))){
                visit(x->then);
            }else if(x->else_) {
                visit(x->else_);
            }
        }
        if(auto x = dynamic_cast<WhileNode*>(n)){
            while(std::get<bool>(visit(x->condition))){
                visit(x->body);
            }
        }
        if(auto x = dynamic_cast<ForNode*>(n)){
            NodeVal startVal = visit(x->start);
            NodeVal endVal = visit(x->end);
            if(!std::holds_alternative<double>(startVal) || !std::holds_alternative<double>(endVal)){
                throw std::runtime_error("For loops bounds must be numbers");
            }
            double s = std::get<double>(startVal);
            double e = std::get<double>(endVal);

            vars[x->value] = s;

            while(std::get<double>(vars[x->value]) <= e){
                visit(x->body);
                vars[x->value] = std::get<double>(vars[x->value]) + 1;
            }
        }
        return 0.0;
    }
};

//to run this in a .bas file
//cd to file directory
//g++ bas_main.cpp -o basic
//basic [bas_file_name.bas]

// ================= MAIN =================
int main(int argc, char* argv[]) {
    std::cout << "=== Interpreter ===\n";

    if (argc == 1) {
    std::string line;
    Interpreter interp;

        while (true) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (line == "exit") break;

            Lexer lexer(line);
            Parser parser(lexer);
            auto ast = parser.parse();
            interp.visit(ast);
            std::cout << "\n";
        }
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
