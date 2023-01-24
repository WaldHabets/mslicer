#ifndef GUARD_PROGRESS_TRACKER_HPP
#define GUARD_PROGRESS_TRACKER_HPP

#include <ctime>
#include <chrono>

using namespace std::chrono;

class ProgressTracker 
{

public:

    void start(double maximum);
    void tick();
    void tick_zoom(double zoom_maximum);

    double get_elapsed_time() const;
    double get_estimated_time() const;
    double get_total_progress_count() const;
    double get_total_progress_percent() const;
    double get_zoom_progress_count() const;
    double get_zoom_progress_percent() const;

private:

    high_resolution_clock::time_point start_time;
    high_resolution_clock::time_point latest_tick;

    double elapsed_time = 0;

    double maximum = 0;
    double zoom_maximum = 0;

    double progress = 0;
    double zoom_progress = 0;


};


#endif // GUARD_PROGRESS_TRACKER_HPP