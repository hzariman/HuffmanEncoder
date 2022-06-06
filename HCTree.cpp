/*
 * Name: Hariz Megat Zariman 
 * Email: mqmegatz@ucsd.edu
 *
 * Sources Used: Huffman Lecture Slides and Stepik 8.3.
 *
 * This file provides the implementation for the methods declared in the
 * huffman tree. It contains the fields and methods used to build a huffman
 * tree and encode/decode symbols. It also contains methods on how to 
 * seriliaze/deserialize a tree from an input stream.
 */

#include "HCTree.hpp"
#include <algorithm>

    /**
     * Deconstructor, which deletes all nodes in the tree.
     */
HCTree::~HCTree() {
    
    // Clears all nodes in the tree, recursively starting
    // from the root.
    clear(root);
}

/**
 * Use the Huffman algorithm to build a Huffman coding tree.
 * PRECONDITION:  freqs is a vector of ints, such that freqs[i] is the
 *                frequency of occurrence of byte i in the input file.
 * POSTCONDITION: root points to the root of the trie, and leaves[i]
 *                points to the leaf node containing byte i.
 *
 * @param freqs frequency vector
 */
void HCTree::build(const vector<int>& freqs) {

    // Queue to store the Huffman node trees in min-heap order
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

    // The number of unique symbols with frequencies
    int freqTableSize = freqs.size();

    // For every symbol in the frequency vector that has a frequency
    // larger than 0, create a node (storing its symbol and freq) and
    // push it to the priority queue (pq).
    for (int i = 0; i < freqTableSize; i++) {
        if (freqs[i] != 0) {
            HCNode* h = new HCNode(freqs[i], (unsigned char)i);

            // Store the single-node trees in min-heap order
            pq.push(h);

            // Set the leaves vector at that index to the new node.
            leaves[i] = h;
        }
    }



    // Huffman algorithm: while there are more than two elements in the pq,
    // combine the two smallest trees (frequency) into a new tree
    // and push back into the pq.
    while (pq.size() > 1) {

        // The smallest tree in the pq.
        HCNode* tree1 = pq.top();
        pq.pop();

        // The second smallest tree in the pq.
        HCNode* tree2 = pq.top();
        pq.pop();

        // combine their frequencies
        int freqSum = tree1->count + tree2->count;

        // Create a new node, and make it the parent of the
        // two smallest trees, combining into a larger tree.
        HCNode* h = new HCNode(freqSum, '`');
        h->c0 = tree1;
        h->c1 = tree2;
        tree1->p = h;
        tree2->p = h;

        // Push the new tree into the pq.
        pq.push(h);
    }

    // Once there is only one tree left in the pq, set the root
    // of this Huffman tree to it.
    if (pq.size() != 0) {
        root = pq.top();
    } else {
        root = nullptr;
    }
    

}

/**
 * Write to the given FancyOutputStream the sequence of bits coding the
 * given symbol.
 * PRECONDITION: build() has been called, to create the coding tree,
 *               and initialize root pointer and leaves vector.
 *
 * @param charToEncode symbol to encode
 * @param out output stream for the encoded bits
 */
void HCTree::encode(unsigned char charToEncode, FancyOutputStream & out)const{

    // Obtain the node in the tree that stores the symbol.
    HCNode* currNode = leaves[(int)charToEncode];

    // Bit buffer to keep track of tree traversal
    vector<int> tbuf;

    // From the leaf, traverse to the root of the tree
    // keeping track of how to get there (i.e if the node
    // was a left child, then store a 0 to the buffer and
    // 1 if it was a right child)
    while (currNode != root){
        if (currNode == currNode->p->c0) {
            tbuf.push_back(0);
        } else if (currNode==currNode->p->c1) {
            tbuf.push_back(1);
        }
        
        // Set the current node we are looking at to it's parent.
        currNode = currNode->p;
        
    }

    // The buffer stores the traversal order from leaf to root,
    // so reverse it to get from root to leaf.
    reverse(tbuf.begin(), tbuf.end());

    // Go over every bit in the buffer and write it to the output stream.
    for (auto itr = tbuf.begin(); itr!= tbuf.end(); itr++){
        out.write_bit(*itr);
    }
    
    
    
}

/**
 * Return symbol coded in the next sequence of bits from the stream.
 * PRECONDITION: build() has been called, to create the coding tree, and
 *               initialize root pointer and leaves vector.
 *
 * @param in input stream to find encoded bits
 * @return a single char decoded from the input stream
 */
unsigned char HCTree::decode(FancyInputStream & in) const{
    // Set the current node we are looking at to the root.
    HCNode* currNode = root;
    
    // Iterate over the tree until we reach a leaf node.
    while (currNode->c0 != nullptr && currNode->c1 != nullptr){

        // The bit we obtained from the input stream.
        int bit = in.read_bit();

        // If the bit is a 0, traverse to the left of the tree, else
        /// traverse right.
        if (bit == 0){
            currNode = currNode->c0;
        } else {
            currNode = currNode->c1;
        }
    
    }

    // Return the symbol stored at the leaf node.
    return currNode->symbol;


}

/**
 * Removes all nodes from the Huffman tree. It is called when
 * the tree destructor is used.
 * 
 * @param treeNode the current node we are lookign at.
 */
