/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     struct TreeNode *left;
 *     struct TreeNode *right;
 * };
 */
 #include <stdio.h>


int maxDepth(struct TreeNode* root) {
    if( root == NULL ) return 0;
    // if( root->left != NULL )
    //     printf("%d   ",root->left->val);
    // if( root->right != NULL )
    //     printf("%d\n",root->right->val);

    int left = maxDepth( root->left );
    int right = maxDepth( root->right );

    return (left > right ? left : right ) + 1;
}
