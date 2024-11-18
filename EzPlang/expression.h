#pragma once
#pragma once
#include<iostream>
#include"val.h"
#include"lexer.h"
#include"ast.h"
using namespace std;
def expression(int start, int end, vector<token>& tokens, int minvalsize = 0, int minopsize = 0) {
	//cout << start << " " << end << endl;
	while (val_st.size() > minvalsize) val_st.pop();
	while (op_st.size() > minopsize) op_st.pop();
	for (int i = start; i <= end; i++) {
		if (tokens[i].type == VAL)
			if (is_defined[tokens[i].val] == false) exit(1);//使用了未定义的变量
		if (tokens[i].type == INT || tokens[i].type == DOU || tokens[i].type == CHAR) val_st.push(new ast(tokens[i].num));
		if (tokens[i].type == VAL && i != end) {
			if (tokens[i + 1].type == OPERATOR && tokens[i + 1].op == "[") {
				int expstart, expend;
				vector<int> content;
				stack<bool> opst;//考虑数组内嵌套数组的情况，用栈匹配中括号，由于只考虑有没有出现中括号，并不考虑位置等信息，使用bool节约内存
				content.clear();
				while (opst.size()) opst.pop();
				opst.push(true);
				expstart = expend = i + 2;
				while (opst.size()) {
					while (true) {
						expend++;
						if (tokens[expend].op == "," && opst.size() == 1) break;
						if (tokens[expend].op == "[") opst.push(true);
						if (tokens[expend].op == "]") opst.pop();
						if (!opst.size()) break;
					}
					content.push_back(expression(expstart, expend - 1, tokens, val_st.size(), op_st.size()).get_int());
					if (opst.size()) {
						expend++;
						expstart = expend;
					}
				}
				//cout << tokens[i].val << endl;
				val_st.push(new ast(tokens[i].val, content));
				i = expend;
				continue;
			}
		}
		if (tokens[i].type == VAL) {
			//cout << tokens[i].val<<endl;
			val_st.push(new ast(tokens[i].val));
		}
		if (tokens[i].type == OPERATOR) {
			if (tokens[i].op == "(") op_st.push("(");
			if (tokens[i].op == ")") {
				while (op_st.size() && op_st.top() != "(") addnode();
				op_st.pop();
			}
			if (tokens[i].op != "(" && tokens[i].op != ")") {
				while (op_st.size() && level[op_st.top()] >= level[tokens[i].op]) addnode();
				op_st.push(tokens[i].op);
			}
		}
	}
	while (op_st.size() > minopsize) addnode();
	ast* tmpval = val_st.top();
	while (val_st.size() > minvalsize) val_st.pop();
	while (op_st.size() > minopsize) op_st.pop();
	return tmpval->eval();
}