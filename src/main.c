#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//flags for indent options
int spaces = 0;
int tabs = 1;
int indent_width = 4;

void print_help();
void strip(char* line);
char* indent(char* line, int level);
int nq_search(char* needle, char* haystack);

int main(int argc, char* argv[])
{
	/*
	//too few arguments
	if(argc < 2)
	{
		print_help();
		exit(1);
	}
	*/
	
	char test[] = "\t\t    this \"is a test {\"\n";
	
	if(nq_search("{", test))
	{
		printf("String was found\n");
	}
	else
	{
		printf("String not found\n");
	}
	
	
	return 0;
}

void print_help()
{
	fprintf(stderr, "Usage: crindent [filename]...\n");
}

//remove whitespace from the beginning of a line
void strip(char* line)
{
	int counter = 0;
	int len = strlen(line);
	
	//replace whitespace characters with 0
	for(int i = 0; i < len; i++)
	{
		if(line[i] == ' ' || line[i] == '\t')
		{
			line[i] = 0;
			counter++;
		}
		else
		{
			break;
		}
	}
	
	//move back counter characters
	for(int i = counter; i < len; i++)
	{
		line[i - counter] = line[i];
		line[i] = 0;
	}
}

//indent a previously stripped line to a new level
char* indent(char* line, int level)
{
	//indent with tabs
	if(tabs)
	{
		int curlen = strlen(line);
		int newlen = curlen + level;
		char* to_return = malloc(newlen);
		memset(to_return, 0, newlen);
		
		for(int i = 0; i < level; i++)
		{
			to_return[i] = '\t';
		}
		
		strcpy(to_return + level, line);
		return to_return;
	}
	
	//indent with spaces
	if(spaces)
	{
		int curlen = strlen(line);
		int newlen = curlen + level * indent_width;
		char* to_return = malloc(newlen);
		memset(to_return, 0, newlen);
		
		for(int i = 0; i < level * indent_width; i++)
		{
			to_return[i] = ' ';
		}
		
		strcpy(to_return + (level * 4), line);
		return to_return;
	}
}

//search through a string while ignoring text from quotes
//TODO - need to implement proper quote ignore
int nq_search(char* needle, char* haystack)
{
	int haystack_len = strlen(haystack);
	int needle_len = strlen(needle);
	int in_quote = 0;
		
	for(int i = 0; i < strlen(haystack); i++)
	{	
		if(haystack[i] == '"')
		{
			//checking for escaped "
			if(i > 0)
			{
				if(haystack[i - 1] != '\\')
				{
					in_quote = !in_quote;
				}
			}
			
			//" cannot be escaped
			else
			{
				in_quote = !in_quote;
			}
		}
		
		//see if strings match
		if((haystack[i] == needle[0]) && (i + needle_len < haystack_len) && !in_quote)
		{
			int match = 1;
			for(int j = 0; j < strlen(needle); j++)
			{
				if(needle[j] != haystack[i + j])
				{
					match = 0;
					break;
				}
			}
			
			if(match)
			{
				return 1;
			}
		}
	}
	
	return 0;
}
