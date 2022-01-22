/*
  greedy: huffman-codes

  Huffman coding is a lossless data compression algorithm (prefix
  code). The idea is to assign variable-length codes to input
  characters, lengths of the assigned codes are based on the
  frequencies of corresponding characters. The most frequent character
  gets the smallest code and the least frequent character gets the
  largest code.  The variable-length codes assigned to input
  characters are Prefix Codes, means the codes (bit sequences) are
  assigned in such a way that the code assigned to one character is
  not the prefix of code assigned to any other character. This is how
  Huffman Coding makes sure that there is no ambiguity when decoding
  the generated bitstream.  Let us understand prefix codes with a
  counter example. Let there be four characters a, b, c and d, and
  their corresponding variable length codes be 00, 01, 0 and 1. This
  coding leads to ambiguity because code assigned to c is the prefix
  of codes assigned to a and b. If the compressed bit stream is 0001,
  the de-compressed output may be “cccd” or “ccb” or “acd” or “ab”.

  See this for applications of Huffman Coding.

  There are mainly two major parts in Huffman Coding

  1. Build a Huffman Tree from input characters.
  2. Traverse the Huffman Tree and assign codes to characters.


  Given

  A set of symbols and their weights (usually proportional to
  probabilities).


  Find

  A prefix-free binary code (a set of codewords) with minimum expected
  codeword length (equivalently, a tree with minimum weighted path
  length from the root).


  Here, we consider the problem of designing a binary character code
  in which each character is represented by a unique binary string,
  which we call a codeword. If we use a fixed-length code, we need 3
  bits to represent 6 characters: a = 000, b = 001,...f = 101, this
  method requires 300 000 bits to code the entire file. Can we do
  better?
  A variable-length code can do considerably better than a
  fixed-length code, by giving frequent characters short codewords and
  infrequent characters long codewords.


  Example

  X = { a, b, c, d, e, f } with binary alphabet C = { 1, 0 } shall be
  compressed without losss, at a given text we discover the following
  frequency of each element of X.

   letter | a    | b    | c    | d    | e    | f
  --------+------+------+------+------+------+----
   freq   | 0.05 | 0.09 | 0.12 | 0.13 | 0.16 | 0.45


   this builds up the following huffman tree, each node also stored in
   the heap (array)

  a:0.05 --+ 0
           +-0.14-+ 0
  b:0.09 --+ 1    |
                  +---0.30--+ 1
  e:0.16 ---------+ 1       |
                            +--0.55---+ 1
  c:0.12 ------+ 0          |         |
               +----0.25----+ 0       |
  d:0.13 ------+ 1                    +---1.00-
                                      |
  f:0.45 -----------0.45--------------+ 0


  going through the tree, we build up the following codebook, where
  more frequent letters have smaller codes and less frequent letters
  have longer codes

  codebook:

  f: 0
  c: 100
  d: 101
  a: 1100
  b: 1101
  e: 111

  at naive encoding we would need
    ln(6)/ln(2) = 2.58 bit per symbol

  where at huffman we need:
    0.05 * 4 + 0.09 * 4 + 0.12 * 3 + 0.13 * 3 + 0.16 * 3 + 0.45 * 1 =
    = 2.24 bit per symbol

  entropy H(X) = -(0.05 * ln(0.05)/ln(2)
                 + 0.09 * ln(0.09)/ln(2)
                 + 0.12 * ln(0.12)/ln(2)
                 + 0.13 * ln(0.13)/ln(2)
                 + 0.16 * ln(0.16)/ln(2)
                 + 0.45 * ln(0.45)/ln(2)) =

	       = -(-0.2161 -0.3127 -0.3671 -0.3826 -0.4230 -0.5184 =
               = 2.22 bit per symbol


  encoding/decoding

  encoding and decoding work based on the codebook, but is not
  implemented here


  references:
  https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
  https://en.wikipedia.org/wiki/Huffman_coding
 */

#include "greedy-huffman.h"

// some additional headers needed for matrix_create()
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


/* utils */

void greedy_debug(const char* format, ...)
{
#ifdef DEBUG
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
#endif
}

