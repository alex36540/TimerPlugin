#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"



class TimerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	double currentTime;
	double initialTime;
	
	double timerTimeLeft;
	
	bool timerEnabled = false;
	bool lookBreakEnabled = false;
	bool standBreakEnabled = false;

	double timerTimeElapsed;
	double lookBreakTimeElapsed;
	double standBreakTimeElapsed;

	double timerStartTime;
	double lookBreakStartTime;
	double standBreakStartTime;

	double timerTotal;
	double lookBreakTotal;
	double standBreakTotal;

public:
	virtual void onLoad();
	virtual void onUnload();
	double timeNow();
	
	void timerOnChangeValue(std::string oldValue, CVarWrapper cvar);
	void lookBreakOnChangeValue(std::string oldValue, CVarWrapper cvar);
	void standBreakOnChangeValue(std::string oldValue, CVarWrapper cvar);

	void runTimers();
	void checkTimer();
	void checkLookBreak();
	void checkStandBreak();
};