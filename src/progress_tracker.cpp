#include "progress_tracker.hpp"

void ProgressTracker::start(double maximum) {
    this->start_time = high_resolution_clock::now();
    this->latest_tick = this->start_time;
    this->maximum = maximum;
}

void ProgressTracker::tick() {
    high_resolution_clock::time_point tick = high_resolution_clock::now();
    if (tick >= this->latest_tick) {
        this->latest_tick = tick;
        this->elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(this->latest_tick - this->start_time).count();
    }
    ++this->progress;
    ++this->zoom_progress;
}

void ProgressTracker::tick_zoom(double zoom_maximum) {
    this->zoom_maximum = zoom_maximum;
    this->zoom_progress = 0;
}


double ProgressTracker::get_elapsed_time() {
    return this->elapsed_time;
}
double ProgressTracker::get_estimated_time() {
    double frac = this->maximum / this->progress;
    return frac * this->elapsed_time;
}
double ProgressTracker::get_total_progress_count() {
    return this->progress;
}
double ProgressTracker::get_total_progress_percent() {
    return (this->progress / this->maximum) * 100;
}
double ProgressTracker::get_zoom_progress_count() {
    return this->zoom_progress;
}
double ProgressTracker::get_zoom_progress_percent() {
    return (this->zoom_progress / this->zoom_maximum) * 100;
}