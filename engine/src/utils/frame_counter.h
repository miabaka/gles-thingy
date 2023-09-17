#pragma once

typedef struct {
	double _timeElapsedSeconds;
	double _logIntervalSeconds;
	int _frames;
} FrameCounter;

void FrameCounter_init(FrameCounter *, double logIntervalSeconds);

void FrameCounter_newFrame(FrameCounter *, double timeDeltaSeconds);