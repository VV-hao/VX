#include <mmo_game/core/WorldManager.h>
#include <mutex>    // for unique_lock
using namespace mmo;


std::unique_ptr<WorldManager> mmo::GlobalWorldManager = std::make_unique<WorldManager>(
    500.0F, 500.0F, 10.0F
);

void WorldManager::AddPlayer(const std::shared_ptr<Player>& player) {
    {
        std::unique_lock<std::shared_mutex> guard(rw_mutex_);
        assert(players_.find(player->GetPid()) == players_.end());
        players_[player->GetPid()] = player;    
    }

    aoiManager_.AddPlayerToGrid(player->GetPid(), player->GetPosition());
}

void WorldManager::AddPlayer(std::shared_ptr<Player>&& player) {
    const uint32_t pid = player->GetPid();
    const Position& pos = player->GetPosition();
    {
        std::unique_lock<std::shared_mutex> guard(rw_mutex_);
        assert(players_.find(player->GetPid()) == players_.end());
        players_[player->GetPid()] = std::move(player);    
    }

    aoiManager_.AddPlayerToGrid(pid, pos);
}

void WorldManager::RemovePlayerByPid(int32_t pid) {
    assert(players_.find(pid) != players_.end());
    std::shared_ptr<Player>& target_player = players_[pid];

    /* notify that the target player is offline to aoi_manager */
    const Position& pos_of_target_player = target_player->GetPosition();
    aoiManager_.RemovePlayerFromGrid(pid, pos_of_target_player);
    /* remove player instace */
    std::unique_lock<std::shared_mutex> guard(rw_mutex_);
    size_t ret = players_.erase(pid);
    assert(ret == 1);
}