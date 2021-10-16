#pragma once

#include <iterator>

#include <string>  
#include <iostream> 
#include <sstream>
#include <cmath>
#include <chrono>

#include "task.h"

class Test
{
	std::istream& m_input;
	std::ostream& m_output_ref;

	std::stringstream m_output;

	struct TestContainer
	{
		Task& m_task;
		std::istream& m_input;
		std::stringstream& m_output;

		int m_size_after;
		int m_size_before;

		long long ms_duration;

		std::string m_result;

		TestContainer(Task& task, std::istream& input, std::stringstream& output) : m_task(task), m_input(input), m_output(output), m_size_after(0), m_size_before(0) {}

		TestContainer& input(const std::string command)
		{
			m_size_before = m_output.str().size();

			std::istringstream buffer(command);

			m_input.rdbuf(buffer.rdbuf());

			auto start = std::chrono::system_clock::now();

			m_task.run(m_input, m_output);

			auto end = std::chrono::system_clock::now();

			ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

			m_result = std::move(m_output.str());

			m_size_after = m_result.size();
			
			return (*this);
		}

		bool expect(const std::string result, const int precision = -1) const
		{
			if (precision == -1)
			{
				const auto substr = m_result.substr(m_size_before, m_size_after - m_size_before);

				const bool b_result = (m_size_after > m_size_before) && (result == substr);

				if (b_result == false)
				{
					std::cout << "Result: " << substr << ", expected: " << result << std::endl;
				}
				else
				{
					std::cout << "Test passed in: " << ms_duration << " ms" << std::endl;
				}

				return b_result;
			}
			else
			{
				const auto substr = m_result.substr(m_size_before, precision);
				const auto subres = result.substr(0, precision);

				const bool b_result = (m_size_after > m_size_before) && (subres == substr);

				if (b_result == false)
				{
					std::cout << "Result: " << substr << ", expected: " << result << std::endl;
				}
				else
				{
					std::cout << "Test passed!" << std::endl;
				}

				return b_result;
			}
		}
	};

public:

	Test(std::istream& input, std::ostream& output) : m_input(input), m_output_ref(output) {}

	TestContainer open(Task& task) const
	{
		return TestContainer(task, m_input, const_cast<std::stringstream&>(m_output));
	}

};