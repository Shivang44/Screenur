# Screenur ![Screenur Icon](http://i.imgur.com/pc9lOm9.png "Screenur Icon") 
A fast and minimal screenshot tool that instantaneously uploads screenshots to imgur.com

Developed in C++ using the Winapi (win32) along with GDI/GDI+ for graphics and libcurl for uploading to imgur. 

# Features
- Region screenshot and fullscreen screenshots
- Automatically uploads screenshot to imgur, opens in browser, and copies url to clipboard
- Small footprint: Less than 500kb and very light on system resources
- No install required and no external dependancies
- Completely open source and images are never stored -- inspect the source and build it yourself if you don't believe me!

# Download
The latest binary can be found [here](https://github.com/Shivang44/Screenur/raw/master/Screenur.exe)

# Usage
CTRL-SHIFT-3 - Uploads fullscreen screenshot to imgur, opens in browser, and copies url to clipboard

CTRL-SHIFT-4 - Same as above but allows you to take a screenshot of a selectable region on your screen

# What's left to do
- Implement asynchronous uploads so users can spam the screenshot buttons and have each request processed
- Fix multimonitor issues


***


App icon made by [freepik.com](http://www.freepik.com) from [flaticon.com](http://www.flaticon.com) is licensed by [CC 3.0 BY](http://creativecommons.org/licenses/by/3.0/)
