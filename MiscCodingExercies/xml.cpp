#include <stdlib.h>
#include "common.h"
#include "stack.h"
#include "string.h"
#include "xml.h"

#define MAX_XML_FILE_SIZE 2048

char *s_xmlTokenTypeDescription[XML_TOKEN_TYPE_COUNT] =
{
	"tag_open",
	"tag_close",
	"tag_data",
	"tag_attib"
};

void XML_TokenList_Add(XML_TokenList *list, XML_TokenType type, char *data)
{
	XML_Token *token = &list->tokens[(list->count)++];
	token->type = type;
	token->data = data;
}

XML_TokenList* XML_Tokenize(const char *contents, unsigned int length)
{
	LinkedListNode *charStack = NULL;
	char *hashChar = "#";
	Stack_PushChar(hashChar, &charStack);
	XML_TokenList *tokenList = new XML_TokenList();

	const unsigned int bufferMaxLength = 512;
	char tempToken[bufferMaxLength + 1];
	unsigned int tempTokenLength = 0;

	XML_TokenType tokenType = XML_TOKEN_TAG_DATA;

	for (unsigned int i = 0; i <= length; i++)
	{
		char ch = contents[i];
		char charStackTop = *Stack_PeekChar(charStack);

		if( charStackTop == '<' )
		{
			assert( tokenType == XML_TOKEN_TAG_OPEN || tokenType == XML_TOKEN_TAG_CLOSE );
			assert( ch != '<');

			if( ch == '/' )
			{
				assert( tempTokenLength == 0 );
				tokenType =  XML_TOKEN_TAG_CLOSE;
			}
			else if( ch == '>' || ch == ' ' )
			{
				// copy 
				tempToken[tempTokenLength++] = '\0';
				char *tagName = (char *)malloc(sizeof(char) * tempTokenLength);
				strcpy_s(tagName, tempTokenLength, tempToken);

				// add to list
				XML_TokenList_Add(tokenList, tokenType, tagName);

				// reset
				char *popCh = Stack_PopChar(&charStack);
				free(popCh);
				tempTokenLength = 0;

				// attribute list
				if( ch == ' ' )
				{
					// attrib list
					char *pushChar = (char *)malloc(sizeof(char));
					*pushChar = '=';	// means reading attrib list
					tokenType = XML_TOKEN_TAG_ATTRIB;
					Stack_PushChar(pushChar, &charStack);
				}
			}
			else
			{
				tempToken[tempTokenLength++] = ch;
				assert(tempTokenLength < bufferMaxLength);
			}
		}
		else if (charStackTop == '=')
		{
			assert(tokenType == XML_TOKEN_TAG_ATTRIB);
			if( ch == '>' || ch == ' ')
			{
				// key=value is in token
				// split them and add them into the token list
				// copy 
				tempToken[tempTokenLength++] = '\0';
				char *attribKVP = (char *)malloc(sizeof(char) * tempTokenLength);
				strcpy_s(attribKVP, tempTokenLength, tempToken);

				// add to list
				XML_TokenList_Add(tokenList, tokenType, attribKVP);

				// reset
				tempTokenLength = 0;

				if( ch == '>' )
				{
					// no more attributes
					char *popCh = Stack_PopChar(&charStack);
					free(popCh);
				}
			}
			else
			{
				tempToken[tempTokenLength++] = ch;
				assert(tempTokenLength < bufferMaxLength);
			}
		}
		else
		{
			assert( ch != '>');

			if( ch == ' ' || ch == '\n' || ch == '\t' )
			{
				// do nothing - white space ignores
			}
			else if( ch == '<')
			{
				if( tempTokenLength > 0 )
				{
					// push token into token stack
					tempToken[tempTokenLength++] = '\0';
					char *tagContent = (char *)malloc(sizeof(char) * tempTokenLength);
					strcpy_s(tagContent, tempTokenLength, tempToken);

					// stack changes
					XML_TokenList_Add(tokenList, tokenType, tagContent);

					// reset
					tempTokenLength = 0;
				}

				tokenType = XML_TOKEN_TAG_OPEN;

				char *pushChar = (char *)malloc(sizeof(char));
				*pushChar = ch;
				Stack_PushChar(pushChar, &charStack);
			}
			else
			{
				tokenType = XML_TOKEN_TAG_DATA;
				tempToken[tempTokenLength++] = ch;
				assert(tempTokenLength < bufferMaxLength);
			}
		}
	}
	return tokenList;
}

