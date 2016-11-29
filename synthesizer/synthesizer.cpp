/*
 * synthesizer.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#include "synthesizer.h"
#include "writer.h"

Synthesizer::Synthesizer() :
		fs(44100) {
}
Synthesizer::Synthesizer(unsigned int f) :
		fs(f) {
}

void Synthesizer::write(double start, double length,
		double (*wavFunc)(const vector<double>&, double),
		const vector<double>& params) {
	unsigned long pos1 = (unsigned long) (start * fs);
	unsigned long l = (unsigned long) (length * fs);
	array.resize(std::max(array.size(), pos1 + l));
	for (unsigned long i = 0; i < l; i++) {
		array[pos1 + i] = wavFunc(params, (double) i / fs);
	}
}

void Synthesizer::write_to_file(const char* filename) {
	write_audio(filename, fs, array);
	array = vector<double>();
}

double exp_decay_sin(const vector<double>& params, double t) {
	double freq = params[0];
	double baseAmp = params[1];
	double rate = params[2];

	constexpr double two_pi = 6.283185307179586476925286766559;

	double amp = baseAmp * pow(2.7182818284, -rate * t);
	return amp * sin(two_pi * freq * t);
}

void Synthesizer::normalize(double max_volume) {
	double curr_max = 0;
	for (double point : array) {
		curr_max = std::max(curr_max, std::abs(point));
	}
	if (curr_max <= max_volume)
		return;
	double r = max_volume / curr_max;
	for (unsigned long i = 0; i < array.size(); i++) {
		array[i] *= r;
	}
}
