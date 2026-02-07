# Lesson 3.1 - AST NODES


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

```cpp

    struct AST { virtual ~AST() = default; };

```

We will define different Nodes which Inherits from the AST


The **NumberNode** stores a number. It could be 20, 5, 1000, or any number.

```cpp

    struct NumberNode : AST {
        double v;
        NumberNode(double x):v(x){}
    };

```

The **StringNode** stores strings.

```cpp

    struct StringNode : AST {
        std::string value;
        StringNode(std::string x): value(x) {}
    };

```


**VarNode** stores the name of the variable passed into it. Let's say _x_ is passed, the _name = x_

this is used for accesing the value of the variable like in PRINT x

```cpp

    struct VarNode : AST {
        std::string n;
        VarNode(std::string x):n(x){}
    };

```


**AssignNode** assigns and stores the name of the variable and its value. Ex: _x = 5 ->_  _name = x, value = NumberNode(5)_

```cpp

    struct AssignNode : AST {
        std::string n;
        AST* value;
        AssignNode(std::string a,AST*b):n(a),value(b){}
    };

```

**BinOpNode** are called to store binary operations such as +, -, /, \*


the **_AST\* left and right_** variable can store the following: _VarAccessNode, VarAssignNode, NumberNode,_ and _BinOpNode_

while the **_Token op_** stores the operations. Ex: _{TokenType::PLUS, "+:}_


```cpp

    struct BinOpNode : AST {
        AST*l;
        Token op;
        AST*r;
        BinOpNode(AST*a,Token b,AST*c):l(a),op(b),r(c){}
    };

```


**PrintNode** - strore the node that is to be printed

example would be:
```
    PRINT "Hello"

    PrintNode(StringNode("Hello"))

```

```cpp

    struct PrintNode : AST {
        AST* e; PrintNode(AST*x) : e(x){}
    };

```


**BlockNode** - this indicates the scope of a For, While, and If-Else Nodes. To indicate if a block has ended. You write END to end its scope

```

    FOR x = 1 to 10
        PRINT x
        PRINT "\n"
    END

```

in the above, the block holds two PrintNodes. One for printing the value of x and the other for printing the newline.


```cpp

    struct BlockNode : AST {
        std::vector<AST*> s;
        BlockNode(std::vector<AST*>x):s(x){}
    };

```


**IfNode** - the IfNode holds the condition, then branch(what happens if true). and the else branch. the then branch and else branch are both BlockNodes. the default value for else is nullptr. this is the case, because there are some instances where the user defines an If statement but not an else statement.

```cpp

    struct IfNode : AST {
        AST *condition, *then, *else_;
        IfNode(AST*a,AST*b,AST*d=nullptr):condition(a),then(b),else_(d){}
    };

```


**WhileNode** - this node holds the condition and body for the While loop. Its body holds a blocknode.


```cpp

    struct WhileNode : AST {
        AST*condition,*body;
        WhileNode(AST*a,AST*d):condition(a),body(d){}
    };

```

**ForNode** - this holds the name of the variable, its starting range and its last range, and the body or the BlockNodes.

```cpp

    struct ForNode : AST {
        std::string value;
        AST*start,*end,*body;
        ForNode(std::string a,AST*b,AST*c,AST*d):value(a),start(b),end(c),body(d){}
    };

```


**InputNode** - this holds the text that is to be displayed when asking the user for an input

```

    > Enter a number:

```


```cpp

    struct InputNode : AST{
        AST* text;
        InputNode(AST* str) : text(str) {}
    };

```


**ArrayAccessNode** - holds both the index and the name of the array.
**ArrayAssignNode** - this node is created when an array is decalred and given a value

```cpp

    a[0] = 10 // ArrayAssignNode
    PRINT a[0] // ArrayAccessNode

```

