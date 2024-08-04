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
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

std::random_device rnd;
std::mt19937 mt_for_action(0);

// 時間を管理するクラス
class TimeKeeper {
 private:
  std::chrono::high_resolution_clock::time_point start_time_;
  int64_t time_threshold_;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeper(const int64_t& time_threshold)
      : start_time_(std::chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const {
    auto diff = std::chrono::high_resolution_clock::now() - this->start_time_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(diff)
               .count() >= time_threshold_;
  }
};

/*
negascoutを実装したがどこかがバグっているので直す
*/

const vector<vector<int>> CELL_EVALATIONS = {
    {30, -12, 0, -1, -1, 0, -12, 30},     {-12, -15, -3, -3, -3, -3, -15, -12},
    {0, -3, 0, -1, -1, 0, -3, 0},         {-1, -3, -1, -1, -1, -1, -3, -1},
    {-1, -3, -1, -1, -1, -1, -3, -1},     {0, -3, 0, -1, -1, 0, -3, 0},
    {-12, -15, -3, -3, -3, -3, -15, -12}, {30, -12, 0, -1, -1, 0, -12, 30}};
vector<vector<int>> cellBitEvaluations(8, vector<int>(256));

// 位置だけで優先度を決める
const vector<int> POSITION_MOVE_ORDERING = {
    0, 6, 1, 2, 2, 1, 6, 0, 6, 7, 5, 5, 5, 5, 7, 6, 1, 5, 3, 4, 4, 3,
    5, 1, 2, 5, 4, 0, 0, 4, 5, 2, 2, 5, 4, 0, 0, 4, 5, 2, 1, 5, 3, 4,
    4, 3, 5, 1, 6, 7, 5, 5, 5, 5, 7, 6, 0, 6, 1, 2, 2, 1, 6, 0};

// 各行を取り出すマスクビッド
vector<uint64_t> maskVec = {0x00000000000000FFULL, 0x000000000000FF00ULL,
                            0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                            0x000000FF00000000ULL, 0x0000FF0000000000ULL,
                            0x00FF000000000000ULL, 0xFF00000000000000ULL};
// 4辺を取り出すマスクビッド
vector<uint64_t> maskVecFS = {
    0xFF00000000000000ULL,  // 上辺
    0x00000000000000FFULL,  // 下辺
    0x0101010101010101ULL,  // 右辺
    0x8080808080808080ULL   // 左辺
};

// 黒目線の確定石数
vector<vector<int>> cellBitFixedStone(256, vector<int>(256));
// 左辺、右辺用
map<pair<uint64_t, uint64_t>, int> cellBitFixedStoneLeftMap;
map<pair<uint64_t, uint64_t>, int> cellBitFixedStoneRightMap;

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
    if (rev == 0LL) {
      cerr << "不正操作：反転しない箇所に石を置こうとしている." << endl;
      printBoard();
      printBinary(position);
      cerr << isBlack << endl;
      return 0;
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

  // 盤位置による評価
  int evaluateBoardPosition(bool isBlack) {
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

  // 確定石による評価（自分の確定石数 - 相手の確定石数の近似値）
  int evaluateFixedStone(bool isBlack) {
    int ret = 0;
    // 上辺
    uint64_t tmpb = (black & maskVecFS[0]) >> 56;
    uint64_t tmpw = (white & maskVecFS[0]) >> 56;
    ret += cellBitFixedStone[tmpb][tmpw];
    // 下辺
    tmpb = black & maskVecFS[1];
    tmpw = white & maskVecFS[1];
    ret += cellBitFixedStone[tmpb][tmpw];
    // 右辺
    tmpb = black & maskVecFS[2];
    tmpw = white & maskVecFS[2];
    ret += cellBitFixedStoneRightMap[make_pair(tmpb, tmpw)];
    // 左辺
    tmpb = black & maskVecFS[3];
    tmpw = white & maskVecFS[3];
    ret += cellBitFixedStoneLeftMap[make_pair(tmpb, tmpw)];
    // 4隅を除外
    if (black & 1) ret--;
    if (white & 1) ret++;
    if (black & 128) ret--;
    if (white & 128) ret++;
    if (black & 0x0100000000000000ULL) ret--;
    if (white & 0x0100000000000000ULL) ret++;
    if (black & 0x8000000000000000ULL) ret--;
    if (white & 0x8000000000000000ULL) ret++;

    if (!isBlack) ret *= -1;
    return ret;
  }

  // 盤面評価
  int evaluate(bool isBlack) {
    int bp = evaluateBoardPosition(isBlack);
    int fs = evaluateFixedStone(isBlack);
    int cn = (int)findLegalMovesIdx(isBlack).size();
    return 10 * bp + 5 * fs + 10 * cn;
  }

  void printBoard() {
    for (int row = 7; row >= 0; row--) {
      for (int col = 7; col >= 0; col--) {
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
  uint64_t advance(const int action) {
    uint64_t position = 1ULL << action;
    uint64_t flips = board.calculateFlips(position, isBlack);
    if (flips == 0) {
      cerr << "異常" << endl;
      return 0;
    }
    board.placeStone(position, flips, isBlack);
    isBlack ^= 1;
    return flips;
  }

  // advanceした状態から戻す
  void retreat(const int action, const uint64_t flips) {
    isBlack ^= 1;
    uint64_t position = 1ULL << action;
    board.revPlaceStone(position, flips, isBlack);
  }

  // 現在のプレイヤーが可能な行動を全て取得する
  vector<int> legalActions() { return board.findLegalMovesIdx(isBlack); }

  // 勝敗情報を取得する
  WinningStatus getWinningStatus() {
    int cntB = __builtin_popcountll(board.black);
    int cntW = __builtin_popcountll(board.white);
    int cntE = 64 - cntB - cntW;
    if (cntB && cntW && cntE) return WinningStatus::NONE;
    if (cntB == 0) return isBlack ? WinningStatus::LOSE : WinningStatus::WIN;
    if (cntW == 0) return !isBlack ? WinningStatus::LOSE : WinningStatus::WIN;

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

  __uint128_t getKey() {
    return ((__uint128_t)board.black << 64) + board.white;
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
    // cerr << "Score: " << getScore() << endl;
  }
};

namespace iterativedeepening {
// ある盤面、手番における可能な着手と暫定スコア
vector<unordered_map<__uint128_t, vector<pair<int, int>>>> provisionalScores(2);
// 必勝フラグ
bool isVictory = false;
// 訪問ノード（性能評価用）
int visited_node = 0;

// 前回の探索で枝刈りされなかったノードへのボーナス
int cache_hit_bonus = 1000;
// 現在の探索結果を入れる置換表(上限) 同じ局面に当たった時用
vector<unordered_map<__uint128_t, ScoreType>> transpose_table_upper(2);
// 現在の探索結果を入れる置換表(下限):// 同じ局面に当たった時用
vector<unordered_map<__uint128_t, ScoreType>> transpose_table_lower(2);
// 前回の探索結果が入る置換表(上限): move orderingに使う
vector<unordered_map<__uint128_t, ScoreType>> former_transpose_table_upper(2);
// 前回の探索結果が入る置換表(下限): move orderingに使う
vector<unordered_map<__uint128_t, ScoreType>> former_transpose_table_lower(2);

// 指し手を有望度降順でソートする
vector<int> moveOrdering(vector<int>& legal_actions, State& state, int depth) {
  vector<int> ret;
  ret.reserve(legal_actions.size());

  // 盤面による優先度評価
  vector<vector<int>> bucket(8);
  for (auto&& v : legal_actions) {
    bucket[POSITION_MOVE_ORDERING[v]].push_back(v);
  }
  for (auto& vec : bucket)
    for (auto&& v : vec) ret.push_back(v);

  // // 浅い探索に基づく優先度評価
  // __uint128_t key = ((__uint128_t)state.board.black << 64) +
  // state.board.white; if (depth > 3) {
  //   vector<int> ret2;
  //   ret2.reserve(legal_actions.size());
  //   vector<pair<int, int>> score_acion =
  //   provisionalScores[!state.isBlack][key];

  //   unordered_set<int> st;

  //   sort(score_acion.rbegin(), score_acion.rend());

  //   for (auto [score, action] : score_acion) {
  //     ret2.push_back(action);
  //     st.insert(action);
  //   }

  //   for (auto action : legal_actions)
  //     if (!st.count(action)) ret2.push_back(action);

  //   swap(ret, ret2);
  // }

  // // 浅い探索に基づく優先度の削除
  // provisionalScores[!state.isBlack][key].clear();

  assert(legal_actions.size() == ret.size());
  return ret;
}

ScoreType nega_alpha_transpose(State& state, ScoreType alpha, ScoreType beta,
                               bool passed, const int depth,
                               const TimeKeeper& time_keeper) {
  visited_node++;
  // 葉ノードでは評価関数を実行する
  WinningStatus winningStatus = state.getWinningStatus();
  if (winningStatus == WinningStatus::WIN) return INF;
  if (winningStatus == WinningStatus::LOSE) return -INF;
  if (winningStatus == WinningStatus::DRAW) return 0;
  if (depth == 0) return state.getScore();

  __uint128_t key = state.getKey();

  // 置換表から上限値と下限値があれば取得
  ScoreType u = INF, l = -INF;
  if (transpose_table_upper[!state.isBlack].count(key))
    u = transpose_table_upper[!state.isBlack][key];
  if (transpose_table_lower[!state.isBlack].count(key))
    l = transpose_table_lower[!state.isBlack][key];
  // upper == lower、つまりもうminimax値が求まっていれば探索終了
  if (u == l) return u;

  // 置換表の値を使って探索窓を狭められる場合は狭める
  chmax(alpha, l);
  chmin(beta, u);

  // 葉ノードでなければ子ノードを列挙
  vector<int> legal_actions = state.legalActions();
  // パスの処理 手番を交代して同じ深さで再帰する
  if (legal_actions.empty()) {
    // if (passed) return state.getScore();
    state.isBlack ^= 1;
    auto ret = -nega_alpha_transpose(state, -beta, -alpha, true, depth - 1,
                                     time_keeper);
    state.isBlack ^= 1;
    return ret;
  }
  // move ordering実行
  legal_actions = moveOrdering(legal_actions, state, depth);
  // 探索
  ScoreType maxScore = -INF;

  rep(i, legal_actions.size()) {
    const int action = legal_actions[i];
    const uint64_t flips = state.advance(action);
    ScoreType score = -nega_alpha_transpose(state, -beta, -alpha, false,
                                            depth - 1, time_keeper);
    state.retreat(action, flips);
    if (score >= beta) {
      // 興味の範囲よりも上の時は枝刈り（fail high）
      if (score > l) {
        // 置換票の下限値を更新
        transpose_table_lower[!state.isBlack][key] = score;
      }
      return score;
    }
    chmax(alpha, score);
    chmax(maxScore, score);
    if (time_keeper.isTimeOver()) return 0;
  }

  if (maxScore < alpha) {
    // fail low
    transpose_table_upper[!state.isBlack][key] = maxScore;
  } else {
    // mini, max値が求まった
    transpose_table_lower[!state.isBlack][key] = maxScore;
    transpose_table_upper[!state.isBlack][key] = maxScore;
  }

  return maxScore;
}
// alphabetaのためのスコア計算
ScoreType negaScout(State& state, ScoreType alpha, ScoreType beta, bool passed,
                    const int depth, const TimeKeeper& time_keeper) {
  visited_node++;
  // 葉ノードでは評価関数w御実行する
  WinningStatus winningStatus = state.getWinningStatus();
  if (winningStatus == WinningStatus::WIN) return INF;
  if (winningStatus == WinningStatus::LOSE) return -INF;
  if (winningStatus == WinningStatus::DRAW) return 0;
  if (depth == 0) return state.getScore();

  __uint128_t key = state.getKey();

  // 置換表から上限値と下限値があれば取得
  ScoreType u = INF, l = -INF;
  if (transpose_table_upper[!state.isBlack].count(key))
    u = transpose_table_upper[!state.isBlack][key];
  if (transpose_table_lower[!state.isBlack].count(key))
    l = transpose_table_lower[!state.isBlack][key];
  // upper == lower、つまりもうminimax値が求まっていれば探索終了
  if (u == l) return u;

  // 置換表の値を使って探索窓を狭められる場合は狭める
  chmax(alpha, l);
  chmin(beta, u);

  // 葉ノードでなければ子ノードを列挙
  vector<int> legal_actions = state.legalActions();
  // パスの処理 手番を交代して同じ深さで再帰する
  if (legal_actions.empty()) {
    // if (passed) return state.getScore();
    state.isBlack ^= 1;
    auto ret = -negaScout(state, -beta, -alpha, true, depth - 1, time_keeper);
    state.isBlack ^= 1;
    return ret;
  }
  // move ordering実行
  legal_actions = moveOrdering(legal_actions, state, depth);

  // 探索
  ScoreType maxScore = -INF;
  // i = 0は普通に探索する
  {
    const int action = legal_actions[0];
    const uint64_t flips = state.advance(action);
    ScoreType score =
        -negaScout(state, -beta, -alpha, false, depth - 1, time_keeper);
    state.retreat(action, flips);
    // 興味の範囲よりもminimax値が上のときは枝刈り fail high
    if (score >= beta) {
      // 置換表の下限値に登録
      if (score > l) transpose_table_lower[!state.isBlack][key] = score;
      return score;
    }
    chmax(alpha, score);
    chmax(maxScore, score);
  }

  for (int i = 1; i < (int)legal_actions.size(); i++) {
    // 幅1でnws
    const int action = legal_actions[i];
    const uint64_t flips = state.advance(action);
    ScoreType score = -nega_alpha_transpose(state, false, -alpha - 1, -alpha,
                                            depth - 1, time_keeper);
    state.retreat(action, flips);
    // 興味の範囲よりもminimax値が上のときは枝刈り fail high
    if (score >= beta) {
      if (score > l) {
        // 置換表の下限値に登録
        transpose_table_lower[!state.isBlack][key] = score;
      }
      return score;
    }
    // 最善手候補よりも良い手が見つかった場合、普通にサーチする
    if (chmax(alpha, score)) {
      state.advance(action);
      score = -negaScout(state, -beta, -alpha, false, depth - 1, time_keeper);
      state.retreat(action, flips);
      // 興味の範囲よりもminimax値が上のときは枝刈り fail high
      if (score >= beta) {
        if (score > l) {
          // 置換表の下限値に登録
          transpose_table_lower[!state.isBlack][key] = score;
        }
        return score;
      }
    }
    chmax(alpha, score);
    chmax(maxScore, score);

    if (alpha == INF) {
      isVictory = true;
      cerr << "必勝" << endl;
      break;
    }
    if (time_keeper.isTimeOver()) return 0;
  }

  if (maxScore < alpha) {
    // 置換表の上限値に登録 fail low
    transpose_table_upper[!state.isBlack][key] = maxScore;
  } else {
    // minimax値が求まった
    transpose_table_upper[!state.isBlack][key] = maxScore;
    transpose_table_lower[!state.isBlack][key] = maxScore;
  }

  return alpha;
}

// 深さを指定してalphabetaで行動を決定する
int alphaBetaActionWithTimeThreshold(State& state, const int depth,
                                     const TimeKeeper& time_keeper) {
  ScoreType best_action = -1;
  ScoreType alpha = -INF - 1;
  ScoreType beta = INF;
  vector<int> legal_actions = state.legalActions();
  legal_actions = moveOrdering(legal_actions, state, depth);

  // i = 0は普通に探索する
  {
    const int action = legal_actions[0];
    const uint64_t flips = state.advance(action);
    ScoreType score =
        -negaScout(state, -beta, -alpha, false, depth - 1, time_keeper);
    state.retreat(action, flips);
    if (chmax(alpha, score)) best_action = action;
  }
  // null window serachする
  for (int i = 1; i < (int)legal_actions.size(); i++) {
    // 幅1でnws
    const int action = legal_actions[i];
    const uint64_t flips = state.advance(action);
    ScoreType score = -nega_alpha_transpose(state, false, -alpha - 1, -alpha,
                                            depth - 1, time_keeper);

    // 最善手候補よりも良い手が見つかった場合、普通にサーチする
    if (chmax(alpha, score)) {
      score = -negaScout(state, -beta, -alpha, false, depth - 1, time_keeper);

      chmax(alpha, score);
      best_action = action;
    }
    // 戻す
    state.retreat(action, flips);

    if (alpha == INF) {
      isVictory = true;
      cerr << "必勝" << endl;
      break;
    }
    if (time_keeper.isTimeOver()) return 0;
  }
  cerr << "depth: " << depth << ", score: " << alpha << endl;
  return (int)best_action;
}

// 制限時間(ms)を指定して反復深化で行動を決定する
int iterativeDeepeningAction(State& state, const int64_t time_threshold = 145) {
  isVictory = false;
  auto time_keeper = TimeKeeper(time_threshold);
  transpose_table_upper.clear();
  transpose_table_lower.clear();
  former_transpose_table_upper.clear();
  former_transpose_table_lower.clear();
  int best_action = -1;
  int depth = 5;
  for (; depth < 61; depth++) {
    visited_node = 0;

    int action = alphaBetaActionWithTimeThreshold(state, depth, time_keeper);
    cerr << "depth = " << depth << ", action = " << action
         << ", visited_node = " << visited_node << endl;
    if (time_keeper.isTimeOver()) {
      break;
    } else {
      best_action = action;
    }
    if (isVictory) break;

    transpose_table_upper.swap(former_transpose_table_upper);
    transpose_table_upper.clear();
    transpose_table_lower.swap(former_transpose_table_lower);
    transpose_table_lower.clear();
  }
  cerr << "depth: " << depth << ", best_action: " << best_action << endl;
  // if (best_action == -1) {
  //   cerr << time_keeper.isTimeOver() << endl;
  //   state.printLegalActions();
  //   state.printBoard();
  // }
  return best_action;
}

}  // namespace iterativedeepening

void initCellBitEvaluations() {
  rep(i, 8) {
    rep(j, 256) {
      int v = 0;
      rep(k, 8) if ((j >> k) & 1) v += CELL_EVALATIONS[i][k];
      cellBitEvaluations[i][j] = v;
    }
  }
}

// 辺のパターンに対する確定石を前計算しておく
void initCellBitFixedStone() {
  int cnt = 0;
  rep(black, 256) rep(white, 256) {
    // 黒の確定石、白の確定石
    int fsb = 0, fsw = 0;
    // 同じマスに黒と白が両方ある盤面はありえない
    bool ng = false;
    rep(i, 8) if (black >> i & 1 && white >> i & 1) ng = true;
    if (ng) continue;
    // 空きマスがなければ全て確定石
    if (__builtin_popcount(black) + __builtin_popcount(white) == 8) {
      fsb = __builtin_popcount(black);
      fsw = __builtin_popcount(white);
    } else {
      // そうでなければ端から連結した個数
      rep(i, 8) {
        if (!(black >> i & 1)) break;
        fsb++;
      }
      rep(i, 8) {
        if (!(black >> (7 - i) & 1)) break;
        fsb++;
      }
      rep(i, 8) {
        if (!(white >> i & 1)) break;
        fsw++;
      }
      rep(i, 8) {
        if (!(white >> (7 - i) & 1)) break;
        fsw++;
      }
    }
    //
    cnt++;
    cellBitFixedStone[black][white] = fsb - fsw;
  }
  assert(cnt == 6561);  // 有効な盤面は3^8通り
  assert(cellBitFixedStone[0][255] == -8);
  assert(cellBitFixedStone[1][0] == 1);
  assert(cellBitFixedStone[7][0] == 3);
  assert(cellBitFixedStone[3][1] == 0);      // 無効盤面
  assert(cellBitFixedStone[252][1] == 5);    // fsb = 6, fsw = 1
  assert(cellBitFixedStone[81][174] == -2);  // fsb = 3, fsw = 5

  rep(black, 256) rep(white, 256) {
    bool ng = false;
    rep(i, 8) if (black >> i & 1 && white >> i & 1) ng = true;
    if (ng) continue;
    uint64_t keyLeftBlack = 0, keyLeftWhite = 0;
    uint64_t keyRightBlack = 0, keyRightWhite = 0;
    // 左辺
    uint64_t tmp = 128;
    rep(i, 8) {
      if (black >> i & 1) keyLeftBlack += tmp;
      if (white >> i & 1) keyLeftWhite += tmp;
      tmp <<= 8;
    }
    // 右辺
    tmp = 1;
    rep(i, 8) {
      if (black >> i & 1) keyRightBlack += tmp;
      if (white >> i & 1) keyRightWhite += tmp;
      tmp <<= 8;
    }

    cellBitFixedStoneLeftMap[make_pair(keyLeftBlack, keyLeftWhite)] =
        cellBitFixedStone[black][white];
    cellBitFixedStoneRightMap[make_pair(keyRightBlack, keyRightWhite)] =
        cellBitFixedStone[black][white];
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
    State state(board, isBlack);

    int res = iterativedeepening::iterativeDeepeningAction(state);

    if (res == -1) {
      cerr << "負け" << endl;
      cout << actions[0] << endl;
      continue;
    }

    cout << actionToString(res) << endl;
  }
}