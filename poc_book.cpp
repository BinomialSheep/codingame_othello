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
盤面評価方法を変更。確定石と置ける個数を追加
55位
bp fs cn
10 5  10 55位

参考：盤面の評価値によるオセロプログラム
https://www.info.kindai.ac.jp/~takasi-i/thesis/2014_10-1-037-0140_S_Okigaki_thesis.pdf

*/

// 定石
const vector<string> books = {"f5",
                              "f5d6",
                              "f5d6c3g5",
                              "f5d6c3g5c6c5",
                              "f5d6c3g5c6c5c4b6",
                              "f5d6c3g5c6c5c4b6f6f4",
                              "f5d6c3g5c6c5c4b6f6f4e6d7",
                              "f5d6c3g5c6c5c4b6f6f4e6d7c7g6",
                              "f5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5",
                              "f5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3",
                              "f5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3a6e3",
                              "f5d6c3g5c6c5c4b6f6f4e6d7c7g6d8b5e7b3a6e3a5d3",
                              "f5d6c3g5f6d3",
                              "f5d6c3g5f6d3e3c2",
                              "f5d6c3g5f6d3e3c2c1e6",
                              "f5d6c3g5f6d3e3c2c1e6f4f3",
                              "f5d6c3g5f6d3e3c2c1e6f4f3f2g4",
                              "f5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2",
                              "f5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4",
                              "f5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4h5f7",
                              "f5d6c3g5f6d3e3c2c1e6f4f3f2g4g6d2h3h4h5f7e7g3",
                              "f5d6c3g5g6d3",
                              "f5d6c3g5g6d3c4e3",
                              "f5d6c3g5g6d3c4e3f3b4",
                              "f5d6c3g5g6d3c4e3f3b4f6e6",
                              "f5d6c3g5g6d3c4e3f3b4f6e6f4g4",
                              "f5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5",
                              "f5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3",
                              "f5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3h3f7",
                              "f5d6c3g5g6d3c4e3f3b4f6e6f4g4h4h5h6g3h3f7f8c2",
                              "f5d6c4b3",
                              "f5d6c4b3b4f4",
                              "f5d6c4b3b4f4f6g5",
                              "f5d6c4b3b4f4f6g5f3e7",
                              "f5d6c4b3b4f4f6g5f3e7c5e6",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4d3",
                              "f5d6c5b4",
                              "f5d6c5b4d7e7",
                              "f5d6c5b4d7e7c7d8",
                              "f5d6c5b4d7e7c7d8c3d3",
                              "f5d6c5b4d7e7c7d8c3d3c4b3",
                              "f5d6c5b4d7e7c7d8c3d3c4b3d2e2",
                              "f5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3",
                              "f5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2",
                              "f5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2c6b5",
                              "f5d6c5b4d7e7c7d8c3d3c4b3d2e2c2e3f4f2c6b5f3c8",
                              "f5d6c4",
                              "f5d6c4b3b4",
                              "f5d6c4b3b4f4f6",
                              "f5d6c4b3b4f4f6g5f3",
                              "f5d6c4b3b4f4f6g5f3e7c5",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4",
                              "f5d6c4b3b4f4f6g5f3e7c5e6c3g4c6g3h3e3f2b6h4d3e2",
                              "f5d6c4d3c3",
                              "f5d6c4d3c3b3d2",
                              "f5d6c4d3c3b3d2e1b5",
                              "f5d6c4d3c3b3d2e1b5c5b4",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2a4c6",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6f1e6",
                              "f5d6c4d3c3b3d2e1b5c5b4e3c2a4c6d1e2c7b6f1e6f3f2",
                              "f5d6c4d3c3f4f6",
                              "f5d6c4d3c3f4f6f3e6",
                              "f5d6c4d3c3f4f6f3e6e7f7",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g5e3",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4h3d2",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g5e3d7c6e2g4h3d2g3f1",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g6e3",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5c6d8",
                              "f5d6c4d3c3f4f6f3e6e7f7c5b6g6e3e2f1d1g5c6d8g4h6",
                              "f5d6c4d3c3f4f6b4c2",
                              "f5d6c4d3c3f4f6b4c2f3e3",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6f2c5",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7b3g5",
                              "f5d6c4d3c3f4f6b4c2f3e3e2c6f2c5e6d2g4d7b3g5c8h4",
                              "f5d6c4d3c3f4f6g5e3",
                              "f5d6c4d3c3f4f6g5e3f3g6",
                              "f5d6c4d3c3f4f6g5e3f3g6e2h5",
                              "f5d6c4d3c3f4f6g5e3f3g6e2h5c5g4",
                              "f5d6c4d3c3f4f6g5e3f3g6e2h5c5g4g3f2",
                              "f5d6c4d3c3b5b4",
                              "f5d6c4d3c3b5b4f4c5",
                              "f5d6c4d3c3b5b4f4c5a4b3",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6a3e3",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6g3e2",
                              "f5d6c4d3c3b5b4f4c5a4b3d2a6a3e3f3g4e6f6g3e2c2f2",
                              "f5d6c4g5f6",
                              "f5d6c4g5f6f4f3",
                              "f5d6c4g5f6f4f3d3c3",
                              "f5d6c4g5f6f4f3d3c3g6e3",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5d2e2",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6b4b3",
                              "f5d6c4g5f6f4f3d3c3g6e3e6h5d2e2c2c6c5b6b4b3c7a4",
                              "f5f6e6",
                              "f5f6e6f4g6",
                              "f5f6e6f4g6c5f3",
                              "f5f6e6f4g6c5f3g4e3",
                              "f5f6e6f4g6c5f3g4e3d6g5",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3h5c4",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3f7e7",
                              "f5f6e6f4g6c5f3g4e3d6g5g3c3h5c4d7h6h7h3f7e7f8h4",
                              "f5f6e6f4g6c5f3g5d6",
                              "f5f6e6f4g6c5f3g5d6e3h4",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4h6e2",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6e7f2",
                              "f5f6e6f4g6c5f3g5d6e3h4g3g4h6e2d3h5h3c6e7f2c4d2",
                              "f5f6e6f4g6d6g4",
                              "f5f6e6f4g6d6g4g5h4",
                              "f5f6e6f4g6d6g4g5h4e7f3",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7e8f8",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7e3c5",
                              "f5f6e6f4g6d6g4g5h4e7f3h6f7e8f8g8d3h5h7e3c5c4g3",
                              "f5f6e6d6f7",
                              "f5f6e6d6f7e3c6",
                              "f5f6e6d6f7e3c6e7f4",
                              "f5f6e6d6f7e3c6e7f4c5d8",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7f8b5",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3g5b6",
                              "f5f6e6d6f7e3c6e7f4c5d8c7d7f8b5c4e8c8f3g5b6d3b4",
                              "f5f6e6d6f7f4d7",
                              "f5f6e6d6f7f4d7e7d8",
                              "f5f6e6d6f7f4d7e7d8g5c6",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6h5h6",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8c5e3",
                              "f5f6e6d6f7f4d7e7d8g5c6f8g6h5h6h7c4e8g8c5e3d3c7"};

