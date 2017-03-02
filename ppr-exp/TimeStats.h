#ifndef TIMESTATS_H_
#define TIMESTATS_H_

#include <vector>
#include <chrono>

using namespace std;

typedef struct Stat {
	double micros;
} Stat;

class TimeStats {
public:
	TimeStats() {}
	virtual ~TimeStats();

	// Call begin_op just before the operation
	void begin_op();
	// Call end_op just after the operation (this will add the op time to the list)
	void end_op();

	const vector<Stat> stats() {
		return stats_;
	}
	void discard_last(){
	    stats_.resize(stats_.size()-1);
	}

private:
	void reset_window();

	std::chrono::system_clock::time_point last_time_ = std::chrono::system_clock::now();
	vector<Stat> stats_;
};

#endif /* TIMESTATS_H_ */
