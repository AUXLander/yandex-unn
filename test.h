#pragma once

#include <iterator>

#include <string>  
#include <iostream> 
#include <sstream>
#include <cmath>

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

		std::string m_result;

		TestContainer(Task& task, std::istream& input, std::stringstream& output) : m_task(task), m_input(input), m_output(output), m_size_after(0), m_size_before(0) {}

		TestContainer& input(const std::string command)
		{
			m_size_before = m_output.str().size();

			std::istringstream buffer(command);

			m_input.rdbuf(buffer.rdbuf());

			m_task.run(m_input, m_output);

			m_result = std::move(m_output.str());

			m_size_after = m_result.size();
			
			return (*this);
		}

		bool expect(const std::string result) const
		{
			const auto substr = m_result.substr(m_size_before, m_size_after - m_size_before);

			const bool b_result = (m_size_after > m_size_before) && (result == substr);

			if (b_result == false)
			{
				std::cout << "Result: " << substr << " but was expected: " << result << std::endl;
			}
			else
			{
				std::cout << "Test passed!" << std::endl;
			}

			return b_result;
		}
	};

public:

	Test(std::istream& input, std::ostream& output) : m_input(input), m_output_ref(output) {}

	TestContainer open(Task& task) const
	{
		return TestContainer(task, m_input, const_cast<std::stringstream&>(m_output));
	}

};