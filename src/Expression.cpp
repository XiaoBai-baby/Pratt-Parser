#include "Expression.h"

Lexer::Lexer(const string& input)
{
	string Atom_result;
	string Operator_result;

	// 遍历字符串;
	for (int i = 0; i < input.size(); i++)
	{
		char c = input[i];

		// 检测为空白字符, 则跳过;
		if (c >= 0 && isspace(c))
			continue;

		// 判断是否输入错误;
		if (i == input.size() - 1)
			checkError(c, Operator_result);

		if (c == '.' || c >= '0' && c <= '9')			// 添加数字;
		{
			if (Operator_result.size() > 0)
				addOperator(Operator_result);

			// 添加数字;
			Atom_result += c;
		}
		else if (isLetter(c))				//添加变量;
		{
			if (Operator_result.size() > 0)
				addOperator(Operator_result);

			// 添加变量;
			addVariate(c, Atom_result);
		}
		else if (c == '(')				// 前括号运算符的特殊处理;
		{
			if (Atom_result.size() > 0)
				addAtom(Atom_result);

			if (Operator_result.size() > 0)
				addOperator(Operator_result);

			Operator_result = c;
			addOperator(Operator_result, true);
		}
		else if (c == ')')				// 后括号运算符的特殊处理;
		{
			if (Atom_result.size() > 0)
				addAtom(Atom_result);

			Operator_result = c;
			addOperator(Operator_result, true);
		}
		else if (c == '=')				// 等于号运算符的特殊处理;
		{
			// 等于号 之前的字符必须为变量;
			char first = Atom_result[0];
			if (isLetter(first)) {}
			else { throw string("'=' must be preceded by a variable ."); }

			if (Atom_result.size() > 0)
				addAtom(Atom_result);

			if (Operator_result.size() > 0)
				throw string("'=' must be preceded by a variable .");

			Operator_result = "=";
			addOperator(Operator_result);
		}
		else if (Atom_result.size() > 0)
		{
			addAtom(Atom_result);
			Operator_result += c;			// 添加运算符;
		}
		else
		{
			// 添加单个字节的运算符, 防止单字节与多字节运算符合并出错, 如 1 + √4;
			if(isSingle(Operator_result))
				addOperator(Operator_result);

			// 添加多个字节的运算符;
			Operator_result += c;
		}
	}
		
	// 添加最后一个原子或运算符;
	addEnd(Atom_result, Operator_result);
	
	// 判断是否为单个原子, 防止输出异常;
	if (tokens.size() <= 1)
	{
		if(tokens[0].variateAtom.size() <= 0)
			throw string(input);
		else
			single_variate = true;
	}
	
	// 反转字符, 以便后续操作;
	std::reverse(tokens.begin(), tokens.end());
}

bool Lexer::isLetter(char c)
{
	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
		return true;
	return false;
}

void Lexer::checkError(char c, string& Operator_result)
{
	//e.g: a = 5 + 8 +
	if (isSingle(&c))
		throw string("input the error expression .");

	// 是否为 括号操作符;
	bool is_operater = false;
	if (c == '(' || c == ')')
		is_operater = true;

	// e.g: a = 9 + )
	if (Operator_result.size() > 0 && is_operater)
	{
		if (isSingle(Operator_result))
			throw string("input the error expression .");

		// "√" 字符串;
		if (isMulti(Operator_result))
			throw string("input the error expression .");
	}
}

// bracket: Operator 是否为括号;
void Lexer::addOperator(string& Operator, bool bracket)
{
	if(!isSingle(Operator) && !isMulti(Operator) && !bracket)
		throw string("bad operator of " + Operator + " .");

	Token v;
	v.Operator = Operator;
	tokens.push_back(v);
	Operator.clear();
}

void Lexer::addVariate(char c, string& Atom)
{
	// 添加变量;
	if(Atom.size() <= 0)
	{
		Atom += c;
	}
	else
	{
		// 变量的首字母不能为数字;
		char first = Atom[0];
		if(isLetter(first))
			Atom += c;
		else
			throw string("define erroneous variable .");
	}
}

