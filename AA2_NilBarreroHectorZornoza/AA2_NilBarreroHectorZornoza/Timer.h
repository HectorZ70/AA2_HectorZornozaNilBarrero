#pragma once 
#include <mutex>
#include <functional>

static class Timer
{
public:
	typedef std::function<void()> BoolCheck();
	
	typedef std::function<void()> OnTimeElapsed;
	typedef std::function<void()> OnTimeElapsedWithLoop;

	static void SleepThread(unsigned long timeRequired);
	//static void BlockThreadWhile(BlockCheck blockCheck, unsigned long timeRequiredMilisecondsToNextCheck = 0);

	static void StartTimer(unsigned long timeRequired, OnTimeElapsed onTimeElapsed);
	static void StartLoopTimer(unsigned long timeRequired, OnTimeElapsedWithLoop onTimeElapsed);
};