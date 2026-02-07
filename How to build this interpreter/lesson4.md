# Lesson 4 - The Interpreter

Add this first, as it will be used in making the interpreter


```cpp
  inlcude <unordered_map>;
```


**Add the PRINT as a token** type as it will be used to identify what to print.
```cpp
  enum TokenType{
  
  ……,
  
  PRINT
  
  }
```


Modify the **identifier()** in the lexer to **_classify PRINT as PRINT and not as KEYWORD_**

```cpp
Token Lexer::identifier(){
    std::string result;
    while(isalnum(current)){
        result += current;
        advance();
    }
    static std::unordered_set<std::string> keywords = {
        "PRINT", "LET", "IF", "ELSE", "INPUT", "THEN", "FOR", "TO", "NEXT", "END"
    };
//add this part
    if (keywords.count(result)) {
        if (result == "PRINT") return {TokenType::PRINT, result};
        return {TokenType::KEYWORD, result};
    }
    return {TokenType::IDENTIFIER, result};
}
```


The **PrintNode** will contain the values to be printed.

```cpp
Struct PrintNode : AST{
AST* expr;
PrintNode(AST* e) : expr(e) {}

};
```


The **_statement()_** is used to help determine what kind of AST nodes to create. If the Token Type is PRINT, it will create a PrintNode, If it is an identifier it will create a VarAssignNode, and otherwise, it will call on the expr() to further determine what node to make.


```cpp
AST* Parser::statement(){
If(currentToken.type == TokenType::PRINT{
eat(TokenType::PRINT);
AST* node = expr();
return new PrintNode(node);
}else if(currentToken.type == TokenType::IDENTIFIER){
Token temp = currentToken;
eat(TokenType::IDENTIFIER);
eat(TokenType::EQUAL);
AST* value = expr();
return new VarAssignNode(temp.value, value);
}else{
return expr();
}
```

change the parse() to call the statement() instead of expr();

```cpp
AST* Parser::parse(){
return statement();
}
```



# Building the Interpreter

Note: The interpreter is not yet built to process while loops, strings, if else, etc… as their nodes are not built yet, it can only basic things such as Printing a number or basic math and assigning a number to a variable.
The visit() method is a recursive method that receives the node that the parser returns.
This Interpreter class consists of if statements that serves to determines what type of node is being returned by the parser. While the globals unordered_map serves as the holder of the variables and the value that the VarAssignNode has. Onto which will be compared with the content of the VarAccessNode. and if the VarAccessNode has a variable which is not in the globals, the interpreter will throw an error. s

```cpp
class Interpreter{
    std::unordered_map<std::string, double> globals;

    public:
    double visit(AST* node){
        if (auto n = dynamic_cast<NumberNode*>(node)){
            return n->value;
        }
        if (auto b = dynamic_cast<BinOpNode*>(node)){
            double left = visit(b->left);
            double right = visit(b->right);
            switch(b->op.type){
                case TokenType::PLUS: return left + right;
                case TokenType::MINUS: return left - right;
                case TokenType::STAR: return left * right;
                case TokenType::SLASH: return left / right;
                default: throw std::runtime_error("Unknown operator");
            }
        }
        if (auto v = dynamic_cast<VarAccessNode*>(node)){
            if(globals.find(v->name) != globals.end()){
                return globals[v->name];
            }
            throw std::runtime_error("Undefined variable: " + v->name);
        }
        if( auto a = dynamic_cast<VarAssignNode*>(node)){
            double val = visit(a->value);
            globals[a->name] = val;
            return val;
        }
        if(auto p = dynamic_cast<PrintNode*>(node)){
            double val = visit(p->expr);
            std::cout << val << "\n";
            return val;
        }
        throw std::runtime_error("Unknown AST node");
    }

};
```

Try tinkering on it on the main function to see how it goes. Here's an example:

int main() {
```cpp
int main() {
    try {
        Interpreter interpreter;

        {
            Lexer lexer("x = 10 + 6");
            Parser parser(lexer);
            AST* tree = parser.parse();
            interpreter.visit(tree);
        }
        std::cout << "\n";
        {
            Lexer lexer("PRINT x");
            Parser parser(lexer);
            AST* tree = parser.parse();
            interpreter.visit(tree);
        }
        std::cout << "\n";
    }
    catch (std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}
```

Output: 
```cpp
16
```
