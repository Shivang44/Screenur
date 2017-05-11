# Screenur
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
- Implement asynchronous uploads so users can spam the screenshot buttons and have each request processed
- Design icons (would love suggestions/input on this!)

# Download
The latest binary (still in pre-development stages) can be found [here](https://github.com/Shivang44/Screenur/raw/master/Screenur.exe)
