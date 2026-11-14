#include "IconResolver.h"
#include <iostream>
int main(int, char** argv) {
    IconResolver r(argv[1]);
    std::cout << r.TypeCount() << " types, " << r.LocationCount() << " locations\n";
    for (auto t : {"SettlementMarker","vaultmarker","CaveMarker","QuestMarker"}) {
        auto p = r.Resolve(t);
        std::cout << t << " -> " << (p ? p->filename().string() : "(vanilla)") << "\n";
    }
}
