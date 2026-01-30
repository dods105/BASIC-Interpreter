# Lesson 3.2 - Building the Parser

 
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