#include <iostream>
#include <vector>
#include <string>
#include <map>

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
  int labor;
  double demand;
};

map < string, Materials > materialDatabase;
map < string, Commodity > commodityDatabase;

double materialBalancePlanning(const Materials & material, double demand) {
  double shortage = 0.0;
  double requiredAmount = demand * material.usage_rate;
  double availableAmount = material.inventory + material.production_capacity;
  if (availableAmount < requiredAmount) {
    shortage = requiredAmount - availableAmount;
  }
  return shortage;
}

int main() {
  materialDatabase["Material A"] = { "Material A", 50, 100, 0.5, 15 };
  materialDatabase["Material B"] = { "Material B", 40, 150, 0.6, 14 };
  materialDatabase["Material C"] = { "Material C", 60, 200, 0.7, 18 };

  vector < Materials > materials;
  materials.push_back(materialDatabase["Material A"]);
  materials.push_back(materialDatabase["Material B"]);

  commodityDatabase["Chair"] = { "Chair", materials, 13, 100 };

  materials.clear();
  materials.push_back(materialDatabase["Material A"]);
  materials.push_back(materialDatabase["Material C"]);

  commodityDatabase["Table"] = { "Table", materials, 16, 200 };

  double cost = 0;
  for (auto & c: commodityDatabase) {
    Commodity commodity = c.second;
    for (const auto & material: commodity.materials) {
      double shortage = materialBalancePlanning(material, commodity.demand);
      if (shortage > 0) {
        cout << "Shortage of " << material.name << " for commodity " << commodity.name << ": " << shortage << endl;
        cost += shortage * material.cost;
        cout << "You will need " << shortage * material.cost << " to fix this shortage" << endl;
      } else {
        cout << "There are no shortages of " << material.name << endl;
      }
    }
  }
  cout << "Total cost for plan: " << cost << endl;
  return 0;
}
