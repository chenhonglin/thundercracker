#ifndef GAMESTATEMACHINE_H
#define GAMESTATEMACHINE_H

#include <sifteo.h>
#include "StateMachine.h"
#include "ScoredGameState.h"
#include "ScoredGameState_EndOfRound.h"
#include "CubeStateMachine.h"
using namespace Sifteo;

enum ScoredGameStateIndex
{
    ScoredGameStateIndex_Play,
    ScoredGameStateIndex_EndOfRound,

    ScoredGameStateIndex_NumStates
};

// HACK workaround inability to check if a Cube is actually connected
const unsigned MAX_CUBES = 6;

class GameStateMachine : public StateMachine
{
public:
    GameStateMachine(Cube cubes[]);

    virtual void update(float dt);
    virtual void onEvent(unsigned eventID, const EventData& data);
    static void sOnEvent(unsigned eventID, const EventData& data);
    static unsigned GetNumCubes() { return MAX_CUBES; }// TODO
    static CubeStateMachine* findCSMFromID(Cube::ID cubeID);

protected:
    virtual State& getState(unsigned index);
    virtual unsigned getNumStates() const { return 1; }


private:
    ScoredGameState mScoredState;
    ScoredGameState_EndOfRound mScoredEndOfRoundState;
    CubeStateMachine mCubeStateMachines[MAX_CUBES];
    static GameStateMachine* sInstance;
};

#endif // GAMESTATEMACHINE_H
