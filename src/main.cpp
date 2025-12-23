#include "Expression.h"

int main()
{
	Expression expression;
	while (true)
	{
		std::cout << ">> ";
		std::cout.flush();
		string input;
		std::getline(std::cin, input);
		if (input.size() < 1)
		{
			continue;
		}
		if (input == "exit")
		{
			break;
		}

		// 使用普拉特解析器;
		try
		{
			Expression::Type expr = expression.from_str(input);

			long double value = expression.eval(expr);
			std::cout << value << std::endl;
		}
		catch (string& e)
		{
			std::cout << e << std::endl;
		}
	}

	// 测试原子表达式;
	// Expression::Type a = expression.from_str("a * b - (- c)");
	// Expression::Type b = expression.from_str("a * b - (1 - c)");
	// Expression::Type c = expression.from_str("-3 * 4");
	// Expression::Type d = expression.from_str("a * 3 - (b + 4 - (c - 5))");

	return 0;
}