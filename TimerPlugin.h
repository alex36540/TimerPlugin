#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"



class TimerPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	float currentTime;
	float initialTime;
	
	float timerTimeLeft;
	
	bool timerEnabled;
	bool lookBreakEnabled;
	bool standBreakEnabled;

	float timerTimeElapsed;
	float lookBreakTimeElapsed;
	float standBreakTimeElapsed;

	float timerStartTime;
	float lookBreakStartTime;
	float standBreakStartTime;

	float timerTotal;
	float lookBreakTotal;
	float standBreakTotal;

	std::shared_ptr<int> displayX;
	std::shared_ptr<int> displayY;
	bool timerDisplayOn;
	bool lookBreakDisplayOn;

public:
	virtual void onLoad();
	virtual void onUnload();
	float timeNow();
	
	void timerOnChangeValue(std::string oldValue, CVarWrapper cvar);
	void lookBreakOnChangeValue(std::string oldValue, CVarWrapper cvar);
	void standBreakOnChangeValue(std::string oldValue, CVarWrapper cvar);

	void runTimers();
	void checkTimer();
	void checkLookBreak();
	void checkStandBreak();

	void onDraw(CanvasWrapper cw);
};

struct RenderOptions
{
	Vector2 currentPosition = { 0,0 };
	Vector2 boxSize = { 0, 0 };
	float textSize = 14;
};