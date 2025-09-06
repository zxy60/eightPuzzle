#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
using namespace std;
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running;

public:
    Timer() : is_running(false) {}
    
    // 开始计时
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        is_running = true;
    }
    
    // 停止计时
    void stop() {
        if (is_running) {
            end_time = std::chrono::high_resolution_clock::now();
            is_running = false;
        }
    }
    
    // 获取经过的时间（毫秒）
    double elapsed_milliseconds() {
        if (is_running) {
            auto current_time = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(current_time - start_time).count();
        } else {
            return std::chrono::duration<double, std::milli>(end_time - start_time).count();
        }
    }
    
    // 获取经过的时间（秒）
    double elapsed_seconds() {
        return elapsed_milliseconds() / 1000.0;
    }
    
    // 获取经过的时间（微秒）
    double elapsed_microseconds() {
        if (is_running) {
            auto current_time = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::micro>(current_time - start_time).count();
        } else {
            return std::chrono::duration<double, std::micro>(end_time - start_time).count();
        }
    }
    
    // 重置计时器
    void reset() {
        is_running = false;
    }
};
// 目标状态
const string TARGET_STATE = "123804765";

// 规定目标状态各数字位置
const int TARGET_POS[8][2] = {
    {0, 0}, {0, 1}, {0, 2},
    {1, 2}, {2, 2}, {2, 1},
    {2, 1}, {1, 0}
};

// 移动方向：上、下、左、右
const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};
const string move_names[] = {"上", "下", "左", "右"};

// 棋盘状态类
class BoardState {
public:
    string state;    // 当前状态字符串
    int empty_pos;   // 空格位置(0-8)
    vector<string> path; // 移动路径
    int depth;      // 当前搜索深度
    int manhattan_cost;
    BoardState(string s, int pos, int d = 0, vector<string> p = {}) 
        : state(s), empty_pos(pos), depth(d) {
            path.push_back(state);
            calculateManhattanCost();
            }
        
    void calculateManhattanCost(){
        for(int i = 0; i < 9; i++) {
            if(state[i] != '0') {
                int target_x = TARGET_POS[static_cast<int>(state[i]-'1')][0];
                int target_y = TARGET_POS[static_cast<int>(state[i]-'1')][1];
                int current_x = i / 3;
                int current_y = i % 3;
                manhattan_cost += abs(target_x - current_x) + abs(target_y - current_y);
            }
        }
    }
    bool operator < (const BoardState &b) const{
            return manhattan_cost < b.manhattan_cost;
        }
        bool operator > (const BoardState &b) const{
            return manhattan_cost > b.manhattan_cost;
        }
        bool operator <= (const BoardState &b) const{
            return manhattan_cost <= b.manhattan_cost;
        }
        bool operator >= (const BoardState &b) const{
            return manhattan_cost >= b.manhattan_cost;
        }
        bool operator == (const BoardState &b) const{
            return manhattan_cost == b.manhattan_cost;
    }
    
};

// 检查状态是否为目标状态
bool isTargetState(const string& state) {
    return state == TARGET_STATE;
}

// 打印棋盘状态
void printState(const string& state) {
    for (int i = 0; i < 9; i++) {
        if (state[i] == '0') {
            cout << "  ";
        } else {
            cout << state[i] << " ";
        }
        
        if (i % 3 == 2) {
            cout << endl;
        }
    }
    cout << "-----" << endl;
}

// 贪心搜索函数
bool greedy(const string& initial_state, int empty_pos) {
    priority_queue<BoardState,vector<BoardState>,greater<BoardState>> s;
    set<string> visited;
    
    // 初始状态入队列
    s.push(BoardState(initial_state, empty_pos, 0));
    visited.insert(initial_state);
    
    
    while (!s.empty()) {
        BoardState current = s.top();
        s.pop();
        
        
        
        // 检查是否达到目标状态
        if (isTargetState(current.state)) {
            cout << "找到解决方案！共需要 " << current.depth << " 步" << endl;
            for(int i = 0; i < current.path.size(); i++) {
                cout << "步骤 " << i << ":" << endl;
                printState(current.path[i]);
            }
            return true;
        }
        
        int x = current.empty_pos / 3;
        int y = current.empty_pos % 3;
        
        // 尝试四个方向的移动
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx >= 0 && nx < 3 && ny >= 0 && ny < 3) {//检查空格移动有效性
                int new_empty_pos = nx * 3 + ny;
                string new_state = current.state;
                
                // 交换空格和相邻数字
                swap(new_state[current.empty_pos], new_state[new_empty_pos]);
                
                // 如果新状态未被访问过，则加入队列中
                if (visited.count(new_state) == 0) {
                    visited.insert(new_state);
                    BoardState new_board = current;
                    new_board.state = new_state;
                    new_board.empty_pos = new_empty_pos;
                    new_board.depth = current.depth + 1;
                    new_board.path.push_back(new_state);
                    new_board.calculateManhattanCost();
                    s.push(new_board);
                }
            }
        }
        
        
        
    }
    return false;
}

int main() {
    Timer timer;
    timer.start();
    // 初始状态
    //string initial_state = "283164705";
    string initial_state = "076543821";
    int empty_pos = 7; // 空格位置(0-8)
    
    // 执行贪心搜索
    greedy(initial_state, empty_pos);
    timer.stop();
        
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "运行时间: " << timer.elapsed_milliseconds() << " 毫秒" << std::endl;
    return 0;
}