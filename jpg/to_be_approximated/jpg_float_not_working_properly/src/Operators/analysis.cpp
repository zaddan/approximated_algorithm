#include <iostream>
#include <complex>
#include <vector>
#include <map>

#include "analysis.h"

double mgf_cal(const map<int,double> dist, double t) {
	double sum;
	sum = 0;

	for (map<int,double>::const_iterator it = dist.begin(); it != dist.end(); it++) {
		sum += (*it).second * exp(t*(*it).first);
	}

	return sum;
}
