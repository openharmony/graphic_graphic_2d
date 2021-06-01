

The API of windows manager can refer to the file "windows_ manager.h”。

1. First, you need to call windowsmanager:: getinstance() to get the singleton object pointer of WM, and call CreateWindow to create a window (the returned object is an object pointer of window)


2. In addition to the width, height, coordinates and other information of the screen, there are also function pointers for mouse operation in the windowconfig structure passed in when creating a window_ xxxx_cb) and graph refresh callback function sync, which need to be passed in when creating window



3. Through the window pointer, you can perform some operations on the window (such as show, hide, etc.) and get the window ID. through the window ID, you can get the created surface, and then write data to the buffer(Surface::GetInstance(windowid)).

