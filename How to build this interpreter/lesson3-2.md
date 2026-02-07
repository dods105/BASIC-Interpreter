# Lesson 3.2 - Building the Parser


## PARSER CLASS

Once we got the token that the lexer produced, it will be passed onto the parser. And the parser will then make a node of that token and assign its precedence.

I will not explain what a parser is in this lesson. if you want to know what a parser is, read lesson 3.1

```

    __SYNTAX -> LEXER -> TOKEN -> PARSER -> NODE -> INTERPRETER -> EXCUTION__

```


## Parser Class

```cpp
    class Parser {
      Lexer& lexer;
      Token currentToken;

    public:
      Parser(Lexer& l) : lexer(l) {
        currentToken = lexer.getNextToken(); // start with first token
      }

      AST* parse();
      AST *block();
      AST* statement();
      AST* comparison();
      AST* expr();
      AST* term();
      AST* factor();
      void eat(TokenType type);
};
```

### **Method `eat(TokenType t)`**
- **Purpose**: Validates and consumes the expected token. Emphasize on _expected_. Because if the passed token type is not the same one as expected, it will throw an error.
- **how it works:**:
Checks if current token type matches expected type `t`.

If match succeeds:
    Advances to next token via `l.getNextToken()`
    Updates `cur` with new token
If match fails, it throws `std::runtime_error`

```cpp
  void Parser::void eat(TokenType t){
        if(cur.type==t){
            cur=l.getNextToken();
        }
        else{
            std::string error = "Unexpected Token: " + tokenNames(cur.type) + " Value: " + cur.value;
            throw std::runtime_error(error);
        }
    }
```

---

### **Method `factor()`**
- **Purpose**: Creates nodes based on the current token type. Then returns that node on the method that previously called it.

The node it can create are the following:
  - `INPUT(expr)` - Input operations
  - Numeric literals (`NUMBER`) → Creates `NumberNode`
  - String literals (`STRING`) → Creates `StringNode`
  - Variables (`IDENTIFIER`) → Creates `VarNode`
  - Array access (`IDENTIFIER[expr]`) → Creates `ArrayAccessNode`
  - Parenthesized expressions (`(expr)`) → Recursively parses inner expression

```cpp

AST* factor(){
    if (cur.type == TokenType::INPUT) {
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


```

---

### **Method `term()`**
- **Purpose**: Creates BinOpNode that has multiplication and division as  the operation
- **How it works:**:
  Parses first `factor()` for the left operand.
  While current token is `*` or `/`:
    - Stores operator token
    - Eats operator
    - Parses next `factor()` for the right operand
    - Creates `BinOpNode` with left operand, operator, right operand
- **Note**: Left-associative (e.g., `a*b/c` becomes `((a*b)/c)`) unless you specify in your code that it's supposed to be `a*(b/c)`

```
    PRINT a*b/c
    //default: (a*b)/c
    //unless specified
    PRINT a*(b/c)


```

```cpp

    AST* term(){
        auto n = factor();
        while(cur.type == TokenType::STAR||cur.type == TokenType::SLASH){
            auto o = cur; eat(cur.type);
            n = new BinOpNode(n,o,factor());
        }
        return n;
    }


```

---

### **Method `expr()`**
- **Purpose**: if term is for the * and / operators, expr() is for the + and - operators.
- **How it works**:
  Parses first `term()` - left operand
  While current token is `+` or `-`:
     - Stores operator token
     - Eats operator
     - Parses next `term()` - right operand
     - Creates `BinOpNode`
- **Note**: Lower precedence than `term()` (multiply/divide happen before add/subtract)


```cpp

    AST* expr(){
        auto n=term();
        while(cur.type==TokenType::PLUS||cur.type==TokenType::MINUS){
            auto o=cur; eat(cur.type);
            n=new BinOpNode(n,o,term());
        }
        return n;
    }

```


---

### **Method `comparison()`**
- **Purpose**: Parses relational/comparison operations this is for the following:
    `('>' | '<' | '===' | '!=' | '>=' | '<=')`
- **How it workds**:
  1. Parses first `expr()` - left operand
  2. While current token is any comparison operator:
     - Stores operator token
     - Eats operator
     - Parses next `expr()` - right operand
     - Creates `BinOpNode`


```cpp

    AST* comparison(){
        auto n = expr();
        while(cur.type==TokenType::GREATER||cur.type==TokenType::LESS||cur.type==TokenType::STRICTEQ || cur.type == TokenType::NOTEQ
            || cur.type == TokenType::GREATEQ || cur.type == TokenType::LESSEQ
        ){
            auto operation = cur;
            eat(cur.type);
            n=new BinOpNode(n,operation,expr());
        }
        return n;
    }

```


---

### **Method `statement()`**
- **Handles**:
    This handles the creation of keyword nodes.
    **Print statements**: `PRINT expr` → `PrintNode`
    **Assignments**:
     - Variable: `IDENTIFIER = expr` → `AssignNode`
     - Array element: `IDENTIFIER[expr] = expr` → `ArrayAssignNode`
    **If statements**: `IF comparison block (ELSE block)? END` → `IfNode`
    **While loops**: `WHILE comparison block END` → `WhileNode`
    **For loops**: `FOR IDENTIFIER = expr TO expr block END` → `ForNode`


```cpp

    AST* statement(){
        if(cur.type==TokenType::PRINT){
            eat(TokenType::PRINT);
            return new PrintNode(expr());
        }

        if(cur.type==TokenType::IDENTIFIER){
            auto name = cur.value;

            eat(TokenType::IDENTIFIER);
            if(cur.type == TokenType::LBRACKET){
                eat(TokenType::LBRACKET);
                AST *index = expr();
                eat(TokenType::RBRACKET);
                eat(TokenType::EQUAL);

                return new ArrayAssignNode(name, index, expr());
            }

            eat(TokenType::EQUAL);
            return new AssignNode(name,expr());
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

```


---

### **Method `block()`**
- **Purpose**: Parses sequences of statements. it only stops once it encounters `ELSE, END, and EOF_TOK`. this is used for the body of if-else statements, while and for loops.
- **How it works**:
    Collects statements into vector while current token is not:
     - `END` (block terminator)
     - `ELSE` (if-else separator)
     - `EOF_TOK` (end of file)
    The returns `BlockNode` containing the statement sequence to the method that previously called it.

```cpp

    AST* block(){
        std::vector<AST*> r;
        while(cur.type!=TokenType::END && cur.type!=TokenType::ELSE && cur.type!=TokenType::EOF_TOK)
            r.push_back(statement());
        return new BlockNode(r);
    }

```

---

### **Method `parse()`**
- **Purpose**: Main entry point. this is the first method called when a Parser object is created.
- **Behavior**:
  Repeatedly calls `statement()` until `EOF_TOK` encountered
  Collects all statements into vector
  Returns `BlockNode` representing the entire program

```cpp

    AST* parse(){
        std::vector<AST*> r;
        while(cur.type!=TokenType::EOF_TOK){
            r.push_back(statement());
        }
        return new BlockNode(r);
    }

```
