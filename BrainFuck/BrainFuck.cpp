// BrainFuck.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <print>
#include <array>
#include <map>
#include <iostream>
#include <numeric>
#include <functional>
#include <ranges>
#include <sstream>
#include <stack>
#include <chrono>
#include <thread>
namespace
{
	bool printInstructions{ false };
	using Memory = std::array<unsigned char,32>;

	void printBytes(const Memory& memory)
	{
		for (const auto c : memory)
		{
			std::println("{0:08b} {0:#8x} {0:8} {1:>8?}", c,char(c));
		}
		std::println("");
	}

	const std::map<char, unsigned char> byteCodeMap
	{
		{'+', 0},
		{ '-', 1 },
		{'>', 2},
		{ '<', 3 },
		{'.', 4},
		{',', 5 },
		{'[',6 },
		{']',7},
	};

	void printProgramCode(const std::string& code)
	{
		if (printInstructions)
		{
			for (const auto [index, c] : code | std::views::enumerate)
			{
				auto it = byteCodeMap.find(c);
				if (it != byteCodeMap.end())
				{
					std::println("{:#04x} {:03b} {:>8?}", index, it->second, c);
				}
			}
		std::println("");
		}
		for (const auto  c : code )
		{
			auto it = byteCodeMap.find(c);
			if (it != byteCodeMap.end())
			{
				std::print("{:03b} ", it->second);
			}
		}
		std::println("");
	}

	struct InputOutput
	{
		std::istream& input;
		std::ostream& output;
	};

	const std::map<char, std::function<void(Memory::iterator&,const InputOutput&)>> functionMap
	{
		{'+', [](Memory::iterator& memoryPtr,const InputOutput&) {(*memoryPtr)++; }},
		{'-', [](Memory::iterator& memoryPtr,const InputOutput&) {(*memoryPtr)--; } },
		{'>', [](Memory::iterator& memoryPtr,const InputOutput&) {memoryPtr++; }},
		{'<', [](Memory::iterator& memoryPtr,const InputOutput&) {memoryPtr--; } },
		{',', [](Memory::iterator& memoryPtr,const InputOutput& inOut) {inOut.input >> *memoryPtr; }},
		{'.', [](Memory::iterator& memoryPtr,const InputOutput& inOut) {inOut.output << *memoryPtr; } },
		{'[', [](Memory::iterator& memoryPtr,const InputOutput& inOut) { }},
		{']', [](Memory::iterator& memoryPtr,const InputOutput& inOut) { } },
	};

	std::function<void(Memory::iterator&,const InputOutput&)> toFunction(const unsigned char input)
	{
		auto it = functionMap.find(input);
		if (it != functionMap.end())
		{
			return it->second;
		}
		return [](Memory::iterator&,const InputOutput&) {};
	}

	void present(std::string& code, Memory& memory, std::stringstream& output)
	{
		system("CLS");
		std::println("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");
		std::println("");
		printProgramCode(code);
		std::println("");
		printBytes(memory);
		std::println("");
		std::println("Code:				{}", code);
		std::println("Program output:	{}", output.str());

		std::println("");
		std::println("Type in Code:");
	}
}



int main()
{
	Memory memory{};
	Memory::iterator memoryPtr{ memory.begin()};
	//std::ranges::iota(memory, 0);

	std::stringstream output{};
	std::string code;
	while (true)
	{
		present(code, memory, output);

		std::string input;
		std::cin >> input;

		auto validInput{ input | std::views::filter([](auto c) {return functionMap.contains(c); }) };
		code.append(std::string{ validInput.begin(),validInput.end()});
		output.clear();
		memory = {};
		memoryPtr = memory.begin();

		auto codeStack{ code /*| std::views::transform(toFunction) */};
		std::stack<decltype(codeStack.begin())> loopStack;

		auto runCode = [&](this auto& self, decltype(codeStack.begin())& codeIt)   -> void
			{
			const auto beginOfCurrendStack = codeIt;
			for (; codeIt != codeStack.end(); ++codeIt)
			{
				present(code, memory, output);
				//using namespace std::chrono_literals;
				//std::this_thread::sleep_for(200ms);

				if ((*codeIt) == '[')
				{
					if ((*memoryPtr) != 0)
					{
						++codeIt;
						self(codeIt);
					}
					continue;
				}
				else if((*codeIt) == ']')
				{
					if ((*memoryPtr) != 0)
					{
						if (beginOfCurrendStack == codeStack.begin())
						{
							int i = 0; 
						}
						else {
							codeIt = beginOfCurrendStack;
							codeIt--;
							continue;
						}
					}
					else
					{
						//++codeIt;
						return;
					}
				}
				toFunction(*codeIt)(memoryPtr, InputOutput{ std::cin, output });
			}
			};


		auto iter = codeStack.begin();
		runCode(iter);
	}
 



}

