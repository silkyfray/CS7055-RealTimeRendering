#pragma once
/**
@brief A class which has a singleton instance serving as a global as but can also be instantiated separately
*/
class Timer
{
public:
	/**Retrieve global instance of the Timer*/
	static Timer& get();
	Timer();

	/**@brief Get the absolute time*/
	int getTime() const;
	/**@brief Retrieve the interval from when updateInteval was last called*/
	int getLastInterval() const;
	/**@brief delta difference curr - last */
	void updateInterval();
	/**Sets m_LastTime to current time and m_LastInterval to 0*/
	void reset();

	float getFrameRate();
private:
	int m_LastTime; //<! the last time (absolute) updateInterval was called
	int m_LastInterval; //<! delta difference curr - last
	int m_FrameMillis;
	int m_FrameUpdates;
	int m_FrameRate;
};