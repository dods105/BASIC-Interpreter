# Lesson 3 - Parser and AST


## What is a Parser and Why is it Needed?

Well think of this:

```
  x = 3 + 5 * (2 + 1)
```


In math there is something called __PEMDAS__ or __BODMAS__.



__Parenthesis -> Exponents -> Multiplication/Division -> Addition/Subtraction__

The parser organizes token according to these rules.

Without a parser the _x = 24_ and not _x = 18._



Example of the execution flow:

```  
  Input: x + 20 * 5
```


Produces AST:

```
  +
  
  / \
  
  x  *
  
    / \
  
   20   5
```


## AST (Abstract Syntax Tree)  

We will define different Nodes which Inherits from the AST
<br>
<br>
The **NumberNode** stores a number. It could be 20, 5, 1000, or any number.
<br>
<br>
**BinOpNode** are called to store binary operations such as +, -, /, \*
<br>
<br>
the **_AST\* left and right_** variable can store the following: _VarAccessNode, VarAssignNode, NumberNode,_ and _BinOpNode_  

while the **_Token op_** stores the operations. Ex: _{TokenType::PLUS, "+:}_  

**VarAssignNode** assigns a stores the name of the variable and its value. Ex: _x = 5 ->_  _name = x, value = 5_  
  
**VarAccessNode** stores the name of the variable passed into it. Let's say _x_ is passed, the _name = x_  

```cpp
  struct AST { virtual ~AST() = default; };

struct NumberNode : AST {        // a number literal
    double value;
    NumberNode(double v) : value(v) {}
};

struct BinOpNode : AST {         // a binary operation like +, -, *, /
    AST* left;
    Token op;
    AST* right;
    BinOpNode(AST* l, Token o, AST* r) : left(l), op(o), right(r) {}
};

struct VarAssignNode : AST {     // assignment like x = 5
    std::string name;
    AST* value;
    VarAssignNode(std::string n, AST* v) : name(n), value(v) {}
};

struct VarAccessNode : AST {     // using a variable
    std::string name;
    VarAccessNode(std::string n) : name(n) {}
};

```

_Reminder: as the interpreter is being built the methods of the classes will be subjected to modification._  

  
## PARSER CLASS

Once we got the token that the lexer produced, it will be passed onto the parser. And the parser will then make a node of that token and assign its precedence.

```cpp
    class Parser {
      Lexer& lexer;
      Token currentToken;
    
    public:
      Parser(Lexer& l) : lexer(l) {
        currentToken = lexer.getNextToken(); // start with first token
      }
      AST* parse();
      AST* expr();
      AST* term();
      AST* factor();
      void eat(TokenType type);
};
```

### Function Explanation

The __eat ()__ method is called on to check whether the passed type parameter is the same with the currentToken type. If so, it updates the currentToken to the next token.

```cpp
  void Parser::eat(TokenType type) {
      if (currentToken.type == type)
          currentToken = lexer.getNextToken(); // move to next token
      else
          throw std::runtime_error("Unexpected token");
  }
```

  
After making the Parser object, the interpreter will call on the __parse()__. Then it will start parsing with the lowest precedence rule, which is Addition/Subtraction

```cpp
AST* Parser::parse() {
    return expr();  // start parsing with the lowest-precedence rule
}
```

  
__expr()__ then calls on the term(). Once the term returns a value of the node, it will check the currentToken type if it is PLUS or MINUS. Then will call the eat() to get the next token. The main function of the expr() function is to store the node returned by the term() call, the operation (Addition or subtraction) and another node returned by a term() call to the BinOpNode.

```cpp
AST* Parser::expr() {
    AST* node = term();
    while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
        Token op = currentToken;
        if (op.type == TokenType::PLUS) eat(TokenType::PLUS);
        else eat(TokenType::MINUS);
        node = new BinOpNode(node, op, term());
    }
    return node;
}
```

  
The __term()__ will call on the factor() and store what it returns in the AST* node'. The main function of the term function is to store the node returned by the factor() call, the operation (division or multiplication) and another node returned by a factor() call to the BinOpNode.

```cpp
  AST* Parser::term() {
      AST* node = factor();
      while (currentToken.type == TokenType::STAR || currentToken.type == TokenType::SLASH) {
          Token op = currentToken;
          if (op.type == TokenType::STAR) eat(TokenType::STAR);
          else eat(TokenType::SLASH);
          node = new BinOpNode(node, op, factor());
      }
      return node;
  }
```
  
  
__factor()__ method calls on the eat() method then returns a node type depending of the token type. What it returns will then be stored in the AST\*node of the previous method that called it.

```cpp
  AST* Parser::factor() {
  Token t = currentToken;
    if (t.type == TokenType::NUMBER) {
        eat(TokenType::NUMBER);
        return new NumberNode(std::stod(t.value));
    } else if (t.type == TokenType::IDENTIFIER) {
        eat(TokenType::IDENTIFIER);
        return new VarAccessNode(t.value);
    } else if (t.type == TokenType::LPAREN) {
        eat(TokenType::LPAREN);
        AST* node = expr();        // evaluate inside parentheses
        eat(TokenType::RPAREN);
        return node;
    }
  }
```

## Full Parser and AST Code

```cpp
  //Lesson 3
  
  //AST is the base clss for all the nodes
  // Operations
  // Numbers
  // Variables
  // Assignments
  struct AST { 
      virtual ~AST() = default; 
  };
  
  //Children of AST
  struct NumberNode : AST {        // a number literal
      double value;
      NumberNode(double v) : value(v) {}
  };
  
  struct BinOpNode : AST {         // a binary operation like +, -, *, /
      AST* left;
      Token op;
      AST* right;
      BinOpNode(AST* l, Token o, AST* r) : left(l), op(o), right(r) {}
  };
  
  struct VarAssignNode : AST {     // assignment like x = 5
      std::string name;
      AST* value;
      VarAssignNode(std::string n, AST* v) : name(n), value(v) {}
  };
  
  struct VarAccessNode : AST {     // using a variable
      std::string name;
      VarAccessNode(std::string n) : name(n) {}
  };
  
  struct PrintNode : AST{
      AST * expr;
      PrintNode(AST* e) : expr(e) {}
  };
  
  
  class Parser {
      Lexer& lexer;
      Token currentToken;
  
  public:
      Parser(Lexer& l) : lexer(l) {
          currentToken = lexer.getNextToken(); // start with first token
      }
  
      AST* parse();
      AST* expr();
      AST* term();
      AST* factor();
      void eat(TokenType type);
  
      AST* statement();
  
  };
  
  void Parser::eat(TokenType type) {
      std::cout << "EAT: " << tokenNames(currentToken.type)  << " -> " << currentToken.value << "\n";
  
      if (currentToken.type == type){
          currentToken = lexer.getNextToken(); // move to next token
          std::cout << "currentToken(updated): " << "{" << tokenNames(currentToken.type) << ", " << currentToken.value << "}\n"; 
      }
      else
          throw std::runtime_error("Unexpected token");
  }
  
  AST* Parser::factor() {
  Token t = currentToken;
  if (t.type == TokenType::NUMBER) {
      eat(TokenType::NUMBER);
      return new NumberNode(std::stod(t.value));
  } else if (t.type == TokenType::IDENTIFIER) {
      eat(TokenType::IDENTIFIER);
      return new VarAccessNode(t.value);
  } else if (t.type == TokenType::LPAREN) {
      eat(TokenType::LPAREN);
      AST* node = expr();        // evaluate inside parentheses
      eat(TokenType::RPAREN);
      return node;
  }
  throw std::runtime_error("Invalid factor");
  }
  
  AST* Parser::term() {
      AST* node = factor();
      while (currentToken.type == TokenType::STAR || currentToken.type == TokenType::SLASH) {
          Token op = currentToken;
          if (op.type == TokenType::STAR) eat(TokenType::STAR);
          else eat(TokenType::SLASH);
          node = new BinOpNode(node, op, factor());
      }
      return node;
  }
  
  AST* Parser::expr() {
      AST* node = term();
      while (currentToken.type == TokenType::PLUS || currentToken.type == TokenType::MINUS) {
          Token op = currentToken;
          if (op.type == TokenType::PLUS) eat(TokenType::PLUS);
          else eat(TokenType::MINUS);
          node = new BinOpNode(node, op, term());
      }
      return node;
  }
  
  AST* Parser::parse() {
      return expr();
  }

```
