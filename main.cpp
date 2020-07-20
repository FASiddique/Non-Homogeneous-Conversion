#include "util.h"

typedef std::vector < vector <int> > VecBox;

std::map <string, StatePtr> stateIDPairList;
std::map <pair<int, StatePtr>, StateVec> reverseTransition;
std::map <pair<int, StatePtr>, int> card;
std::vector <pair<StatePtr, StatePtr>> w;
std::queue <pair<StatePtr, StatePtr>> q;
std::map <int, VecBox> N;
StateVec F;
StateVec remStates;

void addRevTran(const StatePtr& state, int symb) {
    for (StatePtr s : state->getReachableStateList(symb)) {
        auto key = pair<int, StatePtr> (symb, s);
        StateVec v;
        if (reverseTransition.find(key) != reverseTransition.end()) {
            reverseTransition.at(key).push_back(state);
            continue;
        }
        v.push_back(state);
        reverseTransition.insert(make_pair(key, v));
    }
}

void preprocess() {
    for (const auto& elem : stateIDPairList) {
        StatePtr state = elem.second;
        for (int i = 0; i < 256; ++i) {
            auto key = pair<int, StatePtr> (i, state);
            if (state->bitCol.test(i)) {
                addRevTran(state, i);
                card.insert(make_pair(key, state->getReachableStateList(i).size()));
            } else {
                card.insert(make_pair(key, 0));
            }
        }
    }

    for (int i = 0; i < 256; ++i) {
        VecBox v(stateIDPairList.size());
        for (int j = 0; j < stateIDPairList.size(); ++j) {
            v[j].resize(stateIDPairList.size());
            for (int k = 0; k < stateIDPairList.size(); ++k) {
                v[j][k] = 0;
            }
        }
        N.insert(make_pair(i, v));
    }

}

void initialize() {
    for (const auto& elem : stateIDPairList) {
        StatePtr tempState = elem.second;
        if(elem.second->reportingState()) F.push_back(tempState);
        else remStates.push_back(tempState);
    }

    for (StatePtr repState : F) {
        for (StatePtr notRepState : remStates) {
            w.push_back(make_pair(repState, notRepState));
            w.push_back(make_pair(notRepState, repState));
            q.push(make_pair(repState, notRepState));
        }
    }
}

