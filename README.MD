# OCR-Librarian
OCR Librarian is a Linux C++ CLI program that allows users to quickly and efficiently rename image folders in bulk using open source OCR technology. 

Supports the Following image formats:
* .jpg, .jpeg, .png, .bmp, .tiff, .tif, .ppm, .pgm, .pbm, .jp2, .webp

![alt text](https://github.com/zzz-Ricky/OCR-librarian/blob/main/images/OCRLibDemo.gif?raw=true)


# OCRLibrarian Installation Guide

OCRLibrarian is a tool that utilizes OpenCV and Tesseract OCR to process images. Follow the instructions below to install and set up the program on your Debian-based Linux system 
* (Ubuntu, Linux Mint, Tails, Proxmox, Kali Linux, Pardus, TrueNAS SCALE, and Astra Linux).

## Prerequisites

Before proceeding with the installation, ensure that you have the following:

- A working Debian-based system (Ubuntu).
- A terminal with sudo privileges.

If you wish to run this program on an Arch based Linux system, there exists an alternative OCRLibrarian_Arch.sh installation script in the source code. This version is however unverified for functionality. Feel free to experiment with the script for the needs of your system. All installation instructions should remain the same.

## Installation Instructions

### 1. Clone the Repository

First, clone the repository to your local machine. Open a terminal and run the following command:

```bash
git clone https://github.com/zzz-Ricky/OCR-librarian.git
```

### 2. Navigate to the Repository
Once the repository is cloned, change into the project directory:
```
cd OCRLibrarian
```
### 3. Run the Installation Script
The installation script will install necessary dependencies, compile the required code, and set up the program.

Run the installation script with the following commands:
```
chmod +x OCRLibrarian.sh  # Make the script executable
```
```
./OCRLibrarian.sh # Execute the script
```
This will:

* Install required dependencies (e.g., OpenCV, Tesseract, and Leptonica).
* Compile the C++ source files (OCRLibEngine.cpp and OCRLibrarian.cpp).
* Move the binaries and the vocabulary text file to /usr/local/OCRLibrarian/.
* Create symbolic links in /usr/local/bin/ for easy access to the OCRLib and OCRLibEngine binaries.

If you are using an Arch-based system, please run the following instead:
```
chmod +x OCRLibrarian_Arch.sh  # Make the script executable
```
```
./OCRLibrarian_Arch.sh # Execute the script
```
### 4. Verify the Installation
```
OCRLib --help
```
if everything was set up correctly, this command should display usage information for the tool.

### 5. Running the Program
Once installed, you can run the OCRLibrarian program from anywhere in the terminal by simply typing:
```
OCRLib <Directory> <Mode>
```
Please refer to the --help display for additional usage instructions

### 6. (Optional) Remove the Program

If you wish to uninstall the program at any time, simply delete the binaries and remove the symbolic links:
```bash
sudo rm /usr/local/bin/OCRLib
sudo rm /usr/local/bin/OCRLibEngine
sudo rm -r /usr/local/myprogram
```
### Notes:
Ensure that OCRVocabulary.txt is in the correct location (/usr/local/myprogram/) as required by the program.
You can modify the OCRLibrarian.sh script to change the installation paths if necessary.

### Troubleshooting

If the script fails to install dependencies, ensure your package list is up to date by running:
```
sudo apt update
```
Or on Arch linux:
```
pacman -Syu
```
If you encounter any other issues, check the OCRLibrarian.cpp and OCRLibEngine.cpp scripts for missing dependencies or incorrect paths using your favorite IDE. If any are underlined red, then they are worth double checking.

For instance, the following import statement
```
#include <opencv4/opencv2/opencv.hpp>
```
may be incorrect for your current Linux installation, to verify your paths, navigate to your binary folder and verify that the #include calls all point to existing files and locations in the following directory
```
/usr/include
```
In this case, verify that
```
/usr/include/opencv4/opencv2/opencv.hpp
```
exists on your system.
* If possible, modify the .cpp files to match your system or install the correct dependencies if they have changed in your Linux branch.

# Acknowledgements

This project utilizes the word list from the following sources:

- [English Words from dwyl/english-words](https://github.com/dwyl/english-words)
- [OpenCV](https://github.com/opencv/opencv)
- [Tesseract OCR](https://github.com/tesseract-ocr/tesseract)

These resources have been invaluable in the development of this project.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
