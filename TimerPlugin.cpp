#include "TimerPlugin.h"
#include <string.h>
#include <ctime>
#include <Windows.h> // Don't have to check for linux bc no more linux support :(

using namespace std::chrono;

BAKKESMOD_PLUGIN(TimerPlugin, "Timer Plugin", "1.0", PLUGINTYPE_FREEPLAY)


void TimerPlugin::onLoad()
{
	// Regular Timer
	
	cvarManager->registerNotifier("timer_start", [this](std::vector<string> params) {

		this->timerEnabled = true;
		this->timerOnChangeValue();

		}, "Starts the timer.", PERMISSION_ALL);

	cvarManager->registerNotifier("timer_stop", [this](std::vector<string> params) {
		this->timerEnabled = false;
		}, "Stops the timer.", PERMISSION_ALL);

	cvarManager->registerCvar("timer_set_hours", "0", "How many hours added to the timer.", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_set_minutes", "0", "How many minutes added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_set_seconds", "0", "How many seconds added to the timer.", true, true, 0, true, 60, true);

	
	// Looking Break

	cvarManager->registerCvar("lookBreak_enabled", "0", "Whether looking break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::lookOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("timer_lookBreak_hours", "0", "How many hours until alert for looking around is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_lookBreak_minutes", "20", "How many minutes until alert for looking around is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_lookBreak_seconds", "0", "How many hours until alert for looking around is called", true, true, 0, true, 60, true);


	// Standing Break

	cvarManager->registerCvar("standBreak_enabled", "0", "Whether standing break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::standOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("timer_standBreak_hours", "0", "How many hours until alert for standing is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_standBreak_minutes", "30", "How many minutes until alert for standing is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_standBreak_seconds", "0", "How many seconds until alert for standing is called", true, true, 0, true, 60, true);



	initialTime = timeNow();
}

void TimerPlugin::onUnload()
{
}

void TimerPlugin::timerOnChangeValue()
{
	timerTotal = cvarManager->getCvar("timer_set_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_set_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_set_seconds").getIntValue(); // convert cvars into seconds

	cvarManager->log("Timer started with " + to_string(timerTotal) + " seconds");

	startTime = timeNow();
	checkTime();
	

}

void TimerPlugin::lookOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{
		lookBreakEnabled = true;
		lookTotal = cvarManager->getCvar("timer_lookBreak_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_lookBreak_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_lookBreak_seconds").getIntValue(); // convert cvars into seconds
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		lookBreakEnabled = false;
	}
	

}

void TimerPlugin::standOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue()) 
	{
		standBreakEnabled = true;
		standTotal = cvarManager->getCvar("timer_standBreak_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_standBreak_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_standBreak_seconds").getIntValue(); // convert cvars into seconds
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		standBreakEnabled = false;
	}
}

double TimerPlugin::timeNow()
{
	return clock() / (double) CLOCKS_PER_SEC;
}

void TimerPlugin::checkTime()
{
	if (!timerEnabled && !lookBreakEnabled && !standBreakEnabled) 
	{
		return;
	}
	else if (timerEnabled && !lookBreakEnabled && !standBreakEnabled)
	{
		runTimer();
	}
	else if (timerEnabled && lookBreakEnabled && !standBreakEnabled)
	{

	}
	else if (timerEnabled && lookBreakEnabled && standBreakEnabled)
	{

	}
	else if (!timerEnabled && lookBreakEnabled && !standBreakEnabled)
	{

	}
	else if (!timerEnabled && lookBreakEnabled && standBreakEnabled)
	{

	}
}

void TimerPlugin::runTimer()
{
	double timeElapsed = 0;
	currentTime = timeNow();
	timeElapsed = currentTime - startTime;

	if (timeLeft > 0)
	{
		timeLeft = timerTotal - timeElapsed;
	}


	if (timeElapsed < timerTotal)
	{
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->checkTime();
			}, 0.1f);
	}
	else if (!gameWrapper->IsInOnlineGame())
	{
		timerEnabled = 0;
		cvarManager->log("Timer finsihed");
		return;
	}
	else
	{
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->checkTime();
			}, 0.1f);
	}
}


