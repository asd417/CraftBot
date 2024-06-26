#pragma once
#include "tools/MapTools.h"
#include <BWAPI.h>
#include <array>

class CraftBot {
	
	MapTools m_mapTools;
	virtual struct BuildingQueueStruct {
	public:
		BWAPI::Unit builder;
		BWAPI::UnitType building;
		BWAPI::Position buildPosition;
		void(*onCompleteFunction)(CraftBot*);
	};

public:
	std::vector<BuildingQueueStruct> buildQueue;
	std::array<bool, 2> buildOrderCounter = { false, false };

	CraftBot() = default;

	// helper functions to get you started with bot programming and learn the API
	void sendIdleWorkersToMinerals();
	void sendIdleWorkersAtBaseToMinerals();
	void trainAdditionalWorkers();
	void buildAdditionalSupply();
	int  getWorkerCount();
	BWAPI::Position getCheesePos();

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