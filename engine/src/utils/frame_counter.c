#include "frame_counter.h"

#include <stdio.h>

void FrameCounter_init(FrameCounter *this, double logIntervalSeconds) {
	this->_timeElapsedSeconds = 0.;
	this->_logIntervalSeconds = logIntervalSeconds;
	this->_frames = 0;
}

static void writeStats(const FrameCounter *counter) {
	printf(
			"%.1f fps / %.3f mspf :: %d frames in %.2f seconds\n",
			(double) counter->_frames / counter->_timeElapsedSeconds,
			counter->_timeElapsedSeconds / (double) counter->_frames * 1000,
			counter->_frames,
			counter->_timeElapsedSeconds
	);
}

void FrameCounter_newFrame(FrameCounter *this, double timeDeltaSeconds) {
	if (this->_timeElapsedSeconds >= this->_logIntervalSeconds) {
		writeStats(this);
		this->_timeElapsedSeconds = 0.;
		this->_frames = 0;
	}

	this->_timeElapsedSeconds += timeDeltaSeconds;
	this->_frames++;
}