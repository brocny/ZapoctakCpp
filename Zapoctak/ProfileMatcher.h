#ifndef PROFILE_MATCHER_H_
#define PROFILE_MATCHER_H_

#include "LangProfile.h"

class ProfileMatcher
{
public:
	ProfileMatcher() = default;
	explicit ProfileMatcher(const std::vector<LangProfile> & ref_lang_profiles)
	{
		for(auto && lang : ref_lang_profiles)
		{
			reference_languages_.push_back(lang.get_ranking());
		}
	}

	using matching_results = std::vector<std::pair<std::wstring, size_t>>;

	matching_results match_prof(const LangProfile & prof);
	matching_results match_prof(const NgramRanking & ranking);
	void load_ref_langs(const std::string & path);
	void load_ref_langs_from_text(const std::string & path);
	const std::vector<NgramRanking> & get_ref_langs() const { return reference_languages_; }


private: 
	std::vector<NgramRanking> reference_languages_;
};

#endif