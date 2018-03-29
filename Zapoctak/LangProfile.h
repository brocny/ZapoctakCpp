#ifndef LANG_PROF_H_
#define LANG_PROF_H_

#include <string>
#include <vector>
#include <unordered_map>

using ngram_count = std::unordered_map<std::wstring, size_t>;

class NgramRanking
{
public:
	NgramRanking() = default;
	NgramRanking(const ngram_count & bag, size_t count, std::wstring name = L"");
	static NgramRanking read(std::wistream & s);
	void print(std::wostream & s) const;

	size_t get_rank(const std::wstring & ngram) const
	{
		for(size_t i = 0; i < top_ngrams_.size(); ++i)
		{
			if (top_ngrams_[i] == ngram) return i;
		}

		return int(1.2 * top_ngrams_.size());
	}

	const std::wstring & get_lang_name() const { return lang_name_; }
	const std::vector<std::wstring> & get_top_ngrams() const { return top_ngrams_; }
private:
	std::vector<std::wstring> top_ngrams_;
	std::wstring lang_name_;
};

template <typename S, typename T>
static bool cmp_second(const std::pair<S, T> & lhs, const std::pair<S, T> & rhs)
{
	if (lhs.second > rhs.second) return true;
	if (lhs.second < rhs.second) return false;
	return lhs.first < rhs.first;
}

class LangProfile
{
public:
	LangProfile() = default;

	NgramRanking get_ranking(int count = 300) const;

	void update_profile(const ngram_count & count, bool overwrite);
	void update_profile(const LangProfile & profile, bool overwrite);
	void update_profile(const std::vector<std::wstring> & ngrams);
	void set_lang_name(const std::wstring & name) { lang_name_ = name; }
	const ngram_count & get_ngram_count() const { return ngram_count_; }
	std::wstring get_lang_name() const { return lang_name_; }
	void print(std::wostream & s) const;
private:
	ngram_count ngram_count_;
	std::wstring lang_name_;
};
#endif