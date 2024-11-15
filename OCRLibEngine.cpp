#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <algorithm>
#include <regex>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

// Forward declaration of the loadDictionary function
std::set<std::string> loadDictionary(const std::string &filename);

// Global dictionary variable
std::set<std::string> dictionary = loadDictionary("/usr/local/bin/OCRVocabulary.txt");

std::string filterAlphabetsOnly(const std::string &input)
{
    return std::regex_replace(input, std::regex("[^A-Za-z ]"), "");
}

std::string splitAndRemoveDuplicates(const std::string &ocrText)
{
    std::istringstream iss(ocrText);
    std::unordered_set<std::string> uniqueWords;
    std::string word;
    std::string result;

    while (iss >> word)
    {
        if (uniqueWords.find(word) == uniqueWords.end())
        {
            uniqueWords.insert(word);
            result += word + " ";
        }
    }

    if (!result.empty())
    {
        result.pop_back();
    }

    return result;
}

std::string toUpperCase(const std::string &input)
{
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(), ::toupper);
    return output;
}

std::set<std::string> loadDictionary(const std::string &filename)
{
    std::set<std::string> dict;
    std::ifstream file(filename);
    std::string word;

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
        return dict;
    }

    while (file >> word)
    {
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);
        dict.insert(word);
    }

    return dict;
}

std::string removeNonWords(const std::string &text)
{
    std::istringstream iss(text);
    std::string word;
    std::string result;

    while (iss >> word)
    {
        std::transform(word.begin(), word.end(), word.begin(), ::toupper);

        if (dictionary.find(word) != dictionary.end())
        {
            result += word + " ";
        }
    }

    if (!result.empty())
    {
        result.pop_back();
    }

    return result;
}

std::string processOCRResult(const std::string &ocrText)
{
    std::string filteredText = filterAlphabetsOnly(ocrText);
    filteredText = splitAndRemoveDuplicates(filteredText);
    filteredText = toUpperCase(filteredText);
    filteredText = removeNonWords(filteredText);
    return filteredText;
}

bool isBimodal(const cv::Mat &hist)
{
    int peaks = 0;
    for (int i = 1; i < hist.rows - 1; ++i)
    {
        float prev = hist.at<float>(i - 1);
        float curr = hist.at<float>(i);
        float next = hist.at<float>(i + 1);

        if (curr > prev && curr > next)
        {
            peaks++;
            if (peaks > 1)
                return true;
        }
    }
    return false;
}

std::string processNewName(const std::string renameSetting, const std::string &nameInput)
{
    int convertedRenameSetting = std::stoi(renameSetting);
    switch (convertedRenameSetting)
    {
    case (0):
        // Replace all whitespaces with underscores
        {
            std::string result = nameInput;
            for (char &ch : result)
            {
                if (ch == ' ')
                {
                    ch = '_';
                }
            }
            return result;
        }

    case (1):
        // Find the longest word in the input string
        {
            std::istringstream stream(nameInput);
            std::string word;
            std::string longestWord;

            while (stream >> word)
            {
                if (word.length() > longestWord.length())
                {
                    longestWord = word;
                }
            }
            return longestWord;
        }

    default:
        // Invalid renameSetting, exit program
        std::cerr << "Invalid renameSetting: " << renameSetting << std::endl;
        exit(1);
    }
}

cv::Mat deskew(const cv::Mat &src)
{
    cv::Mat m = cv::Mat::zeros(2, 3, CV_32F);
    cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
    cv::Mat rotated;

    cv::getRotationMatrix2D(center, 0, 1).copyTo(m);
    cv::warpAffine(src, rotated, m, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255));
    return rotated;
}

// Function to rename the file and return recognized text
void RenameFile(const std::string &filePath, const std::string &newTitle)
{
    // Extract the file extension
    std::string extension = fs::path(filePath).extension().string();

    // Create the initial new file name using the provided newTitle and the original extension
    std::string newFileName = newTitle + extension;

    // Check if the file already exists
    fs::path newFilePath = fs::path(filePath).parent_path() / newFileName;
    int counter = 1;

    // If the file exists, append a counter to the new file name until a unique name is found
    while (fs::exists(newFilePath))
    {
        newFileName = newTitle + "_" + std::to_string(counter++) + extension;
        newFilePath = fs::path(filePath).parent_path() / newFileName;
    }

    // Rename the file
    try
    {
        fs::rename(filePath, newFilePath);
        std::cout << "\033[34m" << "File " << filePath << " successfully renamed to: ./" << newFileName << "\033[0m" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error renaming file: " << filePath << " " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: OCR <image_file> <renameMode>" << std::endl;
        return 1;
    }

    std::string imagePath = argv[1];
    std::string renameMode = argv[2];

    tesseract::TessBaseAPI tess;
    if (tess.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY))
    {
        std::cerr << "Could not initialize tesseract." << std::endl;
        return 1;
    }

    cv::Mat img = cv::imread(imagePath);

    if (img.empty())
    {
        std::cerr << "Could not open or find the image." << std::endl;
        return 1;
    }

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    gray = deskew(blurred);

    cv::Scalar mean, stddev;
    cv::meanStdDev(gray, mean, stddev);

    cv::Mat binarized;
    if (stddev[0] < 30)
    {
        cv::Mat claheOutput;
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(4);
        clahe->apply(gray, claheOutput);
        cv::threshold(claheOutput, binarized, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    }
    else
    {
        cv::Mat hist;
        int histSize = 256;
        float range[] = {0, 255};
        const float *histRange = {range};
        cv::calcHist(&gray, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

        if (isBimodal(hist))
        {
            cv::threshold(gray, binarized, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        }
        else
        {
            cv::adaptiveThreshold(gray, binarized, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);
        }
    }

    cv::Mat morphResult;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(binarized, morphResult, cv::MORPH_CLOSE, element);

    // if (!cv::imwrite("processed_image.png", morphResult))
    // {
    //     std::cerr << "Failed to save the processed image." << std::endl;
    //     return 1;
    // }

    // std::cout << "Processed image saved as 'processed_image.png'." << std::endl;

    Pix *pix = pixCreate(morphResult.cols, morphResult.rows, 32);
    memcpy(pixGetData(pix), morphResult.data, morphResult.total());

    tess.Clear();
    tess.SetImage(pix);

    std::string outText = tess.GetUTF8Text();
    std::string cleanText = processOCRResult(outText);
    cleanText = processNewName(renameMode, cleanText);

    RenameFile(imagePath, cleanText);

    pixDestroy(&pix);
    tess.End();

    return 0;
}
