# SFG
SFG - Simple File Generator

SFG is a simple and lightweight application written in C, that allows users to create empty files for benchmarking and/or other purposes.

# Usage
__sfg [license] [-help] [-bs_stat] path=[path] size=[size] unit=[unit] bs=[bs] name=[name]__

## Note:
* [license]: Displays license information.
* [-help]: Displays help page.
* [-bs_stat]: Displays kernel default bs.
* [size]: Filesize. Must be a valid integer.
* [unit]: Filesize unit. Must be a valid data size unit (i.e. b, kb, mb, gb).
* [bs]: Blocksize. Must be a valid integer. [bs] is always expressed in bytes (b) and must be smaller than the total file size. SFG can also determine the system I/O [bs] if the argument 'auto' is passed.
* If a directory does not exist, sfg can create it (0755). If a file already exists, sfg will not overwrite it.

# Installation
To install SFG simply download the source code (either by cloning this repo or as zip)<br/>
```$ git clone https://github.com/MarcelFerrari/sfg```<br>
CD into the directory, compile it using clang/gcc and install it to /usr/bin on linux or /usr/local/bin for OS X<br>
Linux: ```$ sudo clang sfg.c -o sfg && chmod +x sfg && mv sfg /usr/bin/```<br>
OS X: ```$ sudo clang sfg.c -o sfg && chmod +x sfg && mv sfg /usr/local/bin/```<br>

__Or simply download the precompiled bin file :D__ <br>
(You might need to make the bin file executable with ```$ sudo chmod +x sfg-XX-XX-XX```)<br>
( ͡° ͜ʖ ͡°)