void HCTree::clear(HCNode* treeNode) {

    // Base case: if we already reached a leaf node, return
    if (treeNode ==  nullptr) {
        return;
    }

    // Recursively delete the left and right child
    clear(treeNode->c0);
    clear(treeNode->c1);

    // Delete the current node itself.
    delete(treeNode);
}

/**
 * Serializes the path of the current node (from the root)
 * and writes it to the output stream.
 *
 * @param currNode the current node we are looking at (recursive)
 * @param out the output stream.
 */
void HCTree::serialization(HCNode* currNode, FancyOutputStream & out){

    const int byteMSBIndex = 7;
    // If the current node we are looking at is the root, write its
    // frequency (i.e the total frequency of the file) to the header
    // of the output stream to store it.

    if (root == nullptr) {
        return;
    }

    if (currNode == root) {
        out.write<int>(currNode->count);
    }
    
    // Base case: if we reach a null reference then return.
    if (currNode == nullptr) {
        return;
    }

    // If we reach a leaf node, write a '1'
    // followed by a byte representation of the symbol to the output stream.
    // (Serialization algorithm seen in lecture)
    if (currNode->c0 == nullptr && currNode->c1 == nullptr){
        out.write_bit(1);

        // The character we are going to write to the output stream
        // in bits.
        char nodeChar = currNode->symbol;

        // Read the character (8 bits) from left (MSB) to right (LSB),
        // and write each bit to the output stream.
        for (int i = byteMSBIndex; i >= 0; i--) {

            //Bitwise operation to check if bit at ith position
            //from the right is a 1, then write a 1 and 0 otherwise.
            if (((nodeChar & (1<<i)) >> i) == 1) {
                out.write_bit(1);
            
            } else {
                out.write_bit(0);
            }
        }

    } else {

        // If we havent reached a leaf node, write a '0' bit to the output.
        out.write_bit(0);
    }

    // Recursively call this function on the left and right child.
    serialization(currNode->c0, out);
    serialization(currNode->c1, out);
}

/**
 * Represents the tree as its serialized verson and stores it in
 * the output stream by calling serialization() on the root node.
 * PRECONDITION: The output stream is functioning correctly
 *               and tree has more than one node (i.e there is at least
 *               one symbol).
 *
 * @param out the output stream.
 */
void HCTree::serialize(FancyOutputStream & out) {

    // Call serialization() starting at the root of the tree.
    serialization(root, out);

    // Write the buffer to the output file (including any padding that
    // is needed)
    out.flush();
}

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
HCNode* HCTree::deseriallization(int& index, int len, FancyInputStream& in, int &bitcounter){

        // If we reach an index that is greater than the bitstring than return null.
        if (index >= len) return nullptr;

        // Read a bit from the input stream.
        int bit = in.read_bit();

        // Increment our bitcounter
        bitcounter++;

        // If the bit we read is a 0, then construct a empty node
        // and recursively call this function to create its
        // left and right children.
        if (bit == 0){
            HCNode* curr = new HCNode(0, '`');
            curr->c0= deseriallization(++index, len, in, bitcounter);
            curr->c1= deseriallization(++index, len, in, bitcounter);
            return curr;
        
        // If the bit we read is a 1, then the next 8 bits represent
        // a symbol. Obtain the symbol
        } else {
            
            // The Symbol c we are obtaining.
            unsigned char decodedChar = 0;

            // Iterates 8 times to get 8 bits
            for (int i = 7; i >= 0; i--) {

                // The bit we obtain from the input stream
                bit = in.read_bit();
                // Increment the bit counter (how many bits we've read)
                bitcounter++;

                // If the bit is 1, set the bit at the ith position
                // from the right of symbol c to 1 and 0 otherwise
                if (bit == 1) {
                    decodedChar = (decodedChar&~(1<<i)) | ((1<<i));
                } else {
                    decodedChar = (decodedChar&~(1<<i)) | ((0<<i));
                }
            }

            // Create a new leaf node for this symbol.
            HCNode* curr = new HCNode(0, decodedChar);
            leaves[(int)decodedChar] = curr;

            // Return the node (because of the recursive call earlier).
            return curr;
        }  
}

/**
 * Deserializes the huffman tree stored in the header of the input stream.
 * PRECONDITION: The input stream stores the serialized tree correctly
 *               and in the right location. The read header is in the
 *               correct location.
 *
 * @param len the length of the serialized tree bit string.
 * @param in the input stream.
 */
void HCTree::deserialize(int len, FancyInputStream & in) {

    // The start of the bitstring that represents the tree
    int index = 0;

    // How many bits we will read because of the deserialization
    int bitcounter = 0;

    // Set the root node to the node returned from deserialization.
    root = deseriallization(index, len, in, bitcounter);
    
    // Using bit counter, see how many times we read a bit and mod
    // it by 8 to see how far into a byte we have already read (1-8). 
    // Then, because of padding we need to read the extra padded '0' bits
    // to align our readheader to the start of the next byte.
    const int bytesize = 8;

    for (int i = 0; i < (bytesize-(bitcounter%bytesize)); i++){
        in.read_bit();
    }
}