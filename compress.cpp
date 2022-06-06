/*
 * Name: Hariz Megat Zariman 
 * Email: mqmegatz@ucsd.edu
 *
 * Sources Used: Huffman Lecture Slides , EDStem FAQ.
 *
 * This file provides the main workflow to compress a file using
 * Huffman Encoding and writes the result to an output file. It handles
 * the input arguments, constructing a Huffamn Tree object and manipulation
 * of the Tree functions to accomplish this.
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

    // Constants for styling purposes.
    const int expectedArgs = 3;
    const int secondArg = 2;
    const int maxFreq = 256;

    // If we don't read the correct number of arguments, display an error
    // and return to stderr.
    if (argc != expectedArgs) {
        error("Incorrect parameters\n");
        return 1;
    }

    // The input and output streams.
    FancyInputStream* inputFile;
    FancyOutputStream* outputFile;

    // The unsigned char representation of the byte we are reading.
    unsigned char nextChar; 
    // Vector of all possible symbols and frequency
    vector<int> symFreq(maxFreq);
    
    // The Huffman Tree constructed from reading file.
    HCTree* huffTree;

    // Open the input stream from the first argument of the program.
    inputFile = new FancyInputStream(argv[1]);

    // Obain a byte from the file
    nextChar = inputFile->read<unsigned char>();

    // Read the input file until we reach the end
    while (inputFile->good()){

        //Increment the frequency at the char index
        symFreq[nextChar]++;

        // Read the next byte.
        nextChar = inputFile->read<unsigned char>();
    }

    // Contruct a new Huffman Tree
    huffTree = new HCTree();
    // Build its internal node structure using the frequency table.
    huffTree->build(symFreq);
    
    // Open the output stream from the second argument of the program.
    outputFile = new FancyOutputStream(argv[secondArg]);

    // Serialize the tree and write it to the output stream.
    huffTree->serialize(*outputFile);

    // Start at the beggining of the input stream.
    inputFile->reset();

    // Read the next byte from the input stream.
    nextChar = inputFile->read<unsigned char>();
    
    // Read the input file until we reach the end.
    while (inputFile->good()){
        
        // Encode each symbol we read from the input stream,
        // and write the huffman encoding of it to the output stream.
        huffTree->encode(nextChar, *outputFile);

        // Read the next byte from the input file.
        nextChar = inputFile->read<unsigned char>();
    
    }

    // Write everything from the output buffer to the output file.
    outputFile->flush();

    // Delete the tree, input stream and output stream objects.
    delete(huffTree);
    delete(inputFile);
    delete(outputFile);


}