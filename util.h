//
// Created by Farzana Ahmed Siddique on 2019-05-27.
//

#include "pugixml.hpp"
#include "State.h"
#include "SymbolParser.h"
#include <iostream>
#include <queue>

using namespace std;

#ifndef NFAMINIMIZATION_UTIL_H
#define NFAMINIMIZATION_UTIL_H

class Util {
public:

    static void parse(map <string, StatePtr> &stateIDPairList) {
        pugi::xml_document doc;

        if (!doc.load_file("/Users/farzana/CLionProjects/NFAMinimization/leven_wahoo_d1.anml", pugi::parse_default | pugi::parse_declaration)) {
            cout << "Could not load .xml file: " << endl;
            return;
        }

        pugi::xml_node nodes = doc.child("anml").child("automata-network");

        int32_t stateID = 1;
        for (pugi::xml_node node = nodes.first_child(); node;
             node = node.next_sibling()) {

            string nodeName;
            nodeName = node.name();

            if (nodeName == "state-transition-element") {
                string id;
                string symbol_set;
                string start;
                bool eod = false;

                // gather attributes
                for (pugi::xml_attribute attr = node.first_attribute(); attr; attr = attr.next_attribute()) {

                    string str = attr.name();

                    if (str == "id") {
                        id = attr.value();
                    } else if (str == "symbol-set") {
                        symbol_set = attr.value();
                    } else if (str == "start") {
                        start = attr.value();
                    }
                }

                StatePtr newState;

                if (stateIDPairList.find(id) != stateIDPairList.end()) newState = stateIDPairList.at(id);
                else {
                    newState = StatePtr (new State());
                    newState->setStateID(stateID);
                    stateID++;
                }

                newState->setGivenID(id);
                newState->setSymbolSet(symbol_set);
                parseSymbolSet(newState->bitCol, symbol_set);
                stateIDPairList.insert(pair<string, StatePtr>(newState->getGivenID(), newState));

                if (start == "all-input") {
                    newState->setIsStartingState(false);
                    StatePtr startState = StatePtr(new State());
                    startState->setGivenID("id" + id);
                    startState->setIsStartingState(true);
                    newState->parentState.push_back(startState);
                    startState->setStateID(stateID);
                    stateID++;
                    stateIDPairList.insert(pair<string, StatePtr>(startState->getGivenID(), startState));
                } else {
                    newState->setIsStartingState(false);
                }

                newState->setIsReportingState(false);

                for (pugi::xml_node aom : node.children("activate-on-match")) {

                    StatePtr nextState;
                    if (aom.attribute("element").value() == id) nextState = newState;
                    else if (stateIDPairList.find(aom.attribute("element").value()) != stateIDPairList.end()) nextState = stateIDPairList.at(aom.attribute("element").value());
                    else {
                        nextState = StatePtr (new State());
                        nextState->setStateID(stateID);
                        stateID++;
                    }

                    nextState->setGivenID(aom.attribute("element").value());
                    nextState->parentState.push_back(newState);
                    stateIDPairList.insert(pair<string, StatePtr>(nextState->getGivenID(), nextState));
                }

                for (pugi::xml_node aom : node.children("report-on-match")) {
                    newState->setIsReportingState(true);
//                    StatePtr reportState = StatePtr(new State());
//                    reportState->setGivenID("id" + id);
//                    reportState->setIsReportingState(true);
//                    reportState->setIsStartingState(false);
//                    reportState->setSymbolSet(symbol_set);
//                    reportState->setStateID(stateID);
//                    reportState->parentState.push_back(newState);
//                    stateID++;
//                    stateIDPairList.insert(pair<string, StatePtr>(reportState->getGivenID(), reportState));
//                    for (int a = 0; a < 256; ++a) {
//                        if (newState->bitCol.test(a)) {
//                            newState->setReachableStateList(a, reportState);
//                        }
//                    }
                }
            }
        }

        for (const auto& elem : stateIDPairList) {
            if (!elem.second->parentState.empty()) {
                for (int a = 0; a < 256; ++a) {
                    if (elem.second->bitCol.test(a)) {
                        for (const StatePtr& parent : elem.second->parentState) {
                            parent->setReachableStateList(a, elem.second);
                            elem.second->setRevStateList(a, parent);
                        }
                    }
                }
            }
        }
    }
};


#endif //NFAMINIMIZATION_UTIL_H
