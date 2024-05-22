#include "craftBot.hpp"
#include "tools/Tools.h"
#include "tools/MapTools.h"

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

void CraftBot::onFrame()
{
    m_mapTools.onFrame();

    sendIdleWorkersToMinerals();
    
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
        buildPylon();
        break;
    case 9:
        if(scout != nullptr) scout = Tools::BuildBuildingGetBuilder(BWAPI::UnitTypes::Enum::Protoss_Gateway);
    }

}
