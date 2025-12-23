#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using std::string;

// 负数因子;
static int is_minus = 0;

// 单变量因子;
static bool single_variate = false;

// 单字节运算符;
static string single_operator = "+-*/^%=,";

// 多字节运算符;
static const char multibyte_operator[4][4] = { {0xa1, 0xcc} };

// 普拉特解析器;
class Expression;

// 判断是否为多字节运算符;
bool isMulti(string Operator);

// 判断是否为单字节运算符;
bool isSingle(string Operator);

// 原子的令牌;
struct Token
{
	string Atom;			// 原子;
	string Operator;		// 运算符;
	string variateAtom;		// 变量原子;
};

/* Lexer
	分词器程序 tokenizer;
	其中基本原子是单个字符数和变量，并且使用标点符号作为运算符;
*/
class Lexer
{
public:
    Lexer(const string& input);
	
private:
	// 字符是否为字母;
	bool isLetter(char c);

	// 检查表达式的末尾错误;
	void checkError(char c, string& Operator_result);

private:
	// 添加运算符;
	void addOperator(string& Operator, bool bracket = false);
	
	// 添加变量;
	void addVariate(char c, string& Atom);
	
	// 添加原子;
	void addAtom(string& Atom);
	
	// 添加最后的原子;
	void addEnd(string& Atom, string& Operator);
	
public:
	// 获取原子, 并销毁最后一个元素;
    string next();
	
	// 获取运算符;
    string peek();

private:
	// 存储原子和运算符;
    std::vector<Token> tokens;
};

class Expression
{
public:
	// 原子表达式;
	struct Type
	{
		long double Atom;					// 原子;
		string variateAtom;					// 变量原子;
		
		// 原子表达式, 即前缀表示法, 如: a - b * c = (- a (* b c));
		std::pair<string, std::vector<Type>> Operation;
	};

public:
	Expression();

    Expression(long double atom);

    Expression(string operation, std::vector<Type> operands);
	
public:
	// 测试原子表达式;
    Expression::Type from_str(const string& input);

private:
	// 判断是否有 存储变量的值;
	bool is_exist(string variate) const;

	// 将字符串转化为原子, 辅助 parse_expression 使用;
	void string_to_atom(Expression::Type& lhs, string first_token);

	// 解析表达式, 将 Lexer类 转化为原子表达式;
    Type parse_expression(Lexer& lexer, double min_bp = 0.0);
	
	// 运算符左右原子的绑定强度, 即优先级;
    std::pair<float, float> infix_binding_power(string op);
	
private:
	// 右值为表达式, 辅助 eval 使用;
	double right_value_is_expression(string& lvalue_reference, Type& operands, int expression_size);

	// 左值存在有效变量, 辅助 eval 使用;
	double left_value_existing_variable(string& lvalue_reference, string& rvalue_reference, Type& operands);

	// 创建左值变量赋值时, 右值为有效变量, 辅助 eval 使用;
	double right_value_existing_variable(string& lvalue_reference, string& rvalue_reference, Type& operands);

public:
	// 解析器程序;
	double eval(Type operands);

private:
	// 检查是否有赋值操作;
	bool is_asign(Type& operands) const;

	// 检查是否为 可携带单个原子的操作符;
	void check_atom(string operation) const;

	// 变量原子转换为原子;
	void variable_value(Type& operands) const;

	// 计算原子表达式的值, 辅助 parse_atomExpression 使用;
	double calculator(string operation, std::vector<Type> operands);

	// 解析原子表达式 或所有原子;
	std::vector<Type> parse_atomExpression(std::vector<Type> Operation, string operation = "");

private:
	// 初始化函数返回使用;
    Type type;
    long double atomValue;

private:
	// 存放全部变量的值;
	std::unordered_map<string, long double> variables;
};
#endif