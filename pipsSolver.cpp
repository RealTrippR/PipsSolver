#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <cctype>
#include <chrono>
#include <set>
#include <array>
using namespace std;

typedef vector<vector<tuple<char, int, string>>> Board; // 2d board (rule char, color ID, rule)
typedef vector<tuple<int,int,int>> Dominos; // list of domino values and a color
typedef vector<vector<pair<int,int>>> Solution; // 2d solution (cell value, domino color)
typedef unordered_map<int,pair<string,vector<pair<int,int>>>> RuleMap; // color ID -> (rule, positions)

std::array<std::array<int,3>,15> colors = {
    {{0,0,255},
    {220,20,147},
    {0,255,255},
    {255,165,0},
    {0,128,255},
    {34,139,34},
    {128,0,128},
    {255,0,255},
    {0,128,128},
    {210,105,30},
    {0,255,0},
    {255,255,0},
    {128,128,0},
    {255,105,180},
    {255,215,0}}
};

// Function to get the board rules from the user
// if success is set to false any usage of the
// returned board is invalid and may
// lead to undefined behavior.
Board setupBoard(bool* success){
    *success=true;
    // User inputs number 1-3
    string fileNum = "";
    do{
        cout << "Select a puzzle to solve (1-3): ";
        getline(cin, fileNum);
    }while(fileNum != "1" && fileNum != "2" && fileNum != "3");

    // Read in the puzzle board
    Board board;
    ifstream inputFile("puzzle" + fileNum + ".txt");
    string line;
    while(getline(inputFile, line)){
        vector<tuple<char, int, string>> row;
        for(char cell : line){
            if (isalpha(cell)) {
                cell = toupper(cell);
            }
            row.emplace_back(cell, -1, ".");  // Initialize with -1 color and empty rule
        }
        board.push_back(row);
    }
    inputFile.close();

    // Assign colors to board based on character
    unordered_map<char, int> letterToColor; // letter -> color ID
    int colorIndex = 0;
    for(auto& row : board){
        for(auto& cell : row){
            if (colorIndex>=colors.size()) {
                *success = false; // prevent out of bounds read
                return board;
            }
            if(isalpha(get<0>(cell)) && letterToColor.find(get<0>(cell)) == letterToColor.end()){
                letterToColor[get<0>(cell)] = colorIndex++;
            }

            if(isalpha(get<0>(cell))) get<1>(cell) = letterToColor[get<0>(cell)];
        }
    }

    // Prompt the user to input a rule for each color
    for(int i = 0; i < colorIndex; i++){
        string rule = "";
        string input = "";
        int value = -1;

        // Print the board
        for(const auto& row : board){
            for(const auto& cell : row){
                if(isalpha(get<0>(cell))){
                    int color = get<1>(cell);
                    cout << "\033[38;2;" << colors[color][0] << ";" << colors[color][1] << ";" << colors[color][2] << "m" << (char)(toupper(get<0>(cell))) << "\033[0m ";
                } else {
                     cout << get<0>(cell) << " ";
                }
            }
            cout << endl;
        }
        
        // Get type of rule
        cout << "\nEntering rule for ";
        cout << "\033[38;2;" << colors[i][0] << ";" << colors[i][1] << ";" << colors[i][2] << "m" << (char)('A'+i) << "\033[0m:";
        cout << "\n[1] Number\n[2] Equal\n[3] Not Equal\n[4] Greater Than\n[5] Less Than\n" << endl;
        do{
            cout << "Choose (1-5): ";
            getline(cin, input);
        }while(input != "1" && input != "2" && input != "3" && input != "4" && input != "5");

        // Get specifics of rule
        if(input == "1"){
            do{
                // Setup
                bool allDigits = true;
                value = -1;

                // Get value
                cout << "Input the value: ";
                getline(cin, input);
                for(int j = 0; j < input.length(); j++)
                    if(!isdigit(input[j])) allDigits = false;

                // Set value
                if(allDigits && input.length() < 10) value = stoi(input);
            }while(value < 0);

            rule = "#" + to_string(value);
        }else if(input == "2"){
            rule = "=";
        }else if(input == "3"){
            rule = "!";
        }else if(input == "4"){
            do{
                // Setup
                bool allDigits = true;
                value = -1;

                // Get value
                cout << "Input the value: ";
                getline(cin, input);
                for(int j = 0; j < input.length(); j++)
                    if(!isdigit(input[j])) allDigits = false;

                // Set value
                if(allDigits && input.length() < 10) value = stoi(input);
            }while(value < 0);

            rule = ">" + to_string(value);
        }else if(input == "5"){
            do{
                // Setup
                bool allDigits = true;
                value = -1;

                // Get value
                cout << "Input the value: ";
                getline(cin, input);
                for(int j = 0; j < input.length(); j++)
                    if(!isdigit(input[j])) allDigits = false;

                // Set value
                if(allDigits && input.length() < 10) value = stoi(input);
            }while(value < 0);

            rule = "<" + to_string(value);
        }
        
        for(auto& row : board){
            for(auto& cell : row){
                if(get<1>(cell) == i) get<2>(cell) = rule;
            }
        }
    }

    return board;
}

