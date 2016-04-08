#ifndef FUZZY_SET_HPP
#define FUZZY_SET_HPP

class FuzzySet
{
public:
    virtual ~FuzzySet() = 0;
    virtual float getValue(int entry) = 0;
};

class LambdaSet : public FuzzySet
{
public:
    LambdaSet(int startRange, int endRange, int alpha, int beta, int gamma);
    virtual ~LambdaSet();
    virtual float getValue(int entry);

private:
    int startRange_, endRange_;
    int alpha_, beta_, gamma_;
};

#endif