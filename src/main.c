#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//TODO - --check flag to see if file has mixed tabs and spaces

const int LINE_MAX = 8192;
const int BUFF_SIZE = 100;

//flags for indent options
int spaces = 0;
int tabs = 1;
int indent_width = 4;

//flags for operations
int check_file = 0;

//helper function for arguments
void print_help();

//indentation functions
void strip(char* line);
int mixed_indent(const char* line);
void indent(char* line, int level);
int check_indent(const char* filename);

//file operations
void replace_file(const char* input);
int copy_file(const char* input, const char* output);
void process_files(char files[][LINE_MAX], int* num_files);
void write_buffer(char buff[][LINE_MAX], int* index, FILE* output);

//search functions
int nq_search(const char* needle, const char* haystack);

int main(int argc, char* argv[])
{
	//too few arguments
	if(argc < 2)
	{
		print_help();
		exit(1);
	}
	
	//buffer to hold file names
	char files[BUFF_SIZE][LINE_MAX];
	int file_index = 0;
	memset(files, 0, BUFF_SIZE * LINE_MAX);
	
	//process arguments
	for(int i = 1; i < argc; i++)
	{
		//argument is a parameter
		if(strncmp(argv[i], "-", 1) == 0)
		{
			if(strncmp(argv[i], "-spaces", 7) == 0)
			{
				spaces = 1;
				tabs = 0;
			}
			
			else if(strncmp(argv[i], "-width", 6) == 0)
			{
				char* width = strtok(argv[i], "=");
				width = strtok(NULL, "=");
				indent_width = atoi(width);
			}
			
			else if(strncmp(argv[i], "-check", 6) == 0)
			{
				check_file = 1;
			}
		}
		
		//argument is a filename
		else
		{
			strcpy(files[file_index], argv[i]);
			file_index++;
			
			//process all files in the buffer and empty buffer
			if(file_index == BUFF_SIZE)
			{
				process_files(files, &file_index);
			}
		}
	}
	
	//process any additional files that haven't been processed
	if(file_index > 0)
	{
		process_files(files, &file_index);
	}
	
	return 0;
}

void print_help()
{
	fprintf(stderr, "Usage: crindent [flags] [file1] [file2]...\n\n");
	fprintf(stderr, "[FLAGS]\n");
	fprintf(stderr, "-spaces:  use spaces instead of tabs (default width 4)\n");
	fprintf(stderr, "-width=n: sets the width in spaces to n\n");
	fprintf(stderr, "-check:   will check for mixed tabs and width instead of replacing files\n");
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
int mixed_indent(const char* line)
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
int nq_search(const char* needle, const char* haystack)
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

int copy_file(const char* input, const char* output)
{
	//line buffer
	char buff[BUFF_SIZE][LINE_MAX];
	memset(buff, 0, BUFF_SIZE * LINE_MAX);
	int line_index = 0;
	
	//read file line by line and print the lines
	char line[LINE_MAX];
	memset(line, 0, LINE_MAX);
	
	FILE* input_file = fopen(input, "r");
	if(input_file == NULL)
	{
		fprintf(stderr, "Error opening file %s. File will not be replaced.\n", input);
		return 0;
	}
	
	//output file will only be opened if there was no issue opening input file
	FILE* output_file = fopen(output, "w");
	if(output_file == NULL)
	{
		fprintf(stderr, "Error opening file %s. File will not be replaced.\n", output);
		return 0;
	}
	
	while(fgets(line, LINE_MAX, input_file) != NULL)
	{	
		//copy line to buffer
		strcpy(buff[line_index], line);
		line_index++;	
		
		//empty buffer to file
		if(line_index == BUFF_SIZE)
		{
			write_buffer(buff, &line_index, output_file);
		}
		
		//reset line
		memset(line, 0, LINE_MAX);
	}
	
	//empty remaining lines from buffer
	if(line_index > 0)
	{
		write_buffer(buff, &line_index, output_file);
	}
	
	fclose(input_file);
	fclose(output_file);
	
	return 1;
}

void replace_file(const char* input)
{
	char old_file[LINE_MAX];
	memset(old_file, 0, LINE_MAX);
	strcpy(old_file, input);
	strcat(old_file, ".old");
	if(!copy_file(input, old_file))
	{
		return;
	}
	
	//line buffer
	char buff[BUFF_SIZE][LINE_MAX];
	memset(buff, 0, BUFF_SIZE * LINE_MAX);
	int line_index = 0;
	
	//read file line by line and print the lines
	char line[LINE_MAX];
	memset(line, 0, LINE_MAX);
	int indent_level = 0;
	
	//open files
	FILE* input_file = fopen(old_file, "r");
	if(input_file == NULL)
	{
		fprintf(stderr, "Error opening file %s. File will not be replaced.\n", old_file);
		return;
	}
	
	FILE* output_file = fopen(input, "w");
	if(output_file == NULL)
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
		if(line_index == BUFF_SIZE)
		{
			write_buffer(buff, &line_index, output_file);
		}
		
		//reset line
		memset(line, 0, LINE_MAX);
	}
	
	if(line_index > 0)
	{
		write_buffer(buff, &line_index, output_file);
	}
	
	fclose(input_file);
	fclose(output_file);
}

//helper function to clear writing buffer in file replace
void write_buffer(char buff[][LINE_MAX], int* index, FILE* output)
{
	for(int i = 0; i < *index; i++)
	{
		fprintf(output, "%s", buff[i]);
		memset(buff[i], 0, LINE_MAX);
	}
	
	*index = 0;
}

//process list of files
void process_files(char files[][LINE_MAX], int* num_files)
{
	for(int i = 0; i < *num_files; i++)
	{
		printf("Replacing file %s...\n", files[i]);
		//have to process checking files here too
		replace_file(files[i]);
		memset(files[i], 0, LINE_MAX);
	}
	
	*num_files = 0;
}