// Function to get the dominos from the user
Dominos getDominos(){
    Dominos dominos;
    string input = "";
    int numDoms = -1;

    // Get the number of dominos
    do{
        bool allDigits = true;
        cout << "Input the number of dominos: ";
        getline(cin, input);
        for(int j = 0; j < input.length(); j++)
            if(!isdigit(input[j])) allDigits = false;

        // Set value
        if(allDigits && input.length() < 10) numDoms = stoi(input);
    }while(numDoms < 0);

    // Get the dominos
    int dominoColor = 0;
    for(int i = 0; i < numDoms; i++){
        bool allDigits;
        do{
            allDigits = true;
            cout << "Input the values for domino " << i + 1 << " (i.e. 0 6): ";
            getline(cin, input);
            for(int j = 0; j < input.length(); j++)
                if(!isdigit(input[j]) && input[j] != ' ') allDigits = false;
            if(allDigits && input.length() == 3)
                dominos.emplace_back(input[0]-'0', input[2]-'0', dominoColor++);
        }while(!allDigits || input.length() != 3);
    }

    return dominos;
}

// Make sure rules aren't broken before placing a domino
bool checkRule(int r, int c, const Solution& solution, const string& rule, vector<pair<int,int>> positions){
    // Find how many spaces for this rule are still empty
    int availableRuleSpace = positions.size();
    int ruleSum = 0;
    for(const auto& pos : positions){
        if(solution[pos.first][pos.second].first != -1){
            availableRuleSpace--;
            ruleSum += solution[pos.first][pos.second].first;
        }
    }

    // Number
    if(rule[0] == '#'){
        int ruleTarget = stoi(rule.substr(1));
        if(availableRuleSpace > 0) return ruleSum <= ruleTarget;
        else return ruleSum == ruleTarget;
    
    // Equal
    }else if(rule[0] == '='){
        for(const auto& pos : positions)
            if(solution[pos.first][pos.second].first != -1)
                if(solution[r][c].first != solution[pos.first][pos.second].first)
                    return false;

    // Not Equal
    }else if(rule[0] == '!'){
        for(const auto& pos : positions)
            if(!(pos.first == r && pos.second == c))
                if(solution[r][c].first == solution[pos.first][pos.second].first)
                    return false;

    // Greater Than
    }else if(rule[0] == '>'){
        int ruleTarget = stoi(rule.substr(1));
        if(availableRuleSpace == 0) return ruleSum > ruleTarget;
        
    // Less than
    }else if(rule[0] == '<'){
        int ruleTarget = stoi(rule.substr(1));
        return ruleSum < ruleTarget;
    }

    // If reached, no problem with rule
    return true;
}

// Helper to serialize a solution board
string serializeSolution(const Solution& solution) {
    string s;
    for (const auto& row : solution) {
        for (const auto& cell : row) {
            s += to_string(cell.first) + "," + to_string(cell.second) + ";";
        }
        s += "|";
    }
    return s;
}