void Lexer::addAtom(string& Atom)
{
	Token v;
	char first = Atom[0];
	
	if(isLetter(first))
	{
		// 变量原子;
		v.variateAtom = Atom;
	}
	else
	{
		// 数字原子;
		v.Atom = Atom;
	}
				
	// 添加原子;
	tokens.push_back(v);
	Atom.clear();
}

void Lexer::addEnd(string& Atom, string& Operator)
{
	// 添加原子;
	if(Atom.size() > 0)
	{
		addAtom(Atom);
	}
	
	// 添加运算符;
	if(Operator.size() > 0)
	{
		addOperator(Operator);
	}
}

string Lexer::next()
{
	// 判断容器是否为空;
	if (tokens.empty()) 
	{
		return "";		// Eof
	}
		
	// 获取容器最后一个元素;
	string Atom = tokens.back().Atom;
	if (Atom.size() <= 0)
	{
		Atom = tokens.back().variateAtom;
	}

	// 销毁最后一个元素;
	tokens.pop_back();
	return Atom;
}

string Lexer::peek()
{
	return tokens.empty() ? "" : tokens.back().Operator;
}

bool isMulti(string Operator)
{
	if (Operator.size() < 2)
		return false;

	int count = 0;
	int max_count = sizeof(multibyte_operator[0]);

	do
	{
		int number = 0;
		for (int i = 0; i < Operator.size(); i++)
		{
			if ((char)Operator[i] != multibyte_operator[count][i])
				break;
			else
				number++;

			// 全部字符都相同, 返回 true;
			if (number == Operator.size())
				return true;
		}
		count++;
	} while (count < max_count);

	return false;
}

bool isSingle(string Operator)
{
	if (Operator.size() > 1 && Operator.size() <= 0)
		return false;

	int flag= single_operator.find_first_of(Operator[0]);
	if (flag >= 0)
		return true;

	return false;
}


/****************************************************************************************
 * 	This is the main function of the Pratt Parser.                                      *
 *  when an equation passed in,                                                         *
 *  the parser will check the equation from the left                                    *
 *  and link up a token tree according to the OPERATOR precedence.                      *
 *                                                                                      *
 *  example1: precedence of multiply(*) is higher than add(+),                          *
 *            so when an equation "2 + 3 * 4" entered to the parser,                    *
 *            it will link (3 * 4) and form a brunch,                                   *
 *            then parser will link the brunch to integer "2" by (+)                    *
 *            last, it will form a tree as shown below.                                 *
 *            This is an example of LINK DOWN.                                          *
 *                                                                                      *
 *                    (+)                                                               *
 *                    / \                                                               *
 *                  (2) (*)                                                             *
 *                      / \                                                             *
 *                    (3) (4)                                                           *
 *                                                                                      *                                                                                       *
 *  example2: precedence of sub(-) is smaller than multiply(*),                         *
 *            so when an equation "2 * 3 - 4" entered to the parser,                    *
 *            it will link (2 * 3) and form a brunch,                                   *
 *            then parser will link the brunch to integer "2" by (-)                    *
 *            last, it will form a tree as shown below.                                 *
 *            This is an example of LINK UP.                                            *
 *                                                                                      *
 *                    (-)                                                               *
 *                    / \                                                               *
 *                  (*) (4)                                                             *
 *                  / \                                                                 *
 *                (2) (3)                                                               *
 *                                                                                      *
 ****************************************************************************************/
Expression::Expression()
{
	
}

Expression::Expression(long double atom): atomValue(atom)
{
	Type type;
	type.Atom = atom;
}

Expression::Expression(string operation, std::vector<Type> operands)
{
	type.Operation.first = operation;
	type.Operation.second = operands;
}

Expression::Type Expression::from_str(const string& input)
{
	Lexer lexer(input);
	return parse_expression(lexer, 0.0);
}


bool Expression::is_exist(string variate) const
{
	auto same_variate = variables.find(variate);
	if (same_variate != variables.end())
		return true;

	return false;
}

