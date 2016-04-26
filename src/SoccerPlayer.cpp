//------------------------------------------------------------------------------
// Inclusão da biblioteca que implementa a interface com o SoccerMatch.
#include "environm.h"

#include <iostream>

#include <vector>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "fuzzy_set.hpp"

#define LEFT 0
#define FRONT 1
#define RIGHT 2
#define BACK 3

#define CLOSE 0
#define AVG_DIST 1
#define FAR 2

#define SLOW 0
#define AVG_SPEED 1
#define FAST 2


#define START_ANGLE -180
#define END_ANGLE 180
#define STEP_ANGLE 10

#define START_DIST 0
#define END_DIST 1600

#define START_SPEED 0
#define END_SPEED 100
#define STEP_SPEED 10

#define COEF_ACTION 0.7

using namespace std;

typedef std::vector< std::vector<int> > vvi;
typedef std::vector<FuzzySet*> conjFuzzy;

const int nbStepAng = (-START_ANGLE + END_ANGLE) / STEP_ANGLE;
const int nbStepSpeed = (START_SPEED + END_SPEED) / STEP_SPEED;

conjFuzzy ballSet, ballDistSet, goalDistSet, goalSet, directionSet, speedSet;


/* Matrix mapping the rules
    - First index corresponds to the ball
    - Second index corresponds to the goal
    - The resulted index indicates which exitSet to used.
    */
vvi rulesDirection, rulesSpeed;

void initFuzzySet()
{
    ballSet.resize(4);
    ballSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 90, 180);
    ballSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    ballSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -90, 0);
    ballSet[BACK] = new USet(START_ANGLE, END_ANGLE, -180, -90, 90, 180);

    goalSet.resize(4);
    goalSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 90, 180);
    goalSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    goalSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -90, 0);
    goalSet[BACK] = new USet(START_ANGLE, END_ANGLE, -180, -90, 90, 180);

    ballDistSet.resize(3);
    ballDistSet[CLOSE] = new LambdaSet(START_DIST, END_DIST,  0, 0, 300);
    ballDistSet[AVG_DIST] = new LambdaSet(START_DIST, END_DIST, 0, 300, 500);
    ballDistSet[FAR] = new LambdaSet(START_DIST, END_DIST, 300, END_DIST, END_DIST);

    goalDistSet.resize(3);
    goalDistSet[CLOSE] = new LambdaSet(START_DIST, END_DIST,  0, 0, 300);
    goalDistSet[AVG_DIST] =new LambdaSet(START_DIST, END_DIST, 150, 300, 450);
    goalDistSet[FAR] =  new LambdaSet(START_DIST, END_DIST, 300, END_DIST, END_DIST);

    speedSet.resize(3);
    speedSet[SLOW] = new LambdaSet(START_SPEED, END_SPEED,0, 0, 50);
    speedSet[AVG_SPEED] = new LambdaSet(START_SPEED, END_SPEED, 25, 50 ,75);
    speedSet[FAST] = new LambdaSet(START_SPEED, END_SPEED, 50, 100, 100);

    directionSet.resize(3);
    directionSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 180, 180);
    directionSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    directionSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -180, 0);
}

void initRules()
{
    rulesDirection.resize(4);
    for(int i=0; i<4; ++i)
        rulesDirection[i].resize(4);

    //First index : ball, second : goal
    rulesDirection[LEFT][LEFT] = LEFT;
    rulesDirection[LEFT][FRONT] = LEFT;
    rulesDirection[LEFT][RIGHT] = LEFT;
    rulesDirection[LEFT][BACK] = LEFT;

    rulesDirection[FRONT][LEFT] = RIGHT;
    rulesDirection[FRONT][FRONT] = FRONT;
    rulesDirection[FRONT][RIGHT] = LEFT;
    rulesDirection[FRONT][BACK] = LEFT;

    rulesDirection[RIGHT][LEFT] = RIGHT;
    rulesDirection[RIGHT][FRONT] = RIGHT;
    rulesDirection[RIGHT][RIGHT] =  RIGHT;
    rulesDirection[RIGHT][BACK] = RIGHT;

    rulesDirection[BACK][LEFT] = RIGHT;
    rulesDirection[BACK][FRONT] = RIGHT;
    rulesDirection[BACK][RIGHT] =  LEFT;
    rulesDirection[BACK][BACK] = RIGHT;

    rulesSpeed.resize(3);
    for(int i=0; i<3; ++i)
        rulesSpeed[i].resize(3);

    rulesSpeed[CLOSE][CLOSE] = SLOW;
    rulesSpeed[CLOSE][AVG_DIST] = SLOW;
    rulesSpeed[CLOSE][FAR] = SLOW;

    rulesSpeed[AVG_DIST][CLOSE] = SLOW;
    rulesSpeed[AVG_DIST][AVG_DIST] = AVG_SPEED;
    rulesSpeed[AVG_DIST][FAR] = AVG_SPEED;

    rulesSpeed[FAR][CLOSE] = FAST;
    rulesSpeed[FAR][AVG_DIST] = AVG_SPEED;
    rulesSpeed[FAR][FAR] = FAST;
}

