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
ビッドボードでminmax
54/374位

ビットボード解説
https://speakerdeck.com/antenna_three/bitutobodojie-shuo
オセロをビットボードで実装する
https://qiita.com/sensuikan1973/items/459b3e11d91f3cb37e43
*/

const vector<vector<int>> CELL_EVALATIONS = {
    {30, -12, 0, -1, -1, 0, -12, 30},     {-12, -15, -3, -3, -3, -3, -15, -12},
    {0, -3, 0, -1, -1, 0, -3, 0},         {-1, -3, -1, -1, -1, -1, -3, -1},
    {-1, -3, -1, -1, -1, -1, -3, -1},     {0, -3, 0, -1, -1, 0, -3, 0},
    {-12, -15, -3, -3, -3, -3, -15, -12}, {30, -12, 0, -1, -1, 0, -12, 30}};
vector<vector<int>> cellBitEvaluations(8, vector<int>(256));

vector<uint64_t> maskVec = {0x00000000000000FFULL, 0x000000000000FF00ULL,
                            0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                            0x000000FF00000000ULL, 0x0000FF0000000000ULL,
                            0x00FF000000000000ULL, 0xFF00000000000000ULL};

class OthelloBoard {
 public:
  uint64_t black;  // 黒石の位置を表すビットボード
  uint64_t white;  // 白石の位置を表すビットボード

  // 各方向のマスク定義
  const uint64_t right_mask = 0x7f7f7f7f7f7f7f7fULL;
  const uint64_t left_mask = 0xfefefefefefefefeULL;
  const uint64_t up_down_mask = 0xffffffffffffff00ULL;
  const uint64_t diag_mask = 0x7e7e7e7e7e7e7e7eULL;

  OthelloBoard() {
    black = 0x0000000810000000ULL;
    white = 0x0000001008000000ULL;
  }
  OthelloBoard(vector<string>& sboard) { stringToBoard(sboard); }

  void stringToBoard(vector<string>& sboard) {
    black = 0, white = 0;
    rep(i, 8) rep(j, 8) {
      if (sboard[i][j] == '0') black |= 1ULL << (i * 8 + j);
      if (sboard[i][j] == '1') white |= 1ULL << (i * 8 + j);
    }
  }

  // 合法手を求める
  uint64_t findLegalMoves(uint64_t player, uint64_t opponent) {
    // 空きマス
    uint64_t empty = ~(player | opponent);
    // 着手可能な位置
    uint64_t legal_moves = 0;

    // 8方向すべてについてチェックする
    // 右方向
    uint64_t mask = opponent & diag_mask;
    uint64_t t = (player << 1) & mask;
    rep(i, 5) t |= (t << 1) & mask;
    legal_moves |= empty & (t << 1);
    // 左方向
    mask = opponent & diag_mask;
    t = (player >> 1) & mask;
    rep(i, 5) t |= (t >> 1) & mask;
    legal_moves |= empty & (t >> 1);
    // 上方向
    mask = opponent & up_down_mask;
    t = (player >> 8) & mask;
    rep(i, 5) t |= (t >> 8) & mask;
    legal_moves |= empty & (t >> 8);
    // 下方向
    mask = opponent & up_down_mask;
    t = (player << 8) & mask;
    rep(i, 5) t |= (t << 8) & mask;
    legal_moves |= empty & (t << 8);
    // 右上斜め
    mask = opponent & diag_mask;
    t = (player >> 7) & mask;
    rep(i, 5) t |= (t >> 7) & mask;
    legal_moves |= empty & (t >> 7);
    // 左上斜め
    mask = opponent & diag_mask;
    t = (player >> 9) & mask;
    rep(i, 5) t |= (t >> 9) & mask;
    legal_moves |= empty & (t >> 9);
    // 右下斜め
    mask = opponent & diag_mask;
    t = (player << 9) & mask;
    rep(i, 5) t |= (t << 9) & mask;
    legal_moves |= empty & (t << 9);
    // 左下斜め
    mask = opponent & diag_mask;
    t = (player << 7) & mask;
    rep(i, 5) t |= (t << 7) & mask;
    legal_moves |= empty & (t << 7);
    return legal_moves;
  }

  // 合法手数を数える
  int countLegalMoves(uint64_t player, uint64_t opponent) {
    return __builtin_popcountll(findLegalMoves(player, opponent));
  }

  // 合法手のindexを求める
  vector<int> findLegalMovesIdx(bool isBlack) {
    uint64_t& player = isBlack ? black : white;
    uint64_t& opponent = isBlack ? white : black;

    uint64_t moves = findLegalMoves(player, opponent);
    vector<int> ret;
    rep(i, 64) if (moves & (1LL << i)) ret.push_back(i);
    return ret;
  }

  // 石を置いた場合の反転箇所を求める
  uint64_t calculateFlips(uint64_t position, bool isBlack) {
    uint64_t& player = isBlack ? black : white;
    uint64_t& opponent = isBlack ? white : black;

    uint64_t rev = 0;
    for (int k = 0; k < 8; k++) {
      uint64_t rev_ = 0;
      uint64_t mask = transfer(position, k);
      while (mask != 0 && (mask & opponent) != 0) {
        rev_ |= mask;
        mask = transfer(mask, k);
      }
      if ((mask & player) != 0) {
        rev |= rev_;
      }
    }
    assert(rev != 0LL);
    return rev;
  }

  // 石を置く
  void placeStone(uint64_t position, uint64_t flips, bool isBlack) {
    uint64_t& player = isBlack ? black : white;
    uint64_t& opponent = isBlack ? white : black;
    player |= position | flips;
    opponent &= ~flips;
  }