void Expression::string_to_atom(Expression::Type& lhs, string first_token)
{
	long double token;
	std::istringstream iss(first_token);

	if (first_token.size() <= 0)
		return;

	// 单个变量的处理;
	if (single_variate)
	{
		single_variate = false;
		bool is_integer = false;
		
		// 判断是否为整数;
		long double decimal = variables[first_token];
		long int integer = (long int) decimal;
		if (decimal - integer == 0)
			is_integer = true;

		// 返回变量的值;
		if (!is_exist(first_token))
		{
			throw first_token;
		}
		else if(is_integer)
		{
			throw std::to_string(integer);
		}
		else
		{
			throw std::to_string(decimal);
		}
	}

	char first = first_token[0];
	if (first == '.')
		throw string("input the error expression .");

	if (first >= '0' && first <= '9')
	{
		// 变量的首字母不能为数字;
		iss >> token;

		if (is_minus != 1)
		{
			lhs.Atom = token;
		}
		else
		{
			// 存在负数因子;
			lhs.Atom = -token;
			is_minus = 0;
		}
	}
	else if (is_minus != 1)
	{
		// 变量原子;
		lhs.variateAtom = first_token;
	}
	else
	{
		// 存在负数 变量因子;
		lhs.variateAtom = "-";
		lhs.variateAtom += first_token;
		is_minus = 0;
	}
}

// min_bp 为 前运算符的优先级, 用于函数归递的时候, 对比下一个运算符的优先级;
Expression::Type Expression::parse_expression(Lexer& lexer, double min_bp)
{
	Expression::Type lhs;

	// 判断是否存在负数;
	string negative_sign = lexer.peek();
	if (negative_sign == "-" && min_bp < 1)
	{
		// 销毁负号;
		lexer.next();
		is_minus = 1;
	}

	// 优先处理括号内的表达式;
	string first_token = lexer.peek();
	if (first_token == "(")
	{
		// 销毁前括号;
		lexer.next();

		// min_bp 必须为 0, 以折叠括号内的表达式;
		lhs = parse_expression(lexer, 0.0);

		first_token = lexer.peek();
		if(first_token != ")")
			throw string("bad operator, undefined ')' symbol .");
	}
	
	// 获得表达式的原子;
	if (!isSingle(first_token) && !isMulti(first_token))
	{
		first_token = lexer.next();
		if (first_token.size() <= 0)
			return lhs;
		string_to_atom(lhs, first_token);
	}

	while (true)
	{
		string op = lexer.peek();
		if (op == "\0" || op == ")" || op == "")
		{
			break;
		}

		// 判断多字节运算符是否携带单个原子 或单条原子链;
		bool isSingle_atom = false;
		if (isMulti(op) && isMulti(first_token))
			isSingle_atom = true;

		float l_bp, r_bp;
		std::tie(l_bp, r_bp) = infix_binding_power(op);
		if (l_bp < min_bp)
		{
			/*
				在这里结束循环, 说明函数归递的时候, 遇到优先级较低的运算符, /
				函数返回单个原子, 并通过 while 循环, 获取完整的原子表达式;
			*/
			break;
		}

		// 销毁运算符;
		lexer.next();
		Expression::Type rhs = parse_expression(lexer, r_bp);			// 当 r_bp 的优先级过高时, 则返回单个原子; 优先级过低时, 则返回表达式;
		lhs = Expression(op, { lhs, rhs }).type;
		if (isSingle_atom)
			lhs.Operation.second[0].Atom = 1;							// 多字节运算符左边的原子置为1;
	}
	
	return lhs;
}

std::pair<float, float> Expression::infix_binding_power(string op)
{
	/* 多字节运算符 */
	if (op.size() > 1)
	{
		// "√" 字符串;
		if(isMulti(op))
			return { 3.1f, 3.0f };
		else
			throw string("bad operator of " + op + " .");
	}
	
	/* 单字节运算符 */
	char Operator = op[0];
	switch (Operator)
	{
		case '=': return { 0.2f, 0.1f };
		case '+':
		case '-': return { 1.0f, 1.1f };
		case '*':
		case '/': return { 2.0f, 2.1f };
		case '^': return { 3.1f, 3.0f };
		case '(':
		case ')': return { 9.0f, 9.1f };
		default: throw string("bad operator of " + op + " .");
	}
}


