#include "ProfileMatcher.h"
#include "TextAnalyzer.h"

#include <fstream>
#include <filesystem>

using namespace std;

ProfileMatcher::matching_results ProfileMatcher::match_prof(const LangProfile & prof)
{
	return match_prof(prof.get_ranking());
}

ProfileMatcher::matching_results ProfileMatcher::match_prof(const NgramRanking& ranking)
{
	matching_results mr;
	for (auto && ref_lang : reference_languages_)
	{
		size_t score = 0;
		size_t ref_rank = 0;
		for (auto && ng : ref_lang.get_top_ngrams())
		{
			size_t rank = ranking.get_rank(ng);
			score += ref_rank > rank ? ref_rank - rank : rank - ref_rank;
			ref_rank++;
		}
		mr.emplace_back(ref_lang.get_lang_name(), score);
	}

	std::sort(mr.begin(), mr.end(), [](auto p, auto q) {return cmp_second(q, p); });

	return mr;
}

void ProfileMatcher::load_ref_langs(const std::string & path)
{
	for(auto && file : experimental::filesystem::directory_iterator(path))
	{
		wifstream f;
		f.open(file);

		reference_languages_.push_back(NgramRanking::read(f));
	}
}

void ProfileMatcher::load_ref_langs_from_text(const std::string & path)
{
	for(auto && file : experimental::filesystem::directory_iterator(path))
	{
		if(!is_regular_file(file)) continue;

		wifstream f;
		f.open(file);

		TextAnalyzer analyzer(f);
		LangProfile profile = analyzer.analyze();

		if (profile.get_lang_name().empty())
		{
			profile.set_lang_name(L"[" + file.path().generic_wstring() + L"]");
		}

		reference_languages_.push_back(profile.get_ranking());
	}
}
