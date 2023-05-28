#include <iostream>
#include <fstream> 
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <limits.h>
#include <cmath>
#include <set>


using namespace std::chrono;
using namespace std;

const int SEED = 0;

int CELLS_NUM;
int CONNECT_NUM;
int ROWS;
int COLS;

long long TWL;

struct Net {
    multiset<int> rows, cols;
    int est_wire_len = 0;
};

bool hasCell(vector<vector<int>>& core, int row, int col) {
    return core[row][col] != -1;
}

void printCellsGrid(vector<vector<int>>& core, bool printID = true) {
    int space = to_string(CELLS_NUM).size(); // adjust spacing in the table according to number of cells
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (!hasCell(core, i, j)) {
                if (printID) {
                    cout << setfill('-');
                    cout << setw(space) << "" << ' ';
                } else {
                    cout << "1 ";
                }
            } else {
                if (printID) {
                    cout << setfill('0');
                    cout << setw(space) << core[i][j] << ' ';
                } else {
                    cout << "0 ";
                }
            }
        }
        cout << endl;
    }
    cout << endl;
}

void updateNetsConnectedToCell(int compt_idx, int row, int col, vector<vector<int>>& compnt_nets, vector<Net>& nets_data) {
    for (int j = 0; j < compnt_nets[compt_idx].size(); j++) {
        int net_idx = compnt_nets[compt_idx][j];

        int curr_est_wire_len = nets_data[net_idx].est_wire_len;

        nets_data[net_idx].rows.insert(row);
        nets_data[net_idx].cols.insert(col);

        int new_min_row = *nets_data[net_idx].rows.begin();
        int new_min_col = *nets_data[net_idx].cols.begin();
        int new_max_row = *nets_data[net_idx].rows.rbegin();
        int new_max_col = *nets_data[net_idx].cols.rbegin();

        int new_est_wire_len = new_max_row - new_min_row + new_max_col - new_min_col;

        nets_data[net_idx].est_wire_len = new_est_wire_len;

        TWL += new_est_wire_len - curr_est_wire_len;
    }
}

void readConnections(ifstream& test, vector<vector<int>>& compnt_nets) {
    for (int i = 0; i <  CONNECT_NUM; i++) {
        int comps_num;
        test >> comps_num;
        for (int j = 0; j < comps_num; j++) {
            int comp;
            test >> comp;
            compnt_nets[comp].push_back(i);
        }
    }
}

void findValidPlace(vector<vector<int>>& core, int& row, int& col) {
    do {
        row = rand() % ROWS;
        col = rand() % COLS;
    } while (hasCell(core, row, col));
}

void randomPlacer(vector<vector<int>>& core, vector<vector<int>>& compnt_nets, vector<Net>& nets_data) {
    
    for (int i = 0; i < CELLS_NUM; i++) {
        
        int row, col;
        int compt_idx = i;

        findValidPlace(core, row, col);

        core[row][col] = compt_idx;

        updateNetsConnectedToCell(compt_idx, row, col, compnt_nets, nets_data);
    }
}

int swapCells(int row1, int col1, int row2, int col2, vector<vector<int>>& core, vector<vector<int>>& compnt_nets, vector<Net>& nets_data) {
    int change = 0;

    // remove both cells from their nets
    int cell1 = core[row1][col1], cell2 = core[row2][col2];
    if (hasCell(core, row1, col1)){
        for (int j = 0; j < compnt_nets[cell1].size(); j++) {
            int net_idx = compnt_nets[cell1][j];

            int curr_est_wire_len = nets_data[net_idx].est_wire_len;

            // remove old coordinates
            nets_data[net_idx].rows.erase(nets_data[net_idx].rows.find(row1));
            nets_data[net_idx].cols.erase(nets_data[net_idx].cols.find(col1));

            // add new coordinates
            nets_data[net_idx].rows.insert(row2);
            nets_data[net_idx].cols.insert(col2);

            int new_min_row = *nets_data[net_idx].rows.begin();
            int new_min_col = *nets_data[net_idx].cols.begin();
            int new_max_row = *nets_data[net_idx].rows.rbegin();
            int new_max_col = *nets_data[net_idx].cols.rbegin();
            int new_est_wire_len = new_max_row - new_min_row + new_max_col - new_min_col;

            nets_data[net_idx].est_wire_len = new_est_wire_len;

            change += new_est_wire_len - curr_est_wire_len;
        }
    }
    if (hasCell(core, row2, col2)){
        for (int j = 0; j < compnt_nets[cell2].size(); j++) {
            int net_idx = compnt_nets[cell2][j];

            int curr_est_wire_len = nets_data[net_idx].est_wire_len;

            // remove old coordinates
            nets_data[net_idx].rows.erase(nets_data[net_idx].rows.find(row2));
            nets_data[net_idx].cols.erase(nets_data[net_idx].cols.find(col2));

            // add new coordinates
            nets_data[net_idx].rows.insert(row1);
            nets_data[net_idx].cols.insert(col1);

            int new_min_row = *nets_data[net_idx].rows.begin();
            int new_min_col = *nets_data[net_idx].cols.begin();
            int new_max_row = *nets_data[net_idx].rows.rbegin();
            int new_max_col = *nets_data[net_idx].cols.rbegin();
            int new_est_wire_len = new_max_row - new_min_row + new_max_col - new_min_col;

            nets_data[net_idx].est_wire_len = new_est_wire_len;

            change += new_est_wire_len - curr_est_wire_len;
        }
    }

    return change;
}