void solvePuzzle(const Board& board, RuleMap& ruleMap, Solution solution, Dominos dominos, int& numberOfSolution, set<string>& uniqueSolutions){
    // Check if all dominos have been placed (solution found!)
    if(dominos.empty()){
        string serialized = serializeSolution(solution);
        if (uniqueSolutions.find(serialized) == uniqueSolutions.end()) {
            uniqueSolutions.insert(serialized);
            numberOfSolution++;
            cout << "\n=== SOLUTION " << numberOfSolution << " FOUND! ===\n";
            // Print the solved board with domino values
            for(int r = 0; r < board.size(); r++){
                for(int c = 0; c < board[r].size(); c++){
                    if(get<0>(board[r][c]) != '.'){
                        int value = solution[r][c].first;
                        int color = solution[r][c].second;
                        cout << "\033[38;2;" << colors[color][0] << ";" << colors[color][1] << ";" << colors[color][2] << "m" << value << "\033[0m ";
                    }else {
                        cout << get<0>(board[r][c]) << " ";
                    }
                }
                cout << endl;
            }
            cout << "=========================\n";
        }
        return;
    }

    bool checkFirst, checkSecond;
    for(int k = 0; k < dominos.size(); k++){
        for(int r = 0; r < board.size(); r++){
            for(int c = 0; c < board[r].size(); c++){
                if(get<0>(board[r][c]) != '.' && solution[r][c].first == -1){
                    // Place domino going up
                    if(r > 0 && get<0>(board[r-1][c]) != '.' && solution[r-1][c].first == -1){
                        solution[r][c] = {get<0>(dominos[k]), get<2>(dominos[k])};
                        solution[r-1][c] = {get<1>(dominos[k]), get<2>(dominos[k])};
                        checkFirst = checkRule(r, c, solution, get<2>(board[r][c]), (ruleMap[get<1>(board[r][c])]).second);
                        checkSecond = checkRule(r-1, c, solution, get<2>(board[r-1][c]), (ruleMap[get<1>(board[r-1][c])]).second);
                        if(checkFirst && checkSecond){
                            tuple<int,int,int> tempDomino = dominos[k];
                            dominos.erase(dominos.begin() + k);
                            solvePuzzle(board, ruleMap, solution, dominos, numberOfSolution, uniqueSolutions);
                            dominos.insert(dominos.begin() + k, tempDomino);
                        }
                        solution[r][c] = {-1, -1};
                        solution[r-1][c] = {-1, -1};
                    }
                    // Place domino going right
                    if(c < board[r].size() - 1 && get<0>(board[r][c+1]) != '.' && solution[r][c+1].first == -1){
                        solution[r][c] = {get<0>(dominos[k]), get<2>(dominos[k])};
                        solution[r][c+1] = {get<1>(dominos[k]), get<2>(dominos[k])};
                        checkFirst = checkRule(r, c, solution, get<2>(board[r][c]), (ruleMap[get<1>(board[r][c])]).second);
                        checkSecond = checkRule(r, c+1, solution, get<2>(board[r][c+1]), (ruleMap[get<1>(board[r][c+1])]).second);
                        if(checkFirst && checkSecond){
                            tuple<int,int,int> tempDomino = dominos[k];
                            dominos.erase(dominos.begin() + k);
                            solvePuzzle(board, ruleMap, solution, dominos, numberOfSolution, uniqueSolutions);
                            dominos.insert(dominos.begin() + k, tempDomino);
                        }
                        solution[r][c] = {-1, -1};
                        solution[r][c+1] = {-1, -1};
                    }
                    // Place domino going down
                    if(r < board.size() - 1 && get<0>(board[r+1][c]) != '.' && solution[r+1][c].first == -1){
                        solution[r][c] = {get<0>(dominos[k]), get<2>(dominos[k])};
                        solution[r+1][c] = {get<1>(dominos[k]), get<2>(dominos[k])};
                        checkFirst = checkRule(r, c, solution, get<2>(board[r][c]), (ruleMap[get<1>(board[r][c])]).second);
                        checkSecond = checkRule(r+1, c, solution, get<2>(board[r+1][c]), (ruleMap[get<1>(board[r+1][c])]).second);
                        if(checkFirst && checkSecond){
                            tuple<int,int,int> tempDomino = dominos[k];
                            dominos.erase(dominos.begin() + k);
                            solvePuzzle(board, ruleMap, solution, dominos, numberOfSolution, uniqueSolutions);
                            dominos.insert(dominos.begin() + k, tempDomino);
                        }
                        solution[r][c] = {-1, -1};
                        solution[r+1][c] = {-1, -1};
                    }
                    // Place domino going left
                    if(c > 0 && get<0>(board[r][c-1]) != '.' && solution[r][c-1].first == -1){
                        solution[r][c] = {get<0>(dominos[k]), get<2>(dominos[k])};
                        solution[r][c-1] = {get<1>(dominos[k]), get<2>(dominos[k])};
                        checkFirst = checkRule(r, c, solution, get<2>(board[r][c]), (ruleMap[get<1>(board[r][c])]).second);
                        checkSecond = checkRule(r, c-1, solution, get<2>(board[r][c-1]), (ruleMap[get<1>(board[r][c-1])]).second);
                        if(checkFirst && checkSecond){
                            tuple<int,int,int> tempDomino = dominos[k];
                            dominos.erase(dominos.begin() + k);
                            solvePuzzle(board, ruleMap, solution, dominos, numberOfSolution, uniqueSolutions);
                            dominos.insert(dominos.begin() + k, tempDomino);
                        }
                        solution[r][c] = {-1, -1};
                        solution[r][c-1] = {-1, -1};
                    }
                }
            }
        }
        return;
    }
}

