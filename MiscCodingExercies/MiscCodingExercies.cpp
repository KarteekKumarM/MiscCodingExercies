#include "stdafx.h"
#include "xml.h"
#include "string.h"
#include "common.h"
#include "stdlib.h"

void XML_Test()
{
	// read contents of file
	char *contents;
	unsigned int contentLength = XML_ReadFile("Sample.xml", &contents);
	assert(contentLength > 0);

	XML_TokenList *tokenList = XML_Tokenize(contents, contentLength);

	free(contents);

	XMl_DebugPrintTokens(tokenList);

	XML_Element *root = XML_Parse(tokenList);



	delete tokenList;
	XML_Element_Free( &root );

}

int _tmain(int argc, _TCHAR* argv[])
{
	XML_Test();
	return 0;
}

