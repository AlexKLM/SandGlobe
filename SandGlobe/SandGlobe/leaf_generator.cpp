#include "leaf_generator.h"

leaf_generator::leaf_generator(void)
{
}


leaf_generator::~leaf_generator(void)
{
}

void leaf_generator::setup(const std::string& _start,const  std::string& _rule,const std::string& _target)
{
	start = _start;
	rule = _rule;
	target = _target;
}

bool leaf_generator::generate(int generations, std::string& out_result)
{
	int debug_gen;
	out_result = start;
	for(int i = 0; i != generations; i++)
	{
		replace(out_result);
		debug_gen = i;
	}
	ofstream out("test.txt");
	out<<out_result.c_str();

	out.close();
	return true;
}


void leaf_generator::replace(std::string& current) const
{
	size_t start_pos = 0;
	start_pos = current.find(target, start_pos);
	while(start_pos != std::string::npos)
	{
        current.replace(start_pos, target.length(), rule); //replace the target with its current rule set eg: turns a to ab if the rule is a->ab
        start_pos += rule.length(); 
		start_pos = current.find(target, start_pos);
    }

}