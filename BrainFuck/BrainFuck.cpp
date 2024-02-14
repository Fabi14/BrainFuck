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

#include <windows.h>

namespace
{
	bool printInstructions{ false };
	using Memory = std::array<unsigned char,32>;

	void printBytes(const Memory& memory, Memory::iterator memoryPtr)
	{
		for (auto it{ memory.begin() }; it<memory.end(); ++it)
		{
			if (it == memoryPtr)
			{
				std::println("\x1b[31m{0:08b} {0:#8x} {0:8} {1:>8?}\x1b[m", *it, static_cast<char>(*it));
			}
			else
			{
				std::println("{0:08b} {0:#8x} {0:8} {1:>8?}", *it,static_cast<char>(*it));
			}
			
		}
		std::println("");
	}

	const std::map<char, unsigned char> byteCodeMap
	{
		{'+',static_cast<unsigned char>( 0u)},
		{'-',static_cast<unsigned char>( 1u)},
		{'>',static_cast<unsigned char>( 2u)},
		{'<',static_cast<unsigned char>( 3u)},
		{'.',static_cast<unsigned char>( 4u)},
		{',',static_cast<unsigned char>( 5u)},
		{'[',static_cast<unsigned char>( 6u)},
		{']',static_cast<unsigned char>( 7u)},
	};

	void printProgramCode(const std::string& code, std::string::iterator codeIter)
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
		for (auto codeIt = code.begin(); codeIt<code.end(); codeIt++ )
		{
			auto it = byteCodeMap.find(*codeIt);
			if (it != byteCodeMap.end())
			{
				if (codeIt == codeIter)
				{
					std::print("\x1b[31m{:03b} \x1b[m", it->second);
				}
				else
				{
					std::print("{:03b} ", it->second);
				}
			}
		}
		std::println("");
	}

	struct InputOutput
	{
		std::istream& input;
		std::ostream& output;
	};

	void present(std::string& code, Memory& memory, std::stringstream& output, Memory::iterator memoryPtr, std::string::iterator codeIter,  bool bColor)
	{
		if (bColor)
		{
			static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			static short i{ 1 };
			SetConsoleTextAttribute(hConsole, ++i);
			i = i >= 255 ? 0 : i;
		}

		system("CLS");
		std::println("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.");
		std::println("+[>>>->-[>->----<<<]>>]>.---.>+..+++.>>.<.>>---.<<<.+++.------.<-.>>+.");
		printProgramCode(code, codeIter);
		std::println("");
		if (bColor)
		{
			static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			static short i{ 1 };
			SetConsoleTextAttribute(hConsole, ++i);
			i = i >= 255 ? 0 : i;
		}
		printBytes(memory, memoryPtr);
		std::println("");
		std::print("Code:				");
		for (auto codeIt = code.begin(); codeIt < code.end(); codeIt++)
		{
			if (codeIt == codeIter)
			{
				std::print("\x1b[31m{}\x1b[m", *codeIt);
			}
			else
			{
				if (bColor)
				{
					static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
					static short i{ 1 };
					SetConsoleTextAttribute(hConsole, ++i);
					i = i >= 255 ? 0 : i;
				}
				std::print("{}", *codeIt);
			}

		}
		std::println("");
		std::println("Output:	{}", output.str());

		std::println("");
		std::println("Type in Code:");
	}
}

int main()
{
	bool bShow{ false };
	bool bColor{ false };
	Memory memory{};
	Memory::iterator memoryPtr{ memory.begin()};

	std::stringstream output{};
	std::string code;

	std::function<void()> runLoop;

	const std::map<char, std::function<void(const InputOutput&)>> functionMap
	{
		{'+', [&](const InputOutput&) {(*memoryPtr)++; }},
		{'-', [&](const InputOutput&) {(*memoryPtr)--; } },
		{'>', [&](const InputOutput&) {memoryPtr = memoryPtr == memory.end()-1 ? memory.begin(): memoryPtr+1; }},
		{'<', [&](const InputOutput&) {memoryPtr = memoryPtr == memory.begin() ? memory.end()-1 : memoryPtr - 1; } },
		{',', [&](const InputOutput& inOut) {inOut.input >> *memoryPtr; }},
		{'.', [&](const InputOutput& inOut) {inOut.output << *memoryPtr; } },
		{'[', [&](const InputOutput& ) {runLoop(); }},
		{']', [&](const InputOutput& ) {} },
	};

	auto toFunction = [&](const unsigned char input) -> std::function<void( const InputOutput&)>
		{
			auto it = functionMap.find(input);
			if (it != functionMap.end())
			{
				return it->second;
			}
			return [](const InputOutput&) {};
		};

	auto runCode = [&](auto func, std::string::iterator codeIter)
		{
			if (bShow)
			{
				present(code, memory, output, memoryPtr, codeIter, bColor);
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(200ms);
			}
			func(InputOutput{ std::cin, output });
		};

	while (true)
	{
		present(code, memory, output, memoryPtr, code.end(),  bColor);

		std::string input;
		std::cin >> input;
		
		if (input == "c")
		{
			code.clear();
		}
		else if (input == "s")
		{
			bShow = !bShow;
		}
		else if (input == "f")
		{
			bColor = !bColor;
		}

		auto validInput{ input | std::views::filter([&](auto c) {return functionMap.contains(c); }) };
		code.append(std::string{ validInput.begin(),validInput.end() });
		output.str("");
		memory = {};
		memoryPtr = memory.begin();

		auto codeIt = code.begin();

		runLoop = [&]()
			{
				++codeIt; // '[' wurde schon behandelt
				auto beginOfLoop = codeIt;
				while ((*memoryPtr) != 0)
				{
					codeIt = beginOfLoop; //jump to begin of Loop
					for (; (*codeIt) != ']'; ++codeIt)
					{
						runCode(toFunction(*codeIt), codeIt);
					}
				}
			};

		for (; codeIt != code.end(); ++codeIt)
		{
			runCode(toFunction(*codeIt), codeIt);
		}
	}
}

