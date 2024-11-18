#pragma once
#pragma once
#include<iostream>
#include"val.h"
#include"lexer.h"
#include"expression.h"
#include<map>
#include<stack>
using namespace std;
//map<string, def> mp_val;
map<string, bool> is_defined;//定义
map<string, bool> is_alist;//是否为数组
map<string, int> dimension;//数组维度
map<string, vector<int>> dimsize;//每一维空间大小
map<string, unsigned long long> address;
vector<def> memory;
stack<string> op_st;
class ast {
public:
	int ad;
	def x;
	string op;
	string val;
	Type type;
	ast* L;
	ast* R;
	ast() {};
	ast(ast* _L, string _op, ast* _R) {
		L = _L; R = _R; op = _op;
	}
	ast(string _val) {
		type = VAL;
		val = _val;
		ad = address[val];
	}
	ast(string _val, vector<int> content) {//根据数组索引确定当前地址
		if (content.size() != dimension[_val]) exit(1);//数组下标数量与定义时的维度数量不匹配
		type = VAL;
		val = _val;
		ad = address[val];
		int size = 1;
		for (int i = 0; i < dimension[_val]; i++) {
			size = content[i];
			for (int j = i + 1; j < dimension[_val]; j++) {
				size *= dimsize[_val][j];
			}
			ad += size;
		}
	}
	ast(def _x) {
		type = NUM;
		x = _x;
	}
	def eval() {
		if (type == NUM) return x;
		else if (type == VAL) return memory[ad];
		if (op == "+") return L->eval() + R->eval();
		if (op == "-") return L->eval() - R->eval();
		if (op == "*") return L->eval() * R->eval();
		if (op == "/") return L->eval() / R->eval();
		if (op == "%") return L->eval() % R->eval();
		if (op == "|") return L->eval() | R->eval();
		if (op == "&") return L->eval() & R->eval();
		if (op == "^") return L->eval() ^ R->eval();
		if (op == ">") return L->eval() > R->eval();
		if (op == "<") return L->eval() < R->eval();
		if (op == ">=") return L->eval() >= R->eval();
		if (op == "<=") return L->eval() <= R->eval();
		if (op == "==") return L->eval() == R->eval();
		if (op == "!=") return L->eval() != R->eval();
		if (op == "&&") return L->eval() && R->eval();
		if (op == "||") return L->eval() || R->eval();
		if (op == "!") return !(R->eval() == 1);
		if (op == "neg") return L->eval() - R->eval();
		if (op == "pos") return L->eval() + R->eval();
		if (op == "=") {
			if (L->type != VAL || is_defined[L->val] == false) exit(1);//赋值左值必须为已定义变量
			def tmpval = R->eval();
			memory[L->ad] = tmpval;
			return tmpval;
		}
	}
};
stack<ast*> val_st;
void addnode() {
	string op;
	ast* L;
	ast* R;
	R = val_st.top(); val_st.pop();
	L = new ast(def(0));
	op = op_st.top(); op_st.pop();
	if (op == "!" || op == "neg" || op == "pos") {
		val_st.push(new ast(L, op, R));
	}
	else if (val_st.size()) {
		L = val_st.top();
		val_st.pop();
		val_st.push(new ast(L, op, R));
	}
}