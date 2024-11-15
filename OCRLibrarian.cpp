#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>

// Global Variables
const char *dir_path;
int rename_mode;

// Function to print ASCII art
void print_ascii_art(const int argc, const char *dir_path)
{
    std::cout << '\n';
    std::cout << "  ,ad8888ba,      ,ad8888ba,   88888888ba                       " << std::endl;
    std::cout << " d8\"'    `\"8b    d8\"'    `\"8b  88      \"8b                     " << std::endl;
    std::cout << "d8'        `8b  d8'            88      ,8P                     " << std::endl;
    std::cout << "88          88  88             88aaaaaa8P'                     " << std::endl;
    std::cout << "88          88  88             88\"\"\"\"88'                       " << std::endl;
    std::cout << "Y8,        ,8P  Y8,            88    `8b                       " << std::endl;
    std::cout << " Y8a.    .a8P    Y8a.    .a8P  88     `8b                      " << std::endl;
    std::cout << "  `\"Y8888Y\"'      `\"Y8888Y\"'   88      `8b                     " << std::endl;
    std::cout << "88           88  88                                               88" << std::endl;
    std::cout << "88           \"\"  88                                               \"\"" << std::endl;
    std::cout << "88               88                                              " << std::endl;
    std::cout << "88           88  88,dPPYba,   8b,dPPYba,  ,adPPYYba,  8b,dPPYba,  88  ,adPPYYba,  8b,dPPYba,   " << std::endl;
    std::cout << "88           88  88P'    \"8a  88P'   \"Y8  \"\"     `Y8  88P'   \"Y8  88  \"\"     `Y8  88P'   `\"8a" << std::endl;
    std::cout << "88           88  88       d8  88          ,adPPPPP88  88          88  ,adPPPPP88  88       88" << std::endl;
    std::cout << "88           88  88b,   ,a8\"  88          88,    ,88  88          88  88,    ,88  88       88" << std::endl;
    std::cout << "88888888888  88  8Y\"Ybbd8\"'   88          `\"8bbdP\"Y8  88          88  `\"8bbdP\"Y8  88       88" << std::endl
              << std::endl;
    std::cout << "Welcome to OCR librarian!\n"
              << std::endl
              << "This tool will scan a directory for image files and rename them using OCR."
              << std::endl;
    if (argc == 1)
    {
        std::string dir = std::filesystem::current_path().string();
        std::cout << "\033[31m" // Set text color to red
                  << "WARNING: You are attempting to run the program in your program directory (" << dir << "). "
                  << "The program will rename images in this directory.\nThis is likely not a good idea if this was not your intention. \nPlease type \"OCRLib --help\" for instructions\n"
                  << "\033[0m" // Reset text color
                  << std::endl;
    }
    else if (argc > 1)
    {
        std::string dir = std::filesystem::current_path().string();
        std::cout << "\033[32m" // Set text color to green
                  << "You are currently running the program in the specified directory (" << dir_path << "). "
                  << "The program will rename images in this directory. \nPlease type \"OCRLib --help\" for instructions\n"
                  << "\033[0m" // Reset text color
                  << std::endl;
    }
}

// Function to process each image (assuming OCR is a valid executable)
void process_image(const char *image_path, int rename_mode)
{
    // Construct the command to run OCR on the image file
    char command[1024];
    std::cout << "Attempting to pass " << image_path << "to OCRLibEngine...\n";
    snprintf(command, sizeof(command), "OCRLibEngine \"%s\" %d", image_path, rename_mode);
    system(command); // Execute the command
}

// Function to scan the directory for image files
void scan_directory(const char *dir_path, int rename_mode)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL)
    {
        perror("Unable to open directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Check if file has an image extension (e.g., .jpg, .png)
        const char *image_exts[] = {".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif", ".ppm", ".pgm", ".pbm", ".jp2", ".webp", NULL};
        for (int i = 0; image_exts[i] != NULL; i++)
        {
            if (strstr(entry->d_name, image_exts[i]) != NULL)
            {
                // Construct the full path for the image file
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
                // Process the image file
                process_image(full_path, rename_mode);
                break; // Found the file extension, no need to check further
            }
        }
    }

    closedir(dir);
}

void setRenameMode(std::string input)
{
    try
    {
        rename_mode = std::stoi(input);
        std::cout << "Rename mode set to " << rename_mode << "\n"
                  << std::endl;
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid input: not a number. Exitting..." << std::endl;
        exit(1);
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Invalid input: number out of range. Exitting..." << std::endl;
        exit(1);
    }
}

void displayHelp()
{
    std::cout << "\nOCRLib Usage:\n";
    std::cout << "    OCRLib [dir] [-M]\n\n";
    std::cout << "Description:\n";
    std::cout << "    Processes every image in a specified directory, performing OCR on each image and renaming it according to the specified mode (-M).\n";
    std::cout << "    Supported image formats: .jpg, .jpeg, .png, .bmp, .tiff, .tif, .ppm, .pgm, .pbm, .jp2, .webp\n\n";

    std::cout << "Options:\n";
    std::cout << "    dir     Directory where images are read and renamed by the program.\n";
    std::cout << "            If a directory is not specified, the current directory is used.\n\n";
    std::cout << "    -M      Mode for renaming:\n";
    std::cout << "              0 - Rename to the full OCR result, with words separated by underscores.\n";
    std::cout << "              1 - Rename to the longest word found in the OCR result.\n\n";

    std::cout << "Example:\n";
    std::cout << "    OCRLib /path/to/images 1\n\n";
}

int main(int argc, char *argv[])
{

    if (argc > 1)
    {
        std::string arg = argv[1];
        if (arg == "--help")
        {
            displayHelp();
            return 0;
        }
        else
        {
            dir_path = argv[1]; // Directly assign argv[1] to dir_path
        }
    }
    else
    {
        dir_path = "."; // Use current directory if no argument is provided
    }
    if (argc == 3)
    {
        print_ascii_art(argc, dir_path);
        setRenameMode(argv[2]);
    }
    else
    {
        // Display the ASCII art
        print_ascii_art(argc, dir_path);

        // Prompt user for rename_mode (an integer)
        std::string input;
        std::cout << "Please enter a rename mode: " << std::endl;
        std::cout << "Rename to the full OCR result with underscores: (0)" << std::endl;
        std::cout << "Rename to the Longest word found: (1)" << std::endl;
        std::cin >> input; // Take user input for rename_mode

        if (input == "--help")
        {
            displayHelp();
            return 0;
        }
        else
        {
            setRenameMode(input);
        }
    }

    std::cout << "\nInitializing Directory Read\n";
    // Start scanning the directory for image files and process with OCR
    scan_directory(dir_path, rename_mode);
    std::cout << "\nOCR Librarian has completed running." << std::endl;
    return 0;
}