const vector<vector<int>> CELL_EVALATIONS = {
    {30, -12, 0, -1, -1, 0, -12, 30},     {-12, -15, -3, -3, -3, -3, -15, -12},
    {0, -3, 0, -1, -1, 0, -3, 0},         {-1, -3, -1, -1, -1, -1, -3, -1},
    {-1, -3, -1, -1, -1, -1, -3, -1},     {0, -3, 0, -1, -1, 0, -3, 0},
    {-12, -15, -3, -3, -3, -3, -15, -12}, {30, -12, 0, -1, -1, 0, -12, 30}};
// const vector<vector<int>> CELL_EVALATIONS = {
//     {100, -40, 20, 5, 5, 20, -40, 100},   {-40, -80, -1, -1, -1, -1, -80,
//     -40}, {20, -1, 5, 1, 1, 5, -1, 20},         {5, -1, 1, 0, 0, 1, -1, -5},
//     {5, -1, 1, 0, 0, 1, -1, -5},          {20, -1, 5, 1, 1, 5, -1, 20},
//     {-40, -80, -1, -1, -1, -1, -80, -40}, {100, -40, 20, 5, 5, 20, -40,
//     100}};
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
    return 2 * bp + fs + cn;
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
      : board(argBoard), isBlack(argColor) {};

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

