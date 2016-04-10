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

#define START -180
#define END 180
#define STEP 15

using namespace std;

const int nb_step = (-START + END) / 15;

FuzzySet *ballSet[3], *goalSet[3], *exitSet[3];

/* Matrix mapping the rules
    - First index corresponds to the ball
    - Second index corresponds to the goal
    - The resulted index indicates which exitSet to used.
    */
int rules[3][3];

void initFuzzySet()
{
    ballSet[LEFT] = new LambdaSet(START, END, 0, 180, 180);
    ballSet[FRONT] = new LambdaSet(START, END, -90, 0, 90);
    ballSet[RIGHT] = new LambdaSet(START, END, -180, -180, 0);

    goalSet[LEFT] = new LambdaSet(START, END, 0, 180, 180);
    goalSet[FRONT] = new LambdaSet(START, END, -90, 0, 90);
    goalSet[RIGHT] = new LambdaSet(START, END, -180, -180, 0);

    exitSet[LEFT] = new LambdaSet(START, END, 0, 180, 180);
    exitSet[FRONT] = new LambdaSet(START, END, -180, 0, 180);
    exitSet[RIGHT] = new LambdaSet(START, END, -180, -180, 0);
}

void initRules()
{
    //First index : ball, second : goal
    rules[LEFT][LEFT] = FRONT;
    rules[LEFT][FRONT] = LEFT;
    rules[LEFT][RIGHT] = LEFT;

    rules[FRONT][LEFT] = RIGHT;
    rules[FRONT][FRONT] = FRONT;
    rules[FRONT][RIGHT] = LEFT;

    rules[RIGHT][LEFT] = RIGHT;
    rules[RIGHT][FRONT] = RIGHT;
    rules[RIGHT][RIGHT] =  FRONT;
}

float defuzzification(double ballAngle, double goalAngle)
{
    float valA, valB;
    float pertinenceRules[3];

    float coefExit;
    float cutCoef;
    float sumCoef = 0;
    float sumValueCoef = 0;

    for(int i=0; i<3; ++i)
        pertinenceRules[i] = 0;

    // Compute the degree of pertinence for each rules
    for(int i=0; i<3; ++i) {
        for(int j=0; j<3; ++j) {
            valA = ballSet[i]->getValue(ballAngle);
            valB = goalSet[j]->getValue(goalAngle);

            pertinenceRules[rules[i][j]] = max(pertinenceRules[rules[i][j]], min(valA, valB));
        }
    }

    // Compute the exit fuzzy set and at the same time the exit value.
    for(int i=0; i<=nb_step; ++i) {
        int value = START + i*STEP;

        coefExit = 0;

        for(int j=0; j<3; ++j) {
            cutCoef  = min(exitSet[j]->getValue(value) , pertinenceRules[j]);
            coefExit = max(coefExit, cutCoef);
        }
        sumValueCoef += coefExit*value;
        sumCoef += coefExit;
    }
    return (sumValueCoef / sumCoef);
}

int main( int argc, char* argv[] )
{

    float   ballAngle, targetAngle, leftMotor, rightMotor;
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

        result = defuzzification(ballAngle * 180 / M_PI, targetAngle * 180 / M_PI);
        result = result * M_PI / 180;

        // TODO : Define left and right motor forces...
        leftMotor  = cos( result ) - sin( result );
        rightMotor = cos( result ) + sin( result );

        cout << "Result : " <<  result * 180 / M_PI << endl;

        // Transmite ação do robô ao ambiente. Fica bloqueado até que todos os
        // robôs joguem. Se erro, retorna false (neste exemplo, sai do laco).
        if ( ! environment.act( leftMotor, rightMotor ) ) {
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

