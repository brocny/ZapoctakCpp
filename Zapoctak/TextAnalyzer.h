#ifndef TEXT_ANALYZER_H_
#define TEXT_ANALYZER_H_

#include "LangProfile.h"
#include <iostream>
#include <locale>
#include <codecvt>

class TextAnalyzer
{
public:
	TextAnalyzer(std::wistream & stream = std::wcin) : input_(&stream) {}
	void set_input(std::wistream & stream) { input_ = &stream; }
	LangProfile analyze() const;
	static std::locale & locale() 
	{
		static std::locale locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>);
		return locale;
	}
private:
	std::wistream * input_;
};

#endif