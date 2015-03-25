#include "Timer.h"

#include <GL/freeglut.h>

Timer& Timer::get()
{
	static Timer singleton;
	return singleton;
}

Timer::Timer()
	:m_LastInterval(0), m_LastTime(glutGet(GLUT_ELAPSED_TIME))
{

}

int Timer::getTime() const
{
	return m_LastTime; 
}

int Timer::getLastInterval() const
{
	return m_LastInterval;
}

void Timer::updateInterval()
{
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	m_LastInterval = currTime - m_LastTime;
	m_LastTime = currTime;

	m_FrameMillis += m_LastInterval;
	m_FrameUpdates++;

	if(m_FrameMillis >= 1000)
	{
		m_FrameMillis = 0;
		m_FrameRate = m_FrameUpdates;
		m_FrameUpdates = 0;
	}
}

float Timer::getFrameRate()
{
	return m_FrameRate;
}

void Timer::reset()
{
	m_LastInterval = 0;
	m_LastTime = glutGet(GLUT_ELAPSED_TIME);
	m_FrameMillis = 0;
	m_FrameUpdates = 0;
	m_FrameRate = 0.0f;
}

