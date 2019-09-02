#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <fstream>
using namespace std;

bool pctgSort(const GenomeMatch& a, const GenomeMatch& b)
{
	if (a.percentMatch > b.percentMatch)
		return true;
	else if (a.percentMatch < b.percentMatch)
		return false;

	if (a.genomeName < b.genomeName)
		return true;
	else
		return false;
}

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
	~GenomeMatcherImpl() { delete m_root; }
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;

private:

	struct Match
	{
		Match(int genomeIndex, int pos)
			: m_genome(genomeIndex), m_pos(pos), m_length(0)
		{}

		bool operator ==(const Match& other)
		{
			return m_genome == other.m_genome;
		}

		bool operator <(const Match& other)
		{
			if (m_genome < other.m_genome)
				return true;
			else if (m_genome > other.m_genome)
				return false;

			if (m_length > other.m_length)
				return true;
			else if (m_length < other.m_length)
				return false;

			if (m_pos < other.m_pos)
				return true;
			else if (m_pos > other.m_pos)
				return false;

			return false;
		}

		int m_genome;
		int m_pos;
		int m_length;
	};

	int m_searchLength;
	vector<Genome> m_genomes;
	Trie<Match>* m_root;
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
	: m_searchLength(minSearchLength), m_root(new Trie<Match>)
{}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
	int genomePos = m_genomes.size();
	m_genomes.push_back(genome);

	for (int sequencePos = 0; sequencePos <= genome.length() - m_searchLength; sequencePos++)		//while substring of length m_searchLength can still be extracted, keep adding our
	{																								//extract method already checks for pos + m_searchLength exceeding sequence length
		string sequence;
		genome.extract(sequencePos, m_searchLength, sequence);
		Match temp(genomePos, sequencePos);
		m_root->insert(sequence, temp);
	}
}

int GenomeMatcherImpl::minimumSearchLength() const
{
	return m_searchLength;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
	if (fragment.length() < minimumLength || minimumLength < m_searchLength)
		return false;

	matches.clear();
	string search = fragment.substr(0, m_searchLength);		//format search parameter to be the same length as minSearchLength
	vector<Match> foundSegments = m_root->find(search, exactMatchOnly);

	for (int i = 0; i < foundSegments.size(); i++)
	{
		Match curr = foundSegments[i];
		string full = "";
		m_genomes[curr.m_genome].extract(curr.m_pos, fragment.length(), full);	//extract full minLength from each match location
		
		int matchTil = 0, mismatches = 0;
		for (int i = 0; i < full.size(); i++)				//check how many characters each match actually matches to
		{
			if (fragment[i] == full[i])
				matchTil++;

			else if (!exactMatchOnly)
			{
				if (mismatches >= 1)
					break;

				matchTil++;
				mismatches++;
			}
			else
				break;
		}

		foundSegments[i].m_length = matchTil;						//assign matchLength to each match 
	}

	sort(foundSegments.begin(), foundSegments.end());		//sort matches by name, then matchLength (should result in descending order of matchLengths)
	auto last = unique(foundSegments.begin(), foundSegments.end());	//unique should only keep the first instance of each match's name
	for (auto it = foundSegments.begin(); it != last; it++)
	{
		Match match = *it;

		if (match.m_length < minimumLength)
			continue;

		DNAMatch temp;
		temp.genomeName = m_genomes[match.m_genome].name();
		temp.position = match.m_pos;
		temp.length = match.m_length;
		matches.push_back(temp);
	}

	return matches.size();
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
	if (fragmentMatchLength < m_searchLength)
		return false; 

	results.clear();
	unordered_map<string, int> resultMap;

	int queryLength = query.length();
	int numFrags = queryLength / fragmentMatchLength;

	for (int i = 1; i <= numFrags; i++)
	{
		string sequence;
		int pos = (i - 1) * fragmentMatchLength;
		query.extract(pos, fragmentMatchLength, sequence);

		vector<DNAMatch> found;
		if (findGenomesWithThisDNA(sequence, fragmentMatchLength, exactMatchOnly, found))
		{
			for (int j = 0; j < found.size(); j++)
				resultMap[found[j].genomeName]++;
		}
	}
	
	for (auto it = resultMap.begin(); it != resultMap.end(); it++)
	{
		double pctg = it->second;
		pctg = pctg / numFrags;
		pctg *= 100;

		if (pctg > matchPercentThreshold)
		{
			GenomeMatch tempGen;
			tempGen.genomeName = it->first;
			tempGen.percentMatch = pctg;
			results.push_back(tempGen);
		}
	}

	sort(results.begin(), results.end(), pctgSort);
	return results.size();
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
