#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{
    double luminosityThreshold = 0.5 * 255;

    string inputFileName;

    std::cout << "Name of the picture file (exclude file extension): ";
    std::cin >> inputFileName;

    ifstream image(inputFileName + ".ppm");
    ofstream mask(inputFileName + "Mask.ppm");
    ofstream newImage(inputFileName + "Sorted.ppm");
    ofstream luminositySortedImage(inputFileName + "SortedLuminosity.ppm");
    ofstream luminosityImage(inputFileName + "luminosity.ppm");

    if (!image.is_open())
    {
        cout << "Encountered an error when opening the input file. Terminating the program" << endl;
        return 1;
    }

    if (!mask.is_open())
    {
        cout << "Encountered an error when opening the mask file. Terminating the program" << endl;
        return 1;
    }

    if (!newImage.is_open())
    {
        cout << "Encountered an error when opening the newImage file. Terminating the program" << endl;
        return 1;
    }

    if (!luminositySortedImage.is_open())
    {
        cout << "Encountered an error when opening the luminositySortedImage file. Terminating the program" << endl;
        return 1;
    }

    if (!luminosityImage.is_open())
    {
        cout << "Encountered an error when opening the luminosityImage file. Terminating the program" << endl;
        return 1;
    }

    image.ignore(256, '\n');

    int width, height, maxValue;

    image >> width >> height >> maxValue;

    std::cout << width << ' ' << height << ' ' << maxValue << endl;

    mask << "P1" << endl << width << ' ' << height << endl;

    double* luminosityData = new double[width * height];
    int* maskData = new int[width * height];
    int* colorData = new int[width * height * 3];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int r, g, b;

            image >> r >> g >> b;

            double l = (min({ r, g, b }) + max({ r, g, b })) / 2;

            if (l > luminosityThreshold)
            {
                mask << 0 << ' ';
                maskData[i * width + j] = 0;
            }


            else
            {
                mask << 1 << ' ';
                maskData[i * width + j] = 1;
            }

            luminosityData[i * width + j] = l;
            colorData[(i * width + j) * 3] = r;
            colorData[(i * width + j) * 3 + 1] = g;
            colorData[(i * width + j) * 3 + 2] = b;


        }
    }

    luminosityImage << "P2" << endl << width << ' ' << height << endl << 255 << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            luminosityImage << luminosityData[(i * width + j)] << ' ';
        }
    }

    for (int i = 0; i < width; i++)
    {
        int mPrevious = -1;

        for (int j = 0; j < height; j++)
        {
            int m = maskData[j * width + i];

            if (((mPrevious == -1 || mPrevious == 1) && m == 0))
            {
                int start = j * width + i, length = 0;

                while ((j + length) < height && maskData[(j + length) * width + i] != 1)
                {
                    length++;
                }

                for (int k = start; k < ((length - 1) * width + start); k += width)
                {
                    int minElement = 256;
                    int minElementPos = 0;

                    for (int l = k; l < (length * width + start); l += width)
                    {
                        if (luminosityData[l] < minElement)
                        {
                            minElement = luminosityData[l];
                            minElementPos = l;
                        }
                    }

                    int tempLuminosity, tempR, tempG, tempB;

                    tempLuminosity = luminosityData[k];
                    luminosityData[k] = minElement;
                    luminosityData[minElementPos] = tempLuminosity;
                    tempR = colorData[k * 3];
                    tempG = colorData[k * 3 + 1];
                    tempB = colorData[k * 3 + 2];

                    colorData[k * 3] = colorData[minElementPos * 3];
                    colorData[k * 3 + 1] = colorData[minElementPos * 3 + 1];
                    colorData[k * 3 + 2] = colorData[minElementPos * 3 + 2];

                    colorData[minElementPos * 3] = tempR;
                    colorData[minElementPos * 3 + 1] = tempG;
                    colorData[minElementPos * 3 + 2] = tempB;

                }

            }

            mPrevious = m;
        }
    }

    luminositySortedImage << "P2" << endl << width << ' ' << height << endl << 255 << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            luminositySortedImage << luminosityData[(i * width + j)] << ' ';
        }
    }

    newImage << "P3" << endl << width << ' ' << height << endl << 255 << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            newImage << colorData[(i * width + j) * 3] << ' ' << colorData[(i * width + j) * 3 + 1] << ' ' << colorData[(i * width + j) * 3 + 2] << ' ';
        }
    }

    image.close();
    newImage.close();
    mask.close();
    luminositySortedImage.close();
    luminosityImage.close();

    delete[] luminosityData;
    delete[] maskData;
    delete[] colorData;

    std::cout << "pixel sorted";

    return 0;
}