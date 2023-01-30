#include <iostream>
#include <vector>
#include <map>

using namespace std;

map<string, double> materialRequirements;
map<string, double> shortage;

void calculate(double demand, map<string, double> materials,
                              map<string, double> inventory,
                              map<string, double> productionCapacity) {
    for (auto material : materials) {
        double materialRequirement = demand * material.second;
        materialRequirements[material.first] = materialRequirement;
    }

    for (auto materialRequirement : materialRequirements) {
        double available_supply = inventory[materialRequirement.first] + productionCapacity[materialRequirement.first];
        if (available_supply < materialRequirement.second) {
            shortage[materialRequirement.first] = materialRequirement.second - available_supply;
        }
    }

    if (!shortage.empty()) {
        cout << "There is a shortage of the following materials:" << endl;
        for (auto shortfall : shortage) {
            cout << "-" << shortfall.first << ": " << shortfall.second << endl;
        }
    } else {
        cout << "There is enough supply to meet the demand." << endl;
    }
}

double exponentialSmoothing(const vector<double> &history, double alpha) {
    double forecast = history[0];
    for (int i = 1; i < history.size(); i++) {
        forecast = alpha * history[i] + (1 - alpha) * forecast;
    }
    return forecast;
}

int main() {
  //alpha being 1 gives more weight to close data while 0 gives more weight to historical data
    double alpha = 0.5;
    vector<double> history = {100, 120, 130, 140, 150};
    double demand = exponentialSmoothing(history, alpha);
    //measurments are in kilograms
    map<string, double> materials = {{"Material A", 0.5}, {"Material B", 0.3}, {"Material C", 0.2}};
    map<string, double> inventory = {{"Material A", 50}, {"Material B", 30}, {"Material C", 20}};
    map<string, double> productionCapacity = {{"Material A", 20}, {"Material B", 20}, {"Material C", 20}};

    calculate(demand, materials, inventory, productionCapacity);

    return 0;
}
