#pragma once

#include "Mat.hpp"

#include <vector>

namespace smk
{
    // this code was copied from dlib
    // http://dlib.net/dlib/optimization/max_cost_assignment.h.html
    using CostT = long long;
    using Assignment = std::vector<CostT>;
    using CostMatrix = Matl;


    // calculates the actual cost for the input assigment
    long assignmentCost(const CostMatrix &cost, const Assignment &assignment);

    // Hungarian algorithm for max cost assigment
    Assignment assignmentMaxCost(const CostMatrix &cost);

}
