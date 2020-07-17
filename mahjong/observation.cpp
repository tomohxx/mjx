#include "observation.h"
#include "utils.h"

namespace mj
{
    Score::Score()
    {
        score_.set_round(0);
        score_.set_honba(0);
        score_.set_riichi(0);
        for (int i = 0; i < 4; ++i) score_.add_ten(250);
    }

    std::uint8_t Score::round() const {
        return score_.round();
    }

    std::uint8_t Score::honba() const {
        return score_.honba();
    }

    std::uint8_t Score::riichi() const {
        return score_.riichi();
    }

    std::array<std::int16_t, 4> Score::ten() const {
        assert(score_.ten_size() == 4);
        auto ret = std::array<std::int16_t, 4>();
        for (int i = 0; i < 4; ++i) ret[i] = score_.ten(i);
        return ret;
    }

    PossibleAction::PossibleAction(mjproto::PossibleAction possible_action)
    : possible_action_(std::move(possible_action)) {}

    ActionType PossibleAction::type() const {
        return ActionType(possible_action_.type());
    }

    std::unique_ptr<Open> PossibleAction::open() const {
        return Open::NewOpen(possible_action_.open());
    }

    std::vector<Tile> PossibleAction::discard_candidates() const {
        std::vector<Tile> ret;
        for (const auto& id: possible_action_.discard_candidates()) ret.emplace_back(Tile(id));
        return ret;
    }

    PossibleAction PossibleAction::CreateDiscard(const Hand &hand) {
        assert(hand.Stage() != HandStage::kAfterDiscards);
        auto possible_action = PossibleAction();
        possible_action.possible_action_.set_type(ToUType(ActionType::kDiscard));
        auto discard_candidates = possible_action.possible_action_.mutable_discard_candidates();
        for (auto tile: hand.PossibleDiscards()) discard_candidates->Add(tile.Id());
        assert(discard_candidates->size() <= 14);
        return possible_action;
    }

    std::size_t ActionHistory::size() const {
        return action_history_.taken_actions_size();
    }

    std::vector<PossibleAction> Observation::possible_actions() const {
        assert(action_request_.has_action_history());
        std::vector<PossibleAction> ret;
        for (const auto& possible_action: action_request_.possible_actions()) {
            ret.emplace_back(PossibleAction{possible_action});
        }
        return ret;
    }

    std::uint32_t Observation::game_id() const {
        return action_request_.game_id();
    }

    AbsolutePos Observation::who() const {
        return AbsolutePos(action_request_.who());
    }

    void Observation::ClearPossibleActions() {
        action_request_.clear_possible_actions();
    }

    Observation::~Observation() {
        // Calling release_xxx prevent gRPC from deleting objects after gRPC communication
        assert(action_request_.has_action_history());
        action_request_.release_score();
        action_request_.release_action_history();
        action_request_.release_initial_hand();
    }

    void Observation::add_possible_action(PossibleAction possible_action) {
        // TDOO (sotetsuk): add assertion. もしtypeがdiscardならすでにあるpossible_actionはdiscardではない
        auto mutable_possible_actions = action_request_.mutable_possible_actions();
        mutable_possible_actions->Add(std::move(possible_action.possible_action_));
    }

    Observation::Observation(AbsolutePos who, Score &score, ActionHistory &action_history, Player& player) {
        action_request_.set_who(ToUType(who));
        action_request_.set_allocated_score(&score.score_);
        action_request_.set_allocated_action_history(&action_history.action_history_);
        action_request_.set_allocated_initial_hand(&player.initial_hand_);
    }
}