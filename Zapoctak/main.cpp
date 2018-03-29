#include "TextAnalyzer.h"
#include "ProfileMatcher.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

void no_arg_value_message(const std::string & arg)
{
	std::cerr << "Arguement value not found for argument " + arg << std::endl;
	std::cin.get();
	std::exit(1);
}

bool has_arg(const std::vector<std::string> & args, const std::string & arg_name)
{
	return std::find(args.begin(), args.end(), arg_name) != args.end();
}

bool get_arg(const std::vector<std::string> & args, const std::string & arg_name, std::string & value)
{
	auto it = std::find(args.begin(), args.end(), arg_name);
	if(it != args.end())
	{
		if (++it == args.end()) no_arg_value_message(arg_name);
		value = *it;
		return true;
	}

	return false;
}

void save_ngrams(const std::string & file_name, const LangProfile & profile, const std::vector<std::string> & args)
{
	std::wofstream out;
	out.open(file_name);
	if (!out.good()) 
	{
		std::cout << "Cannot save n-grams to" << file_name << std::endl;
		return;
	}

	std::string arg_value;
	std::cout << "Saving ranked n-grams to " << file_name << std::endl;
	if (get_arg(args, "-n", arg_value) || get_arg(args, "--name", arg_value))
	{
		out << '#' << std::wstring(arg_value.begin(), arg_value.end()) << std::endl;
	}
	if (get_arg(args, "-c", arg_value) || get_arg(args, "--count", arg_value))
	{
		int count = 0;
		try
		{
			count = std::stoi(arg_value);
		}
		catch (std::invalid_argument &)
		{
			std::cerr << "Error parsing argument value for argument count!" << std::endl;
		}
		profile.get_ranking(count).print(out);
	}
	else
	{
		profile.get_ranking().print(out);
	}
}

int main(int argc, char ** argv)
{
	std::vector<std::string> args(argv, argv + argc);
	ProfileMatcher matcher;
	TextAnalyzer ta;
	LangProfile profile;
	std::string name;
	std::string arg_value;
	if(get_arg(args, "-a", arg_value) || get_arg(args, "--analyze", arg_value))
	{
		std::wifstream f;
		f.open(arg_value);
		if(!f.good())
		{
			std::cerr << "Could not open file " << arg_value << std::endl;
		}
		std::cout << "Analyzing language..." << std::endl;
		ta.set_input(f);
		profile = ta.analyze();
		std::cout << "Found " << profile.get_ngram_count().size() << " distinct n-grams." << std::endl;
		auto ranking = profile.get_ranking();
	}
	if(get_arg(args, "-R", arg_value) || get_arg(args, "--refrank", arg_value))
	{
		std::cout << "Loading reference laguages..." << std::endl;
		matcher.load_ref_langs(arg_value);
		std::cout << "Loaded " << matcher.get_ref_langs().size() << " reference languages." << std::endl;
	}
	if(get_arg(args, "-r", arg_value) || get_arg(args, "--reftext", arg_value))
	{
		std::cout << "Loading reference laguages from text..." << std::endl;
		matcher.load_ref_langs_from_text(arg_value);
		std::cout << "Loaded " << matcher.get_ref_langs().size() << " reference languages." << std::endl;
		if (get_arg(args, "-S", arg_value) || get_arg(args, "--saveref", arg_value))
		{
			std::cout << "Saving analyzed reference languages to " << arg_value << std::endl;
			std::wstring dir_path(arg_value.begin(), arg_value.end());
			if (!std::experimental::filesystem::exists(arg_value)) { std::experimental::filesystem::create_directory(arg_value); }
			for (auto && rl : matcher.get_ref_langs())
			{
				std::wofstream of;
				of.open(dir_path + L"/" + rl.get_lang_name() + L".txt");

				rl.print(of);
			}
		}
	}
	if(has_arg(args, "-m") || has_arg(args, "--match"))
	{
		std::cout << "Matching language..." << std::endl;
		auto match = matcher.match_prof(profile);
		for (auto && i : match)
		{
			std::wcout << i.first << "\t" << (i.first.size() <= 7 ? "\t" : "") << i.second << std::endl;
		}
	}
	if (has_arg(args, "-w") || has_arg(args, "--write"))
	{
		profile.get_ranking().print(std::wcout);
	}
	if (has_arg(args, "-W") || has_arg(args, "--writeall"))
	{
		profile.print(std::wcout);
	}
	if(get_arg(args, "-s", arg_value) || get_arg(args, "--save", arg_value))
	{
		save_ngrams(arg_value, profile, args);
	}

	std::cout << "Finished. Press Enter to continue...";
	std::cin.get();
}