void greedy_failure(const char* format, ...)
{
	perror("failed!");
	va_list arglist;
	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

void greedy_huffman_print(huffman_node_p root, int arr[], int top)
{
	if (root->left) {
		arr[top] = 0; // left edge
		greedy_huffman_print(root->left, arr, top + 1);
	}

	if (root->right) {
		arr[top] = 1; // right edge
		greedy_huffman_print(root->right, arr, top + 1);
	}

	// if this is a leaf node, then it contains one of the input
	// characters, print the character and its code
	if (is_leaf(root)) {
		greedy_debug("%c: ", root->data);
		for (int idx = 0; idx < top; ++idx) {
			greedy_debug("%d", arr[idx]);
		}
		greedy_debug("\n");
	}
}

void greedy_huffman_destroy(huffman_node_p root)
{
	if (root->left) {
		greedy_huffman_destroy(root->left);
	}

	if (root->right) {
		greedy_huffman_destroy(root->right);
	}

	if (is_leaf(root)) {
		free(root); root = NULL;
	}
}

void swap(huffman_node_p *a, huffman_node_p *b)
{
	huffman_node_p tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}


// greedy

huffman_node_p new_node(char data, unsigned freq)
{
	huffman_node_p temp = NULL;
	temp = malloc(sizeof(*temp));
	if (!temp) {
		greedy_failure("%s:%d, %s() - allocation failed",
			       __FILE__, __LINE__, __func__);
	}

	temp->left = temp->right = NULL;
	temp->data = data;
	temp->freq = freq;

	return temp;
}

huffman_heap_p create_heap(unsigned capacity)
{
	huffman_heap_p heap;
	heap = malloc(sizeof(*heap));
	if (!heap) {
		greedy_failure("%s:%d, %s() - allocation failed",
			       __FILE__, __LINE__, __func__);
	}

	heap->size = 0;
	heap->capacity = capacity;
	heap->array = calloc(heap->capacity, sizeof(*heap->array));
	if (!heap->array) {
		greedy_failure("%s:%d, %s() - allocation failed",
			       __FILE__, __LINE__, __func__);
	}

	return heap;
}

void destroy_heap(huffman_heap_p *heap)
{
	if (!*heap) {
		return;
	}
	huffman_heap_p pheap = *heap;
	if (pheap->array) {
		for (int idx = pheap->size-1; idx >= 0; idx--) {
			free(pheap->array[idx]);
		}
		free(pheap->array);
		pheap->array = NULL;
	}
	free(*heap); *heap = NULL;
}

void huffman_heapify(huffman_heap_p heap, int idx)
{
	int smallest = idx;
	int left = 2 * idx + 1;
	int right = 2 * idx + 2;

	if (left < heap->size &&
	    heap->array[left]->freq < heap->array[smallest]->freq) {
		smallest = left;
	}

	if (right < heap->size &&
	    heap->array[right]->freq < heap->array[smallest]->freq) {
		smallest = right;
	}

	if (smallest != idx) {
		swap(&heap->array[smallest], &heap->array[idx]);
		huffman_heapify(heap, smallest);
	}
}

huffman_node_p extract_min(huffman_heap_p heap)
{
    huffman_node_p temp = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    --heap->size;
    huffman_heapify(heap, 0);

    return temp;
}

void insert_heap(huffman_heap_p heap, huffman_node_p node)
{
    ++heap->size;
    int idx = heap->size - 1;
    while (idx && node->freq < heap->array[(idx - 1) / 2]->freq) {
        heap->array[idx] = heap->array[(idx - 1) / 2];
        idx = (idx - 1) / 2;
    }

    heap->array[idx] = node;
}

void build_heap(huffman_heap_p heap)
{
    int num = heap->size - 1;
    for (int idx = (num - 1) / 2; idx >= 0; --idx) {
        huffman_heapify(heap, idx);
    }
}

int is_leaf(huffman_node_p root)
{
    return !(root->left) && !(root->right);
}

huffman_heap_p create_heap_and_build(char data[], int freq[], int size)
{
    huffman_heap_p heap = create_heap(size);
    for (int idx = 0; idx < size; ++idx) {
	    heap->array[idx] = new_node(data[idx], freq[idx]);
    }
    heap->size = size;
    build_heap(heap);

    return heap;
}

/*
  HUFFMAN(C)

  n = |C|
  Q = C
  for i = 1 to n-1
    allocate a new node z
    z.left = x = EXTRACT-MIN(Q)
    z.right = y = EXTRACT-MIN(Q)
    z.freq = x.freq + y.freq
    INSERT(Q, z)
  return EXTRACT-min(Q)
*/
huffman_node_p build_huffman_tree(char data[], int freq[], int size)
{
    huffman_node_p left, right, top;

    // step1: create a min heap of capacity equal to size;
    // initially, there are modes equal to size
    huffman_heap_p heap;
    heap = create_heap_and_build(data, freq, size);

    // iterate while size of heap doesn't become 1
    while (heap->size != 1) {

        // step2: extract the two minimum freq items from min heap
        left = extract_min(heap);
        right = extract_min(heap);

        // step3: create a new internal node with frequency equal to
        // the sum of the two nodes frequencies; make the two
        // extracted nodes to be the left and right children of this
        // new node; add this node to the min heap '$' is a special
        // value for internal nodes, not used
        top = new_node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insert_heap(heap, top);
    }

    // step4: the remaining node is the root node and the tree is
    // complete
    huffman_node_p node_min = extract_min(heap);
    destroy_heap(&heap);

    return node_min;
}

/* starting point is here */
void huffman(huffman_node_p *root, char data[], int freq[], int size)
{
    *root = build_huffman_tree(data, freq, size);

    int arr[MAX_TREE_HT], top = 0;
    greedy_huffman_print(*root, arr, top);
}

