#include "fuzzy_set.hpp"

#include <algorithm>

FuzzySet::~FuzzySet()
{

}

LambdaSet::LambdaSet(int startRange, int endRange, int alpha, int beta, int gamma) : startRange_(startRange), endRange_(endRange), alpha_(alpha), beta_(beta), gamma_(gamma)
{

}

LambdaSet::~LambdaSet()
{

}

float LambdaSet::getValue(int entry,float value)
{
	float result;

    if(entry < alpha_ || entry > gamma_)
        result =  0;
    else if(entry > alpha_ && entry <= beta_)
        result = (entry - alpha_) / (float) (beta_ - alpha_);
    else if (entry > beta_ && entry <= gamma_)
        result =  (gamma_ - entry) / (float) (gamma_- beta_);
    else
        result = 0;

    return std::min(result, value);
}

LSet::LSet(int startRange, int endRange, int alpha, int beta): startRange_(startRange), endRange_(endRange), alpha_(alpha), beta_(beta)
{

}

LSet::~LSet()
{

}

float LSet::getValue(int entry, float value)
{	
	float result;
	if(entry < alpha_)
		result = 1;
	else if(entry>= alpha_ && entry <= beta_)
		result = (beta_ - entry)/(beta_ - alpha_);
	else
		result = 0;
	return std::min(result, value);
}