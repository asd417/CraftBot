#pragma once
#include "tools/MapTools.h"

#include <BWAPI.h>

class CraftBot {
	
	MapTools m_mapTools;

public:

	CraftBot() = default;

	// helper functions to get you started with bot programming and learn the API
	void sendIdleWorkersToMinerals();
	void trainAdditionalWorkers();
	void buildAdditionalSupply();
	int  getWorkerCount();

	void drawDebugInformation();

	bool buildPylon();
	bool buildGateWay();

	// functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
	
private:
	BWAPI::Unit scout = nullptr;
};