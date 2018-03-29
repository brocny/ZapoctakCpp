#include "TextAnalyzer.h"
#include <sstream>
#include <algorithm>
#include <locale>

std::vector<std::wstring> make_ngrams_exact_length(const std::wstring & word, int ngram_length)
{
	auto word_local = ngram_length > 1 ? L'^' + word + L'$' : word;
	size_t len = word_local.length();
	std::wstringstream wss(word_local);
	while (len++ < ngram_length) { wss << '_'; }

	auto str = wss.str();

	std::vector<std::wstring> ngrams;
	ngrams.reserve(len - ngram_length);
	for(int i = 0; i < len - ngram_length; i++)
	{
		ngrams.push_back(str.substr(i, ngram_length));
	}

	return ngrams;
}

std::vector<std::wstring> make_ngrams(const std::wstring & word, int ngram_max_length)
{
	std::vector<std::wstring> ngrams;
	if (word.empty()) return ngrams;
	// how many different ngrams will be created of the max length
	size_t ngram_max_count = word.length() + 1;
	// reserve space for the total number of ngrams
	ngrams.reserve(ngram_max_count * (ngram_max_count + 1) / 2);
	for (int i = 1; i <= ngram_max_length; ++i)
	{
		auto ngs_exact = make_ngrams_exact_length(word, i);
		ngrams.insert(ngrams.end(), ngs_exact.begin(), ngs_exact.end());
	}

	return ngrams;
}

LangProfile TextAnalyzer::analyze() const
{
	std::wstring word;
	input_->imbue(locale());
	LangProfile lang_profile;
	if (input_->bad()) 
	{
		return lang_profile;
	}
	*input_ >> word;
	if(word[0] == '#')
	{
		lang_profile.set_lang_name(word.substr(1));
		*input_ >> word;
	}
	while (input_->good())
	{
		transform(word.begin(), word.end(), word.begin(), towlower);
		word.erase(remove_if(word.begin(), word.end(), [](wchar_t x) {return !iswalpha(x) && x != L'\''; }), word.end());
		lang_profile.update_profile(make_ngrams(word, 3));
		*input_ >> word;
	}

	return lang_profile;
}
