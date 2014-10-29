#include "stack.h"
#include "common.h"

void Stack_Push( LinkedListNode *node, LinkedListNode **stackTop )
{
	assert( node != NULL );

	node->next = *stackTop;
	*stackTop = node;
}

LinkedListNode* Stack_Pop( LinkedListNode **stackTop )
{
	assert( stackTop != NULL );

	LinkedListNode *node = *stackTop;
	*stackTop = (*stackTop)->next;
	return node;
}

void Stack_PushHelp( void *data, LinkedListNode **stackTop )
{
	LinkedListNode *node  = LinkedListNode_Make(data);
	Stack_Push(node, stackTop);
}

void Stack_PushString( const char *data, LinkedListNode **stackTop )
{
	Stack_PushHelp((void *)data, stackTop);
}

char* Stack_PopString( LinkedListNode **stackTop )
{
	void *data = Stack_Pop( stackTop )->data;
	return static_cast<char*>(data);
}

void Stack_PushChar( const char *data, LinkedListNode **stackTop )
{
	Stack_PushHelp((void *)data, stackTop);
}

char* Stack_PopChar( LinkedListNode **stackTop )
{
	void *data = Stack_Pop( stackTop );
	return static_cast<char *>(data);
}

char* Stack_PeekChar( const LinkedListNode *stackTop ){
	void *data = stackTop->data;
	return static_cast<char *>(data);
}