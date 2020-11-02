#include "agent_grpc_client.h"

namespace mj
{
    AgentGrpcClient::AgentGrpcClient(PlayerId player_id, const std::shared_ptr<grpc::Channel> &channel):
            Agent(std::move(player_id)), stub_(mjproto::Agent::NewStub(channel)) { }

    Action AgentGrpcClient::TakeAction(Observation &&observation) const {
        // TODO: verify that player_id is consistent (player_id_ == observation.player_id)
        assert(stub_);
        const mjproto::Observation request = observation.proto();
        mjproto::Action response;
        grpc::ClientContext context;
        grpc::Status status = stub_->TakeAction(&context, request, &response);
        if (!status.ok()) {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
        }
        auto action = Action(std::move(response));
        return action;
    }
}  // namespace mj

// int main(int argc, char** argv) {
//     mj::Agent agent(
//             grpc::CreateChannel("127.0.0.1:9090", grpc::InsecureChannelCredentials())
//     );
//
//     // Common observation over 4 players
//     auto common_observation = std::make_unique<mjproto::Observation_CommonObservation>();
//     auto request1 = mjproto::Observation();
//     request1.set_who(1);
//     auto obs1 = std::make_unique<mj::Observation>(request1, common_observation.get());
//     auto request2 = mjproto::Observation();
//     request2.set_who(2);
//     auto obs2 = std::make_unique<mj::Observation>(request2, common_observation.get());
//
//     // action1 happens
//     auto taken_action1 = mjproto::Observation_CommonObservation_TakenAction();
//     common_observation->mutable_taken_actions()->Add(std::move(taken_action1));
//
//     // take first action
//     auto action = agent.TakeAction(std::move(obs1));
//
//     // action2 happens
//     auto taken_action2 = mjproto::Observation_CommonObservation_TakenAction();
//     common_observation->mutable_taken_actions()->Add(std::move(taken_action2));
//
//     // take second action
//     action = agent.TakeAction(std::move(obs2));
//
//     std::cout << "  type: " << action.action_response().type() << std::endl;
//     std::cout << "  action: " << action.action_response().discard() << std::endl;
//
//     return 0;
// }