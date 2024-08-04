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

/*
確定石計算テスト
盤面の全てを計算するのはコストがかかるので、辺だけを考える

TODO：マスクした左辺（右辺）に対して高速に確定石を計算するMap<uint64_t,
int>を前計算しておく
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

vector<uint64_t> maskVec = {0x00000000000000FFULL, 0x000000000000FF00ULL,
                            0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                            0x000000FF00000000ULL, 0x0000FF0000000000ULL,
                            0x00FF000000000000ULL, 0xFF00000000000000ULL};

// 64ビットの盤面を表示する関数
void printBoard(uint64_t black, uint64_t white) {
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

void initCellBitEvaluations() {
  rep(i, 8) {
    rep(j, 256) {
      int v = 0;
      rep(k, 8) if ((j >> k) & 1) v += CELL_EVALATIONS[i][k];
      cellBitEvaluations[i][j] = v;
    }
  }
}

// 黒目線の確定石数
vector<vector<int>> cellBitFixedStone(256, vector<int>(256));
// 左辺、右辺用
map<pair<uint64_t, uint64_t>, int> cellBitFixedStoneMap;
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

    cellBitFixedStoneMap[make_pair(keyLeftBlack, keyLeftWhite)] =
        cellBitFixedStone[black][white];
    cellBitFixedStoneMap[make_pair(keyRightBlack, keyRightWhite)] =
        cellBitFixedStone[black][white];
  }
}

vector<uint64_t> maskVecFS = {
    0xFF00000000000000ULL,  // 上辺
    0x00000000000000FFULL,  // 下辺
    0x0101010101010101ULL,  // 右辺
    0x8080808080808080ULL   // 左辺
};

// 自分の確定石数 - 相手の確定石数（近似値）を返す
int calcFixedStone(uint64_t black, uint64_t white, int isBlack) {
  int ret = 0;
  // 上辺
  int tmpb = (int)((black & maskVecFS[0]) >> 56);
  int tmpw = (int)((white & maskVecFS[0]) >> 56);
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "上辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;
  // 下辺
  tmpb = (int)((black & maskVecFS[1]));
  tmpw = (int)((white & maskVecFS[1]));
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "下辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;
  // 右辺
  tmpb = 0, tmpw = 0;
  rep(i, 8) {
    if (black >> (i * 8) & 1) tmpb += 1;
    if (white >> (i * 8) & 1) tmpw += 1;
    if (i != 7) tmpb <<= 1, tmpw <<= 1;
  }
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "右辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;
  // 左辺
  tmpb = 0, tmpw = 0;
  rep(i, 8) {
    if (black >> (i * 8) & 128) tmpb += 1;
    if (white >> (i * 8) & 128) tmpw += 1;
    if (i != 7) tmpb <<= 1, tmpw <<= 1;
  }
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "左辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;
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

// 自分の確定石数 - 相手の確定石数（近似値）を返す
int calcFixedStoneFast(uint64_t black, uint64_t white, int isBlack) {
  int ret = 0;
  // 上辺
  uint64_t tmpb = (black & maskVecFS[0]) >> 56;
  uint64_t tmpw = (white & maskVecFS[0]) >> 56;
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "上辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;
  // 下辺
  tmpb = black & maskVecFS[1];
  tmpw = white & maskVecFS[1];
  ret += cellBitFixedStone[tmpb][tmpw];
  cerr << "下辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStone[tmpb][tmpw] << endl;

  // 右辺
  tmpb = black & maskVecFS[2];
  tmpw = white & maskVecFS[2];
  ret += cellBitFixedStoneMap[make_pair(tmpb, tmpw)];
  cerr << "右辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStoneMap[make_pair(tmpb, tmpw)] << endl;

  // 左辺
  tmpb = black & maskVecFS[3];
  tmpw = white & maskVecFS[3];
  ret += cellBitFixedStoneMap[make_pair(tmpb, tmpw)];
  cerr << "左辺: " << tmpb << " " << tmpw << " "
       << cellBitFixedStoneMap[make_pair(tmpb, tmpw)] << endl;
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

// マスクビッドFSの動作確認
void verifyMaskVecFS(uint64_t randomBoard) {
  cerr << "Original Board:" << endl;
  printBinary(randomBoard);

  // マスクビットを適用して各辺を表示
  rep(i, maskVecFS.size()) {
    uint64_t maskedBoard = randomBoard & maskVecFS[i];
    cerr << "Mask " << i << " applied:" << endl;
    printBinary(maskedBoard);
  }
}

// ランダムなオセロの盤面を生成する関数
void generateRandomOthelloBoard(uint64_t& black, uint64_t& white) {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> dis(0, 2);
  rep(i, 64) {
    int cell = dis(gen);
    if (cell == 1) black |= (1LL << i);
    if (cell == 2) white |= (1LL << i);
  }
}

int main() {
  initCellBitFixedStone();

  // ランダムな盤面を生成する
  uint64_t black, white;
  generateRandomOthelloBoard(black, white);

  printBoard(black, white);

  // for (auto [p, v] : cellBitFixedStoneMap) {
  //   cerr << p.first << " " << p.second << " " << v << endl;
  // }

  int fs = calcFixedStone(black, white, true);
  assert(fs == calcFixedStoneFast(black, white, true));
  cerr << fs << endl;
}