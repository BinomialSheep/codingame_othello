#include <bits/stdc++.h>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
template <typename T>
inline bool chmax(T& a, T b) {
  return ((a < b) ? (a = b, true) : (false));
}
template <typename T>
inline bool chmin(T& a, T b) {
  return ((a > b) ? (a = b, true) : (false));
}

/*
マスの評価関数のみを用いた1手読みAI
重みは以下を用いると48/373位。
https://note.com/nyanyan_cubetech/n/n17c169271832?magazine_key=m54104c8d2f12


*/
const vector<vector<int>> CELL_EVALATIONS = {
    {30, -12, 0, -1, -1, 0, -12, 30},     {-12, -15, -3, -3, -3, -3, -15, -12},
    {0, -3, 0, -1, -1, 0, -3, 0},         {-1, -3, -1, -1, -1, -1, -3, -1},
    {-1, -3, -1, -1, -1, -1, -3, -1},     {0, -3, 0, -1, -1, 0, -3, 0},
    {-12, -15, -3, -3, -3, -3, -15, -12}, {30, -12, 0, -1, -1, 0, -12, 30}};

int main() {
  int id;  // id of your player.
  cin >> id;
  cin.ignore();
  int board_size;
  cin >> board_size;
  cin.ignore();

  vector<string> board(board_size);

  // game loop
  while (1) {
    rep(i, board_size) {
      cin >> board[i];
      cin.ignore();
    }
    int action_count;  // number of legal actions for this turn.
    cin >> action_count;
    cin.ignore();
    vector<string> actions;
    rep(i, action_count) {
      string action;
      cin >> action;
      cin.ignore();
      actions.push_back(action);
    }

    if (actions.size() == 0) continue;

    // 最大
    int maxEvalValue = -1e9, idx = -1;
    rep(i, actions.size()) {
      int val = CELL_EVALATIONS[actions[i][1] - '1'][actions[i][0] - 'a'];
      cerr << actions[i] << " " << val << endl;
      if (chmax(maxEvalValue, val)) {
        idx = i;
      }
    }
    cout << actions[idx] << endl;  // a-h1-8
  }
}