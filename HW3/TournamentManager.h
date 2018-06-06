#include <functional>
#include <memory>
#include <iostream>
#include <map>
#include "PlayerAlgorithm.h"

class TournamentManager {
    static TournamentManager theTournamentManager;
    std::map<std::string, std::function<std::unique_ptr<PlayerAlgorithm>()>> id2factory;
    // private ctor
    TournamentManager() {}
public:
    static TournamentManager& getTournamentManager() {
        return theTournamentManager;
    }
    void registerAlgorithm(std::string id, std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod) {
        // TODO: should warn if id is already registered Tala says: almost done, see below
        if (id2factory.find(id) == id2factory.end())
        {
            //TODO: WARN?
            //ID ALREADY EXISTS
        }else{
            id2factory[id] = factoryMethod;
        }
    }
    void run()const {
        for(auto& pair : id2factory) {
            const auto& id = pair.first;
            std::cout << id << ": ";
            const auto& factoryMethod = pair.second;
            factoryMethod()->getMove(); //TODO: complete this later
        }
    }
};