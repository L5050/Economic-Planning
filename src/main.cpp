//Based on the BFS system under Che Guevara and Cybersyn under Allende
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <nlohmann/json.hpp>

#define BASIC_NEEDS 1
#define ESSENTIAL_UTILITIES 2
#define EDUCATION_AND_HEALTH 3
#define CONSUMER_GOODS_AND_SERVICES 4
#define STRATEGIC_INVESTMENTS_AND_INITIATIVES 5
#define LUXURY_GOODS_AND_SERVICES 6
#define INFRASTRUCTURE_AND_DEVELOPMENT 7
#define RESEARCH_AND_INNOVATION 8
#define ENVIRONMENTAL_CONSERVATION 9
#define EMERGENCY_SERVICES_AND_DISASTER_MANAGEMENT 10

using namespace std;

struct Materials {
  string name;
  double inventory;
  double production_capacity;
  int cost;
}; //materials are a different struct but are still centrally controlled

struct Worker {
  string name;
  int hoursWorked;
  double wage;
};

struct Commodity {
  string name;
  vector<string> materialNames;
  map<string, double> usageRates;
  int laborRequired;
  int laborAvailable;
  double demand; //future demand specifically, should be calculated based on previous demand
  int priority;
  vector<Worker> workers;
};

void calculateWages(vector<Worker>& workers, int laborRequired, double demand) {
  int totalHoursWorked = 0;
  for (const auto& worker : workers) {
    totalHoursWorked += worker.hoursWorked;
  }
  double wagePerHour;
  double totalWageBudget = (double)laborRequired * demand;
  if (totalHoursWorked == 0) {wagePerHour = 0;} else {wagePerHour = totalWageBudget / totalHoursWorked;}

  for (auto& worker : workers) {
    worker.wage = wagePerHour * worker.hoursWorked;
  }
}

map<string, Materials> materialDatabase;
map<string, Commodity> commodityDatabase;

double materialBalancePlanning(const string& materialName, double demand, double usageRate) {
  Materials& material = materialDatabase[materialName];
  double shortage = 0.0;
  double requiredAmount = demand * usageRate;
  double availableAmount = material.inventory + material.production_capacity;
  if (availableAmount < requiredAmount) {
    shortage = requiredAmount - availableAmount;
  }
  return shortage;
}

double calculatePrice(const Commodity& commodity) {
  double totalCost = 0.0;
  for (const string& materialName : commodity.materialNames) {
    Materials& material = materialDatabase[materialName];
    double usageRate = commodity.usageRates.at(materialName);
    totalCost += usageRate * material.cost;
  }
  return totalCost + commodity.laborRequired;
} //material costs and wages are paid for by a central bank

bool compareCommodity(const pair<string, Commodity>& a, const pair<string, Commodity>& b) {
  if (a.second.priority == b.second.priority)
    return a.second.demand > b.second.demand;
  return a.second.priority < b.second.priority;
}

void loadData() {
  // Load the JSON files into nlohmann::json objects
  ifstream materialFile("materials.json");
  ifstream commodityFile("commodities.json");
  nlohmann::json materialJson, commodityJson;
  materialFile >> materialJson;
  commodityFile >> commodityJson;

  // Populate the materialDatabase from materials.json
  for (const auto& item : materialJson.items()) {
    Materials m;
    m.name = item.key();
    m.inventory = item.value()["inventory"];
    m.production_capacity = item.value()["production_capacity"];
    m.cost = item.value()["cost"];
    materialDatabase[m.name] = m;
  }

  // Populate the commodityDatabase from commodities.json
  for (const auto& item : commodityJson.items()) {
    Commodity c;
    c.name = item.value()["name"];
    c.materialNames = item.value()["materialNames"].get<vector<string>>();
    for (const auto& rate : item.value()["usageRates"].items()) {
      c.usageRates[rate.key()] = rate.value();
    }
    c.laborRequired = item.value()["laborRequired"];
    c.laborAvailable = item.value()["laborAvailable"];
    c.demand = item.value()["demand"];
    c.priority = item.value()["priority"];
    for (const auto& worker : item.value()["workers"]) {
      c.workers.push_back(Worker{worker["name"], worker["hoursWorked"], worker["wage"]});
    }
    commodityDatabase[c.name] = c;
  }
}

int main() {

  loadData();
    streambuf* oldCoutStreamBuf = cout.rdbuf();
    ofstream fileOut("out.txt");
    cout.rdbuf(fileOut.rdbuf());

  vector<pair<string, Commodity>> commodityVector(commodityDatabase.begin(), commodityDatabase.end());
  sort(commodityVector.begin(), commodityVector.end(), compareCommodity);

  double totalCost = 0;
  for (const auto& c : commodityVector) {
    Commodity& commodity = commodityDatabase[c.first];
    double commodityCost = 0;
    cout << "Commodity: " << commodity.name << endl;
    for (const auto& materialName : commodity.materialNames) {
      double usageRate = commodity.usageRates[materialName];
      double shortage = materialBalancePlanning(materialName, commodity.demand, usageRate);
      if (shortage > 0) {
        cout << " Shortage of " << materialName << ": " << shortage << endl;
        commodityCost += shortage * materialDatabase[materialName].cost;
        cout << " Cost to fix shortage: " << shortage * materialDatabase[materialName].cost << endl;
      }
      else {
        cout << " No shortage of " << materialName << endl;
      }
      double actualUsage = min(materialDatabase[materialName].inventory, commodity.demand * usageRate);
      materialDatabase[materialName].inventory -= actualUsage;
    }

    // Checking labor shortage
    double laborRequired = commodity.laborRequired * commodity.demand;
    if (commodity.laborAvailable < laborRequired) {
      cout << " Labor shortage for " << commodity.name << ". Required: " << laborRequired << ", Available: " << commodity.laborAvailable << endl;
    }

    cout << " Total cost for " << commodity.name << ": " << commodityCost << endl;
    commodityCost += commodity.laborRequired * commodity.demand;
    totalCost += commodityCost;
    double price = calculatePrice(commodity);
    cout << " Price for " << commodity.name << ": " << price << endl;

    // Calculate wages for this commodity
    calculateWages(commodity.workers, commodity.laborRequired, commodity.demand);
    for (const auto& worker : commodity.workers) {
      cout << " Wage for " << worker.name << ": " << worker.wage << endl;
    }
  }
  cout << "Total cost for all commodities: " << totalCost << endl;
  cout.rdbuf(oldCoutStreamBuf);
  return 0;
}
