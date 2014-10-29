
enum XML_TokenType
{
	XML_TOKEN_TAG_OPEN,
	XML_TOKEN_TAG_CLOSE,
	XML_TOKEN_TAG_DATA,
	XML_TOKEN_TAG_ATTRIB,

	XML_TOKEN_TYPE_COUNT
};

struct XML_Token
{
	XML_TokenType type;
	char *data;
};

struct XML_TokenList
{
	XML_Token tokens[512];
	unsigned int count;
};

struct XML_ElementAttribute
{
	const char *key;
	const char *value;
	XML_ElementAttribute *next;
};

struct XML_Element
{
	const char *data;
	XML_Element* parent;
	XML_Element* sibling;
	XML_Element* child;

	XML_ElementAttribute *attributes;
};

XML_TokenList* XML_Tokenize(const char *contents, unsigned int length);
XML_Element* XML_Parse( XML_TokenList* tokenList );
void XMl_DebugPrintTokens(XML_TokenList *tokenList);
unsigned int XML_ReadFile( const char *filename, char **contents );

void XML_Element_Free( XML_Element **elem );