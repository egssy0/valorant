namespace threads::cache
{
	void thread()
	{
        while (true)
        {
            player_list.clear();
            uworld = UWorld::Instance();
            persistent_level = uworld->persistent_level();
            game_instance = uworld->game_instance();

            const auto [actors, actor_count] = persistent_level->actors();
            localplayer = game_instance->localplayer();
            local_controller = localplayer->controller();

            local_actor = local_controller->acknowledged_pawn();
            local_state = local_actor->player_state();
            local_team_id = local_state->team_component()->team_id();
            camera_manager = local_controller->camera_manager();
            for (int i = 0; i < actor_count; i++)
            {
                const auto actor = actors->get(i);
                if (!actor || actor == local_actor) continue;
                if ( actor->unique_id( ) != 0x11E0101 ) continue;

                const auto mesh = actor->mesh();
                if (!mesh) continue;

                if (mesh->body_type() != USkeletalMeshComponent::BodyType::Bot)
                {
                    const auto state = actor->player_state();
                    if (!state) continue;

                    const auto team_component = state->team_component();
                    if (!team_component) continue;
                    if (team_component->team_id() == local_team_id) {
                        //actor->outline(EAresOutlineMode::AlwaysOutline);
                        continue;
                    }
                }

                if (!actor->is_dormant()) continue;

                const auto damage_controller = actor->damage_controller();
                if (!damage_controller) continue;
                const auto health = damage_controller->health();

                if (health <= 0.0f) continue;

                //actor->outline( EAresOutlineMode::Outline );

                //Color3 chams_color = Color3( 7.5f, 0.3f, 3.3f, 3.9f ); // R: 0-1,G: 0-1, B: 0-1, A: 0-1
                //if ( UWorld::ally_outline_color( ) != chams_color )
                //    UWorld::ally_outline_color( chams_color );
                //if ( UWorld::enemy_outline_color( ) != chams_color )
                //    UWorld::enemy_outline_color( chams_color );


                const auto player_list_iter = std::find(player_list.begin(), player_list.end(), actor);

                if (player_list_iter != player_list.end() && health <= 0)
                {
                    player_list.erase(player_list_iter);
                    continue;
                }
                else if (player_list_iter != player_list.end() && health > 0)
                    continue;

                if (player_list.size() > 300)
                    player_list.clear();

                player_list.push_back(actor);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        cache_ejected.store(true);
	}
}