// ビッドボードの回転操作（https://qiita.com/ysuzuk81/items/453b08a14d23fb8c6c11）
// Delta Swap
uint64_t delta_swap(uint64_t x, uint64_t mask, int delta) {
  uint64_t t = (x ^ (x >> delta)) & mask;
  return x ^ t ^ (t << delta);
}
// 水平反転
uint64_t flipHorizontal(uint64_t x) {
  x = delta_swap(x, 0x5555555555555555, 1);
  x = delta_swap(x, 0x3333333333333333, 2);
  return delta_swap(x, 0x0F0F0F0F0F0F0F0F, 4);
}
// 垂直反転
uint64_t flipVertical(uint64_t x) {
  x = delta_swap(x, 0x00FF00FF00FF00FF, 8);
  x = delta_swap(x, 0x0000FFFF0000FFFF, 16);
  return delta_swap(x, 0x00000000FFFFFFFF, 32);
}
// A1-H8反転あ
uint64_t flipDiagonalA1H8(uint64_t x) {
  x = delta_swap(x, 0x00AA00AA00AA00AA, 7);
  x = delta_swap(x, 0x0000CCCC0000CCCC, 14);
  return delta_swap(x, 0x00000000F0F0F0F0, 28);
}
uint64_t flipDiagonalA8H1(uint64_t x) {
  x = delta_swap(x, 0x0055005500550055, 9);
  x = delta_swap(x, 0x0000333300003333, 18);
  return delta_swap(x, 0x000000000F0F0F0F, 36);
}
// 時計回りに90度回転
uint64_t rotateClockwise90(uint64_t x) {
  return flipHorizontal(flipDiagonalA1H8(x));
}
// 時計回りに90度回転した時の新しいactionの位置
int rotate90Action(int action) {
  int i = action / 8, j = action % 8;
  int i2 = j, j2 = 7 - i;
  return i2 * 8 + j2;
}
int flipHorizontalAction(int action) {
  int i = action / 8, j = action % 8;
  return i * 8 + (7 - j);
}
int flipVerticalAction(int action) {
  int i = action / 8, j = action % 8;
  return (7 - i) * 8 + j;
}
int flipDiagonalA1H8Action(int action) {
  int i = action / 8, j = action % 8;
  return j * 8 + i;
}
int flipDiagonalA8H1Action(int action) {
  int i = action / 8, j = action % 8;
  return (7 - j) * 8 + (7 - i);
}

vector<unordered_map<__uint128_t, int>> bookMap(2);

void loadBooks() {
  auto stringToAction = [](string s) {
    return (s[1] - '1') * 8 + (s[0] - 'a');
  };
  //   int cnt = 0;
  for (string book : books) {
    // if (++cnt == 10) break;
    OthelloBoard board;
    State state(board, true);
    for (int i = 0; i < (int)book.size() - 2; i += 2) {
      int action = stringToAction(book.substr(i, 2));
      state.advance(action);
    }
    // 今の盤面における最善手が最後の手
    int action = stringToAction(book.substr((int)book.size() - 2, 2));
    uint64_t black = state.board.black, white = state.board.white;
    __uint128_t key;
    rep(i, 4) {
      black = rotateClockwise90(black);
      white = rotateClockwise90(white);
      action = rotate90Action(action);
      rep(j, 2) {
        black = flipHorizontal(black);
        white = flipHorizontal(white);
        action = flipHorizontalAction(action);
        key = ((__uint128_t)black << 64) + white;
        bookMap[!state.isBlack][key] = action;
      }
      rep(j, 2) {
        black = flipVertical(black);
        white = flipVertical(white);
        action = flipVerticalAction(action);
        key = ((__uint128_t)black << 64) + white;
        bookMap[!state.isBlack][key] = action;
      }
    }
  }
}

int main() {
  assert(rotate90Action(58) == 16);
  assert(rotate90Action(0) == 7);
  assert(flipHorizontalAction(9) == 14);
  assert(flipHorizontalAction(14) == 9);

  loadBooks();
}