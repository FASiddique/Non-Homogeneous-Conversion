//
// Created by Farzana Ahmed Siddique on 2019-05-27.
//

#ifndef NFAMINIMIZATION_STATE_H
#define NFAMINIMIZATION_STATE_H

#include <iostream>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <bitset>

class State;

typedef std::shared_ptr<State> StatePtr;
typedef std::vector<StatePtr> StateVec;

class State {
public:
    State();
    void setStateID(int32_t val);
    void setGivenID(std::string val);
    void setSymbolSet(std::string symbSet);
    void setIsStartingState(bool val);
    void setIsReportingState(bool val);
    void setReachableStateList(int a, StatePtr state);
    void setRevStateList(int a, StatePtr state);
    int32_t getStateID();
    std::string getGivenID();
    bool startingState();
    bool reportingState();
    StateVec getReachableStateList(int a);
    StateVec getRevStateList(int a);
    StateVec parentState;
    std::bitset<256> bitCol;
private:
    int32_t stateID;
    std::string givenID;
    std::string symbolSet;
    bool isStartingState;
    bool isReportingState;
    std::map <int, StateVec> reachableStateList;
    std::map <int, StateVec> revStateList;
};


#endif //NFAMINIMIZATION_STATE_H
