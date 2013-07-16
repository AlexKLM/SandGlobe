#pragma once
#include <iostream>
#include <fstream>
using namespace std;

class leaf_generator
{
public:
	leaf_generator(void);
	~leaf_generator(void);
	void setup(const std::string& _start,const std::string& _rule,const std::string& _target);
	bool generate(int generations, std::string& out_results);

private:
	void replace(std::string& current) const;
	std::string target;
	std::string start;
	std::string rule;
};

