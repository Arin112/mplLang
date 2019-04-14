# mplLang
MplLang is a language created to be translated to [CRPL](https://knucklecracker.com/wiki/doku.php?id=crpl:overview).

[![Download mplLang](https://a.fsdn.com/con/app/sf-download-button)](https://sourceforge.net/projects/mpllang/files/latest/download)

## Usage
### For windows
1. Download the program from the link above.
2. Create a file with ".mpl" extension and write a mplLang program in the file.
3. Open it with the downloaded program. (Right click -> open with ... )
4. If your file is not empty and does not contain syntax errors, the program will automatically close and create same-named file with ".crpl" extension.
You can translate multiple files at the same time.

### For Linux & OS X
1. Clone the repo.
2. Try something like `g++ --std=c++14 *.cpp -o mplCompiler` in src folder. (I haven't used any windows-specific features.)
3. Launch with `./mplCompiler file1.mpl file2.mpl file3.mpl`

## Overview
This language was created to simplify the writing of a program in the CRPL language.
The main goals:
1. Save all CRPL language functionality.
2. Make the process of writing programs on the CRPL easier, while maintaining the speed of the final CRPL code.

## Examples
The left column is your mplLang code. The right column is what your code will be translated into.

| mplLang | crpl |
| - | - |
| ```x = 2 + 2 * 2;``` | ```2 2 2 mul add ->x``` |
| ```z = f(x, y);``` | ```<-x <-y @f ->z``` |
| ```[x, y] = CurrentCoords();``` | ```CurrentCoords ->y ->x``` |
| ```once SetText("Amt " $ creepAmt $ endl() $ "Interval: " $ round(Delay/30.0, 2));``` | ``` once```<br>```    "Amt " <-creepAmt concat "```<br>```" concat "Interval: " concat <-Delay 30 div 2 round concat SetText```<br>```endonce``` |
| ```if(a<b && (c+1 == -c)) [a, b, c] = 1, 2.0, 3.14; ``` | ```<-a <-b lt <-c 1 add <-c neg eq and if```<br>```    1 2 3.140000 ->c ->b ->a```<br>```endif``` |
| ```do (7) Trace(i);``` | ```7 .. 0 do```<br>```    i Trace```<br>```loop``` |
| ```do (1 .. 42){```<br>```	a = refRead("num");```<br>```	refWrite(7*(3.14+i), "num");```<br>```}``` | ```42 .. 1 do```<br>```    "num" <-! ->a```<br>```    7 3.140000 i add mul "num" ->!```<br>```loop``` |
| ```if(exists(x)) delete(x);``` | ```-?x if```<br>```    --x```<br>```endif``` |

Check [examples](https://github.com/Arin112/mplLang/tree/master/examples) for complate example.
## Detailed description
placeholder

## Patch notes
### v0.24
#### Bugfixes
+ Fixed `do` expression.
### v0.23
#### Minor Features
+ Add some assignment operators - "+=", "-=", "*=", "/=", "%=", "$=". 
#### Bugfixes
+ Fixed that if you write the name of the standard function in the wrong case it will be recognized as a normal function.
+ Fixed parsing operations ">=" and "<="
