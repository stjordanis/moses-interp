#ifndef __BLEUSCORER_H__
#define __BLEUSCORER_H__

#include <cctype>
#include <ctime>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Util.h"
#include "Scorer.h"

using namespace std;

enum BleuReferenceLength { AVERAGE, SHORTEST, CLOSEST };


/**
  * Bleu scoring
 **/
class BleuScorer: public Scorer {
	public:
		BleuScorer() : Scorer("BLEU"), _preserveCase(false),_normalise(false),_refLengthStrategy(SHORTEST) {}
		virtual void setReferenceFiles(const vector<string>& referenceFiles);
		virtual void prepareStats(int sid, const string& text, ScoreStats& entry);

		virtual float score(const std::vector<unsigned int>& candidates) {
			if (!_scoreData) {
				throw std::runtime_error("score data not loaded");
			}
			return 0;
		}

		static const int LENGTH;	
		
	private:
		//no copy
		BleuScorer(const BleuScorer&);
		BleuScorer& operator=(const BleuScorer&);
				
		typedef map<string,int> encodings_t;
		typedef map<string,int>::iterator encodings_it;

		//Used to construct the ngram map
		struct CompareNgrams {
			int operator() (const vector<int>& a, const vector<int>& b) {
				/*
				cerr << "compare:";
				copy(a.begin(), a.end(), ostream_iterator<int>(cerr," "));
				cerr << " with ";
				copy(b.begin(), b.end(), ostream_iterator<int>(cerr," "));
				cerr << endl;
				*/
				size_t i;
				size_t as = a.size();
				size_t bs = b.size();
				for (i = 0; i < as && i < bs; ++i) {
					if (a[i] < b[i]) {
						//cerr << "true" << endl;
						return true;
					}
					if (a[i] > b[i]) {
						//cerr << "false" << endl;
						return false;
					}
				}
				//entries are equal, shortest wins
				/*if (as < bs) {
						cerr << "true" << endl;
				} else {
						cerr << "false" << endl;
				}*/
				return as < bs;;
			}
		};

		typedef map<vector<int>,int,CompareNgrams> counts_t;
		typedef map<vector<int>,int,CompareNgrams>::iterator counts_it;

		typedef vector<counts_t*> refcounts_t;

		void encode(const string& line, vector<int>& encoded);
		size_t countNgrams(const string& line, counts_t& counts, unsigned int n);

		void dump_counts(counts_t& counts) {
			for (counts_it i = counts.begin(); i != counts.end(); ++i) {
				cerr << "(";
				copy(i->first.begin(), i->first.end(), ostream_iterator<int>(cerr," "));
				cerr << ") " << i->second << ", ";
			}
			cerr << endl;
		} 
		bool _preserveCase;
		bool _normalise;
		BleuReferenceLength _refLengthStrategy;
		
		// data extracted from reference files
		refcounts_t _refcounts;
		vector<vector<size_t> > _reflengths;
		encodings_t _encodings;
};




#endif //__BLEUSCORER_H