void computePertinence(std::vector< std::vector<int> > rules, float pertinenceRules[], conjFuzzy conj1, int val1, conjFuzzy conj2, int val2)
{
    float valA, valB;
    for(int i=0; i<rules.size(); ++i) {
        for(int j=0; j<rules[0].size(); ++j) {
            valA = conj1[i]->getValue(val1);
            valB = conj2[j]->getValue(val2);

            pertinenceRules[rules[i][j]] = max(pertinenceRules[rules[i][j]], min(valA, valB));
        }
    }
}

float computeResult(conjFuzzy exitSet, float pertinenceRules[], float init, float incr, int nb_step)
{
    float sumCoef = 0;
    float sumValueCoef = 0;
    float coef;

    // Compute the exit fuzzy set and at the same time the exit value.
    for(int i=0; i<=nb_step; ++i) {
        float value = init + i*incr;
        coef = 0;
        for(int j=0; j<exitSet.size(); ++j)
                    coef = max(coef, exitSet[j]->getValue(value, pertinenceRules[j]));

        sumCoef += coef;
        sumValueCoef += coef*value;
    }

    return (sumValueCoef / sumCoef); 
}

void defuzzification(double ballAngle, double goalAngle, double distBall, int distGoal, float &angExit, float &speedExit)
{
    float pertinenceRulesAng[3];
    float pertinenceRulesSpeed[3];

    for(int i=0; i<3; ++i){
            pertinenceRulesAng[i] = 0;
            pertinenceRulesSpeed[i] = 0;    
    }

    computePertinence(rulesDirection, pertinenceRulesAng, ballSet, ballAngle, goalSet, goalAngle);
    computePertinence(rulesSpeed, pertinenceRulesSpeed, ballDistSet, distBall, goalDistSet, distGoal);

    angExit = computeResult(directionSet, pertinenceRulesAng, START_ANGLE, STEP_ANGLE, nbStepAng);
    speedExit = computeResult(speedSet, pertinenceRulesSpeed, START_SPEED, STEP_SPEED, nbStepSpeed);  
}

int main( int argc, char* argv[] )
{

    float   ballAngle, targetAngle, distBall, distGoal, leftMotor, leftMotorPrev, rightMotor, rightMotorPrev;
    float   resultAng, resultSpeed;


    leftMotorPrev = 0;
    rightMotorPrev = 0;

    // Declaração do objeto que representa o ambiente.
    environm::soccer::clientEnvironm environment;

    if ( argc != 3 ) {
        printf( "\nInvalid parameters. Expecting:" );
        printf( "\nSoccerPlayer SERVER_ADDRESS_STRING SERVER_PORT_NUMBER\n" );
        printf( "\nSoccerPlayer localhost 1024\n" );
        return 0;
    }

    // Conecta-se ao SoccerMatch. Supõe que SoccerMatch está rodando na máquina
    // local e que um dos robôs esteja na porta 1024. Porta e local podem mudar.
    if ( ! environment.connect( argv[1], atoi( argv[2] ) ) ) {
        printf( "\nFail connecting to the SoccerMatch.\n" );
        return 0;  // Cancela operação se não conseguiu conectar-se.
    }

    initFuzzySet();

    // Laço de execução de ações.
    printf( "\nRunning..." );

    initFuzzySet();
    initRules();

    while ( 1 ) {

        // Deve obter os dados desejados do ambiente. Métodos do clientEnvironm.
        // Exemplos de métodos que podem ser utilizados.
        ballAngle = environment.getBallAngle();
        targetAngle = environment.getTargetAngle( environment.getOwnGoal() );
        distBall = environment.getDistance();
        distGoal = environment.getGoalDist();

        defuzzification(ballAngle * 180 / M_PI, targetAngle * 180 / M_PI, distBall, distGoal, resultAng, resultSpeed);

        resultAng = resultAng * M_PI / 180;

        cout << resultSpeed << endl;

        leftMotor  = (cos( resultAng ) - sin( resultAng )) * (resultSpeed/100);
        rightMotor = (cos( resultAng ) + sin( resultAng )) * (resultSpeed/100);


        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laco).

        if ( ! environment.act( leftMotor , rightMotor ) ) {
            break; // Termina a execução se falha ao agir.
        }

        leftMotorPrev = leftMotor;
        rightMotorPrev = rightMotor;
    }

    return 0;
}
//------------------------------------------------------------------------------

