# SFG
SFG - Simple File Generator

SFG is a simple and lightweight application written in C, that allows users to create empty files for benchmarking and/or other purposes.

# Usage
| sfg [path] [size] [unit] [bs] [filename] | Writes an empty file named [name] of size [size][unit] in [path] using block size [bs]                |
|------------------------------------------|-------------------------------------------------------------------------------------------------------|
| sfg [path] [size] [unit] [bs]            | Writes an empty file named 'file' of size [size][unit] in [path] using block size [bs]                |
| sfg [path] [size] [unit]                 | Writes an empty file named 'file' of size [size][unit] in [path] using kernel default block size|

or __```sfg --help```__

## Note:
* [size] must be a valid integer
* [unit] must be a valid data size unit (i.e. b, kb, mb, gb)
* [bs] must be a valid integer. [bs] is always expressed in bytes (b) and must be smaller than the total file size
* If a directory does not exist, sfg can create it (0755). If a file already exists, sfg will not overwrite it.

# Installation
To install SFG simply download the source code and compile it using clang<br/>
```$ clang sfg.c```<br/>
Then rename a.out to __sfg__<br/>
```$ mv a.out sfg``` <br/>
And move it to /usr/bin/ for Linux or /usr/local/bin/ for OS X users<br/>
```$ mv sfg /usr/bin/```<br/>
or<br/>
```$ mv sfg /usr/local/bin/```<br/>

( ͡° ͜ʖ ͡°)
