#include "LangProfile.h"
#include <sstream>
#include <algorithm>
#include "TextAnalyzer.h"

using namespace std;

NgramRanking LangProfile::get_ranking(int count) const
{
	return NgramRanking(ngram_count_, count, lang_name_);
}

void LangProfile::update_profile(const ngram_count & count, bool overwrite = false)
{
	if(overwrite)
	{
		for (auto && pair : count)
		{
			ngram_count_[pair.first] = pair.second;
		}
	}
	else
	{
		for (auto && pair : count)
		{
			ngram_count_[pair.first] += pair.second;
		}
	}
}

void LangProfile::update_profile(const LangProfile & profile, bool overwrite = false)
{
	update_profile(profile.ngram_count_, overwrite);
}

void LangProfile::update_profile(const std::vector<std::wstring>& ngrams)
{
	for(auto && ng : ngrams)
	{
		ngram_count_[ng]++;
	}
}

void LangProfile::print(std::wostream& s) const
{
	s.imbue(TextAnalyzer::locale());
	for(auto && i : ngram_count_)
	{
		s << i.first << '\t' << i.second << std::endl;
	}
}

NgramRanking::NgramRanking(const ngram_count & bag, size_t count, std::wstring name) : lang_name_(std::move(name))
{
	vector<pair<wstring, int>> v;
	v.reserve(bag.size());
	for(auto && i : bag)
	{
		v.push_back(i);
	}
	
	std::sort(v.begin(), v.end(), cmp_second<wstring, int>);
	top_ngrams_.reserve(count);

	for(size_t i = 0; i < count && i < v.size(); ++i)
	{
		top_ngrams_.push_back(v[i].first);
	}
}

NgramRanking NgramRanking::read(std::wistream& s)
{
	NgramRanking ngram_ranking;
	s.imbue(TextAnalyzer::locale());
	wstring ngram;
	s >> ngram;
	if(ngram[0] == L'#')
	{
		ngram_ranking.lang_name_ = ngram.substr(1);
		s >> ngram;
	}

	while(s.good())
	{
		ngram_ranking.top_ngrams_.push_back(ngram);
		s >> ngram;
	}

	return ngram_ranking;
}

void NgramRanking::print(std::wostream& s) const
{
	s.imbue(TextAnalyzer::locale());
	if (!lang_name_.empty())
	{
		s << '#' << lang_name_ << endl;
	}
	for(auto && i : top_ngrams_)
	{
		s << i << endl;
	}
}
