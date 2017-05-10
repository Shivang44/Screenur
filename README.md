# Screenur
A quick and minimal screenshot tool that instantaneously uploads screenshots to imgur.com

Developed in C++ using the Winapi (win32) along with GDI/GDI+ for graphics and libcurl for uploading to imgur. 

# Usage
CTRL-SHIFT-3 - Uploads fullscreen screenshot to imgur, opens in browser, and copies url to clipboard

CTRL-SHIFT-4 - Same as above but allows you to take a screenshot of a selectable region on your screen

# What's left to do
- Actually upload the generated PNG to imgur using libcurl
- Implement tray icon with notifications
- Develop settings menu
- Design icons (would love suggestions/input on this!)

