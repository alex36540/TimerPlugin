#include "TimerPlugin.h"
#include <string.h>

BAKKESMOD_PLUGIN(TimerPlugin, "Timer Plugin", "1.0", PLUGINTYPE_FREEPLAY)


void TimerPlugin::onLoad()
{
	// Regular Timer
	firstTime = true;
	cvarManager->registerNotifier("timer_start", [this](std::vector<string> params) {
		if (firstTime)
		{
			timerTotal = cvarManager->getCvar("timer_set_hours").getIntValue() * 3600 + cvarManager->getCvar("timer_set_minutes").getIntValue() * 60 + cvarManager->getCvar("timer_set_seconds").getIntValue();
			cvarManager->log("Timer started with " + to_string(timerTotal) + " seconds");
			timerTimeLeft = timerTotal;
			timerStartTime = timeNow();
			firstTime = false;
		}
		else
		{
			timerUnpauseTime = timeNow();
			timerTotalPause += timerUnpauseTime - timerPauseTime;
		}
		timerEnabled = true;
		timerDone = false;
		timerDisplayOn = true;
		this->gameWrapper->RegisterDrawable(std::bind(&TimerPlugin::onDrawTimer, this, std::placeholders::_1));
		this->runTimers();
		}, "Starts and resumes the timer", PERMISSION_ALL);

	cvarManager->registerNotifier("timer_pause", [this](std::vector<string> params) {
		timerEnabled = false;
		timerDone = false;
		timerPauseTime = timeNow();
		}, "Pauses the timer", PERMISSION_ALL);

	cvarManager->registerCvar("timer_set_hours", "0", "How many hours added to the timer.", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("timer_set_minutes", "0", "How many minutes added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerCvar("timer_set_seconds", "0", "How many seconds added to the timer.", true, true, 0, true, 60, true);
	cvarManager->registerNotifier("timer_refresh", [this](std::vector<string> params) { 
		timerTimeLeft = timerTotal; 
		timerEnabled = false;
		firstTime = true;
		timerTotalPause = 0;
		}, "Refreshes the timer", PERMISSION_ALL);
	
	// Looking Break

	cvarManager->registerCvar("lookBreak_enabled", "0", "Whether looking break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::lookBreakOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("lookBreak_set_hours", "0", "How many hours until alert for looking around is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("lookBreak_set_minutes", "0", "How many minutes until alert for looking around is called", true, true, 0, true, 60, true); // default 20
	cvarManager->registerCvar("lookBreak_set_seconds", "5", "How many hours until alert for looking around is called", true, true, 0, true, 60, true);


	// Standing Break

	cvarManager->registerCvar("standBreak_enabled", "0", "Whether standing break timer is enabled", true, true, 0, true, 1, true).addOnValueChanged(std::bind(&TimerPlugin::standBreakOnChangeValue, this, std::placeholders::_1, std::placeholders::_2));
	cvarManager->registerCvar("standBreak_set_hours", "0", "How many hours until alert for standing is called", true, true, 0, true, 1000, true);
	cvarManager->registerCvar("standBreak_set_minutes", "30", "How many minutes until alert for standing is called", true, true, 0, true, 60, true); // default 30
	cvarManager->registerCvar("standBreak_set_seconds", "0", "How many seconds until alert for standing is called", true, true, 0, true, 60, true);

	// Display
	
	displayX = std::make_shared<int>(500);
	displayY = std::make_shared<int>(200);
	cvarManager->registerCvar("timer_display_x", "500", "The x distance from the right border of the screen", true, true, 0.0f, true, 1920.0f, true).bindTo(displayX);
	cvarManager->registerCvar("timer_display_y", "200", "The y distance from the top of the screen", true, true, 0.0f, true, 1080.0f, true).bindTo(displayY);
	cvarManager->registerNotifier("timer_display_show", [this](std::vector<string> params) { this->displayEnabled = true; }, "Shows the display", PERMISSION_ALL);
	cvarManager->registerNotifier("timer_display_hide", [this](std::vector<string> params) { this->displayEnabled = false; }, "Hides the display", PERMISSION_ALL);

	// Other

	initialTime = timeNow();
}

void TimerPlugin::onUnload()
{
}

float TimerPlugin::timeNow()
{
	return clock() / (float)CLOCKS_PER_SEC;
}

bool TimerPlugin::gameEnded()
{
	return gameWrapper->GetOnlineGame().GetbMatchEnded();
}


void TimerPlugin::lookBreakOnChangeValue(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{
		lookBreakTotal = cvarManager->getCvar("lookBreak_set_hours").getIntValue() * 3600 + cvarManager->getCvar("lookBreak_set_minutes").getIntValue() * 60 + cvarManager->getCvar("lookBreak_set_seconds").getIntValue(); 
		cvarManager->log("Look break timer started with " + to_string(lookBreakTotal) + " seconds");
		lookBreakEnabled = true;
		lookBreakStartTime = timeNow();
		gameWrapper->RegisterDrawable(std::bind(&TimerPlugin::onDrawTimer, this, std::placeholders::_1));
		runTimers();
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		lookBreakEnabled = false;
		lookBreakDone = false;
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
		gameWrapper->RegisterDrawable(std::bind(&TimerPlugin::onDrawTimer, this, std::placeholders::_1));
		runTimers();
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		standBreakEnabled = false;
		standBreakDone = false;
	}
}

void TimerPlugin::runTimers()
{
	currentTime = timeNow();

	// tests every case
	if (timerEnabled || lookBreakEnabled || standBreakEnabled)
	{
		if (timerEnabled)
		{
			checkTimer();
		}
		if (lookBreakEnabled)
		{ 
			checkLookBreak();
		}
		if (standBreakEnabled)
		{ 
			checkStandBreak();
		}
		gameWrapper->SetTimeout([this](GameWrapper* gw) {
			this->runTimers();
			}, 1.0f);
	}
	else
	{
		return;
	}
}

void TimerPlugin::checkTimer()
{
	timerTimeElapsed = currentTime - timerStartTime - timerTotalPause;

	if (timerTimeElapsed < timerTotal) 
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
		timerDone = true;
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

	if (lookBreakTimeElapsed >= lookBreakTotal)
	{
		lookBreakStartTime = timeNow();
		lookBreakTimeElapsed = 0;
		lookBreakDone = true;
	}
	if (lookBreakDone)
	{
		if (!gameWrapper->IsInOnlineGame())
		{
			float timeElapsed = timeNow() - lookBreakStartTime;
			
			if (timeElapsed > 60)
			{
				
				lookBreakDone = false;
				return;
			}
		}
	}
}

	

void TimerPlugin::checkStandBreak()
{
	standBreakTimeElapsed = currentTime - standBreakStartTime;

	if (standBreakTimeElapsed >= standBreakTotal)
	{
		standBreakStartTime = timeNow();
		standBreakTimeElapsed = 0;
		standBreakDone = true;
	}
	if (standBreakDone)
	{
		if (!gameWrapper->IsInOnlineGame())
		{
			float timeElapsed = timeNow() - standBreakStartTime;

			if (timeElapsed > 60)
			{

				standBreakDone = false;
				return;
			}
		}
	}
}



void TimerPlugin::onDrawTimer(CanvasWrapper cw)
{
	if ((timerDisplayOn || lookBreakDone || standBreakDone) && displayEnabled)
	{
		if (!gameWrapper->IsInOnlineGame() || gameEnded())
		{
			Vector2 canvasSize = cw.GetSize();
			RenderOptions ro;
			ro.currentPosition = { canvasSize.X - *displayX, *displayY };
			ro.boxSize = { 250, 60 };

			cw.SetPosition(ro.currentPosition);
			cw.SetColor(0, 0, 0, 150);
			cw.FillBox(ro.boxSize);
			ro.currentPosition = { ro.currentPosition.X + 8, ro.currentPosition.Y + 5 };
			cw.SetPosition(ro.currentPosition);
			cw.SetColor(255, 255, 255, 255);

			int minutes = timerTimeLeft / 60;
			int hours = minutes / 60;
			string time_left = "Time left: " + to_string(hours) + " h " + to_string(minutes % 60) + " m " + to_string((int)timerTimeLeft % 60) + " s";

			if (lookBreakDone)
			{
				cw.DrawString("Look 20ft away for 20 seconds!");
				//cvarManager->log("Look Break display is running");
				ro.currentPosition.Y += ro.textSize;
				cw.SetPosition(ro.currentPosition);
			}

			if (standBreakDone)
			{
				cw.DrawString("Stand up for a bit!");
				ro.currentPosition.Y += ro.textSize;
				cw.SetPosition(ro.currentPosition);
			}

			if (timerDisplayOn)
			{
				flashDisplay(cw, (int)timerTimeLeft == 0, timerDone);

				cw.DrawString(time_left);
				ro.currentPosition.Y += ro.textSize;
				cw.SetPosition(ro.currentPosition);
			}
		}
		else
		{
			gameWrapper->UnregisterDrawables();
		}
	}
}

void TimerPlugin::flashDisplay(CanvasWrapper cw, bool cond1, bool cond2)
{
	if (cond1 && cond2)
	{
		float decimal = timeNow() - (int)timeNow();
		if (decimal < 0.5)
		{
			cw.SetColor(0, 0, 0, 0);
		}
		else
		{
			cw.SetColor(255, 255, 255, 255);
		}
	}
}

