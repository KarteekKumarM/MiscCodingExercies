#include "linkedlist.h"

void Stack_Push( LinkedListNode *node, LinkedListNode **stackTop );
LinkedListNode* Stack_Pop( LinkedListNode **stackTop );

void Stack_PushString( const char *data, LinkedListNode **stackTop );
char* Stack_PopString( LinkedListNode **stackTop );

void Stack_PushChar( const char *data, LinkedListNode **stackTop );
char* Stack_PopChar( LinkedListNode **stackTop );
char* Stack_PeekChar( const LinkedListNode *stackTop );