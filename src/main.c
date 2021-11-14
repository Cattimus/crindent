#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//TODO - --check flag to see if file has mixed tabs and spaces
//TODO - lines with empty whitespace are getting ignored

//flags for indent options
int spaces = 0;
int tabs = 1;
int indent_width = 4;

void print_help();
void strip(char* line);
void indent(char* line, int level);
int nq_search(char* needle, char* haystack);
int mixed_indent(char* line);

int main(int argc, char* argv[])
{
	/*
	//too few arguments
	if(argc < 2)
	{
		print_help();
		exit(1);
	}
	* */
	
	//line buffer
	char buff[100][8192];
	memset(buff, 0, 100 * 8192);
	int line_index = 0;
	
	//read file line by line and print the lines
	char line[8192];
	memset(line, 0, 8192);
	FILE* input = fopen("test.cpp", "r");
	int indent_level = 0;
	
	FILE* output = fopen("test - copy.cpp", "w");
	
	
	while(fgets(line, 8192, input) != NULL)
	{
		//strip indent
		strip(line);
		
		//this must happen before indentation
		if(nq_search("}", line))
		{
			indent_level--;
		}
		
		indent(line, indent_level);
		
		//copy line to buffer
		strcpy(buff[line_index], line);
		line_index++;	
		
		//TODO - can be multiple "{" and "}" per line
		if(nq_search("{", line))
		{
			indent_level++;
		}

		
		
		//empty buffer to file
		if(line_index == 100)
		{
			for(int i = 0; i < 100; i++)
			{
				fprintf(output, "%s", buff[i]);
				memset(buff[i], 0, 8192);
			}
			
			line_index = 0;
		}
		
		//reset line
		memset(line, 0, 8192);
	}
	
	for(int i = 0; i < line_index; i++)
	{
		fprintf(output, "%s", buff[i]);
		memset(buff[i], 0, 8192);
	}
			
		line_index = 0;
	
	fclose(input);
	fclose(output);
	
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
	
	
	if(counter == 0)
	{
		return;
	}
	
	//move back counter characters
	for(int i = counter; i < len; i++)
	{
		line[i - counter] = line[i];
		line[i] = 0;
	}
}

//returns 1 if tabs and spaces have been mixed (single line)
int mixed_indent(char* line)
{
	int has_space = 0;
	int has_tab = 0;
	
	for(int i = 0; i < strlen(line); i++)
	{
		if(line[i] == ' ')
		{
			has_space = 1;
		}
		else if(line[i] == '\t')
		{
			has_tab = 1;
		}
		
		//non-whitespace character
		else
		{
			break;
		}
	}
	
	return(has_space && has_tab);
}

//indent a previously stripped line to a new level
void indent(char* line, int level)
{
	//indent with tabs
	if(tabs)
	{
		int curlen = strlen(line);
		int newlen = curlen + level;
		char to_return[8192];
		memset(to_return, 0, 8192);
		
		for(int i = 0; i < level; i++)
		{
			to_return[i] = '\t';
		}
		
		strcpy(to_return + level, line);
		strcpy(line, to_return);
	}
	
	//indent with spaces
	if(spaces)
	{
		int curlen = strlen(line);
		int newlen = curlen + level * indent_width;
		char to_return[8192];
		memset(to_return, 0, 8192);
		
		for(int i = 0; i < level * 4; i++)
		{
			to_return[i] = ' ';
		}
		
		strcpy(to_return + level * 4, line);
		strcpy(line, to_return);
	}
}

//search through a string while ignoring text from quotes
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
