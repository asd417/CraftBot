#include "craftBot.hpp"
#include "tools/Tools.h"
#include "tools/MapTools.h"

#include <tuple>

//Distance squared
int distanceSq(int x1, int y1, int x2, int y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

//Distance squared
int distanceSq(BWAPI::Position a, BWAPI::Position b) {
    return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

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

void CraftBot::sendIdleWorkersAtBaseToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    const BWAPI::TilePosition basePos = BWAPI::Broodwar->self()->getStartLocation();
    const BWAPI::Position basePosReal = { basePos.x * 32, basePos.y * 32 };
    for (auto& unit : myUnits)
    {
        int distToBase = distanceSq(unit->getPosition(), basePosReal);
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle() && distToBase < (256 * 32))
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


void buildOrder1(CraftBot* executor) {
    executor->buildOrderCounter[1] = true;
}

//We are using (2)Benzene.scx
// cheese spots: 
// south: 480 2176
// north: 3614 1376
// Out of these positions, return the furthest one 
BWAPI::Position CraftBot::getCheesePos()
{
    BWAPI::TilePosition basePos = BWAPI::Broodwar->self()->getStartLocation();
    std::cout << "base pos: " + std::to_string(basePos.x) + " " + std::to_string(basePos.y) + "\n";
    BWAPI::Position pixelPos = {basePos.x * 32, basePos.y * 32};
    int maxDist = 0;
    int maxDistPos = 0;
    std::array<BWAPI::Position, 2> cheeseSpots = {
        BWAPI::Position(480, 2176),
        {3614, 1376}
    };
    for (int i = 0; i < 2;i++) {
        int newDist = distanceSq(cheeseSpots[i].x, cheeseSpots[i].y, pixelPos.x, pixelPos.y);
        std::cout << newDist << "\t";
        if (newDist > maxDist) {
            maxDist = newDist;
            maxDistPos = i;
        }
        std::cout << "\n";
    }
    std::cout << "Furthest cheese spot is at ";
    switch (maxDistPos) {
    case 0:
        std::cout << "South";
        break;
    case 1:
        std::cout << "North";
        break;
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
    //TODO: How to get bot player interface??
    //map tile position is (unitPosition / 32)
    int minerals = BWAPI::Broodwar->getPlayer(1)->minerals();
    std::cout << "player has " + std::to_string(minerals) + " minerals\n";
    int workers = getWorkerCount();
    if (workers <= 6) {
        trainAdditionalWorkers();
        sendIdleWorkersToMinerals();
        // Draw some relevent information to the screen to help us debug the bot
        drawDebugInformation();
        return;
    }

    switch (workers)
    {
    case 7:
        if (!buildOrderCounter[0]) {
            sendIdleWorkersToMinerals();
            BWAPI::Position cheesePos = getCheesePos();
            scout = Tools::GetUnitOfType(BWAPI::UnitTypes::Enum::Protoss_Probe);
            //scout = Tools::BuildBuildingGetBuilder(BWAPI::UnitTypes::Enum::Protoss_Pylon, cheeser);
            buildOrderCounter[0] = scout != nullptr;
            if (scout != nullptr) {
                scout->move(cheesePos);
                std::cout << "Sending probe to enemy natural\n";
            }
        }
        trainAdditionalWorkers();
        break;
    case 8:
        if (!buildOrderCounter[1]) {
            BWAPI::Position cheesePos = getCheesePos();
            BWAPI::TilePosition cheeseTilePos = { cheesePos.x / 32, cheesePos.y / 32 };
            if (m_mapTools.isExplored(cheeseTilePos)) {
                std::cout << "Pylon Area not explored yet\n";
                
                //trainAdditionalWorkers();
                //buildOrderCounter[1] = true;
                buildQueue.push_back({ scout, BWAPI::UnitTypes::Enum::Protoss_Pylon, cheesePos, *buildOrder1});
                sendIdleWorkersAtBaseToMinerals();
            }
            std::cout << "Building Pylon at enemy natural\n";
        }
        break;
    }
        
    if (m_mapTools.isExplored(buildQueue[0].buildPosition)) {

        Tools::BuildBuilding(buildQueue[0].building, buildQueue[0].buildPosition, buildQueue[0].builder);
        void (*func)(CraftBot*) = buildQueue[0].onCompleteFunction;
        func(this);
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
