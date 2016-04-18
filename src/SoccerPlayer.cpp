//------------------------------------------------------------------------------
// Inclusão da biblioteca que implementa a interface com o SoccerMatch.
#include "environm.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include "fuzzy_set.hpp"

#define LEFT 0
#define FRONT 1
#define RIGHT 2
#define BACK 3


#define START_ANGLE -180
#define END_ANGLE 180
#define STEP_ANGLE 10

#define START_DIST 0
#define END_DIST 800


#define COEF_ACTION 0.5

using namespace std;

const int nb_step = (-START_ANGLE + END_ANGLE) / STEP_ANGLE;

FuzzySet *ballSet[4], *goalSet[3], *exitSet[3];

/* Matrix mapping the rules
    - First index corresponds to the ball
    - Second index corresponds to the goal
    - The resulted index indicates which exitSet to used.
    */
int rules_1[4][3];

void initFuzzySet()
{

    ballSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 90, 180);
    ballSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    ballSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -90, 0);
    ballSet[BACK] = new USet(START_ANGLE, END_ANGLE, -180, -90, 90, 180);

    goalSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 180, 180);
    goalSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    goalSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -180, 0);

    exitSet[LEFT] = new LambdaSet(START_ANGLE, END_ANGLE, 0, 180, 180);
    exitSet[FRONT] = new LambdaSet(START_ANGLE, END_ANGLE, -90, 0, 90);
    exitSet[RIGHT] = new LambdaSet(START_ANGLE, END_ANGLE, -180, -180, 0);
}

void initRules()
{
    //First index : ball, second : goal
    rules_1[LEFT][LEFT] = LEFT;
    rules_1[LEFT][FRONT] = LEFT;
    rules_1[LEFT][RIGHT] = LEFT;

    rules_1[FRONT][LEFT] = RIGHT;
    rules_1[FRONT][FRONT] = FRONT;
    rules_1[FRONT][RIGHT] = LEFT;

    rules_1[RIGHT][LEFT] = RIGHT;
    rules_1[RIGHT][FRONT] = RIGHT;
    rules_1[RIGHT][RIGHT] =  RIGHT;

    rules_1[BACK][LEFT] = RIGHT;
    rules_1[BACK][FRONT] = RIGHT;
    rules_1[BACK][RIGHT] =  LEFT;
}

void computePertinence(int rules[4][3], float pertinenceRules[], int val1, int val2)
{
    float valA, valB;
    for(int i=0; i<4; ++i) {
        for(int j=0; j<3; ++j) {
            valA = ballSet[i]->getValue(val1);
            valB = goalSet[j]->getValue(val2);

            pertinenceRules[rules[i][j]] = max(pertinenceRules[rules[i][j]], min(valA, valB));
        }
    }
}

float defuzzification(double ballAngle, double goalAngle, double distBall)
{
    float pertinenceRules[3];

    float coefExit;
    float sumCoef = 0;
    float sumValueCoef = 0;

    for(int i=0; i<3; ++i)
        pertinenceRules[i] = 0;

    computePertinence(rules_1, pertinenceRules, ballAngle, goalAngle);
   

    // Compute the exit fuzzy set and at the same time the exit value.
    for(int i=0; i<=nb_step; ++i) {
        int value = START_ANGLE + i*STEP_ANGLE;

        coefExit = 0;

        for(int j=0; j<3; ++j) {
            coefExit = max(coefExit, exitSet[j]->getValue(value, pertinenceRules[j]));
        }

        sumValueCoef += coefExit*value;
        sumCoef += coefExit;
    }
    return (sumValueCoef / sumCoef);
}

int main( int argc, char* argv[] )
{

    float   ballAngle, targetAngle, distBall, leftMotor, rightMotor;
    float   result;

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

        result = defuzzification(ballAngle * 180 / M_PI, targetAngle * 180 / M_PI, distBall);



        result = result * M_PI / 180;

        // TODO : Define left and right motor forces...
        leftMotor  = (cos( result ) - sin( result )) * COEF_ACTION ;
        rightMotor = (cos( result ) + sin( result )) * COEF_ACTION;


        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laco).

        if ( ! environment.act( leftMotor , rightMotor ) ) {
            break; // Termina a execução se falha ao agir.
        }
    }

    for(int i=0; i<3; ++i) {
        delete exitSet[i];
        delete ballSet[i];
        delete goalSet[i];
    }

    return 0;
}
//------------------------------------------------------------------------------

