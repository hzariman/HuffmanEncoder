/*
 * Name: Hariz Megat Zariman
 * Email: mqmegatz@ucsd.edu
 *
 * Sources Used: Huffman Lecture Slides and Stepik 8.3.
 *
 * This file provides a skeleton for a huffman tree. It contains the fields
 * and methods used to build a huffman tree and encode/decode symbols. It 
 * also contains methods on how to seriliaze/deserialize a tree from an
 * input stream.
 */

#ifndef HCTREE_HPP
#define HCTREE_HPP
#include <queue>
#include <vector>
#include <fstream>
#include "Helper.hpp"
using namespace std;

/**
 * A Huffman Code Tree class
 */
class HCTree {
private:

    // The root and leaves of the huffman tree.
    HCNode* root;
    vector<HCNode*> leaves;

public:
    /**
     * Constructor, which initializes everything to null pointers
     */
    HCTree() : root(nullptr) {
        leaves = vector<HCNode*>(256, nullptr);
    }

    /**
     * Deconstructor, which deletes all nodes in the tree.
     */
    ~HCTree();

    /**
     * Use the Huffman algorithm to build a Huffman coding tree.
     * PRECONDITION:  freqs is a vector of ints, such that freqs[i] is the
     *                frequency of occurrence of byte i in the input file.
     * POSTCONDITION: root points to the root of the trie, and leaves[i]
     *                points to the leaf node containing byte i.
     *
     * @param freqs frequency vector
     */
    void build(const vector<int>& freqs);

    /**
     * Write to the given FancyOutputStream the sequence of bits coding the
     * given symbol.
     * PRECONDITION: build() has been called, to create the coding tree,
     *               and initialize root pointer and leaves vector.
     *
     * @param symbol symbol to encode
     * @param out output stream for the encoded bits
     */
    void encode(unsigned char symbol, FancyOutputStream & out) const;

    /**
     * Return symbol coded in the next sequence of bits from the stream.
     * PRECONDITION: build() has been called, to create the coding tree, and
     *               initialize root pointer and leaves vector.
     *
     * @param in input stream to find encoded bits
     * @return a single char decoded from the input stream
     */
    unsigned char decode(FancyInputStream & in) const;

    /**
     * Removes all nodes from the Huffman tree. It is called when
     * the tree destructor is used.
     * 
     * @param treeNode the current node we are looking at
     */
    void clear(HCNode* treeNode);

    /**
     * Serializes the path of the current node (from the root)
     * and writes it to the output stream.
     *
     * @param currNode the current node we are looking at (recursive)
     * @param out the output stream.
     */
    void serialization(HCNode* currNode, FancyOutputStream & out);

    /**
     * Represents the tree as its serialized verson and stores it in
     * the output stream by calling serialization() on the root node.
     * PRECONDITION: The output stream is functioning correctly
     *               and tree has more than one node (i.e there is at least
     *               one symbol).
     *
     * @param out the output stream.
     */
    void serialize(FancyOutputStream & out);

    /**
     * Deserializes the current index of the bitstring
     * from the input stream and creates the huffman tree node structure.
     * (Called recursively).
     * PRECONDITION: The input stream stores the serialized tree correctly
     *               and in the right location. The read header is in the
     *               correct location.
     *
     * @param index the current index of the bitstring.
     * @param len the length of the serialized tree bitstring.
     * @param in the input stream.
     * @param bitcounter how bits have been read
     */
    HCNode* deseriallization(int& index, int len, FancyInputStream & in, int& bitcounter);

    /**
     * Deserializes the huffman tree stored in the header of the input stream.
     * PRECONDITION: The input stream stores the serialized tree correctly
     *               and in the right location. The read header is in the
     *               correct location.
     *
     * @param len the length of the serialized tree bit string.
     * @param in the input stream.
     */
    void deserialize(int len, FancyInputStream & in);
};
#endif // HCTREE_HPP