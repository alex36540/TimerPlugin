#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"



class TimerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	double timeLeft;
	double initialTime;
	double startTime;
	double currentTime;

	double timerTotal;
	double lookTotal;
	double standTotal;
	
	bool timerEnabled = false;
	bool lookBreakEnabled = false;
	bool standBreakEnabled = false;

public:
	virtual void onLoad();
	virtual void onUnload();
	void timerOnChangeValue();
	void lookOnChangeValue(std::string oldValue, CVarWrapper cvar);
	void standOnChangeValue(std::string oldValue, CVarWrapper cvar);

	double timeNow();
	void checkTime();
	void runTimer();
};