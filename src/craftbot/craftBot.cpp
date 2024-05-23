#include "craftBot.hpp"
#include "tools/Tools.h"
#include "tools/MapTools.h"

#include <tuple>

// Send our idle workers to mine minerals so they don't just stand there
void CraftBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
void CraftBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void CraftBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

int CraftBot::getWorkerCount()
{
    return BWAPI::Broodwar->self()->completedUnitCount(BWAPI::UnitTypes::Enum::Protoss_Probe);
}

//Distance squared
int distanceSq(int x1, int y1, int x2, int y2) {
    return (x2 - x1) ^ 2 + (y2 - y1) ^ 2;
}

//We are using (4)Python.scx
// cheese spots: 
// south: 2192, 3491
// north: 1573 557
// west: 908 1866
// east: 3001 1856
// Out of these positions, return the furthest one 
BWAPI::Position CraftBot::getCheesePos()
{
    BWAPI::TilePosition basePos = BWAPI::Broodwar->self()->getStartLocation();
    BWAPI::Position pixelPos = {basePos.x * 32, basePos.y * 32};
    int maxDist = 0;
    int maxDistPos = 0;
    std::array<BWAPI::Position, 4> cheeseSpots = {
        BWAPI::Position(2192,3491),
        {1573, 557},
        {908, 1866},
        {3001, 1856}
    };
    for (int i = 0; i < 4;i++) {
        int newDist = distanceSq(cheeseSpots[i].x, cheeseSpots[i].y, pixelPos.x, pixelPos.y);
        if (newDist > maxDist) {
            maxDist = newDist;
            maxDistPos = i;
        }
    }
    
    return cheeseSpots[maxDistPos];
}

// Draw some relevent information to the screen to help us debug the bot
void CraftBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
    Tools::DrawUnitHealthBars();
}

bool CraftBot::buildPylon()
{
    return Tools::BuildBuilding(BWAPI::UnitTypes::Enum::Protoss_Pylon);
}

bool CraftBot::buildGateWay()
{
    return Tools::BuildBuilding(BWAPI::UnitTypes::Enum::Protoss_Gateway);
}



void CraftBot::onStart()
{
    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
    sendIdleWorkersToMinerals();
}

void CraftBot::onFrame()
{
    m_mapTools.onFrame();
    int height = m_mapTools.height();
    int width = m_mapTools.width();
    
    
    if (scout != nullptr) {
        BWAPI::Position pos = scout->getPosition();
        std::cout << "Map Size " + std::to_string(width) + " " + std::to_string(height) + " Scouting Probe position at " + std::to_string(pos.x) + " " + std::to_string(pos.y) + "\n";
    }
    //map tile position is (unitPosition / 32)

    int workers = getWorkerCount();
    if (workers < 8) {
        trainAdditionalWorkers();
        // Draw some relevent information to the screen to help us debug the bot
        drawDebugInformation();
        return;
    }
    

    switch (workers)
    {
    case 8:
        if (!buildOrderCounter[0]) {
            buildOrderCounter[0] = buildPylon();
            std::cout << "Pylon Built: " + std::to_string(buildOrderCounter[0]) + "\n";
            trainAdditionalWorkers();
        }
        break;
    case 9:
        if (!buildOrderCounter[1]) {
            scout = Tools::BuildBuildingGetBuilder(BWAPI::UnitTypes::Enum::Protoss_Gateway);
            buildOrderCounter[1] = scout != nullptr;
            std::cout << "Built Gateway and set scouter probe\n";
            if (scout != nullptr) {
                BWAPI::Position pos = scout->getPosition();
                std::cout << "Map Height: " + std::to_string(height) + "\n";
                std::cout << "Map Width: " + std::to_string(width) + "\n";
                std::cout << "Scouting Probe position at " + std::to_string(pos.x) + " " + std::to_string(pos.y) + "\n";
                BWAPI::Position newpos = { width * 32 - pos.x, height * 32 - pos.y };
                std::cout << "Sending Scout to the opposite side of the map: " + std::to_string(width - pos.x) + " " + std::to_string(height - pos.y) + "\n";
                scout->attack(newpos);
            }
        }
        break;
    }
    drawDebugInformation();
}

void CraftBot::onEnd(bool isWinner)
{
}

void CraftBot::onUnitDestroy(BWAPI::Unit unit)
{
}

void CraftBot::onUnitMorph(BWAPI::Unit unit)
{
}

void CraftBot::onSendText(std::string text)
{
}

void CraftBot::onUnitCreate(BWAPI::Unit unit)
{
}

void CraftBot::onUnitComplete(BWAPI::Unit unit)
{
}

void CraftBot::onUnitShow(BWAPI::Unit unit)
{
}

void CraftBot::onUnitHide(BWAPI::Unit unit)
{
}

void CraftBot::onUnitRenegade(BWAPI::Unit unit)
{
}
