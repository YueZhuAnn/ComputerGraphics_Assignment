#include <vector>
#include <iostream>
#include "PolyCal.hpp"

using namespace std;

poly polyAdd(const poly poly1, const poly poly2)
{
    unsigned size = poly1.size() > poly2.size()? poly1.size(): poly2.size();

    poly result = vector<double>(size, 0);

    for(unsigned i = 0; i <poly1.size(); i++)
    {
        unsigned coe = poly1.size()-i-1;
        result[size-coe-1] += poly1[i];
    }
    for(unsigned i = 0; i <poly2.size(); i++)
    {
        unsigned coe = poly2.size()-i-1;
        result[size-coe-1] += poly2[i];
    }
    while(result.size() > 1)
    {
        if(*(result.begin()) == 0)
        {
            result.erase(result.begin());
            continue;
        }
        break;
    }
    return result;
}

poly polyMult(const poly poly1, const poly poly2)
{
    unsigned size = poly1.size()+poly2.size()-1;
    poly result = vector<double>(size, 0);
    for(unsigned p = 0; p < poly1.size(); p++)
    {
        for(unsigned q = 0; q < poly2.size(); q++)
        {
            unsigned coe1 = poly1.size()-p-1;
            unsigned coe2 = poly2.size()-q-1;
            unsigned coe3 = coe1+coe2;
            result[size-coe3-1] += poly1[p]*poly2[q];
        }
    }
    while(result.size() > 1)
    {
        if(*(result.begin()) == 0)
        {
            result.erase(result.begin());
            continue;
        }
        break;
    }
    return result;
}

poly polyScale(const poly poly1, const double scale)
{
    unsigned size = poly1.size();
    poly result = vector<double>(size, 0);
    for(unsigned i = 0; i < poly1.size(); i++)
    {
        result[i] = poly1[i]*scale;
    }
    while(result.size() > 1)
    {
        if(*(result.begin()) == 0)
        {
            result.erase(result.begin());
            continue;
        }
        break;
    }
    return result;
}
