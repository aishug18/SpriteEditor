# Pixel Sprite Editor
# Team Hexacoders	
LAB - 10:45 AM	

Anurup Kumar	| Diya Mandot |	Kunj Rathod |	Aishwarya Golakoti |	Morris Lu																			

A modern, user-friendly sprite editor for creating pixel art animations. This Qt application provides essential tools for creating frame-by-frame animations with a clean, intuitive interface.

![image](https://github.com/user-attachments/assets/f497e14d-e70f-4d72-8a91-c6dc33e2db1e)


## Features

- **Intuitive Drawing Tools**: Pen, eraser, color picker and shape tools
- **Animation Support**: Create multi-frame animations with preview functionality
- **Transparency Support**: Full alpha channel support for creating sprites with transparency
- **Frame Management**: Add, delete, and duplicate frames easily
- **Animation Controls**: Adjust playback speed (FPS), play/pause, and navigate between frames
- **Project Files**: Save and load your work with the custom .ssp (Sprite Sheet Project) format
- **Modern Qt UI**: Clean, dark-themed interface with good contrast and usability

## Requirements

- Qt 5.12 or later
- C++11 or later compiler
- Qt Creator (recommended) / qmake

## Building from Source

### Using Qt Creator (Recommended)

1. Clone the repository:
   
  [ git clone https://github.com/yourusername/pixel-sprite-editor.git](https://github.com/UofU-CS3505/cs3505-assignment8-Morris86)
   

2. Open the project file (`sprite-editor.pro`) in Qt Creator

3. Configure the project for your kit(s)

4. Build and run the project using the build/run buttons in Qt Creator

## Project Structure

- **animationcontroller.cpp/.h**: Manages animation playback and timing
- **animationpreview.cpp/.h**: Widget for displaying animation previews
- **framethumbnail.cpp/.h**: Custom widget for frame thumbnails in the timeline
- **main.cpp**: Application entry point
- **mainwindow.cpp/.h/.ui**: Main application window and UI layout
- **spritecanvas.cpp/.h**: The canvas widget for drawing pixel art
- **spriteframe.cpp/.h**: Data structure for individual animation frames
- **icons.qrc**: Qt resource file for application icons

## Usage Guide

### Working with Projects

- **Create New Project**: Launch the application to start with a blank canvas
- **Save Project**: Click the "Save" button to save your work as a .ssp file
- **Load Project**: Click the "Load" button to open an existing .ssp file

### Drawing Tools

- **Pen Tool**: Click the pen icon to draw pixels directly on the canvas
- **Shape Tools**: Select a shape from the dropdown (Rectangle, Square, Circle, Line) and drag on the canvas to create
- **Eraser**: Click the eraser icon to remove pixels
- **Color Picker**: Click the color dropper to select a color from the canvas
- **Color Selector**: Click the color button to open a QColorDialog with alpha channel support

### Frame Management

- **Add Frame**: Click the "+" button in the Frames panel to add a new frame
- **Delete Frame**: Select a frame and click the "-" button to delete it
- **Duplicate Frame**: Select a frame and click the "D" button to create a copy
- **Navigate Frames**: Click on frame thumbnails in the frames panel to switch between them

### Animation Preview

- **Play/Pause**: Click the play/pause button to start or stop the animation
- **Previous/Next Frame**: Use the arrow buttons to navigate frames
- **FPS Control**: Adjust the slider to change animation speed (1-60 FPS)

## File Format

The application uses a custom .ssp (Sprite Sheet Project) file format which is JSON-based and includes:
- Canvas dimensions
- All frame data with full color information (RGBA)
- Animation settings

## Implementation Details

- **QTimer**: Used for controlling animation playback timing
- **QImage**: Core class for frame storage and pixel manipulation
- **QJsonDocument**: Used for saving and loading project files
- **Custom Widgets**: Several custom Qt widgets (SpriteCanvas, FrameThumbnail, AnimationPreview)
- **Qt Layouts**: Flexible UI layout using QVBoxLayout, QHBoxLayout, etc.
- **Qt Stylesheets**: Custom styling for a modern dark interface
