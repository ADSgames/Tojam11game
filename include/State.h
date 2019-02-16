/**
 * State for machine and State Engine
 * Allan Legemaate
 * 30/12/2016
 * Compartmentalize program into states
 *   which can handle only their own logic,
 *   drawing and transitions
 */

#ifndef STATE_H
#define STATE_H

#include <allegro.h>

#include <vector>

// Class
class State;

/*****************
 * STATE ENGINE
 *****************/
class StateEngine {
  public:
    // Update
    void update();

    // Draw
    void draw(BITMAP* buffer);

    // Set next state
    void setNextState(int newState, bool deletePrev);

    // Get state id
    int getStateId();

    // Game states
    enum programStates {
      STATE_NULL,
      STATE_LAST,
      STATE_EXIT,
      STATE_MENU,
      STATE_GAME,
      STATE_GAME_MENU,
      STATE_OPTIONS
    };

    // Game states
    enum stateStatus {
      STATUS_DELETE,
      STATUS_DRAW_ONLY,
      STATUS_UPDATE_ONLY,
      STATUS_FULL
    };
  private:
    // Change state
    void changeState();

    // Stores states
    std::vector<State*> states;

    // Next state
    int nextState = STATE_NULL;

    // State id
    int currentState = STATE_NULL;

    // Suspend previous
    bool delete_previous;

};

/*********
 * STATE
 *********/
class State {
  public:
    // Virtual destructor
    virtual ~State() {};

    // Draw to screen
    virtual void draw(BITMAP *buffer) = 0;

    // Update logic
    virtual void update(StateEngine* engine) = 0;

    // Change state
    void setNextState(StateEngine* engine, int state, bool delete_state = true);

    // Get status
    int getStatus();

  private:
    // State status
    int status;
};

#endif // STATE_H
