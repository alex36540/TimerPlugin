#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"



class TimerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	double totalTime;
	double timeLeft;
	double initialTime;
	double currentTime;

	bool timerEnabled;
	bool lookBreakEnabled;
	bool standBreakEnabled;

public:
	virtual void onLoad();
	virtual void onUnload();
	void countDownTimer();
	void countDownLook();
	void countDownStand();

	void checkTime();
};