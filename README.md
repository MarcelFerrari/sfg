# SFG
SFG - Simple File Generator

SFG is a simple and lightweight application written in C, that allows users to create empty files for benchmarking and/or other purposes.

# Usage
__sfg [-help] [license] [bs_stat] path=[path] size=[size] unit=[unit] bs=[bs] name=[name]__

## Note:
* [-help]: displays help information
* [size]: Filesize. Must be a valid integer
* [unit]: Filesize unit. Must be a valid data size unit (i.e. b, kb, mb, gb)
* [bs] Blocksize. Must be a valid integer. [bs] is always expressed in bytes (b) and must be smaller than the total file size. SFG can also determine the kernel default [bs] if the argument 'auto' is passed.
* If a directory does not exist, sfg can create it (0755). If a file already exists, sfg will not overwrite it.

# Installation
To install SFG simply download the source code (either by cloning this repo or as zip)<br/>
```$ git clone https://github.com/MarcelFerrari/sfg```<br>
Compile it using clang and install it to /usr/bin for linux users or /usr/local/bin for OS X users
And move it to /usr/bin/ for Linux or /usr/local/bin/ for OS X users<br>
Linux: ```$ sudo clang sfg.c -o sfg && chmod +x sfg && mv sfg /usr/bin/```<br>
OS X: ```$ sudo clang sfg.c -o sfg && chmod +x sfg && mv sfg /usr/local/bin/```<br>

( ͡° ͜ʖ ͡°)
