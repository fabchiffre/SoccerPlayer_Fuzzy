#ifndef FUZZY_SET_HPP
#define FUZZY_SET_HPP

class FuzzySet
{
public:
    FuzzySet(int startRange, int endRange);
    virtual ~FuzzySet() = 0;
    virtual float getValue(int entry, float value = 1) = 0;

protected:
    int startRange_, endRange_;
};

class LambdaSet : public FuzzySet
{
public:
    LambdaSet(int startRange, int endRange, int alpha, int beta, int gamma);
    virtual ~LambdaSet();
    virtual float getValue(int entry, float value = 1);

private:
    int alpha_, beta_, gamma_;
};

class LSet : public FuzzySet
{
public:
    LSet(int startRange, int endRange, int alpha, int beta);
    virtual ~LSet();
    virtual float getValue(int entry, float value = 1);

private:
    int alpha_, beta_;
};

class GammaSet : public FuzzySet
{
public:
    GammaSet(int startRange, int endRange, int alpha, int beta);
    virtual ~GammaSet();
    virtual float getValue(int entry, float value = 1);

private:
    int alpha_, beta_;
};

class TrapezeSet : public FuzzySet
{
public:
    TrapezeSet(int startRange, int endRange, int alpha, int beta, int gamma, int delta);
    virtual ~TrapezeSet();
    virtual float getValue(int entry, float value = 1);

private:
    int alpha_, beta_, gamma_, delta_;
};





#endif