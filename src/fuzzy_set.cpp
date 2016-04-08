#include "fuzzy_set.hpp"

FuzzySet::~FuzzySet()
{

}

LambdaSet::LambdaSet(int startRange, int endRange, int alpha, int beta, int gamma) : startRange_(startRange), endRange_(endRange), alpha_(alpha), beta_(beta), gamma_(gamma)
{

}

LambdaSet::~LambdaSet()
{

}

float LambdaSet::getValue(int entry)
{
    if(entry < alpha_ || entry > gamma_)
        return 0;
    else if(entry > alpha_ && entry <= beta_)
        return (entry - alpha_) / (float) (beta_ - alpha_);
    else if (entry > beta_ && entry <= gamma_)
        return (gamma_ - entry) / (float) (gamma_- beta_);
    else
        return 0;
}