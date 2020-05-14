#include "TimerPlugin.h"
#include <string.h>

BAKKESMOD_PLUGIN(TimerPlugin, "Timer Plugin", "1.0", PLUGINTYPE_FREEPLAY)


void TimerPlugin::onLoad()
{
	// Regular Timer
	
	cvarManager->registerCvar("timer_enabled", "0", "Whether the timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::timerOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("timer_set_hours", "0", "How many hours added to the timer.", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_set_minutes", "0", "How many minutes added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_set_seconds", "0", "How many seconds added to the timer.", true, true, 0, true, 60, true);

	
	// Looking Break

	cvarManager->registerCvar("lookBreak_enabled", "0", "Whether looking break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::lookBreakOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("lookBreak_set_hours", "0", "How many hours until alert for looking around is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("lookBreak_set_minutes", "20", "How many minutes until alert for looking around is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("lookBreak_set_seconds", "0", "How many hours until alert for looking around is called", true, true, 0, true, 60, true);


	// Standing Break

	cvarManager->registerCvar("standBreak_enabled", "0", "Whether standing break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::standBreakOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("standBreak_set_hours", "0", "How many hours until alert for standing is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("standBreak_set_minutes", "30", "How many minutes until alert for standing is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("standBreak_set_seconds", "0", "How many seconds until alert for standing is called", true, true, 0, true, 60, true);



	initialTime = timeNow();
}

void TimerPlugin::onUnload()
{
}

double TimerPlugin::timeNow()
{
	return clock() / (double)CLOCKS_PER_SEC;
}


void TimerPlugin::timerOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{	
		timerTotal = cvarManager->getCvar("timer_set_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_set_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_set_seconds").getIntValue(); 
		cvarManager->log("Timer started with " + to_string(timerTotal) + " seconds");
		timerEnabled = true;
		timerStartTime = timeNow();
		runTimers();
	}
	
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		timerEnabled = false;
	}

}

void TimerPlugin::lookBreakOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{
		lookBreakTotal = cvarManager->getCvar("lookBreak_set_hours").getIntValue() * 3600 + cvarManager->getCvar("lookBreak_set_minutes").getIntValue() * 60 + cvarManager->getCvar("lookBreak_set_seconds").getIntValue(); 
		cvarManager->log("Look break timer started with " + to_string(lookBreakTotal) + " seconds");
		lookBreakEnabled = true;
		lookBreakStartTime = timeNow();
		runTimers();
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		lookBreakEnabled = false;
	}
}

void TimerPlugin::standBreakOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue()) 
	{
		standBreakTotal = cvarManager->getCvar("standBreak_set_hours").getIntValue() * 3600 + cvarManager->getCvar("standBreak_set_minutes").getIntValue() * 60 + cvarManager->getCvar("standBreak_set_seconds").getIntValue(); 
		cvarManager->log("Stand break timer started with " + to_string(standBreakTotal) + " seconds");
		standBreakEnabled = true;
		standBreakStartTime = timeNow();
		runTimers();
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		standBreakEnabled = false;
	}
}

void TimerPlugin::runTimers()
{
	currentTime = timeNow();

	// tests every case
	if (!timerEnabled && !lookBreakEnabled && !standBreakEnabled) 
	{
		return;
	}
	else if (timerEnabled && !lookBreakEnabled && !standBreakEnabled)
	{
		checkTimer();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (timerEnabled && lookBreakEnabled && !standBreakEnabled)
	{
		checkTimer();
		checkLookBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (timerEnabled && lookBreakEnabled && standBreakEnabled)
	{
		checkTimer();
		checkLookBreak();
		checkStandBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (!timerEnabled && lookBreakEnabled && !standBreakEnabled)
	{
		checkLookBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (!timerEnabled && lookBreakEnabled && standBreakEnabled)
	{
		checkLookBreak();
		checkStandBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (!timerEnabled && !lookBreakEnabled && standBreakEnabled)
	{
		checkStandBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
	else if (timerEnabled && !lookBreakEnabled && standBreakEnabled)
	{
		checkTimer();
		checkStandBreak();
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 0.1f);
	}
}

void TimerPlugin::checkTimer()
{
	timerTimeElapsed = currentTime - timerStartTime;

	if (timerTimeLeft > 0)
	{
		timerTimeLeft = timerTotal - timerTimeElapsed;
	}


	if (timerTimeElapsed < timerTotal)
	{
		return;
	}
	else if (!gameWrapper->IsInOnlineGame())
	{
		timerEnabled = false;
		cvarManager->log("Timer finished");
		return;
	}
	else
	{
		return;
	}
}

void TimerPlugin::checkLookBreak()
{
	lookBreakTimeElapsed = currentTime - lookBreakStartTime;

	if (lookBreakTimeElapsed < lookBreakTotal)
	{
		return;
	}
	else if (!gameWrapper->IsInOnlineGame())
	{
		lookBreakEnabled = false;
		cvarManager->log("Look 20 ft away for 20 seconds!");
		return;
	}
	else
	{
		lookBreakTimeElapsed = 0;
		return;
	}
}

void TimerPlugin::checkStandBreak()
{
	standBreakTimeElapsed = currentTime - standBreakStartTime;

	if (standBreakTimeElapsed < standBreakTotal)
	{
		return;
	}
	else if (!gameWrapper->IsInOnlineGame())
	{
		standBreakEnabled = false;
		cvarManager->log("Stand up for a couple of minutes!");
		return;
	}
	else
	{
		standBreakTimeElapsed = 0;
		return;
	}
}

void TimerPlugin::OnDraw(CanvasWrapper cw)
{
}


