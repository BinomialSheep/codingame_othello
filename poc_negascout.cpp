#include <bits/stdc++.h>
using namespace std;
#include <chrono>

#pragma GCC diagnostic ignored "-Wsign-compare"
random_device rnd;
mt19937 mt_for_action(0);

/*
サンダー本のalphabetaをnegascoutに改造する

*/

// 時間を管理するクラス
class TimeKeeper {
 private:
  chrono::high_resolution_clock::time_point start_time_;
  int64_t time_threshold_;

 public:
  // 時間制限をミリ秒単位で指定してインスタンスをつくる。
  TimeKeeper(const int64_t &time_threshold)
      : start_time_(chrono::high_resolution_clock::now()),
        time_threshold_(time_threshold) {}

  // インスタンス生成した時から指定した時間制限を超過したか判定する。
  bool isTimeOver() const {
    auto diff = chrono::high_resolution_clock::now() - this->start_time_;
    return chrono::duration_cast<chrono::milliseconds>(diff).count() >=
           time_threshold_;
  }
};

constexpr const int H = 5;    // 迷路の高さ
constexpr const int W = 5;    // 迷路の幅
constexpr int END_TURN = 10;  // ゲーム終了ターン

using ScoreType = int64_t;
constexpr const ScoreType INF = 1000000000LL;

enum WinningStatus {
  WIN,
  LOSE,
  DRAW,
  NONE,
};

class AlternateMazeState {
 private:
  static constexpr const int dx[4] = {1, -1, 0, 0};
  static constexpr const int dy[4] = {0, 0, 1, -1};
  struct Character {
    int y_;
    int x_;
    int game_score_;
    Character(const int y = 0, const int x = 0)
        : y_(y), x_(x), game_score_(0) {}
  };
  vector<vector<int>> points_;  // 床のポイントを1~9で表現する
  int turn_;                    // 現在のターン
  vector<Character> characters_;

  // 現在のプレイヤーが先手であるか判定する
  bool isFirstPlayer() const { return this->turn_ % 2 == 0; }

 public:
  AlternateMazeState(const int seed)
      : points_(H, vector<int>(W)),
        turn_(0),
        characters_(
            {Character(H / 2, (W / 2) - 1), Character(H / 2, (W / 2) + 1)}) {
    auto mt_for_construct = mt19937(seed);

    for (int y = 0; y < H; y++)
      for (int x = 0; x < W; x++) {
        int point = (int)(mt_for_construct() % 10);
        if (characters_[0].y_ == y && characters_[0].x_ == x) {
          continue;
        }
        if (characters_[1].y_ == y && characters_[1].x_ == x) {
          continue;
        }

        this->points_[y][x] = point;
      }
  }

  // [どのゲームでも実装する] : ゲームが終了したか判定する
  bool isDone() { return this->turn_ == END_TURN; }

  // [どのゲームでも実装する] :
  // 指定したactionでゲームを1ターン進め、次のプレイヤー視点の盤面にする
  int advance(const int action) {
    auto &character = this->characters_[0];
    character.x_ += dx[action];
    character.y_ += dy[action];
    auto &point = this->points_[character.y_][character.x_];
    int ret = point;
    if (point > 0) {
      character.game_score_ += point;
      point = 0;
    }
    this->turn_++;
    swap(this->characters_[0], this->characters_[1]);
    return ret;
  }

  void retreat(const int action, int point) {
    swap(this->characters_[0], this->characters_[1]);
    auto &character = this->characters_[0];
    this->points_[character.y_][character.x_] = point;
    character.x_ -= dx[action];
    character.y_ -= dy[action];
    this->turn_--;
  }

  // [どのゲームでも実装する] : 現在のプレイヤーが可能な行動を全て取得する
  vector<int> legalActions() {
    vector<int> actions;
    const auto &character = this->characters_[0];
    for (int action = 0; action < 4; action++) {
      int ty = character.y_ + dy[action];
      int tx = character.x_ + dx[action];
      if (ty >= 0 && ty < H && tx >= 0 && tx < W) {
        actions.emplace_back(action);
      }
    }
    return actions;
  }

  // [どのゲームでも実装する] : 勝敗情報を取得する
  WinningStatus getWinningStatus() {
    if (isDone()) {
      if (characters_[0].game_score_ > characters_[1].game_score_)
        return WinningStatus::WIN;
      else if (characters_[0].game_score_ < characters_[1].game_score_)
        return WinningStatus::LOSE;
      else
        return WinningStatus::DRAW;
    } else {
      return WinningStatus::NONE;
    }
  }

  // [どのゲームでも実装する] : 現在のプレイヤー視点の盤面評価をする
  ScoreType getScore() {
    return characters_[0].game_score_ - characters_[1].game_score_;
  }

  // [実装しなくてもよいが実装すると便利] :
  // 現在のプレイヤーの勝率計算のためのスコアを計算する
  double getFirstPlayerScoreForWinRate() {
    switch (this->getWinningStatus()) {
      case (WinningStatus::WIN):
        if (this->isFirstPlayer()) {
          return 1.;
        } else {
          return 0.;
        }
      case (WinningStatus::LOSE):
        if (this->isFirstPlayer()) {
          return 0.;
        } else {
          return 1.;
        }
      default:
        return 0.5;
    }
  }

