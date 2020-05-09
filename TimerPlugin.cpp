#include "TimerPlugin.h"
#include <string.h>
#include <ctime>
#include <Windows.h> // Don't have to check for linux bc no more linux support :(

using namespace std::chrono;

BAKKESMOD_PLUGIN(TimerPlugin, "Timer Plugin", "1.0", PLUGINTYPE_FREEPLAY)


void TimerPlugin::onLoad()
{
	cvarManager->registerCvar("timer_on", "0", "Determines if the timer is enabled, and will start timer", true, true, 0, true, 1, false).addOnValueChanged(std::bind(&TimerPlugin::countDownTimer, this, std::placeholders::_1, std::placeholders::_2));;
	cvarManager->registerCvar("timer_set_hours", "0", "How many hours added to the timer.", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_set_minutes", "0", "How many minutes added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_set_seconds", "0", "How many seconds added to the timer.", true, true, 0, true, 60, true);

	cvarManager->registerCvar("timer_lookBreak_on", "0", "Determines if lookBreak is enabled.", true, true, 0, true, 1, true);
	cvarManager->registerCvar("timer_lookBreak_hours", "0", "How many hours until alert for looking around is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_lookBreak_minutes", "0", "How many minutes until alert for looking around is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_lookBreak_seconds", "0", "How many hours until alert for looking around is called", true, true, 0, true, 60, true);

	cvarManager->registerCvar("timer_standBreak_on", "0", "Determinds if standBreak is enabled", true, true, 0, true, 1, true);
	cvarManager->registerCvar("timer_standBreak_hours", "0", "How many hours until alert for standing is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_standBreak_minutes", "0", "How many minutes until alert for standing is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_standBreak_seconds", "0", "How many seconds until alert for standing is called", true, true, 0, true, 60, true);
}

void TimerPlugin::onUnload()
{
}

void TimerPlugin::countDownTimer(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{
		double totalTime = cvarManager->getCvar("timer_set_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_set_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_set_seconds").getIntValue(); // convert cvars into seconds

		double initialTime = clock() / (double)CLOCKS_PER_SEC;
		double timeElapsed = 0;
		double currentTime;

		cvarManager->log("Timer started with " + to_string(totalTime) + " seconds");

		while (timeElapsed < totalTime)
		{
			currentTime = clock() / (double)CLOCKS_PER_SEC;
			timeElapsed = currentTime - initialTime;
			timeLeft = totalTime - timeElapsed;

			Sleep(100);
		}


		cvarManager->log("timer is finished");
		// add if user is not in game
	}
}
