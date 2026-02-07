# Psuedocode Interpreter

### Syntax
```
  X = 32;
  PRINT(X)
  PRINT "\n"

  FOR y = 1 TO 10
    PRINT y
    PRINT "\n"
  END

y = INPUT "Enter a number: "
PRINT y

```

__Output__

```

32
1
2
3
4
5
6
7
8
9
10
Enter a number: 3
3

```


In this project, i wll be making a Psuedocode like syntax interpreter using C++.

Okay, before you look at my code, do keep in mind that i am just a beginner. This is my first c++ project. I know there is a better and efficient way to write it, but i just dont know how right now. This is how i thought of writing this interpreter, so bare with me.

Why C++?

I've thought about using python to build this project but then python is also an interpreted language, right?  So that could mean that running an interepreted language on top
of an interpreted language can be slow in runtime. So i opted for C++

Update:
The interpreter currently has the following features:
  - Print Statements
  - Variable assignments (Strings and Double)
  - Arithmetic Operations (+, -, *, /)
  - If-Else statements
  - While/For loops Statements
  - Inputs (Strings and Double)

is this all the features?
well, you can all use array indexing in here. Though, it is not an actual array. But i might add an array feature in the future. I dont know exactly when or if i will ever add one. But for now, i will set aside this and create other projects.

```
a[0] = 7
a[5] = 30
a[3] = a[0]

```



### SYNTAX
```

  PRINT "...."  //This is for printing a string/
  PRINT 4       //This is prints a number, in this case, 4/
  var = 8       //stores 8 in var/
  name = "Bob"  //Stores the string Bob to the name variable/
  var = var + 2 //Adding 2 to var/
  PRINT var     //Outputs 10/

  WHILE var > 5 //While loop declaration/
    PRINT var
    PRINT "\n"  //Prints a newline/
    var = var - 1
  END           //Ends the while loop/

  IF var == 5
    PRINT "Equals to Five"
  ELSE
    PRINT "Var is not 5"
  END


  FOR x = 1 to 5
    PRINT x
    PRINT "\n"
  END


```

if you want to run this in a separate file, you can create your own file and own made-up extension. Yes, made-up extension. Example would be naming your file as __main.ssl__ or __test.w3o__


another thing to consider is what kind of command do you want to use when you want to execute you file. let's say mine is __psued__

make sure that the file you have created is in the _same directory_ as the lexer.h, parser.h, and main.cpp

you can open your command line or shell or terminal and go to the dame directory as these files. This might be different if you have gcc installed instead of g++ mine is g++

```

  > g++ main.cpp -o psued

  > psued <filename>


```

lesson1.md - finished
lesson2.md - finished
lesson3-1.md - finished
lesson3-2.md - not finished
lesson4.md - not finished


