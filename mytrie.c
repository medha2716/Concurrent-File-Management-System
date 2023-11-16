// currently I am assuming if a path doesnt exist , we will get the storage server number(here asking user to give input).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

#define ALPHABET_SIZE 94

struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool end;
    int endnum;
};

struct TrieNode* createNode() {
    struct TrieNode *node = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    node->end=false;
    node->end=0;
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void insertPath(struct TrieNode *root, const char *path,int s) {
    struct TrieNode *current = root;
    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        int index = path[i] - '!';
        if (!current->children[index]) {
            current->children[index] = createNode();
        }
        current = current->children[index];
    }
    current->end=true;
    current->endnum=s;
}

int searchPath(struct TrieNode *root, const char *path) {
    struct TrieNode *current = root;
    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        int index = path[i] - '!';
        if (!current->children[index]) {
            return 0; // Path not found
        }
        current = current->children[index];
    }

    return current->endnum; // Path found
}

bool isEmpty(struct TrieNode* root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return false;
    return true;
}

// TrieNode delete(struct Trienode*,)
void deleteAllNodes(struct TrieNode* node) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            deleteAllNodes(node->children[i]);
            node->children[i]->endnum=0;
            node->children[i]->end=false;
        }
    }

    free(node);
}
void deleteNodesStartingFrom(struct TrieNode* node) {
    int startIndex = '/' - '!';

        if (node->children[startIndex] != NULL) {
            deleteAllNodes(node->children[startIndex]);
            node->children[startIndex] = NULL;
        }
}

struct TrieNode* removee(struct TrieNode* root, const char* key, int depth)
{
    
    // If tree is empty
    if (!root)
        return NULL;
 
    // If last character of key is being processed
    if (depth == strlen(key)) {
 
        // This node is no more end of word after
        // removal of given key
        if (root->end){
            root->endnum=0;
            root->end = false;
        }
        deleteNodesStartingFrom(root);
 
        // If given is not prefix of any other word
        if (isEmpty(root)) {
            free (root);
            root = NULL;
        }
 
        return root;
    }
 
    // If not last character, recur for the child
    // obtained using ASCII value
    int index = key[depth] - '!';
    root->children[index] = removee(root->children[index], key, depth + 1);
 
    // If root does not have any child (its only child got 
    // deleted), and it is not end of another word.
    if (isEmpty(root) && root->end == false) {
        free (root);
        root = NULL;
    }
 
    return root;
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

    // example 
    int n;
    scanf("%d",&n);
    while(n--){
        char *input;
        input=(char*)malloc(sizeof(char)*1024);
        scanf("%s",input);
        int ss_num=searchPath(root,input); // need to get
        if(ss_num==0){
            // ss_num=2;
            printf("give no\n");
            scanf("%d",&ss_num);            
        }
        insertPath(root,input,ss_num);
        free(input);
    }

   
    int s=0;
    // removee(root,"hi",s);
    // char *input;
    //     input=(char*)malloc(sizeof(char)*1024);
    //     scanf("%s",input);
    //     int ss_num=searchPath(root,input); // need to get
    //     if(ss_num==0){
    //         // ss_num=2;
    //         printf("give no\n");
    //         scanf("%d",&ss_num);            
    //     }
    //     insertPath(root,input,ss_num);
    //     free(input);

    // removee(root,"home/user/documents",s);
    // removee(root,"hii",s);
    root=removee(root,"a/b/c",s);
   
    

    // Example search

    if (searchPath(root, "a/b")) {
        printf("Path 1 found in the trie in ss%d !\n",searchPath(root, "a/b"));
    } else {
        printf("Path 1 not found in the trie!\n");
    }
     if (searchPath(root,"a/b/c")) {
        printf("Path 2 found in the trie in ss%d !\n",searchPath(root, "a/b/c"));
    } else {
        printf("Path 2 not found in the trie!\n");
    }
      if (searchPath(root,"a/b/c/d")) {
        printf("Path 3 found in the trie in ss%d\n",searchPath(root, "a/b/c/d"));
    } else {
        printf("Path 3 not found in the trie!\n");
    }
    if (searchPath(root,"a/b/cc/d")) {
        printf("Path 4 found in the trie in ss%d !\n",searchPath(root, "a/b/cc/d"));
    } else {
        printf("Path 4 not found in the trie!\n");
    }



    return 0;
}

// same input and output 
/*
10
a
give no
1
b
give no
2
c
give no
3
a/b/c/d
give no
1
hi
give no
6
hii
give no
7
home/downloads
give no
3
hey
give no
5
y
give no
7
a/b/c/d
Path 1 found in the trie in ss7 !
Path 2 found in the trie in ss6 v!
Path 3 not found in the trie!
Path 4 found in the trie in ss1 !

2nd case with just removing hi once to show it works

3
hi
give no
2
hey
give no
3
hi
hi
give no
4
Path 1 not found in the trie!
Path 2 found in the trie in ss4 !
Path 3 not found in the trie!
Path 4 not found in the trie!

*/