double Expression::right_value_is_expression(string& lvalue_reference, Type& operands, int expression_size)
{
	auto lvalue_variate = variables.find(lvalue_reference);

	for (int i = 0; i < expression_size; i++)
	{
		// 判断右值变量是否有效;
		string variateAtom = operands.Operation.second[1].Operation.second[i].variateAtom;
		if (variateAtom.size() > 0)
		{
			// 等号右值为变量;
			lvalue_variate = variables.find(variateAtom);
			if (lvalue_variate == variables.end())
				throw string("invalid variable of " + variateAtom + " .");
		}
	}

	// 解析原子表达式;
	string op = operands.Operation.second[1].Operation.first;
	std::vector<Type> Operation = operands.Operation.second[1].Operation.second;
	Operation = parse_atomExpression(Operation, op);

	// 更改或存放 左值变量;
	lvalue_variate = variables.find(lvalue_reference);
	if (lvalue_variate != variables.end())
		variables[lvalue_reference] = Operation[0].Atom;
	else
		variables.insert({ lvalue_reference, Operation[0].Atom });

	return Operation[0].Atom;
}

double Expression::left_value_existing_variable(string& lvalue_reference, string& rvalue_reference, Type& operands)
{
	if (operands.Operation.second.size() != 2)
		return 0;

	auto lvalue_variate = variables.find(lvalue_reference);
	auto rvalue_variate = variables.find(rvalue_reference);

	// 等号右值为数值;
	if (rvalue_reference.size() <= 0)
	{
		long double decimal = operands.Operation.second[1].Atom;
		variables[lvalue_reference] = operands.Operation.second[1].Atom;
		return decimal;
	}

	// 等号右值为变量;
	lvalue_variate = variables.find(rvalue_reference);
	if (lvalue_variate == variables.end())
	{
		throw string("invalid variable of " + rvalue_reference + " .");
	}
	else
	{
		// 左值的变量重新赋值;
		long double value = variables[rvalue_reference];
		variables[lvalue_reference] = value;
		return value;
	}

	return 0;
}

double Expression::right_value_existing_variable(string& lvalue_reference, string& rvalue_reference, Type& operands)
{
	auto lvalue_variate = variables.find(lvalue_reference);
	auto rvalue_variate = variables.find(rvalue_reference);

	if (operands.Operation.second.size() != 2)
		return 0;

	// 等号右值为数值;
	string same_variate = rvalue_variate->first;
	if (same_variate.size() <= 0)
	{
		long double decimal = operands.Operation.second[1].Atom;
		variables[lvalue_reference] = operands.Operation.second[1].Atom;
		return decimal;
	}

	// 等号右值为变量;
	lvalue_variate = variables.find(same_variate);
	if (lvalue_variate == variables.end())
	{
		throw string("invalid variable of " + same_variate + " .");
	}
	else
	{
		// 存放位于右值的变量;
		long double value = rvalue_variate->second;
		variables.insert({ lvalue_reference, value });
		return value;
	}

	return 0;
}

double Expression::eval(Type operands)
{
	string op = operands.Operation.first;
	if (is_asign(operands))
	{
		// 注意, second 是一个 vector 类型;
		string lvalue_reference = operands.Operation.second[0].variateAtom;				// 左值引用;
		string rvalue_reference = operands.Operation.second[1].variateAtom;				// 右值引用;
		auto lvalue_variate = variables.find(lvalue_reference);
		auto rvalue_variate = variables.find(rvalue_reference);

		int expression_size = operands.Operation.second[1].Operation.second.size();				// 右值的个数;
		bool is_expression = expression_size >= 2 ? true : false;								// 右值是否为表达式;

		
		if (is_expression)
		{
			// 右值为表达式;
			return right_value_is_expression(lvalue_reference, operands, expression_size);
		}
		else if (lvalue_variate != variables.end())
		{
			// 左值存在有效变量;
			return left_value_existing_variable(lvalue_reference, rvalue_reference, operands);
		}
		else if (rvalue_variate != variables.end())
		{
			// 创建左值变量赋值时, 右值为有效变量;
			return right_value_existing_variable(lvalue_reference, rvalue_reference, operands);
		}
		else
		{
			// 右值变量或数值;
			long double value = operands.Operation.second[1].Atom;
			variables.insert({ lvalue_reference, value });
			return value;
		}
	}
	else
	{
		// 解析表达式;
		std::vector<Type> Operation = operands.Operation.second;
		Operation = parse_atomExpression(Operation, op);
		return Operation[0].Atom;
	}

	return 0;
}