  void revPlaceStone(uint64_t position, uint64_t flips, bool isBlack) {
    uint64_t& player = isBlack ? black : white;
    uint64_t& opponent = isBlack ? white : black;
    player &= ~(position | flips);
    opponent |= flips;
  }

  // 次に石を反転させる場所
  uint64_t transfer(uint64_t position, int k) {
    switch (k) {
      case 0:  // 上
        return (position << 8) & 0xffffffffffffff00ULL;
      case 1:  // 右上
        return (position << 7) & 0x7f7f7f7f7f7f7f00ULL;
      case 2:  // 右
        return (position >> 1) & 0x7f7f7f7f7f7f7f7fULL;
      case 3:  // 右下
        return (position >> 9) & 0x007f7f7f7f7f7f7fULL;
      case 4:  // 下
        return (position >> 8) & 0x00ffffffffffffffULL;
      case 5:  // 左下
        return (position >> 7) & 0x00fefefefefefefeULL;
      case 6:  // 左
        return (position << 1) & 0xfefefefefefefefeULL;
      case 7:  // 左上
        return (position << 9) & 0xfefefefefefefe00ULL;
      default:
        return 0;
    }
  }

  // ボード
  int evaluate(bool isBlack) {
    uint64_t& player = isBlack ? black : white;
    uint64_t& opponent = isBlack ? white : black;

    int ret = 0;
    rep(i, 8) {
      int tmp = (int)((player & maskVec[i]) >> (i * 8));
      ret += cellBitEvaluations[i][tmp];
      tmp = (int)((opponent & maskVec[i]) >> (i * 8));
      ret -= cellBitEvaluations[i][tmp];
    }
    return ret;
  }

  void printBoard() {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        uint64_t mask = 1ULL << (row * 8 + col);
        if (black & mask)
          cerr << "0";
        else if (white & mask)
          cerr << "1";
        else
          cerr << ".";
      }
      cerr << endl;
    }
  }

  void printBinary(uint64_t number) {
    cerr << "printBinary" << endl;
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        uint64_t mask = 1ULL << (row * 8 + col);
        if (number & mask) {
          cerr << "x";
        } else {
          cerr << ".";
        }
      }
      cerr << endl;
    }
  }
};

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
  OthelloBoard board;
  bool isBlack;

  State(OthelloBoard argBoard, bool argColor)
      : board(argBoard), isBlack(argColor){};

  // ゲームが終了したか判定する
  bool isDone() {
    /*処理*/
    return false;
  }

  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  void advance(const int action) {
    uint64_t position = 1ULL << action;
    uint64_t flips = board.calculateFlips(position, isBlack);
    board.placeStone(position, flips, isBlack);
    isBlack ^= 1;
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<int> legalActions() { return board.findLegalMovesIdx(isBlack); }

  // 勝敗情報を取得する
  WinningStatus getWinningStatus() {
    int cntB = __builtin_popcountll(board.black);
    int cntW = __builtin_popcountll(board.white);
    int cntE = 64 - cntB - cntW;
    if (cntB && cntW && cntE) return WinningStatus::NONE;
    if (cntB == 0) return !isBlack ? WinningStatus::LOSE : WinningStatus::WIN;
    if (cntW == 0) return isBlack ? WinningStatus::LOSE : WinningStatus::WIN;

    if (cntE == 0) {
      if (cntB > cntW)
        return isBlack ? WinningStatus::WIN : WinningStatus::LOSE;
      if (cntB < cntW)
        return !isBlack ? WinningStatus::WIN : WinningStatus::LOSE;
      return WinningStatus::DRAW;
    }
    return WinningStatus::NONE;
  }

  // 現在のプレイヤー視点の盤面評価をする
  ScoreType getScore() {
    /*(ScoreType)現在のプレイヤー視点の盤面評価値*/
    return board.evaluate(isBlack);
  }

  // // 現在のゲーム状況を文字列にする
  // string toString() const { return  /*(string)文字列化されたゲーム状況*/; }

  // for debug：オセロ記法で合法手を列挙する
  void printLegalActions() {
    vector<int> res = legalActions();
    for (auto v : res) {
      cerr << (char)('a' + v % 8) << (char)('1' + v / 8) << " ";
    }
    cerr << endl;
  }
  void printBoard() {
    board.printBoard();
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
    state.isBlack ^= 1;
    auto ret = -miniMaxScore(state, depth - 1);
    state.isBlack ^= 1;
    return ret;
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
int miniMaxAction(State& state, const int depth) {
  ScoreType best_action = 0;
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

void initCellBitEvaluations() {
  rep(i, 8) {
    rep(j, 256) {
      int v = 0;
      rep(k, 8) if ((j >> k) & 1) v += CELL_EVALATIONS[i][k];
      cellBitEvaluations[i][j] = v;
    }
  }
}

int main() {
  initCellBitEvaluations();

  int id;
  cin >> id;
  cin.ignore();
  int board_size;
  cin >> board_size;
  cin.ignore();
  bool isBlack = id == 0;

  vector<string> sboard(board_size);

  // game loop
  while (1) {
    for (int i = 0; i < board_size; i++) {
      cin >> sboard[i];
      cin.ignore();
    }

    int action_count;  // number of legal actions for this turn.
    cin >> action_count;
    cin.ignore();
    if (action_count == 0) continue;

    vector<string> actions;
    for (int i = 0; i < action_count; i++) {
      string action;
      cin >> action;
      cin.ignore();
      actions.push_back(action);
    }

    // ビッドボード形式で保持する
    OthelloBoard board(sboard);
    //
    State state(board, isBlack);

    int res = minimax::miniMaxAction(state, 5);

    cout << actionToString(res) << endl;
  }
}