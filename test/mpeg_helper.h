/*
 * mpeg_helper.h
 *
 *  Created on: Aug 23, 2016
 *      Author: fx
 */

#ifndef TEST_MPEG_HELPER_H_
#define TEST_MPEG_HELPER_H_

#include <cstring>
#include <cstdio>
#include <cstdlib>

void create_blank_audio(float duration, const char* filename) {
	char buffer[100];
	// sprintf(buffer, "ffmpeg -v 0 -y -f lavfi -i anullsrc -t %f -c:a copy %s", duration, filename);
	sprintf(buffer, "sox -n -r 44100 -c 2 %s trim 0.0 %f", filename, duration);
	system(buffer);
}

// void add_blank(char* input, float duration, const char* filename)
// {
// 	char buffer[100];
// 	create_blank_audio(duration, "tmp.wav");
// 	sprintf(buffer, "ffmpeg -v 0 -y -i %s -i %s -filter_complex '[0:0][1:0]concat=n=2:v=0:a=1[out]' -map '[out]' %s", "tmp.wav", input, filename);
// 	system(buffer);
// }

void combine_audio(const char* target, const char* source, float pos,
		float volume, const char* filename) {
	char buffer[100];
	create_blank_audio(pos, "tmp.wav");
	sprintf(buffer, "sox -m -v 1 %s \"|sox -v %f %s -p pad %f\" %s", target,
			volume, source, pos, filename);
	//puts(buffer);
	system(buffer);
	std::remove("tmp.wav");
	// sprintf(buffer, "ffmpeg -v 0 -y -i %s -i %s -filter_complex '[0:0][1:0]concat=n=2:v=0:a=1[out]' -map '[out]' %s", "tmp.wav", source, "tmp.wav");
	// system(buffer);
	// sprintf(buffer, "ffmpeg -v 0 -y -i %s -i %s -filter_complex \"amix=inputs=2:duration=first\" %s", target, "tmp.wav", filename);
	// system(buffer);
	// std::remove("tmp.wav");
}

void combine_video_audio(const char* v, const char* a, const char* out) {
	char buffer[100];
	sprintf(buffer, "ffmpeg -v 0 -y -i %s -i %s -shortest %s", v, a, out);
	//puts(buffer);
	system(buffer);
}

#endif /* TEST_MPEG_HELPER_H_ */
