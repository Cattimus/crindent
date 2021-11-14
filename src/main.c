#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//TODO - --check flag to see if file has mixed tabs and spaces

//flags for indent options
int spaces = 0;
int tabs = 1;
int indent_width = 4;

//flags for operations
int check_file = 0;
int replace_file = 0;

const int LINE_MAX = 8192;

void print_help();
void strip(char* line);
int mixed_indent(char* line);
void replace_file(char* input);
void indent(char* line, int level);
void copy_file(char* input, char* output);
int nq_search(char* needle, char* haystack);

int main(int argc, char* argv[])
{
	//too few arguments
	if(argc < 2)
	{
		print_help();
		exit(1);
	}
	
	//handle arguments here
	//create list of files
	
	//replace or check files
	for(int i = 1; i < argc; i++)
	{
		replace_file(argv[i]);
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
	
	//line has no beginning whitespace
	if(counter == 0)
	{
		return;
	}
	
	//move entire string to beginning (if whitespace was removed)
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
		char to_return[LINE_MAX];
		memset(to_return, 0, LINE_MAX);
		
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
		char to_return[LINE_MAX];
		memset(to_return, 0, LINE_MAX);
		
		for(int i = 0; i < level * indent_width; i++)
		{
			to_return[i] = ' ';
		}
		
		strcpy(to_return + level * indent_width, line);
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

void copy_file(char* input, char* output)
{
	//line buffer
	char buff[100][LINE_MAX];
	memset(buff, 0, 100 * LINE_MAX);
	int line_index = 0;
	
	//read file line by line and print the lines
	char line[LINE_MAX];
	memset(line, 0, LINE_MAX);
	FILE* input_file = fopen(input, "r");
	FILE* output_file = fopen(output, "w");
	
	if(input_file == NULL || output_file == NULL)
	{
		fprintf(stderr, "Error opening file %s. File will not be replaced.\n", input);
		return;
	}
	
	while(fgets(line, LINE_MAX, input_file) != NULL)
	{	
		//copy line to buffer
		strcpy(buff[line_index], line);
		line_index++;	
		
		//empty buffer to file
		if(line_index == 100)
		{
			for(int i = 0; i < 100; i++)
			{
				fprintf(output_file, "%s", buff[i]);
				memset(buff[i], 0, LINE_MAX);
			}
			
			line_index = 0;
		}
		
		//reset line
		memset(line, 0, LINE_MAX);
	}
	
	//print remaining lines in buffer
	for(int i = 0; i < line_index; i++)
	{
		fprintf(output_file, "%s", buff[i]);
		memset(buff[i], 0, LINE_MAX);
	}	
	line_index = 0;
	
	fclose(input_file);
	fclose(output_file);
}

void replace_file(char* input)
{
	char old_file[LINE_MAX];
	memset(old_file, 0, 8192);
	strcpy(old_file, input);
	strcat(old_file, ".old");
	copy_file(input, old_file);
	
	//line buffer
	char buff[100][LINE_MAX];
	memset(buff, 0, 100 * LINE_MAX);
	int line_index = 0;
	
	//read file line by line and print the lines
	char line[LINE_MAX];
	memset(line, 0, LINE_MAX);
	FILE* input_file = fopen(old_file, "r");
	FILE* output_file = fopen(input, "w");
	int indent_level = 0;
	
	if(input_file == NULL || output_file == NULL)
	{
		fprintf(stderr, "Error opening file %s. File will not be replaced.\n", input);
		return;
	}
	
	while(fgets(line, LINE_MAX, input_file) != NULL)
	{
		//strip indent
		strip(line);
		
		//this must happen before indent function
		//decreases indent level
		if(nq_search("}", line))
		{
			indent_level--;
		}
		
		indent(line, indent_level);
		
		//copy line to buffer
		strcpy(buff[line_index], line);
		line_index++;	
		
		//TODO - edge case - can be multiple "{" and "}" per line
		//increase indent level
		if(nq_search("{", line))
		{
			indent_level++;
		}
		
		//empty buffer to file
		if(line_index == 100)
		{
			for(int i = 0; i < 100; i++)
			{
				fprintf(output_file, "%s", buff[i]);
				memset(buff[i], 0, LINE_MAX);
			}
			
			line_index = 0;
		}
		
		//reset line
		memset(line, 0, LINE_MAX);
	}
	
	//print remaining lines in buffer
	for(int i = 0; i < line_index; i++)
	{
		fprintf(output_file, "%s", buff[i]);
		memset(buff[i], 0, LINE_MAX);
	}	
	line_index = 0;
	
	fclose(input_file);
	fclose(output_file);
}
