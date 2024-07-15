#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "binary_tree.h"
#include <omp.h>

void print_tree(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    print_tree(root->left);
    printf("%d ", root->data);
    print_tree(root->right);
}

int main() {
    // Fixed seed for random number generation
    srand(42);

    // Initialize the tree with a root node
    TreeNode* root = createNode(10);

    // Add some initial nodes
    // insertNode(root, 5);
    insertNode(root, 15);
    // insertNode(root, 1);
    insertNode(root, 7);
    insertNode(root, 12);
    insertNode(root, 18);

    // Print initial tree structure
    printf("Initial tree structure:\n");
    print_tree(root);
    printf("\n");

    // Traversal Tests
    printf("Inorder Traversal: ");
    inorderTraversal(root);
    printf("\n");

    printf("Preorder Traversal: ");
    preorderTraversal(root);
    printf("\n");

    printf("Postorder Traversal: ");
    postorderTraversal(root);
    printf("\n");

    // Number of operations
    int numOps = 100;

    #pragma omp parallel for
    for (int i = 0; i < 100; i++)
    {
        int r = rand() % 2;
        if (r == 0) {
            // Insert a new node
            int data = rand() % 100;
            printf("Inserting node %d\n", data);
            insertNode(root, data);
        } else if (r == 1) {
            // Delete a node
            int data = rand() % 100;
            printf("Deleting node %d\n", data);
            deleteNode(root, data);
        } else if (r == 2) {
            // Search for a node
            int data = rand() % 100;
            bool found = searchNode(root, data);
            if (!found) {
                printf("Node %d not found\n", data);
            } else {
                printf("Node %d found\n", data);
            }
        } else {
            // Find the minimum value node
            TreeNode* min = findMin(root);
            printf("Minimum value node: %d\n", min->data);
        }
    }
    
        
    

    // Print the tree to check its final structure
    printf("Final tree structure:\n");
    print_tree(root);
    printf("\n");

    // Test the tree structure after all operations
    printf("Final Inorder Traversal: ");
    inorderTraversal(root);
    printf("\n");

    printf("Final Preorder Traversal: ");
    preorderTraversal(root);
    printf("\n");

    printf("Final Postorder Traversal: ");
    postorderTraversal(root);
    printf("\n");

    // Freeing the tree
    freeTree(root);
    root = NULL;

    return 0;
}
