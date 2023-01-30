#include <iostream>
#include <vector>
#include <string>
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
  vector<Materials> materials;
  int labor;
  double demand;
};

double materialBalancePlanning(const Materials &material, double demand) {
  double shortage = 0.0;
  double requiredAmount = demand * material.usage_rate;
  double availableAmount = material.inventory + material.production_capacity;
  if (availableAmount < requiredAmount) {
    shortage = requiredAmount - availableAmount;
  }
  return shortage;
}

int main() {
  vector<Materials> materials = {
    {"Material A", 50, 100, 0.5, 15},
    {"Material B", 40, 150, 0.6, 14},
    {"Material C", 60, 200, 0.7, 18}
  };
  vector<Commodity> commodities = {
    {"Chair", materials, 13, 100},
    {"Table", materials, 16, 200}
  };
  float cost;
  for (int i = 0; i < commodities.size(); i++) {
  for (const auto &material : commodities[i].materials) {
    double shortage = materialBalancePlanning(material, commodities[i].demand);
    if (shortage > 0) {
      cout << "Shortage of " << material.name << " for commodity " << commodities[i].name << ": " << shortage << endl;
      cost += shortage * material.cost;
      cout << "You will need " << shortage * material.cost << " to fix this shortage" << endl;
    } else {
      cout << "There are no shortages of " << material.name << endl;
    }
  }}
  return 0;
}
