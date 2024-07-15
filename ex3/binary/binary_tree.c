#include "binary_tree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// Function to create a new binary tree node
TreeNode* createNode(int data) {
    TreeNode* node = malloc(sizeof(TreeNode));
    if (node) {
        node->data = data;
        node->left = node->right = NULL;
        node->parent = NULL;
        omp_init_lock(&(node->lock));
        omp_init_lock(&(node->lockDel));
    }
    return node;
}

void unset_3d(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    if (root->parent != NULL) {
        if (root->parent->parent != NULL) {
            omp_unset_lock(&(root->parent->parent->lock));
        }
    }
}

void unset_all_3(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    omp_unset_lock(&(root->lock));
    if (root->parent != NULL) {
        omp_unset_lock(&(root->parent->lock));
        if (root->parent->parent != NULL) {
            omp_unset_lock(&(root->parent->parent->lock));
        }
    }
}

void unset_all(TreeNode* root) {
    TreeNode* current = root;
    while (current != NULL)
    {
        omp_unset_lock(&(current->lock));
        current = current->parent;
    }
}

void setDelAncestors(TreeNode* root) {
    if (root->parent != NULL)
    {
        setDelAncestors(root->parent);
        
    }
    if (omp_test_lock(&(root->lockDel)))
        return;
}

void unsetDelAncestors(TreeNode* root) {
    omp_unset_lock(&(root->lockDel));
    if (root->parent != NULL)
    {
        unsetDelAncestors(root->parent);
    }
}


// Function to insert a new node into the binary tree
TreeNode* insertNode(TreeNode* root, int data) {
    TreeNode* current = root;
    TreeNode* parent = NULL;
    while (current != NULL) {
        parent = current;
        omp_set_lock(&(current->lock));
        // omp_set_lock(&(current->lockDel));
        unset_3d(current);

        if (data < current->data) {
            current = current->left;
        } else if (data > current->data) {
            current = current->right;
        } else {
            unset_all_3(current);
            // unset_all_3(parent);
            return root;
        }
    }
    // setDelAncestors(parent);
    TreeNode* newNode = createNode(data);
    if (parent == NULL) {
        root = newNode;
    } else if (data < parent->data) {
        parent->left = newNode;
    } else {
        parent->right = newNode;
    }
    newNode->parent = parent;
    unset_all_3(parent);
    // unsetDelAncestors(parent);
    return root;
}

void localDelete(TreeNode* node) {
    TreeNode* parent = node->parent;
    TreeNode* child = NULL;
    // omp_set_lock(&(node->lockDel));

    // Case 1: Node has no children (leaf node)
    if (node->left == NULL && node->right == NULL) {
        child = NULL;
    }
    // Case 2: Node has only one child
    else if (node->left == NULL) {
        child = node->right;
    } else if (node->right == NULL) {
        child = node->left;
    }
    // Case 3: Node has two children
    else {
        TreeNode* successor = node->right;
        while (successor->left != NULL) {
            successor = successor->left;
            omp_set_lock(&(successor->lock));
        }
        // Setting the successor's data to the current node's data
        node->data = successor->data;
        // Delete the successor node instead
        localDelete(successor);
        omp_unset_lock(&(successor->lock));
        omp_unset_lock(&(node->lockDel));
        return;
    }

    // omp_unset_lock(&(node->lockDel));

    // Attach the child to the parent of the node to be deleted
    if (parent == NULL) {  // Node to delete is root
        node = child;
    } else if (parent->left == node) {
        parent->left = child;
    } else {
        parent->right = child;
    }

    if (child != NULL) {
        child->parent = parent;
    }

    // Release lock and free the node
    omp_unset_lock(&(node->lock));
    free(node);
}



// Function to delete a node from the binary tree
TreeNode* deleteNode(TreeNode* root, int data) {
    TreeNode* current = root;
    TreeNode* parent = NULL;
    while (current != NULL) {
        parent = current;
        omp_set_lock(&(current->lock));
        unset_3d(current);

        if (data < current->data) {
            current = current->left;
        } else if (data > current->data) {
            current = current->right;
        } else {
            localDelete(current);
            unset_all_3(parent);
            return root;
        }
    }
    unset_all_3(parent);
    return root;
}

// Function to search for a node in the binary tree
bool searchNode(TreeNode* root, int data) {
    TreeNode* current = root;
    TreeNode* parent = NULL;
    while (current != NULL) {
        parent = current;
        omp_set_lock(&(current->lock));
        unset_3d(current);
        if (data < current->data) {
            current = current->left;
        } else if (data > current->data) {
            current = current->right;
        } else {
            unset_all_3(current);
            return true;
        }
    }
    unset_all_3(parent);
    return false;
}

// Function to find the minimum value node in the binary tree
TreeNode* findMin(TreeNode* root) {
    TreeNode* current = root;
    // TreeNode* parent = NULL;
    while (current->left != NULL) {
        // parent = current;
        omp_set_lock(&(current->lock));
        unset_3d(current);
        current = current->left;
    }
    TreeNode* out = current;
    unset_all_3(current);
    return out;
}

// Function to perform an in-order traversal of the binary tree
void inorderTraversal(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    omp_set_lock(&(root->lock));
    inorderTraversal(root->left);
    printf("%d ", root->data);
    omp_unset_lock(&(root->lock));
    inorderTraversal(root->right);
}

// Function to perform a pre-order traversal of the binary tree
void preorderTraversal(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    omp_set_lock(&(root->lock));
    printf("%d ", root->data);
    preorderTraversal(root->left);
    omp_unset_lock(&(root->lock));
    preorderTraversal(root->right);
}

// Function to perform a post-order traversal of the binary tree
void postorderTraversal(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    omp_set_lock(&(root->lock));
    postorderTraversal(root->left);
    postorderTraversal(root->right);
    printf("%d ", root->data);
    omp_unset_lock(&(root->lock));
}

// Function to free all nodes in the binary tree
void freeTree(TreeNode* root) {
    if (root == NULL) {
        return;
    }
    omp_set_lock(&(root->lock));
    freeTree(root->left);
    freeTree(root->right);
    omp_unset_lock(&(root->lock));
    free(root);
}

