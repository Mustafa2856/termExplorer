# Terminal File Explorer
file explorer in console on linux
### Dependencies install:
#### Ubuntu/Debian:
```
sudo apt install libncurses-dev trash-cli -y
```
#### Fedora/CentOS:
```
sudo dnf install ncurses-devel trash-cli
```
### To compile:
```bash
clang++ main.cpp -o TE -lncurses -lstdc++fs -std=c++1z
```
### To run:
```
./TE
```
### Key Bindings:
```
h : toggle view hidden files mode
f : open in default file explorer
t : open in terminal (xterm supported by default)
Del : To delete file/folder
s : to enter search mode ( Press of valid character selects first file/folder with that char or bigger )
Up/Down : to navigate in the directory
Right(or Enter) : to open directory/file
Left : to go back a directory
```