int main(){    
    bool board_setup_result;
    Board board = setupBoard(&board_setup_result);
    if (!board_setup_result) {
        std::cout << "Failed to setup board.";
        return -1;
    }
    Dominos dominos = getDominos();

    //print board rules
    for(const auto& row : board){
        for(const auto& cell : row){
            if(isalpha(get<0>(cell))){
                int color = get<1>(cell);
                cout << "\033[38;2;" << colors[color][0] << ";" << colors[color][1] << ";" << colors[color][2] << "m" << get<2>(cell) << "\033[0m ";
            }else cout << get<0>(cell) << " ";
        }
        cout << endl;
    }

    //print dominos
    cout << "\nDominos" << endl;
    for(int i = 0; i < dominos.size(); i++){
        cout << "\033[38;2;" << colors[i][0] << ";" << colors[i][1] << ";" << colors[i][2] << "m" << get<0>(dominos[i]) << " " << get<1>(dominos[i]) << "\033[0m " << endl;
    }

    // Solve Setup
    Solution solution(board.size(), vector<pair<int,int>>(board[0].size(), {-1, -1}));
    RuleMap ruleMap;
    int numOfSolutions = 0;

    // Initialize each rule's entry in map
    for(const auto& row : board){
        for(const auto& cell : row){
            int color = get<1>(cell);
            if(color >= 0 && ruleMap.find(color) == ruleMap.end()){
                const string& rule = get<2>(cell);
                vector<pair<int,int>> positions;
                ruleMap[color] = make_pair(rule, positions);
            }
        }
    }

    // Assign the positions to their respective rule
    for(int r = 0; r < board.size(); r++){
        for(int c = 0; c < board[r].size(); c++){
            int color = get<1>(board[r][c]);
            if(ruleMap.find(color) != ruleMap.end()){
                (ruleMap[color].second).emplace_back(r, c);
            }
        }
    }

    // Recursively try each domino positioning
    auto start = std::chrono::high_resolution_clock::now();
    set<string> uniqueSolutions;
    solvePuzzle(board, ruleMap, solution, dominos, numOfSolutions, uniqueSolutions);

    // Print the number of solutions found
    cout << "\nTotal Solutions Found: " << numOfSolutions << endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    cout << "Time taken: " << duration.count() / 1000.0 << " seconds" << endl;

    if(numOfSolutions == 0) {
        cout << "\nNo solutions found!" << endl;
    }

    return 0;
}