void XML_Element_Add_Child( XML_Element *element, XML_Element *child )
{
	assert(child != NULL);
	assert(element != NULL);

	child->parent = element;

	if(element->child == NULL)
	{
		element->child = child;
	}
	else
	{
		XML_Element *lastExistingChild = element->child;
		while(lastExistingChild->sibling != NULL)
		{
			lastExistingChild = lastExistingChild->sibling;
		}
		lastExistingChild->sibling = child;
	}
}

/**
Basically the reverse of a Depth-first traversal
If we were to write a serializer for this de-serializer
It is a Depth-first traversal
1.Pop element from Stack
2.Print element
3.Push close marker for this element on the Stack
4.Push children of this element onto the stack
**/
XML_Element* XML_Parse( XML_TokenList* tokenList )
{
	XML_Element *root = new XML_Element();
	root->data = "##";
	XML_Element *originalDummyRoot = root;

	for (unsigned int i = 0 ; i < tokenList->count; i++)
	{
		XML_Token *token = &tokenList->tokens[i];
		if( token->type == XML_TOKEN_TAG_OPEN)
		{
			// make a new element
			XML_Element *elem = new XML_Element();
			elem->data = token->data;

			// add it as child to current root
			XML_Element_Add_Child(root, elem);

			// make new element the root
			root = elem;
		}
		else if( token->type == XML_TOKEN_TAG_ATTRIB)
		{
			char *attrib_key = (char *)malloc(sizeof(char) * 256);
			char *attrib_value = (char *)malloc(sizeof(char) * 256);
			unsigned int attrib_i = 0;

			char *it = token->data;

			while(*it != '=')
			{
				assert( it != '\0' );
				attrib_key[attrib_i++] = *it;
				it++;
			}

			assert(attrib_i > 0);
			attrib_key[attrib_i++] = '\0';
			attrib_i = 0;

			it++;
			assert( *it == '\"');
			it++;

			while(*it != '\"')
			{
				assert( it != '\0' );
				attrib_value[attrib_i++] = *it;
				it++;
			}

			assert( attrib_i > 0 );
			attrib_value[attrib_i++] = '\0';

			it++;
			assert( *it == '\0' );

			XML_ElementAttribute *attribute = new XML_ElementAttribute();
			attribute->key = attrib_key;
			attribute->value = attrib_value;

			if( root->attributes == NULL )
			{
				root->attributes = attribute;
			}
			else
			{
				XML_ElementAttribute *attributeIterator = root->attributes;
				while(attributeIterator->next != NULL)
				{
					attributeIterator = attribute->next;
				}
				attributeIterator->next = attribute;
			}
		}
		else if( token->type == XML_TOKEN_TAG_CLOSE)
		{
			// set root as its own parent
			assert(root->parent != NULL);
			assert( strcmp(root->data, token->data) == 0 );
			root = root->parent;
		}
		else if( token->type == XML_TOKEN_TAG_DATA)
		{
			// make a new element
			XML_Element *elem = new XML_Element();
			elem->data = token->data;

			// add it as child to current root
			XML_Element_Add_Child(root, elem);
		}
		else
		{
			assert(false);
		}
	}

	assert(originalDummyRoot == root);

	root = root->child;
	delete originalDummyRoot;

	return root;
}

void XMl_DebugPrintTokens(XML_TokenList *tokenList)
{
	for (unsigned int i = 0 ; i < tokenList->count; i++)
	{
		XML_Token *token = &tokenList->tokens[i];
		printf( "%s %s\n", s_xmlTokenTypeDescription[token->type], token->data);
	}
}

unsigned int XML_ReadFile( const char *filename, char **contents )
{
	FILE *xmlFileHandle;
	fopen_s(&xmlFileHandle, filename, "r");
	assert(xmlFileHandle != NULL);
	*contents = (char *)malloc( sizeof(char*) * MAX_XML_FILE_SIZE );
	unsigned int contentLength = 0;
	for(unsigned int i = 0; i < MAX_XML_FILE_SIZE; i++)
	{
		int c = getc(xmlFileHandle);
		if( c == EOF )
		{
			contents[i] = '\0';
			contentLength = i;
			break;
		}
		(*contents)[i] = c;
	}
	return contentLength;
}

void XML_Element_Free( XML_Element **elem )
{
	XML_Element *childIt = (*elem)->child;
	while( childIt != NULL )
	{
		XML_Element *temp = childIt;
		childIt = childIt->sibling;
		XML_Element_Free(&temp);
	}
	delete elem;
}