#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26

// Trie node structure
struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
};

// Function to initialize a new trie node
struct TrieNode* createNode() {
    struct TrieNode *node = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

// Function to insert a path into the trie
void insertPath(struct TrieNode *root, const char *path) {
    struct TrieNode *current = root;
    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        int index = path[i] - 'a';
        if (!current->children[index]) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
}

// Function to search for a path in the trie
int searchPath(struct TrieNode *root, const char *path) {
    struct TrieNode *current = root;
    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        int index = path[i] - 'a';
        if (!current->children[index]) {
            return 0; // Path not found
        }
        current = current->children[index];
    }

    return 1; // Path found
}

int main() {
    // Example usage
    struct TrieNode *root = createNode();

    // Assuming struct_received.accessible_paths is a string containing paths separated by spaces
    // char *token = strtok(struct_received.accessible_paths, " ");
    // while (token != NULL) {
    //     insertPath(root, token);
    //     token = strtok(NULL, " ");
    // }

    // Add a few more paths
    insertPath(root, "home/user/documents");
    insertPath(root, "var/log/system");
    insertPath(root, "usr/bin");

    // Example search
    if (searchPath(root, "var/log/system")) {
        printf("Path found in the trie!\n");
    } else {
        printf("61: Path not found in the trie!\n");
    }

    return 0;
}

