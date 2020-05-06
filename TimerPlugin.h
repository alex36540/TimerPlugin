#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"



class TimerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	double timeLeft;
public:
	virtual void onLoad();
	virtual void onUnload();
	void countDownTimer(std::string oldValue, CVarWrapper cvar);
};