bool Expression::is_asign(Type& operands) const
{
	Type result;
	string op = operands.Operation.first;
	if (op == "=")
	{
		std::vector<Type> Operation = operands.Operation.second;

		op = Operation[0].Operation.first;
		if (op[0] >= 'a' && op[0] <= 'z' || op[0] >= 'A' || op[0] <= 'Z')
		{
			return true;
		}

		throw string("no variable name or define erroneous variable .");
	}

	return false;
}

void Expression::check_atom(string operation) const
{
	int position = operation.find_first_of("+-*/^");
	if(position >= 0)
		throw string("input superfluous operator of " + operation + " .");
}

void Expression::variable_value(Type& operands) const
{
	string variate = operands.variateAtom;
	if (variate.size() > 0)
	{
		// 查找变量的值;
		auto same_variate = variables.find(variate);
		if (same_variate != variables.end())
		{
			// 变量原子 转化为原子;
			operands.Atom = same_variate->second;
			if (variate[0] == '-' && operands.Atom > 0)
				operands.Atom = -operands.Atom;					// 负数;

			if (variate[0] == '-' && operands.Atom < 0)
				operands.Atom = abs(operands.Atom);				// 正数;

			// 销毁变量原子;
			operands.variateAtom.clear();
		}
	}
}

double Expression::calculator(string operation, std::vector<Type> operands)
{
	long double lhs = operands[0].Atom;
	long double rhs = operands[1].Atom;

	/* 多字节运算符 */
	if (operation.size() > 1)
	{
		// "√" 字符串;
		if (operation[0] == (char)0xa1 && operation[1] == (char)0xcc)
		{
			lhs = pow(lhs, 2);
			return sqrt(lhs * rhs);
		}
		else
		{
			throw string("bad operator of " + operation + " .");
		}
	}

	// 遇到运算符为"-", 右值为负数;
	if (operation == "-" && rhs < 0)
	{
		rhs = abs(rhs);
		return lhs + rhs;
	}

	/* 单字节运算符 */
	if (operation == "+")
		return lhs + rhs;
	else if (operation == "-")
		return lhs - rhs;
	else if (operation == "*")
		return lhs * rhs;
	else if (operation == "/")
		return lhs / rhs;
	else if (operation == "^")
		return std::pow(lhs, rhs);
	else
		throw string("bad operator of " + operation + " .");

	return 0;
}

std::vector<Expression::Type> Expression::parse_atomExpression(std::vector<Type> Operation, string operation)
{
	string op;
	std::vector<Type> operands;
	for (int i = 0; i < 2; i++)				// 二分叉, 故 i < 2;
	{
		op = Operation[i].Operation.first;
		operands = Operation[i].Operation.second;
		
		// 遍历整个二分叉树;
		if (op.size() > 0 && operands.size() > 1)
		{
			// 递归自身, 遍历所有原子;
			operands = parse_atomExpression(operands);
		}
		else if (op.size() > 0 && operands.size() > 0)			// 只有一个原子;
		{
			// 检查操作符;
			check_atom(op);
			variable_value(operands[i]);
		}

		// 将变量转为原子;
		if (operands.size() > 1)
		{
			variable_value(operands[0]);
			variable_value(operands[1]);
		}

		if (op.size() > 0 && operands.size() > 1)
		{
			Operation[i].Atom = calculator(op, operands);
			Operation[i].Operation.first.clear();		// 销毁运算符;
		}
	}

	// 计算 最后一次原子表达式的值;
	if (operation.size() > 0)
	{
		std::ostringstream oss;
		oss << Operation[1].Atom;

		for (int i = 0; i < Operation.size(); i++)
			variable_value(Operation[i]);
		Operation[0].Atom = calculator(operation, Operation);
	}

	return Operation;
}