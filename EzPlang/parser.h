#pragma once
#include<iostream>
#include"val.h"
#include "expression.h"
#include"lexer.h"
#include"ast.h"
using namespace std;
static int topaddress = 0;
void parser_if(int start, int end, vector<token>& tokens);
void parser_while(int start, int end, vector<token>& tokens);
void parser_def(int start, int end, vector<token>& tokens);
void parser_charinput(int start, int end, vector<token>& tokens);
void parser_douinput(int start, int end, vector<token>& tokens);
void parser_intinput(int start, int end, vector<token>& tokens);
void out(int start, int end, vector<token>& tokens);
void process(int start, int end, vector<token>& tokens);
void parser_output(int start, int end, vector<token>& tokens);
void parser(int start, int end, vector<token>& tokens);

void parser_while(int start, int end, vector<token>& tokens) {
	int parentheses = 0;
	for (int i = start; i <= end; i++) {
		if (tokens[i].op == "(") parentheses = parentheses + 1;
		if (tokens[i].op == ")")  parentheses = parentheses - 1;
		if (parentheses == 0 && tokens[i].op == ")") {
			while (expression(start + 2, i - 1, tokens).to_bool()) {
				parser(i + 2, end - 1, tokens);//parser
				//cout << i + 2 << " " << end - 1 << endl;//
			}
			break;
		}
	}
}
void parser_if(int start, int end, vector<token>& tokens) {
	int parentheses = 0;
	for (int i = start; i <= end; i++) {
		if (tokens[i].op == "(") parentheses = parentheses + 1;
		if (tokens[i].op == ")")  parentheses = parentheses - 1;
		if (parentheses == 0 && tokens[i].op == ")") {
			int elsepos = 0, brace = 0;
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "{") brace = brace + 1;
				if (tokens[j].op == "}") brace = brace - 1;
				if (brace == 0 && tokens[j].keyword == "else") elsepos = j;
				if (brace == 0 && j == end) break;
			}
			if (expression(start + 2, i - 1, tokens).to_bool()) {
				if (elsepos != 0) parser(i + 2, elsepos - 2, tokens);//parser
				else parser(i + 2, end - 1, tokens);//parser
			}
			else {
				if (elsepos != 0) parser(elsepos + 2, end - 1, tokens);//parser
			}
			break;
		}
	}
}
void parser_def(int start, int end, vector<token>& tokens) {
	int bracket = 0;
	for (int i = start; i <= end; i++) {
		if (tokens[i].op == "[") bracket = bracket + 1;
		if (tokens[i].op == "]") bracket = bracket - 1;
		if (tokens[i].type == VAL && bracket == 0) {
			if (i == end || (i < end && tokens[i + 1].op != "[")) {//单个元素
				if (!is_defined[tokens[i].val]) {
					is_defined[tokens[i].val] = true;
					is_alist[tokens[i].val] = false;
					address[tokens[i].val] = topaddress;
					topaddress = topaddress + 1;
					memory.push_back(0);
				}
				else exit(1);
			}
			else {//数组
				if (!is_defined[tokens[i].val]) {
					is_defined[tokens[i].val] = true;
					is_alist[tokens[i].val] = true;
					int alistsize = 1, bracket1 = 0, dimension1 = 0;//数组大小、中括号、维度
					int comma1 = i + 1, comma2 = i + 1;
					for (int j = i + 1;; j++) {
						if (tokens[j].op == "[") bracket1 = bracket1 + 1;
						if (tokens[j].op == "]") bracket1 = bracket1 - 1;
						if ((bracket1 == 1 && tokens[j].op == ",") || (bracket1 == 0 && tokens[j].op == "]")) {
							comma1 = comma2;
							comma2 = j;
							dimension1 = dimension1 + 1;
							alistsize = alistsize * expression(comma1 + 1, comma2 - 1, tokens).int_val;
							dimsize[tokens[i].val].push_back(expression(comma1 + 1, comma2 - 1, tokens).int_val);
							//cout << expression(comma1 + 1, comma2 - 1, tokens).int_val << endl;
						}
						if (bracket1 == 0) break;
					}
					address[tokens[i].val] = topaddress;
					topaddress = topaddress + alistsize;
					dimension[tokens[i].val] = dimension1;
					for (int k = 1; k <= alistsize; k++) memory.push_back(0);
				}
				else exit(1);
			}
		}
	}
}

