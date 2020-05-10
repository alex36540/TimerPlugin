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
		this->countDownTimer();

		}, "Starts the timer.", PERMISSION_ALL);

	cvarManager->registerNotifier("timer_stop", [this](std::vector<string> params) {
		this->timerEnabled = false;
		}, "Stops the timer.", PERMISSION_ALL);

	cvarManager->registerCvar("timer_set_hours", "0", "How many hours added to the timer.", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_set_minutes", "0", "How many minutes added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_set_seconds", "0", "How many seconds added to the timer.", true, true, 0, true, 60, true);

	
	// Looking Break

	cvarManager->registerNotifier("lookBreak_start", [this](std::vector<string> params) {
		
		this->lookBreakEnabled = true;
		// this->CheckForBump(); add function

		}, "Starts the look break timer.", PERMISSION_ALL);

	cvarManager->registerNotifier("lookBreak_stop", [this](std::vector<string> params) {
		this->lookBreakEnabled = false;
		}, "Stops the look break timer.", PERMISSION_ALL);

	cvarManager->registerCvar("timer_lookBreak_hours", "0", "How many hours until alert for looking around is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_lookBreak_minutes", "0", "How many minutes until alert for looking around is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_lookBreak_seconds", "0", "How many hours until alert for looking around is called", true, true, 0, true, 60, true);


	// Standing Break

	cvarManager->registerNotifier("standBreak_start", [this](std::vector<string> params) {

		this->standBreakEnabled = true;
		// this->CheckForBump(); add function

		}, "Starts the stand break timer.", PERMISSION_ALL);

	cvarManager->registerNotifier("timer_stop", [this](std::vector<string> params) {
		this->standBreakEnabled = false;
		}, "Stops the stand break timer.", PERMISSION_ALL);

	cvarManager->registerCvar("timer_standBreak_hours", "0", "How many hours until alert for standing is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_standBreak_minutes", "0", "How many minutes until alert for standing is called", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_standBreak_seconds", "0", "How many seconds until alert for standing is called", true, true, 0, true, 60, true);
}

void TimerPlugin::onUnload()
{
}

void TimerPlugin::countDownTimer()
{
	totalTime = cvarManager->getCvar("timer_set_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_set_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_set_seconds").getIntValue(); // convert cvars into seconds

		
	double timeElapsed = 0;

	cvarManager->log("Timer started with " + to_string(totalTime) + " seconds");

	initialTime = clock() / (double)CLOCKS_PER_SEC;
	checkTime();
	

}

void TimerPlugin::countDownLook()
{
	double totalLookTime = cvarManager->getCvar("timer_lookBreak_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_lookBreak_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_lookBreak_seconds").getIntValue(); // convert cvars into seconds

	double initialTime = clock() / (double)CLOCKS_PER_SEC;
	double timeElapsed = 0;
	double currentTime;
}

void TimerPlugin::countDownStand()
{
	double totalStandTime = cvarManager->getCvar("timer_standBreak_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_standBreak_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_standBreak_seconds").getIntValue(); // convert cvars into seconds

	double initialTime = clock() / (double)CLOCKS_PER_SEC;
	double timeElapsed = 0;
	double currentTime;
}

void TimerPlugin::checkTime()
{
	if (!timerEnabled) {
		return; 
	}

	double timeElapsed = 0;
	currentTime = clock() / (double)CLOCKS_PER_SEC;
	timeElapsed = currentTime - initialTime;

	if (timeLeft > 0)
	{ 
		timeLeft = totalTime - timeElapsed; 
	}
	

	if (timeElapsed < totalTime)
	{
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->checkTime();
			}, 0.1f);
		checkTime();
	}
	else if (!gameWrapper->IsInOnlineGame())
	{
		timerEnabled = 0;
		cvarManager->log("Timer finsihed");
		return;
	}
	else
	{
		checkTime();
	}
}


