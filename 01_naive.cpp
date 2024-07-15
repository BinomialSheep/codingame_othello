#include <bits/stdc++.h>
using namespace std;

/*
入出力確認
*/

int main() {
  int id;  // id of your player.
  cin >> id;
  cin.ignore();
  int board_size;
  cin >> board_size;
  cin.ignore();

  // game loop
  while (1) {
    for (int i = 0; i < board_size; i++) {
      string line;  // rows from top to bottom (viewer perspective).
      cin >> line;
      cin.ignore();
    }
    int action_count;  // number of legal actions for this turn.
    cin >> action_count;
    cin.ignore();
    vector<string> actions;
    for (int i = 0; i < action_count; i++) {
      string action;
      cin >> action;
      cin.ignore();
      actions.push_back(action);
      cerr << actions.back() << endl;
    }
    if (actions.size()) cout << actions[0] << endl;  // a-h1-8
  }
}