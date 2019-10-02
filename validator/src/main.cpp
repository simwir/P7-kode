#include "foo.hpp"
#include "utap/utap.h"
#include <iostream>

int main()
{
    UTAP::TimedAutomataSystem system;
    parseXMLFile("train-gate-stat.xml", &system, true);
    for (UTAP::query_t query : system.getQueries()) {
        std::cout << query.formula << std::endl;
    }
    std::cout << foo() << std::endl;
    return 0;
}