  // [実装しなくてもよいが実装すると便利] : 現在のゲーム状況を文字列にする
  string toString() {
    stringstream ss("");
    ss << "turn:\t" << this->turn_ << "\n";
    for (int player_id = 0; player_id < this->characters_.size(); player_id++) {
      int actual_player_id = player_id;
      if (this->turn_ % 2 == 1) {
        actual_player_id =
            (player_id + 1) %
            2;  // 奇数ターンの場合は初期配置の視点で見るとplayer_idが逆
      }
      const auto &chara = this->characters_[actual_player_id];
      ss << "score(" << player_id << "):\t" << chara.game_score_
         << "\ty: " << chara.y_ << " x: " << chara.x_ << "\n";
    }
    for (int h = 0; h < H; h++) {
      for (int w = 0; w < W; w++) {
        bool is_written = false;  // この座標に書く文字が決定したか
        for (int player_id = 0; player_id < this->characters_.size();
             player_id++) {
          int actual_player_id = player_id;
          if (this->turn_ % 2 == 1) {
            actual_player_id =
                (player_id + 1) %
                2;  // 奇数ターンの場合は初期配置の視点で見るとplayer_idが逆
          }

          const auto &character = this->characters_[player_id];
          if (character.y_ == h && character.x_ == w) {
            if (actual_player_id == 0) {
              ss << 'A';
            } else {
              ss << 'B';
            }
            is_written = true;
          }
        }
        if (!is_written) {
          if (this->points_[h][w] > 0) {
            ss << points_[h][w];
          } else {
            ss << '.';
          }
        }
      }
      ss << '\n';
    }

    return ss.str();
  }
};

using State = AlternateMazeState;

namespace iterativedeepening {
// 制限時間が切れた際に停止できるalphabetaのためのスコア計算
ScoreType alphaBetaScore(State &state, ScoreType alpha, const ScoreType beta,
                         const int depth, const TimeKeeper &time_keeper) {
  if (time_keeper.isTimeOver()) return 0;
  if (state.isDone() || depth == 0) {
    return state.getScore();
  }
  auto legal_actions = state.legalActions();
  if (legal_actions.empty()) {
    return state.getScore();
  }
  for (auto action : legal_actions) {
    int point = state.advance(action);
    ScoreType score =
        -alphaBetaScore(state, -beta, -alpha, depth - 1, time_keeper);
    state.retreat(action, point);

    if (time_keeper.isTimeOver()) return 0;
    if (score > alpha) {
      alpha = score;
    }
    if (alpha >= beta) {
      return alpha;
    }
  }
  return alpha;
}
// 深さと制限時間(ms)を指定してalphabetaで行動を決定する
int alphaBetaActionWithTimeThreshold(State &state, const int depth,
                                     const TimeKeeper &time_keeper) {
  int best_action = -1;
  ScoreType alpha = -INF;
  for (const auto action : state.legalActions()) {
    int point = state.advance(action);
    ScoreType score = -alphaBetaScore(state, -INF, -alpha, depth, time_keeper);
    state.retreat(action, point);
    if (time_keeper.isTimeOver()) return 0;
    if (score > alpha) {
      best_action = action;
      alpha = score;
    }
  }
  return best_action;
}

// 制限時間(ms)を指定して反復深化で行動を決定する
int iterativeDeepeningAction(State &state, const int64_t time_threshold) {
  auto time_keeper = TimeKeeper(time_threshold);
  int best_action = -1;
  for (int depth = 1;; depth++) {
    int action = alphaBetaActionWithTimeThreshold(state, depth, time_keeper);

    if (time_keeper.isTimeOver()) {
      break;
    } else {
      best_action = action;
    }
  }
  return best_action;
}
}  // namespace iterativedeepening
using iterativedeepening::iterativeDeepeningAction;

using AIFunction = function<int(State &)>;
using StringAIPair = pair<string, AIFunction>;

// ゲームをgame_number×2(先手後手を交代)回プレイしてaisの0番目のAIの勝率を表示する。
void testFirstPlayerWinRate(const array<StringAIPair, 2> &ais,
                            const int game_number) {
  double first_player_win_rate = 0;
  for (int i = 0; i < game_number; i++) {
    auto base_state = State(i);
    for (int j = 0; j < 2; j++) {  // 先手後手平等に行う
      auto state = base_state;
      auto &first_ai = ais[j];
      auto &second_ai = ais[(j + 1) % 2];
      while (true) {
        state.advance(first_ai.second(state));
        if (state.isDone()) break;
        state.advance(second_ai.second(state));
        if (state.isDone()) break;
      }
      double win_rate_point = state.getFirstPlayerScoreForWinRate();
      if (j == 1) win_rate_point = 1 - win_rate_point;
      if (win_rate_point >= 0) {
        state.toString();
      }
      first_player_win_rate += win_rate_point;
    }
    cout << "i " << i << " w " << first_player_win_rate / ((i + 1) * 2) << endl;
  }
  first_player_win_rate /= (double)(game_number * 2);
  cout << "Winning rate of " << ais[0].first << " to " << ais[1].first << ":\t"
       << first_player_win_rate << endl;
}

int main() {
  auto ais = array<StringAIPair, 2>{
      StringAIPair(
          "iterativeDeepeningAction 100",
          [](State &state) { return iterativeDeepeningAction(state, 100); }),
      StringAIPair(
          "iterativeDeepeningAction 1",
          [](State &state) { return iterativeDeepeningAction(state, 1); }),
  };
  testFirstPlayerWinRate(ais, 100);

  return 0;
}