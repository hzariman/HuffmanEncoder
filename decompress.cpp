/*
 * Name: Hariz Megat Zariman 
 * Email: mqmegatz@ucsd.edu
 *
 * Sources Used: Huffman Lecture Slides, EDStem FAQ.
 *
 * This file provides the main workflow to decompress a file using
 * Huffman Encoding and writes the result to an output file. It handles
 * the input arguments, constructing a Huffamn Tree object and manipulation
 * of the Tree functions to accomplish this. This assumes that the compressed
 * file is written in the format: totalFrequency, serialization, encodings.
 */

#include <iostream>
#include <fstream>
#include <vector>

#include "HCTree.hpp"
#include "Helper.hpp"

/**
 * The Main function of the compress program, handling input
 * argument, reading an input file and compressing it to an output file.
 * 
 * @param argc the number of program arguments
 * @param argv the arguments
 * @return 0 if program successful, otherwise stderr.
 */
int main( int argc, char** argv) {

    const int expectedArgs = 3;
    const int secondArg = 2;

    // If we don't read the correct number of arguments, display an error
    // and return to stderr.
    if (argc != expectedArgs) {
        error("Incorrect parameters\n");
        return 1;
    }

    // The input and output streams.
    FancyInputStream* inputFile;
    FancyOutputStream* outputFile;
    // The Huffman Tree constructed from reading the file.
    HCTree* huffTree;

    // Open the input and output streams by using the input arguments.
    inputFile = new FancyInputStream(argv[1]);
    outputFile = new FancyOutputStream(argv[secondArg]);

    // Get the file size of the input stream.
    int inputfilesize = inputFile->filesize();

    // Read the total symbol frequency from the header of the compressed
    // file.
    int totalFreq = inputFile->read<int>();

    // Construct a new Huffman Tree
    huffTree = new HCTree();
    
    // Deserialize the tree by reading from the input stream
    // of the compressed file.
    huffTree->deserialize(inputfilesize - sizeof(int), *inputFile);

    //How many symbols have been decoded.
    int counter = 0;

    if (!inputFile->good()) {
        totalFreq = 0;
    }
    
    // while the number of symbols read is less than the total symbol freq.
    while (counter < totalFreq) {

        // The character to be decoded.
        unsigned char decodedChar;

        // Decode a character from the input stream, and write it
        // to the output stream.
        decodedChar = huffTree->decode(*inputFile);
        outputFile->write<char>(decodedChar);

        // Increment the symbol counter by one.
        counter++;
    }

    // Write everything from the output stream to the file itself.
    outputFile->flush();

    //Delete the tree, the input and output streams.
    delete(huffTree);
    delete(inputFile);
    delete(outputFile);
}