#pragma once

//read a word from file
//OUT word
void readWord(FILE *file, char *word) {
	int idx = 0;
	char ch;

	word[idx] = '\0';
	//read a word
	do {
		ch = fgetc(file);	//read a char from file
		if (ch == '\n' || ch == EOF || ch == ' ') {
			//if (ch == EOF) word[0] = EOF;
			break;
		}
		word[idx] = ch;
		idx++;
	} while (true);
	if (ch == EOF) word[0] = EOF;
	else word[idx] = '\0';	//end of string 
}