void placer (ifstream& test, double coolingRate) {
    srand(SEED);

    test >> CELLS_NUM >> CONNECT_NUM >> ROWS >> COLS;
    cout << "Cells number: " << CELLS_NUM << ", Nets number: " << CONNECT_NUM << ", Rows: " << ROWS << ", Cols: " << COLS << endl;

    // Data structures
    vector<Net> nets_data(CONNECT_NUM);
    vector<vector<int>> compnt_nets(CELLS_NUM);                 // map each component to the nets connected to it
    vector<vector<int>> core(ROWS, vector<int>(COLS, -1));      // grid represnting a core

    readConnections(test, compnt_nets);

    TWL = 0;                                                    // init total wire length with 0

    
    randomPlacer(core, compnt_nets, nets_data);
    

    long long init_TWL = TWL;
    cout << "Initial placement: " << endl << endl;
    printCellsGrid(core, true);
    printCellsGrid(core, false);
    cout << "Initial total wire length: " << TWL << endl;

    double initial_temp = 500.0 * init_TWL;
    double final_temp = 5e-6 * init_TWL / CONNECT_NUM;
    double curr_temp = initial_temp;

    cout << "Placing Cells ..." << endl;

    while (curr_temp > final_temp) {
        int moves = 10 * CELLS_NUM;
        for (int i = 0; i < moves; i++) {
            int row1, row2, col1, col2;
            do {
                row1 = rand() % ROWS;
                col1 = rand() % COLS;

                row2 = rand() % ROWS;
                col2 = rand() % COLS;
            } while ((row1 == row2 && col1 == col2) || (!hasCell(core, row1, col1) && !hasCell(core, row2, col2)));


            int change = swapCells(row1, col1, row2, col2, core, compnt_nets, nets_data);
            swap(core[row1][col1], core[row2][col2]);
            TWL += change;

            if (change >= 0) {
                // reject with prob (1 - e-ΔL/T)
                double rand_val = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
                double threshold = 1 - exp((double)-change/curr_temp);
                if (rand_val < threshold) {
                    change = swapCells(row1, col1, row2, col2, core, compnt_nets, nets_data);
                    swap(core[row1][col1], core[row2][col2]);
                    TWL += change;
                }
            }
        }
        curr_temp = coolingRate * curr_temp;
    }
    cout << "Final placement: " << endl;
    printCellsGrid(core, true);
    printCellsGrid(core, false);
    cout << "Total wire length: " << TWL << endl;
}


int main() {
    string filename;
    cout << "Enter test file name: ";
    cin >> filename;
    ifstream test;
    test.open(filename);
    if (!test){
        cout << "Invalid test name" << endl;
        exit(1);
    }

    cout << "Enter a cooling rate (from 0 to 1 exclusive): ";
    double coolingRate;
    cin >> coolingRate;
    if (coolingRate >= 1 || coolingRate <= 0) {
        cout << "Invalid cooling rate.\n";
        exit(1);
    }

    cout << "Test name: " << filename << endl;
    cout << "Cooling rate: " << coolingRate << endl;

    auto start = high_resolution_clock::now();
    
    placer(test, coolingRate);

    test.close();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by the program: " << duration.count() / 1e6 << " s." << endl;
} 