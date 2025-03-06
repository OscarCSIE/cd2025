#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked list node definition with frequency field
typedef struct Node {
    char data;
    int freq;
    struct Node* next;
} Node;

// Function to free the linked list
void freeList(Node* head) {
    while (head != NULL) {
        Node* temp = head;
        head = head->next;
        free(temp);
    }
}

// Create a new node for character c, setting frequency to 1
Node* createNode(char c) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        return NULL;
    }
    newNode->data = c;
    newNode->freq = 1;
    newNode->next = NULL;
    return newNode;
}

// Sequential search (O(n)): searches the linked list for the target character
Node* searchLinear(Node* head, char target) {
    for (Node* cur = head; cur != NULL; cur = cur->next) {
        if (cur->data == target) {
            return cur;
        }
    }
    return NULL;
}

// Helper function to print a character's representation,
// showing escape sequences for special characters.
void print_char_rep(char c) {
    switch(c) {
        case '\n':
            printf("\\n");
            break;
        case '\t':
            printf("\\t");
            break;
        case '\r':
            printf("\\r");
            break;
        case ' ':
            printf("' '");
            break;
        default:
            if (c < 32 || c > 126)
                printf("\\x%02x", (unsigned char)c);
            else
                printf("%c", c);
            break;
    }
}

// Helper function to parse an escape sequence input into its actual character.
// Returns 1 if parsing is successful, 0 otherwise.
int parseEscapeSequence(const char *input, char *result) {
    // If the input doesn't start with a backslash and is exactly one character,
    // it's treated as a normal character.
    if (input[0] != '\\') {
        if (strlen(input) == 1) {
            *result = input[0];
            return 1;
        }
        return 0;
    } else {
        // Recognize common escape sequences.
        if (strcmp(input, "\\n") == 0) {
            *result = '\n';
            return 1;
        } else if (strcmp(input, "\\t") == 0) {
            *result = '\t';
            return 1;
        } else if (strcmp(input, "\\r") == 0) {
            *result = '\r';
            return 1;
        } else if (strcmp(input, "\\\\") == 0) {
            *result = '\\';
            return 1;
        } else if (input[1] == 'x') {
            // Handle hexadecimal escape sequence, e.g., "\x41" for 'A'
            int value;
            if (sscanf(input, "\\x%x", &value) == 1) {
                *result = (char)value;
                return 1;
            }
        }
    }
    return 0;
}

int main() {
    // Open this source file (self-reading)
    FILE *fp = fopen(__FILE__, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", __FILE__);
        return 1;
    }
    
    Node* head = NULL;
    Node* tail = NULL;
    
    // Direct index array for O(1) lookup; each index corresponds to an unsigned char value.
    Node* charIndex[256] = {0};
    
    // Read the file character by character
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        unsigned char uc = (unsigned char)ch;
        
        if (charIndex[uc] != NULL) {
            // Character already exists; increment its frequency.
            charIndex[uc]->freq++;
        } else {
            // New character: create a node and add it to the linked list.
            Node* newNode = createNode((char)ch);
            if (newNode == NULL) {
                fprintf(stderr, "Memory allocation failed. Cleaning up allocated nodes.\n");
                freeList(head);
                fclose(fp);
                return 1;
            }
            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
            charIndex[uc] = newNode;
        }
    }
    fclose(fp);
    
    // Print the frequency of each unique character in the order they first appeared.
    printf("\nCharacter : Frequency\n");
    for (Node* cur = head; cur != NULL; cur = cur->next) {
        print_char_rep(cur->data);
        printf(" : %d time(s)\n", cur->freq);
    }
    
    // Loop to handle repeated search queries.
    char input[64];
    while (1) {
        printf("\nEnter a character to search for (or type 'exit' to quit):\n");
        if (scanf("%63s", input) != 1) {
            printf("Input error. Exiting...\n");
            break;
        }
        
        if (strcmp(input, "exit") == 0) {
            printf("Exiting program...\n");
            break;
        }
        
        // Parse the user input to handle escape sequences.
        char query;
        if (!parseEscapeSequence(input, &query)) {
            printf("Invalid input. Please enter exactly one character or a valid escape sequence (e.g., \\n, \\t, \\r, \\\\).\n");
            continue;
        }
        
        // O(n) sequential search
        Node* foundLinear = searchLinear(head, query);
        printf("Using linear search O(n): '");
        print_char_rep(query);
        if (foundLinear) {
            printf("' is found in the linked list with frequency: %d\n", foundLinear->freq);
        } else {
            printf("' is not found in the linked list.\n");
        }
        
        // O(1) search using direct index
        Node* foundDirect = charIndex[(unsigned char)query];
        printf("Using direct index search O(1): '");
        print_char_rep(query);
        if (foundDirect) {
            printf("' is found with frequency: %d\n", foundDirect->freq);
        } else {
            printf("' is not found.\n");
        }
    }
    
    // Free the linked list memory.
    freeList(head);
    
    return 0;
}