void parser_charinput(int start, int end, vector<token>& tokens) {
	int bracket = 0;
	for (int i = start; i < end; i++) {
		if (tokens[i].op == "[") bracket = bracket + 1;
		if (tokens[i].op == "]") bracket = bracket - 1;
		if (tokens[i].type == VAL && bracket == 0) {
			if (tokens[i + 1].op != "[") {
				if (!is_defined[tokens[i].val]) exit(1);
				cin >> memory[address[tokens[i].val]].char_val;
				memory[address[tokens[i].val]].type = CHAR;
			}
			else {
				if (!is_defined[tokens[i].val] || !is_alist[tokens[i].val]) exit(1);
				int comma1 = i + 1, comma2 = i + 1;
				int dimention1 = 0;
				int content[100], t = 1;
				int ad = address[tokens[i].val];
				for (int j = i + 1;; j++) {
					if (tokens[j].op == "[") bracket = bracket + 1;
					if (tokens[j].op == "]") bracket = bracket - 1;
					if ((bracket == 1 && tokens[j].op == ",") || (bracket == 0 && tokens[j].op == "]")) {
						dimention1 = dimention1 + 1;
						comma1 = comma2;
						comma2 = j;
						content[t++] = expression(comma1 + 1, comma2 - 1, tokens).int_val;
					}
					if (bracket == 0) break;
				}
				if (dimention1 != dimension[tokens[i].val]) {
					cout << dimention1 << " " << dimension[tokens[i].val] << endl;
					exit(1);
				}
				int q = 1;
				ad += content[dimention1];
				for (int p = dimension[tokens[i].val]; p > 1; p--) {
					q *= dimsize[tokens[i].val][p - 1];
					ad += q * content[p - 1];
				}
				cin >> memory[ad].char_val;
				memory[ad].type = CHAR;
			}
		}
	}
}

void parser_intinput(int start, int end, vector<token>& tokens) {
	int bracket = 0;
	for (int i = start; i < end; i++) {
		if (tokens[i].op == "[") bracket = bracket + 1;
		if (tokens[i].op == "]") bracket = bracket - 1;
		if (tokens[i].type == VAL && bracket == 0) {
			if (tokens[i + 1].op != "[") {
				if (!is_defined[tokens[i].val]) exit(1);
				cin >> memory[address[tokens[i].val]].int_val;
				memory[address[tokens[i].val]].type = INT;
			}
			else {
				if (!is_defined[tokens[i].val] || !is_alist[tokens[i].val]) exit(1);
				int comma1 = i + 1, comma2 = i + 1;
				int dimention1 = 0;
				int content[100], t = 1;
				int ad = address[tokens[i].val];
				for (int j = i + 1;; j++) {
					if (tokens[j].op == "[") bracket = bracket + 1;
					if (tokens[j].op == "]") bracket = bracket - 1;
					if ((bracket == 1 && tokens[j].op == ",") || (bracket == 0 && tokens[j].op == "]")) {
						dimention1 = dimention1 + 1;
						comma1 = comma2;
						comma2 = j;
						content[t++] = expression(comma1 + 1, comma2 - 1, tokens).int_val;
					}
					if (bracket == 0) break;
				}
				if (dimention1 != dimension[tokens[i].val]) {
					cout << dimention1 << " " << dimension[tokens[i].val] << endl;
					exit(1);
				}
				int q = 1;
				ad += content[dimention1];
				for (int p = dimension[tokens[i].val]; p > 1; p--) {
					q *= dimsize[tokens[i].val][p - 1];
					ad += q * content[p - 1];
				}
				cin >> memory[ad].int_val;
				memory[ad].type = INT;
			}
		}
	}
}

void parser_douinput(int start, int end, vector<token>& tokens) {
	int bracket = 0;
	for (int i = start; i < end; i++) {
		if (tokens[i].op == "[") bracket = bracket + 1;
		if (tokens[i].op == "]") bracket = bracket - 1;
		if (tokens[i].type == VAL && bracket == 0) {
			if (tokens[i + 1].op != "[") {
				if (!is_defined[tokens[i].val]) exit(1);
				cin >> memory[address[tokens[i].val]].dou_val;
				memory[address[tokens[i].val]].type = DOU;
			}
			else {
				if (!is_defined[tokens[i].val] || !is_alist[tokens[i].val]) exit(1);
				int comma1 = i + 1, comma2 = i + 1;
				int dimention1 = 0;
				int content[100], t = 1;
				int ad = address[tokens[i].val];
				for (int j = i + 1;; j++) {
					if (tokens[j].op == "[") bracket = bracket + 1;
					if (tokens[j].op == "]") bracket = bracket - 1;
					if ((bracket == 1 && tokens[j].op == ",") || (bracket == 0 && tokens[j].op == "]")) {
						dimention1 = dimention1 + 1;
						comma1 = comma2;
						comma2 = j;
						content[t++] = expression(comma1 + 1, comma2 - 1, tokens).int_val;
					}
					if (bracket == 0) break;
				}
				if (dimention1 != dimension[tokens[i].val]) {
					cout << dimention1 << " " << dimension[tokens[i].val] << endl;
					exit(1);
				}
				int q = 1;
				ad += content[dimention1];
				for (int p = dimension[tokens[i].val]; p > 1; p--) {
					q *= dimsize[tokens[i].val][p - 1];
					ad += q * content[p - 1];
				}
				cin >> memory[ad].dou_val;
				memory[ad].type = DOU;
			}
		}
	}
}

