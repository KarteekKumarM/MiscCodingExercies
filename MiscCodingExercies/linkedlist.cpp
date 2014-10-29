#include "linkedlist.h"
#include "common.h"

LinkedListNode* LinkedListNode_Make(void *data)
{
	LinkedListNode *node = new LinkedListNode();
	node->data = data;
	node->next = NULL;
	return node;
}