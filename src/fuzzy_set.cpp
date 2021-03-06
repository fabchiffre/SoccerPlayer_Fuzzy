#include "fuzzy_set.hpp"

#include <algorithm>

FuzzySet::FuzzySet(int startRange, int endRange) : 
    startRange_(startRange), endRange_(endRange)
{

}

FuzzySet::~FuzzySet()
{

}

LambdaSet::LambdaSet(int startRange, int endRange, 
	int alpha, int beta, int gamma) : FuzzySet(startRange, endRange), alpha_(alpha), beta_(beta), gamma_(gamma)
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

LSet::LSet(int startRange, int endRange, 
	int alpha, int beta): FuzzySet(startRange, endRange), alpha_(alpha), beta_(beta)
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


GammaSet::GammaSet(int startRange, int endRange, 
	int alpha, int beta): FuzzySet(startRange, endRange), alpha_(alpha), beta_(beta)
{

}

GammaSet::~GammaSet()
{

}

float GammaSet::getValue(int entry, float value)
{
    float result;
    if(entry < alpha_)
        result = 0;
    else if(entry>= alpha_ && entry <= beta_)
        result = (entry - alpha_)/(beta_ - alpha_);
    else
        result = 1;
    return std::min(result, value);
}

TrapezeSet::TrapezeSet(int startRange, int endRange, 
	int alpha, int beta, int gamma, int delta): FuzzySet(startRange, endRange), alpha_(alpha), beta_(beta), gamma_(gamma), delta_(delta)
{

}

TrapezeSet::~TrapezeSet()
{

}

float TrapezeSet::getValue(int entry, float value)
{
    float result;
    if(entry < alpha_)
        result = 0;
    else if(entry>= alpha_ && entry <= beta_)
        result = (entry - alpha_)/(beta_ - alpha_);
    else if(entry > beta_ && entry < gamma_)
        result = 1;
    else if(entry >= gamma_ && entry < delta_)
        result = (entry - gamma_) / (gamma_ - delta_);
    else
        result = 0;
    return std::min(result, value);
}

USet::USet(int startRange, int endRange, int alpha, int beta, int gamma, int delta):
    FuzzySet(startRange, endRange), alpha_(alpha), beta_(beta), gamma_(gamma), delta_(delta)
{

}

USet::~USet()
{

}

float USet::getValue(int entry, float value)
{
    float result;
    if(entry < alpha_)
        result = 1;
    else if(entry >= alpha_ && entry <= beta_)
        result = (entry - alpha_)/(beta_ - alpha_);
    else if(entry > beta_ && entry < gamma_)
        result = 0;
    else if(entry >= gamma_ && entry < delta_)
        result = (entry - gamma_) / (gamma_ - delta_);
    else
        result = 1;
    return std::min(result, value);
}