// currently I am assuming if a path doesnt exist , we will get the storage server number(here asking user to give input).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>



struct store
{
    char *stringvalues[5];
    int ss_num[5];
    int index;
};
int check(const char *path, struct store *lru)
{
    int anss = 0;
    for (int i = 0; i < 5; i++)
    {
        if ((lru->stringvalues[i]!=NULL) && strcmp(path, lru->stringvalues[i]) == 0)
        {
            anss = lru->ss_num[i];
            break;
        }
    }
    return anss;
}



#define ALPHABET_SIZE 94
struct info{
    int nm_port;
    int client_port;

    int nm_port_backup1;
    int client_port_backup1;
    
    int nm_port_backup2;
    int client_port_backup2;
};

struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool end;
    int endnum;
    struct info* myinfo;
};

struct TrieNode* createNode() {
    struct TrieNode *node = (struct TrieNode*)malloc(sizeof(struct TrieNode));
    node->end=false;
    node->endnum=0;
    if (node) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void insertPath(struct TrieNode *root, const char *path,int s, int nm_port, int client_port) {
    struct TrieNode *current = root;
    int len = strlen(path);
    int port=1111; // get port , need to change

    for (int i = 0; i < len; i++) {
        int index = path[i] - '!';
        if (!current->children[index]) {
            current->children[index] = createNode();
            current->children[index]->myinfo=(struct info*)malloc(sizeof(struct info));
        }
        current = current->children[index];
    }
    current->end=true;
    current->endnum=s;
    current->myinfo->nm_port=nm_port;
    current->myinfo->client_port=client_port;
}

int* searchPath(struct TrieNode *root, const char *path) {
    int* ans=(int*)malloc(sizeof(int)*7);
    for(int i=0;i<7;i++){
        ans[i]=0;
    }
    if(!root){
        return ans;
    }
    struct TrieNode *current = root;

    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        int index = path[i] - '!';
        if (!current->children[index]) {
            ans[0]=0;
            ans[1]=0;
            ans[2]=0;
            ans[3]=0;
            ans[4]=0;
            ans[5]=0;
            ans[6]=0;
            return ans; // Path not found
        }
        current = current->children[index];
    }
    ans[0]=current->endnum;
    ans[2]=current->myinfo->nm_port;
    ans[1]=current->myinfo->client_port;
    ans[3]=current->myinfo->client_port_backup1;
    ans[5]=current->myinfo->client_port_backup2;
    ans[4]=current->myinfo->nm_port_backup1;
    ans[6]=current->myinfo->nm_port_backup2;
    return ans; // Path found
}

bool isEmpty(struct TrieNode* root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return false;
    return true;
}

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
    
    if (!root)
        return NULL;
 
    if (depth == strlen(key)) {
 
        if (root->end){
            root->endnum=0;
            root->end = false;
        }
        deleteNodesStartingFrom(root);
 
        if (isEmpty(root)) {
            free (root);
            root = NULL;
        }
 
        return root;
    }
 
   
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
    root->myinfo=(struct info*)malloc(sizeof(struct info));
    struct store *lru = (struct store *)malloc(sizeof(struct store));
    lru->index = 0;
    // Assuming struct_received.accessible_paths is a string containing paths separated by spaces
    // char *token = strtok(struct_received.accessible_paths, " ");
    // while (token != NULL) {
    //     insertPath(root, token);
    //     token = strtok(NULL, " ");
    // }

    // Add a few more paths

    // example 
    int n;


    scanf("%d\n",&n);
        char c='w';
    while(n--){
        // scanf("%c",&c);
        char input[1024];
        scanf("%s",input);
    // int x=check(input,lru);
    // if (x != 0)
    // {
    //     printf("Path %s is in ss number %d\n", input, x); // execute
    //     if(c=='D'){ //deleting directory
    //         bzero(lru->stringvalues[x],1024);
    //         lru->ss_num[x]=0;
    //     }
    // }
    // else{
        char* dup=strdup(input);
        int* search_ss_port=searchPath(root,input);
        int ss_num=search_ss_port[0];
         if(ss_num==0){
            // ss_num=2;
            printf("give no\n");
            scanf("%d",&ss_num);            
        }
        if (c != 'D')
        {

            int ss; // get ss from tries
            // scanf("%d", &ss);
            lru->stringvalues[lru->index % 5] = strdup(input);
            lru->ss_num[lru->index % 5] = ss_num;
            lru->index = (lru->index + 1) % 5;
        }

        int port_num=search_ss_port[1]; // need to get
        char tmp[1024];
        bzero(tmp,1024);
     for(int i=0;i<strlen(dup);i++){
        if(dup[i]=='/'){
        insertPath(root,tmp,ss_num,ss_num,2);
        }
        tmp[i]=dup[i];
     }
        insertPath(root,input,ss_num,ss_num,2);
        // free(input);
    // }

   
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
   
    }
    

    // Example search
    // int lru1=check("a/b",lru);
    // if(lru1!=0){
    //     printf("Path 1 found using lru in ss %d\n",lru1);
    // }
    // else{

    int* ex1=searchPath(root, "a/b");
    if (ex1[0]) {
        printf("Path 1 found in the trie in ss %d !\n",ex1[2]);
    } else {
        printf("52: Path 1 not found in the trie!\n");
    }
    // }
    //  int lru2=check("a/b/c",lru);
    // if(lru2!=0){
    //     printf("Path 2 found using lru in ss %d\n",lru2);
    // }
    // else{
    int* ex2=searchPath(root, "a/b/c");

     if (ex2[0]) {
        printf("Path 2 found in the trie in ss%d !\n",ex2[2]);
    } else {
        printf("52: Path 2 not found in the trie!\n");
    }
    // }
    //  int lru3=check("a",lru);
    // if(lru3!=0){
    //     printf("Path 3 found using lru in ss %d\n",lru3);
    // }
    // else{
int *ex3=searchPath(root, "a");
      if (ex3[0]) {
        printf("Path 3 found in the trie in ss%d\n",ex3[2]);
    } else {
        printf("52: Path 3 not found in the trie!\n");
    }
    // }
    //  int lru4=check("a/b/cc/d",lru);
    // if(lru4!=0){
    //     printf("Path 4 found using lru in ss %d\n",lru4);
    // }
    // else{
int* ex4=searchPath(root,"a/b/c/d");
    if (ex4[0]) {
        printf("Path 4 found in the trie in ss%d !\n",ex4[2]);
    } else {
        printf("52: Path 4 not found in the trie!\n");
    // }
    }



    return 0;
}

/*
example
2
a/b  
give no
3
a/b/c/d
give no
9
Path 1 found in the trie in ss 9 !
52: Path 2 not found in the trie!
Path 3 found in the trie in ss9
52: Path 4 not found in the trie!
*/

// search[0] gives ss num , search[1] gives port number
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