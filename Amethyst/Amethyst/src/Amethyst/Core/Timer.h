#pragma once

namespace Amethyst
{
	class Timer
	{
	public:
		Timer(float t = 0.0f) : time(t)
		{

		}

		float GetSeconds() const { return time; }
		float GetMilliSeconds() const { return time * 1000.0f; }

	private:
		float time;
	};
}