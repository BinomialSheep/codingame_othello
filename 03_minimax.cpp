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
minimax法
コドゲの150ms制約では深さ2しか読めず、あまりにもツラい
https://note.com/nyanyan_cubetech/n/n98c9a37a54fc?magazine_key=m54104c8d2f12
*/

const vector<vector<int>> CELL_EVALATIONS = {
    {30, -12, 0, -1, -1, 0, -12, 30},     {-12, -15, -3, -3, -3, -3, -15, -12},
    {0, -3, 0, -1, -1, 0, -3, 0},         {-1, -3, -1, -1, -1, -1, -3, -1},
    {-1, -3, -1, -1, -1, -1, -3, -1},     {0, -3, 0, -1, -1, 0, -3, 0},
    {-12, -15, -3, -3, -3, -3, -15, -12}, {30, -12, 0, -1, -1, 0, -12, 30}};

using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;

vector<int> dy = {-1, -1, -1, 0, 0, 1, 1, 1}, dx = {-1, 0, 1, -1, 1, -1, 0, 1};
const int BLACK = 0, WHITE = 1;

string actionToString(int action) {
  string ret;
  ret.push_back((char)('a' + action % 8));
  ret.push_back((char)('1' + action / 8));
  return ret;
}

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

class State {
 private:
 public:
  vector<string> board;
  int color;

  State(vector<string> argBoard, int argColor)
      : board(argBoard), color(argColor){};

  // ゲームが終了したか判定する
  bool isDone() const {
    /*処理*/
    return false;
  }

  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  void advance(const int action) {
    // そのマスをプレイヤーで埋める
    int sy = action / 8, sx = action % 8;
    char c = '0' + color;
    board[sy][sx] = c;
    // 周囲8方向にひっくり返す
    rep(k, 8) {
      bool ok = false;
      int y = sy, x = sx;
      while (true) {
        y += dy[k], x += dx[k];
        if (min(y, x) < 0 || max(y, x) == 8) break;
        if (board[y][x] == '.') break;
        if ((color == BLACK && board[y][x] == '0') ||
            (color == WHITE && board[y][x] == '1')) {
          // 隣接していなければひっくり返せる
          if (max(abs(y - sy), abs(x - sx)) > 1) ok = true;
          break;
        }
      }
      if (ok) {
        y = sy, x = sx;
        y += dy[k], x += dx[k];
        while (board[y][x] != c) {
          board[y][x] = c;
          y += dy[k], x += dx[k];
        }
      }
    }
    color ^= 1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<int> legalActions() const {
    vector<int> actions;

    rep(i, 8) rep(j, 8) {
      if (board[i][j] != '.') continue;
      bool ok = false;
      rep(k, 8) {
        int y = i, x = j;
        while (true) {
          y += dy[k], x += dx[k];
          if (min(y, x) < 0 || max(y, x) == 8) break;
          if (board[y][x] == '.') break;
          if ((color == BLACK && board[y][x] == '0') ||
              (color == WHITE && board[y][x] == '1')) {
            // 隣接していなければひっくり返せる
            if (max(abs(y - i), abs(x - j)) > 1) ok = true;
            break;
          }
        }
        if (ok) break;
      }
      if (ok) actions.push_back(i * 8 + j);
    }
    return actions;
  }

  // 勝敗情報を取得する
  WinningStatus getWinningStatus() const {
    int cntB, cntW;
    int cntE;
    rep(i, 8) rep(j, 8) {
      if (board[i][j] == '0')
        cntB++;
      else if (board[i][j] == '1')
        cntW++;
      else
        cntE++;
      if (cntB && cntW && cntE) return WinningStatus::NONE;
    }
    if (cntB == 0)
      return color == WHITE ? WinningStatus::LOSE : WinningStatus::WIN;
    if (cntW == 0) {
      return color == BLACK ? WinningStatus::LOSE : WinningStatus::WIN;
    }
    if (cntB + cntW == 64) {
      if (cntB > cntW)
        return color == BLACK ? WinningStatus::WIN : WinningStatus::LOSE;
      if (cntB < cntW)
        return color == WHITE ? WinningStatus::WIN : WinningStatus::LOSE;
      return WinningStatus::DRAW;
    }
    return WinningStatus::NONE;
  }

  // 現在のプレイヤー視点の盤面評価をする
  ScoreType getScore() const {
    /*(ScoreType)現在のプレイヤー視点の盤面評価値*/
    int ret = 0;
    rep(i, 8) rep(j, 8) {
      if (board[i][j] == '0') {
        if (color == BLACK) {
          ret += CELL_EVALATIONS[i][j];
        } else {
          ret -= CELL_EVALATIONS[i][j];
        }
      } else if (board[i][j] == '1') {
        if (color == WHITE) {
          ret += CELL_EVALATIONS[i][j];
        } else {
          ret += CELL_EVALATIONS[i][j];
        }
      }
    }
    return ret;
  }

  // // 現在のゲーム状況を文字列にする
  // string toString() const { return  /*(string)文字列化されたゲーム状況*/; }

  // for debug：オセロ記法で合法手を列挙する
  void printLegalActions() const {
    vector<int> res = legalActions();
    for (auto v : res) {
      cerr << (char)('a' + v % 8) << (char)('1' + v / 8) << " ";
    }
    cerr << endl;
  }
  void printBoard() const {
    rep(i, 8) cerr << board[i] << endl;
    cerr << "Score: " << getScore() << endl;
  }
};

namespace minimax {
// minimaxのためのスコア計算
ScoreType miniMaxScore(State& state, const int depth) {
  WinningStatus winningStatus = state.getWinningStatus();

  if (winningStatus == WinningStatus::WIN) return INF;
  if (winningStatus == WinningStatus::LOSE) return -INF;
  if (winningStatus == WinningStatus::DRAW) return 0;

  if (depth == 0) {
    return state.getScore();
  }
  auto legal_actions = state.legalActions();
  if (legal_actions.empty()) {
    state.color ^= 1;
    return -miniMaxScore(state, depth - 1);
  }

  ScoreType bestScore = -INF;
  for (const auto action : legal_actions) {
    State next_state = state;
    next_state.advance(action);
    ScoreType score = -miniMaxScore(next_state, depth - 1);
    chmax(bestScore, score);
  }
  return bestScore;
}
// 深さを指定してminimaxで行動を決定する
int miniMaxAction(const State& state, const int depth) {
  ScoreType best_action = -1;
  ScoreType best_score = -INF;
  for (const auto action : state.legalActions()) {
    State next_state = state;
    next_state.advance(action);
    ScoreType score = -miniMaxScore(next_state, depth);
    cerr << actionToString(action) << " " << score << endl;
    if (chmax(best_score, score)) best_action = action;
  }
  return (int)best_action;
}
}  // namespace minimax

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
    if (action_count == 0) continue;
    // vector<string> actions;
    rep(i, action_count) {
      string action;
      cin >> action;
      cin.ignore();
      // actions.push_back(action);
    }

    State state(board, id);
    // assert(actions.size() == state.legalActions().size());
    // if (actions.size() == 0) continue;

    int res = minimax::miniMaxAction(state, 2);

    cout << actionToString(res) << endl;  // a-h1-8
  }
}