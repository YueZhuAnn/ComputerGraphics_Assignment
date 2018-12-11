#pragma once

#include<vector>

typedef std::vector<double> poly;

poly polyMult(const poly poly1, const poly poly2);

poly polyAdd(const poly poly1, const poly poly2);

poly polyScale(const poly poly1, const double scale);