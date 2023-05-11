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

using namespace std;

struct Materials {
  string name;
  double inventory;
  double production_capacity;
  double usage_rate;
  int cost;
};

struct Commodity {
  string name;
  vector < Materials > materials;
  int laborRequired;
  int laborAvailable;
  double demand;
  int priority;
};

map < string, Materials > materialDatabase;
map < string, Commodity > commodityDatabase;

double materialBalancePlanning(const Materials & material, double demand) {
  double shortage = 0.0;
  double inventory = material.inventory;
  if (materialDatabase[material.name].inventory != inventory) inventory = materialDatabase[material.name].inventory;
  double requiredAmount = demand * material.usage_rate;
  double availableAmount = inventory + material.production_capacity;
  if (availableAmount < requiredAmount) {
    shortage = requiredAmount - availableAmount;
  }
  return shortage;
}

double calculatePrice(const Commodity & commodity) {
  double totalCost = 0.0;
  for (const Materials & material : commodity.materials) {
    totalCost += material.usage_rate * material.cost;
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
    0.5,
    15
  };
  materialDatabase["Material B"] = {
    "Material B",
    40,
    150,
    0.6,
    14
  };
  materialDatabase["Material C"] = {
    "Material C",
    60,
    200,
    0.7,
    18
  };

  vector < Materials > materials;
  materials.assign({
    materialDatabase["Material A"],
    materialDatabase["Material B"]
  });
  commodityDatabase["Chair"] = {
    "Chair",
    materials,
    13,
    1000,
    100,
    CONSUMER_GOODS_AND_SERVICES
  };

  materials.assign({
    materialDatabase["Material A"],
    materialDatabase["Material C"]
  });
  commodityDatabase["Bread"] = {
    "Bread",
    materials,
    16,
    5000,
    201,
    BASIC_NEEDS
  };

  vector<pair<string, Commodity>> commodityVector(commodityDatabase.begin(), commodityDatabase.end());
  sort(commodityVector.begin(), commodityVector.end(), compareCommodity);

  double totalCost = 0;
  for (const auto & c: commodityVector) {
    const Commodity commodity = c.second;
    double commodityCost = 0;
    cout << "Commodity: " << commodity.name << endl;
    for (const auto & material: commodity.materials) {
      double shortage = materialBalancePlanning(material, commodity.demand);
    if (shortage > 0) {
      cout << " Shortage of " << material.name << ": " << shortage << endl;
      commodityCost += shortage * material.cost;
      cout << " Cost to fix shortage: " << shortage * material.cost << endl;
    } else {
      cout << " No shortage of " << material.name << endl;
    }
    double actualUsage = min(materialDatabase[material.name].inventory, commodity.demand * material.usage_rate);
    materialDatabase[material.name].inventory -= actualUsage;
  }

  // Checking labor shortage
  double laborRequired = commodity.laborRequired * commodity.demand;
  if (commodity.laborAvailable < laborRequired) {
    cout << " Warning: Labor shortage for " << commodity.name << ". Required: " << laborRequired << ", Available: " << commodity.laborAvailable << endl;
  }

  cout << " Total cost for " << commodity.name << ": " << commodityCost << endl;
  commodityCost += commodity.laborRequired * commodity.demand;
  totalCost += commodityCost;
  double price = calculatePrice(commodity);
  cout << " Price for " << commodity.name << ": " << price << endl;
}
cout << "Total cost for all commodities: " << totalCost << endl;
  return 0;
}