void process(int start, int end, vector<token>& tokens) {
	bool flag = false;
	for (int i = start; i <= end; i++) {
		if (tokens[i].op == "\'") {
			flag = true;
			cout << tokens[i + 1].ch;
			i += 2;
		}
		if (tokens[i].op == "\"") {
			flag = true;
			cout << tokens[i + 1].str;
			i += 2;
		}
	}
	if (flag == false) {
		cout << expression(start, end, tokens);
	}
}

void out(int start, int end, vector<token>& tokens) {
	int current_start = start;
	int current_end = end;
	bool flag = false;
	for (int i = start; i <= end; i++) {
		if (tokens[i].op == ",") {
			flag = true;
			current_end = i - 1;
			process(current_start, current_end, tokens);
			current_start = i + 1;
		}
		if (i == end && flag == true) {
			process(current_start, end, tokens);
		}
	}
	if (flag == false) {
		process(start, end, tokens);
	}
}

void parser_output(int start, int end, vector<token>& tokens) {
	for (int i = start; i <= end; i++) {
		if (tokens[i].keyword == "output") {
			int numOfBracket = 0;
			for (int j = i + 1; j <= end; j++) {
				int innerstart = i + 2;
				if (tokens[j].op == "(") {
					numOfBracket++;
				}
				if (tokens[j].op == ")") {
					numOfBracket--;
				}
				if (numOfBracket == 0 && tokens[j].op == ")") {
					int innerEnd = j - 1;
					out(innerstart, innerEnd, tokens);
					break;
				}
			}
		}
	}
}
void parser(int start, int end, vector<token>& tokens) {
	for (int i = start; i <= end; i++) {
		if (tokens[i].keyword == "if") {
			int brace = 0, flag = 0, block = 1, parentheses = 0;
			for (int n = i + 1; n <= end; n++, block++) {
				if (tokens[n].op == "(") parentheses++;
				if (tokens[n].op == ")") parentheses--;
				if (parentheses == 0) {
					block++;
					break;
				}
			}
			for (int j = i + block;; j++) {
				if (tokens[j].op == "{") brace++;
				if (tokens[j].op == "}") brace--;
				if ((brace == 0 && j < end && tokens[j + 1].keyword != "else") || (brace == 0 && j == end)) {
					parser_if(i, j, tokens);
					i = j;
					break;
				}
				if (brace == 0 && j < end && tokens[j + 1].keyword == "else") {
					int brace1 = 0;
					flag = 0;
					for (int t = j + 2;; t++) {
						if (tokens[t].op == "{") brace1++;
						if (tokens[t].op == "}") brace1--;
						if (brace1 == 0) {
							flag = 1;
							parser_if(i, t, tokens);
							i = t;
							break;
						}
					}
				}
				if (flag == 1) break;
			}
		}
		if (tokens[i].keyword == "while") {
			int brace = 0, block = 1, parentheses = 0;
			for (int n = i + 1; n <= end; n++, block++) {
				if (tokens[n].op == "(") parentheses++;
				if (tokens[n].op == ")") parentheses--;
				if (parentheses == 0) {
					block++;
					break;
				}
			}
			for (int j = i + block; j <= end; j++) {
				if (tokens[j].op == "{") brace++;
				if (tokens[j].op == "}") brace--;
				if (brace == 0) {
					parser_while(i, j, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].keyword == "def") {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					parser_def(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].keyword == "charinput") {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					parser_charinput(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].keyword == "douinput") {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					parser_douinput(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].keyword == "intinput") {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					parser_intinput(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].keyword == "output") {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					parser_output(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
		if (tokens[i].type == VAL || tokens[i].type == NUM) {
			for (int j = i; j <= end; j++) {
				if (tokens[j].op == "\n" || tokens[j].op == ";") {
					expression(i, j - 1, tokens);
					i = j;
					break;
				}
			}
		}
	}
}