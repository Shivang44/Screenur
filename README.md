# Screenur ![Screenur Icon](http://i.imgur.com/pc9lOm9.png "Screenur Icon") 
A fast and minimal screenshot tool that instantaneously uploads screenshots to imgur.com

Developed in C++ using the Winapi (win32) along with GDI/GDI+ for graphics and libcurl for uploading to imgur. 

# Usage
CTRL-SHIFT-3 - Uploads fullscreen screenshot to imgur, opens in browser, and copies url to clipboard

CTRL-SHIFT-4 - Same as above but allows you to take a screenshot of a selectable region on your screen

# What's left to do
- Implement tray icon with notifications
- Develop settings menu with options to 
  - Change screenshot hotkeys
  - Change post-screenshot actions (automatically open in browser or not, copy to clipboard or not, etc)
  - Save screenshots locally
  - Autostart on startup
    - For the time being you can just put the .exe file in the Startup Folder (Press start, type %APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup and put the file in that folder)
- Implement asynchronous uploads so users can spam the screenshot buttons and have each request processed
- Design icons (would love suggestions/input on this!)

# Download
The latest binary (still in pre-development stages) can be found [here](https://github.com/Shivang44/Screenur/raw/master/Screenur.exe)




App icon made by [freepik.com](http://www.freepik.com) from [flaticon.com](http://www.flaticon.com) is licensed by [CC 3.0 BY](http://creativecommons.org/licenses/by/3.0/)
