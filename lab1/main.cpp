#include<iostream>
#include<string>
#include<algorithm>
#include<vector>
#include<sstream>

using namespace std;

#define MAX_NUM 30


static int charNum;
static int lineNum;
static int wordNum;
//set all types of variables(int, double...) to auto

string arr = "";
vector<string> keyWords = {
	"auto", "typedef", "define", "const", "extern", "static", "void",
	"if", "while", "for", "do", "witch", "goto", "continue", "break",
	"default", "sizeof", "return", "main"
};
vector<char> symbols = {
	'{', '}', '#', ';', '[', ']', '(', ')'
};

vector<char> operators = {
	'+', '-', '*', '/', '='
};

//annotion and errors
vector<string> unknown(1);
vector<string> annotion(1);

//check c whether key word
int isKeyWord(string c) {
	vector<string>::iterator it;
	for (it = keyWords.begin(); it != keyWords.end(); it++) {
		if ((*it).compare(c) == 0) {
			return 1;
		}
	}
	return 0;
}

int isSymbols(char ch) {
	vector<char>::iterator it;
	for (it = symbols.begin(); it != symbols.end(); it++) {
		if ((*it) == ch)
			return 1;
	}
	return 0;
}

int isLetter(char c) {
	if (((c <= 'z') && (c >= 'a')) || ((c <= 'Z') && (c >= 'A')))
		return 1;
	else
		return 0;
}

int isDigit(char c) {
	if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;
}

int isBlank(char c) {
	if (c == ' ' || c == '\t' || c == ';')
		return 1;
	else
		return 0;
}

int isOperator(char ch) {
	vector<char>::iterator it;
	for (it = operators.begin(); it != operators.end(); it++) {
		if ((*it) == ch)
			return 1;
	}
	return 0;
}

//digitDFA
void digitDFA(FILE *fpin) {
	char ch;
	stringstream ss;
	//status = 1 means there is a point. after the digit
	//status = 0 means no point. after the digit
	int status = 1;
	int result = 1;
	while ((ch = fgetc(fpin))) {
		charNum++;
		
		//if the next is digit, continue
		if (isDigit(ch)) {
			arr += ch;
			status = 1;
			continue;
		}
		//if the next is point., set the status
		else if (ch == '.') {
			arr += ch;
			status = 0;
		}
		//if the next is letter, it must be wrong, the set the result = false
		else if (isLetter(ch)) {
			arr += ch;
			result = false;
		}

		//if the next is \n, then lineNum++
		else if (ch == '\n') {
			lineNum++;
		}

		//the end of the DFA
		else if (ch == ';' || ch == ' ' || isSymbols(ch) || ch == 'EOF') {
			//
			if (result && status) {
				cout << arr << "\t\t$number" << endl;
			}
			else {
				ss << lineNum;
				string temp = "Unknow, line " + ss.str() + ",   " + arr;
				unknown.push_back(temp);
			}
			//fseek the file pointer
			fseek(fpin, -1L, SEEK_CUR);
			return;
		}
	}
}

void annotionDFA(FILE *fpin) {
	char ch;
	while ((ch = fgetc(fpin))) {
		charNum++;
		//end of the DFA
		if (ch == '%') {
			annotion.push_back(arr);
			return;
		}
		else if(ch == '\n'){
			arr += ch;
			lineNum++;
			continue;
		}

		//end of the DFA
		else if (ch == EOF) {
			stringstream ss;
			ss << lineNum;
			string temp = "Unknow, line " + ss.str() + ",   %" + arr;
			unknown.push_back(temp);
			fseek(fpin, -1L, SEEK_CUR);
			return;
		}
		else {
			arr += ch;
			continue;
		}
	}
}

void variableDFA(FILE *fpin) {
	char ch;
	while ((ch = fgetc(fpin))) {
		charNum++;
		//the end of the word
		if (ch == ' ' || ch == '\t' || ch == ';' || ch == EOF || isSymbols(ch)) {
			if (isKeyWord(arr)) {
				cout << arr << "\t\t$KeyWords" << endl;
				
			}
			else {
				cout << arr << "\t\t$Variables" << endl;
			}
			wordNum++;
			fseek(fpin, -1L, SEEK_CUR);
			return;
		}
		else {
			arr += ch;
			continue;
		}
	}
	fseek(fpin, -1L, SEEK_CUR);

}
void read(FILE  *fpin) {
	char ch;
	while ((ch = fgetc(fpin)) != EOF) {
		charNum++;

		if (isBlank(ch));
		else if (ch == '\n')
			lineNum++;
		else if (isSymbols(ch)) {
			cout << ch << "\t\tSymbols" << endl;;
		}
		//enter the digitDFA
		else if (isDigit(ch)) {
			arr += ch;
			digitDFA(fpin);
			arr = "";
		}
		else if (isOperator(ch)) {
			cout << ch << "\t\t$Operator" << endl;;
		}
		//enter the variableDFA
		else if (isLetter(ch)) {
			arr += ch;
			variableDFA(fpin);
			arr = "";
		}
		//enter the annotionDFA 
		else if (ch == '%') {
			annotionDFA(fpin);
			arr = "";
		}
	}
}

int main() {
	FILE * fpin;
	fpin = fopen("test.txt", "r");
	read(fpin);

	cout << "---------------------------------------" << endl;
	cout << "Annotions:" << endl;
	vector<string>::iterator it;
	for (it = annotion.begin(); it != annotion.end(); it++) {
		cout << (*it) << endl;
	}

	cout << "---------------------------------------" << endl;
	cout << "Errors:" << endl;
	for (it = unknown.begin(); it != unknown.end(); it++) {
		cout << (*it) << endl;
	}

	cout << "------------------Conclusion:-----------" << endl;
	cout << "Words Num: " << wordNum << endl;
	cout << "charNum: " << charNum << endl;
	cout << "line Num: " << lineNum << endl;
	fclose(fpin);

	getchar();
}