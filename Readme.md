# Chengine

Chengine is my first project written in C. A friend of mine challenged me to write chess engines and compete them against each other. His engine (Him having 10+ years C and C++ development experience) of course crushed mine.
He searched something up to 21ply while my engine is somewhere at 11ply. 
While at the point that I am now I can see many flaws with the code itselfI am nevertheless proud of having a running finished project.

The engine is compatible with Xboard and plays way better than I do although in the tournament itself I found that it seems to have a bug not following through with exchanges.

Especially the code around opening books and the opening book itself is not from me. I was also inspired by gnuchess and stockfish. I tried to have remarks about it everywhere, please let me know if you think I missed something.

## Install

To compile it just run:

```
g++ -O3 *.c -o chengine
```

and then 

```
./chengine
```

for playing inside the terminal, or select the `chengine` file in Xboard.

In terminal it is defaulted as human vs human, in order to play the AI type `new` if AI should be black or `go` to make the AI play at any specific point.
