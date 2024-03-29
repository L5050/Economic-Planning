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
  float cost; // updated to float
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

void loadData() {
    ifstream materialFile("materials.json");
    ifstream commodityFile("commodities.json");

    // Check if files open successfully
    if (!materialFile.is_open() || !commodityFile.is_open()) {
        cerr << "Error opening files. Please ensure the 'materials.json' and 'commodities.json' files exist in the correct location." << endl;
        exit(EXIT_FAILURE);
    }

    nlohmann::json materialJson, commodityJson;

    try {
        materialFile >> materialJson;
        commodityFile >> commodityJson;
    } catch (nlohmann::json::parse_error &e) {
        cerr << "Parse error: " << e.what() << '\n';
        exit(EXIT_FAILURE);
    }

    for (const auto &item : materialJson.items()) {
        Materials m;
        try {
            m.name = item.key();
            m.inventory = item.value().at("inventory");
            m.production_capacity = item.value().at("production_capacity");
            m.cost = item.value().at("cost");
        } catch (nlohmann::json::out_of_range &e) {
            cerr << "Json key error in materials.json: " << e.what() << '\n';
            exit(EXIT_FAILURE);
        }
        materialDatabase[m.name] = m;
    }

    for (const auto &item : commodityJson.items()) {
        Commodity c;
        try {
            c.name = item.value().at("name");
            c.materialNames = item.value().at("materialNames").get<vector<string>>();
            for (const auto &rate : item.value().at("usageRates").items()) {
                c.usageRates[rate.key()] = rate.value();
            }
            c.laborRequired = item.value().at("laborRequired");
            c.laborAvailable = item.value().at("laborAvailable");
            c.demand = item.value().at("demand");
            c.priority = item.value().at("priority");
            for (const auto &worker : item.value().at("workers")) {
                c.workers.push_back(Worker{worker.at("name"), worker.at("hoursWorked"), worker.at("wage")});
            }
        } catch (nlohmann::json::out_of_range &e) {
            cerr << "Json key error in commodities.json: " << e.what() << '\n';
            exit(EXIT_FAILURE);
        }
        commodityDatabase[c.name] = c;
    }

    // Close files
    materialFile.close();
    commodityFile.close();
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

    double laborRequired = commodity.laborRequired * commodity.demand;
    if (commodity.laborAvailable < laborRequired) {
      cout << " Labor shortage for " << commodity.name << ". Required: " << laborRequired << ", Available: " << commodity.laborAvailable << endl;
    }

    commodityCost += commodity.laborRequired * commodity.demand;
    totalCost += commodityCost;
    cout << " Total cost for " << commodity.name << ": " << commodityCost << endl;
    double price = calculatePrice(commodity);
    cout << " Price for " << commodity.name << ": " << price << endl;

    calculateWages(commodity.workers, commodity.laborRequired, commodity.demand);
    for (const auto& worker : commodity.workers) {
      cout << " Wage for " << worker.name << ": " << worker.wage << endl;
    }
  }
  cout << "Total cost for all commodities: " << totalCost << endl;
  cout.rdbuf(oldCoutStreamBuf);
  return 0;
}
