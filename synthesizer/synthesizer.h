/*
 * synthesizer.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef SYNTHESIZER_SYNTHESIZER_H_
#define SYNTHESIZER_SYNTHESIZER_H_

#include <vector>

using std::vector;

class Synthesizer {
public:
	Synthesizer();
	Synthesizer(unsigned int f);

	void write(double start, double length,
			double (*wavFunc)(const vector<double>&, double),
			const vector<double>& params);

	void write_to_file(const char* filename);

	void normalize(double max_volume);

private:
	vector<double> array;
	unsigned int fs;
};

double exp_decay_sin(const vector<double>& params, double t);

#endif /* SYNTHESIZER_SYNTHESIZER_H_ */
