
#include "TimeStats.h"
#include <cstring>
#include <ctime>
#include <cassert>
#include <chrono>
#include <iostream>


using namespace std;

TimeStats::~TimeStats() {
}

void TimeStats::end_op() {
	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
			now - last_time_);

	Stat new_stat;
	new_stat.micros = elapsed.count();
	last_time_ = std::chrono::system_clock::now();

	stats_.push_back(new_stat);
}

void TimeStats::begin_op() {
	last_time_ = std::chrono::system_clock::now();
}

