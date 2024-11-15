#!/bin/bash

# Step 1: Install dependencies
echo "Installing dependencies..."

# Update package list (ensure system is up-to-date)
sudo pacman -Syu

# Install dependencies:
# - g++ for compiling C++ code
# - opencv for OpenCV
# - tesseract for OCR
# - boost for boost libraries
sudo pacman -S --noconfirm g++ opencv tesseract boost

# Step 2: Compile OCRLibEngine.cpp
echo "Compiling OCRLibEngine.cpp..."
g++ OCRLibEngine.cpp -o OCRLibEngine `pkg-config --cflags --libs opencv4` -ltesseract -llept -lboost_filesystem -lboost_system

# Step 3: Compile OCRLibrarian.cpp
echo "Compiling OCRLibrarian.cpp..."
g++ OCRLibrarian.cpp -o OCRLib `pkg-config --cflags --libs opencv4` -ltesseract -llept -lboost_filesystem -lboost_system

# Step 4: Create a directory for the program files
echo "Creating directory for program files..."
sudo mkdir -p /usr/local/OCRLibrarian

# Step 5: Copy the vocabulary text file into the program directory
echo "Copying vocabulary text file..."
sudo cp OCRVocabulary.txt /usr/local/OCRLibrarian/

# Step 6: Move the OCRLib and OCRLibEngine binaries into the program directory
echo "Moving OCRLib binary to /usr/local/OCRLibrarian..."
sudo mv OCRLib /usr/local/OCRLibrarian/
echo "Moving OCRLibEngine binary to /usr/local/OCRLibrarian..."
sudo mv OCRLibEngine /usr/local/OCRLibrarian/

# Step 7: Create symbolic links in /usr/local/bin for easy execution from anywhere
echo "Creating symbolic links in /usr/local/bin..."
sudo ln -sf /usr/local/OCRLibrarian/OCRLib /usr/local/bin/OCRLib
sudo ln -sf /usr/local/OCRLibrarian/OCRLibEngine /usr/local/bin/OCRLibEngine

# Final message
echo "Installation and setup complete! You can now run 'OCRLib' and 'OCRLibEngine' from anywhere."
