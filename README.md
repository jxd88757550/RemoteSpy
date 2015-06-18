# RemoteSpy

Spy program is designed to monitor computer activity, able to receive remote commands and send reports to owner ftp server.
Spy is controlled via Jabber protocol by sending codes and parameters. All collected by the spy data sent to specified
ftp server. Owner must have ftp hosting account and any Jabber client program.

Capabilities:
- Logging pressed keys
- Taking screenshot by realtime command or by typed keyword on monitored machine
- Recognition of directories structure and files with specific extensions
- Monitoring working processes and taking screenshots when specific processes are launched
- Monitoring opened windows and taking screenshots when specific windows are opened
- Executing arbitrary commands, like in cmd.exe
- Showing Windows popup message with text and title specified by real-time command
- Ability to add specific windows and processes to track list, so when they appeared, screenshots are periodically taken
- Copy specific files and directories to owner ftp server by real-time command

Command codes
- 0 - ping spy program
- 1 - take screenshot immediately
- 2 - log pressed keys
- 3 - log all directories
- 4 - log of files with specific extensions to ftp server, example: 4;.txt;.jpg
- 5 - log working processes
- 6 - log opened windows
- 7 - stop tracking processes and windows
- 8 - execute arbitrary system command, for example, launching calc.exe: 8;calc;5 (5 - window is visible, 0 - invisible)
- 9 - show windows popup message, example: 9;text;header
- 10 - add process to tracking list, example: 10;firefox.exe
- 11 - add window to tracking list, example: 11;StarTrek
- 12 - copy and send file(s) to ftp server, example: 12;C:/Folder/File.rar;D:/Photo.jpg
- 13 - send all collected logs and screenshots to ftp server