void calc() {
    while (!q.empty()) {
        StatePtr i, j;
        const auto key = q.front();
        q.pop();
        i = key.first;
        j = key.second;
        for (int a = 0; a < 256; ++a) {
            auto key1 = pair<int, StatePtr> (a, j);
            if (reverseTransition.find(key1) != reverseTransition.end()) {
                for (StatePtr k : reverseTransition.at(key1)) {
                    N.at(a)[i->getStateID()-1][k->getStateID()-1] = N.at(a)[i->getStateID()-1][k->getStateID()-1] + 1;
                    if (N.at(a)[i->getStateID()-1][k->getStateID()-1] == card.at(pair<int, StatePtr> (a, k))) {
                        auto key2 = pair <int, StatePtr> (a, i);
                        if (reverseTransition.find(key2) != reverseTransition.end()) {
                            for (StatePtr l : reverseTransition.at(key2)) {
                                if(w.end() == find(w.begin(), w.end(), make_pair(l, k))) {
                                    w.push_back(make_pair(l,k));
                                    q.push(make_pair(l,k));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void printCard() {
    cout << "w: \n";
//    for (const auto elem : w) {
//        cout <<elem.first->getStateID()<<" "<< elem.second->getStateID() << "\n";
//    }
    std::vector <pair<StatePtr, StatePtr>> m;
    for (const auto& elem1 : stateIDPairList) {
        for (const auto& elem2 : stateIDPairList) {
            if (elem1.second != elem2.second) {
                if(w.end() == find(w.begin(), w.end(), make_pair(elem1.second, elem2.second))) {
                    if (elem1.second->reportingState() || elem2.second->reportingState()) {
                        continue;
                    }
                    if (elem1.second->startingState() || elem2.second->startingState()) {
                        continue;
                    }
                    m.push_back(make_pair(elem1.second, elem2.second));
                }
            }
        }
    }

//    for (const auto elem : stateIDPairList) {
//        cout << elem.second->getStateID() << " " << elem.second->startingState() << " " << elem.second->reportingState() << "\n";
//        for (int i = 0; i< 256; i++) {
//            if (!elem.second->getReachableStateList(i).empty()) {
//                cout << i << ": ";
//                for (const auto vec : elem.second->getReachableStateList(i)) {
//                    cout << vec->getStateID() << " ";
//                }
//                cout << "\n";
//            }
//        }
//
//    }
    cout << "m: " << m.size() << "\n";
    for (const auto& elem : m) {
        cout << elem.first->getStateID() << " " << elem.second->getStateID() << "\n";
    }
}

void printAutomata () {
    for (const auto elem : stateIDPairList) {
        cout << "State ID: " << elem.second->getStateID() << ": ";
        for (int i = 0; i < 256; i++) {
            if (!elem.second->getReachableStateList(i).empty()) {
                cout << "s";
            }
        }
    }
}

bool hammingHeuristic(const StatePtr& i, const StatePtr& j, const StateVec& vi, const StateVec& vj) {

    if ((vi.empty() && vj.empty())) {
        return true;
    }
    if ((vi.size() != vj.size()) && (vi.empty() || vj.empty())) {
        return false;
    }
    else if (find(vi.begin(), vi.end(), j) == vi.end()) {
        if (find(vj.begin(), vj.end(), i) == vj.end()) {
            for (const StatePtr& s1 : vi) {
                if (find(vj.begin(), vj.end(), s1) == vj.end()) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool forwardHeuristic(const StatePtr& i, StatePtr j, StateVec vi, StateVec vj) {

    if ((vi.empty() && vj.empty())) {
        return true;
    }
    if ((vi.size() != vj.size()) && (vi.empty() || vj.empty())) {
        if (find(vi.begin(), vi.end(), j) != vi.end()) {
            return true;
        } else if (find(vj.begin(), vj.end(), i) != vj.end()) {
            return true;
        }
        return false;
    }
    else if (find(vi.begin(), vi.end(), j) == vi.end()) {
        if (find(vj.begin(), vj.end(), i) == vj.end()) {
            for (const StatePtr& s1 : vi) {
                if (find(vj.begin(), vj.end(), s1) == vj.end()) {
                    return false;
                }
            }
        }
    }
    return true;
}

void alt() {
    for (const auto& elem1 : stateIDPairList) {
        StatePtr i;
        i = elem1.second;
        for (const auto& elem2 : stateIDPairList) {
            StatePtr j = elem2.second;
            if (w.end() == find(w.begin(), w.end(), make_pair(i, j))) {
//                if (i->reportingState() || j->reportingState()) {
//                    w.push_back(make_pair(i,j));
//                    continue;
//                }
                if (i != j) { //check if i,j pair already exists in the non mergeable list
                    for (int a = 0; a < 256; a++) {
                        StateVec vi = i->getReachableStateList(a);
                        StateVec vj = j->getReachableStateList(a);
                        if (!forwardHeuristic(i, j, vi, vj)) {
                            w.push_back(make_pair(i, j));
                            break;
                        } else {
                            vi = i->getRevStateList(a);
                            vj = j->getRevStateList(a);
                            if (!forwardHeuristic(i, j, vi, vj)) {
                                w.push_back(make_pair(i,j));
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

//    for (const auto& elem1 : stateIDPairList) {
//        StatePtr i;
//        i = elem1.second;
//        for (const auto& elem2 : stateIDPairList) {
//            StatePtr j = elem2.second;
//            if (w.end() == find(w.begin(), w.end(), make_pair(i, j))) {
//                if (i != j) { //check if i,j pair already exists in the non mergeable list
//                    for (int a = 0; a < 256; a++) {
//                        StateVec vi = i->getReachableStateList(a);
//                        StateVec vj = j->getReachableStateList(a);
//                        if (!hammingHeuristic(i, j, vi, vj)) {
//                            w.push_back(make_pair(i, j));
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
}



int main() {
    Util::parse(stateIDPairList);
    preprocess();
    initialize();
//    calc();
    alt();
    cout <<"w: "<< w.size()  << " n: " << stateIDPairList.size() << std::endl;
    printCard();
    return 0;
}