//
// Created by Farzana Ahmed Siddique on 2019-05-27.
//

#include "State.h"

void State::setStateID(int32_t val) {
    stateID = val;
}

State::State() {
    for (int i = 0; i < 256; ++i) {
        bitCol.set(i, 0);
    }
}

void State::setGivenID(std::string val) {
    givenID = val;
}

void State::setIsStartingState(bool val) {
    isStartingState = val;
}

void State::setIsReportingState(bool val) {
    isReportingState = val;
}

int32_t State::getStateID() {
    return stateID;
}

bool State::startingState() {
    return isStartingState;
}

bool State::reportingState() {
    return isReportingState;
}

void State::setSymbolSet(std::string symbSet) {
    symbolSet = symbSet;
}

void State::setReachableStateList(int a, StatePtr state) {
    StateVec v;
    if (reachableStateList.find(a) != reachableStateList.end()) {
        reachableStateList.at(a).push_back(state);
        return;

    }
    v.push_back(state);
    reachableStateList.insert(std::pair<int, StateVec>(a, v));
}

std::string State::getGivenID() {
    return givenID;
}

StateVec State::getReachableStateList(int a) {
    StateVec v;
    if (reachableStateList.find(a) == reachableStateList.end()) return v;
    return reachableStateList.at(a);
}

StateVec State::getRevStateList(int a) {
    StateVec v;
    if (revStateList.end() == revStateList.find(a)) return v;
    return revStateList.at(a);
}

void State::setRevStateList(int a, StatePtr state) {
    StateVec v;
    if (revStateList.find(a) != revStateList.end()) {
        revStateList.at(a).push_back(state);
        return;

    }
    v.push_back(state);
    revStateList.insert(std::pair<int, StateVec>(a, v));
}