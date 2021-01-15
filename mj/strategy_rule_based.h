#ifndef MAHJONG_STRATEGY_RULE_BASED_H
#define MAHJONG_STRATEGY_RULE_BASED_H

#include "strategy.h"
#include "observation.h"

namespace mj
{
    class StrategyRuleBased final : public Strategy
    {
    public:
        ~StrategyRuleBased() final = default;
        [[nodiscard]] std::vector<mjproto::Action> TakeActions(std::vector<Observation> &&observations) final;
        [[nodiscard]] static mjproto::Action TakeAction(Observation &&observation);
    private:
        template<typename RandomGenerator>
        static Tile SelectDiscard(std::vector<Tile> &discard_candidates, const Hand &curr_hand, RandomGenerator& g);
    };
}

#endif //MAHJONG_STRATEGY_RULE_BASED_H