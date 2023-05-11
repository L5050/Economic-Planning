//Based on the BFS system under Che Guevara
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#define BASIC_NEEDS 1
#define ESSENTIAL_UTILITIES 2
#define STRATEGIC_INVESTMENTS_AND_INITIATIVES 3
#define EDUCATION_AND_HEALTH 4
#define CONSUMER_GOODS_AND_SERVICES 5
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
};

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
  double demand;
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
}

bool compareCommodity(const pair<string, Commodity>& a, const pair<string, Commodity>& b) {
  if (a.second.priority == b.second.priority)
    return a.second.demand > b.second.demand;
  return a.second.priority < b.second.priority;
}

int main() {
  materialDatabase["Material A"] = {
    "Material A",
    50,
    100,
    15
  };
  materialDatabase["Material B"] = {
    "Material B",
    40,
    150,
    14
  };
  materialDatabase["Material C"] = {
    "Material C",
    60,
    200,
    18
  };

  vector<Worker> chairWorkers = {
    {"Worker 1", 40, 0},
    {"Worker 2", 40, 0}
  };

  vector<Worker> breadWorkers = {
    {"Worker 3", 40, 0},
    {"Worker 4", 40, 0}
  };

  commodityDatabase["Chair"] = {
    "Chair",
    {"Material A", "Material B"},
    {{"Material A", 0.5}, {"Material B", 0.6}},
    13,
    1000,
    100,
    CONSUMER_GOODS_AND_SERVICES,
    chairWorkers,
  };

  commodityDatabase["Bread"] = {
    "Bread",
    {"Material A", "Material C"},
    {{"Material A", 0.5}, {"Material C", 0.7}},
    16,
    5000,
    201,
    BASIC_NEEDS,
    breadWorkers,
  };
  vector<pair<string, Commodity>> commodityVector(commodityDatabase.begin(), commodityDatabase.end());
  sort(commodityVector.begin(), commodityVector.end(), compareCommodity);

  double totalCost = 0;
  for (const auto& c : commodityVector) {
    Commodity& commodity = commodityDatabase[c.first];
    double commodityCost = 0;
    cout << "Commodity: " << commodity.name << endl;
    for (const auto& materialName : commodity.materialNames) {
      double usageRate = commodity.usageRates[materialName]; // Retrieve the specific usage rate for the material
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
  return 0;
}
