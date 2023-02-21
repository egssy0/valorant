namespace threads::render
{
	void thread()
	{
        if (GetAsyncKeyState(VK_INSERT) & 1)
            settings::menu_enabled = !settings::menu_enabled;

        if (settings::menu_enabled)
        { 
            ImDui::ProcessInput();
            ImDui::NewFrame();                                                                                                                                                      
            ImDui::BeginWindow(_("nasa"), nullptr, ImFloat2(20, 20), ImFloat2(600, 400), -1.0f, ImDuiWindowFlags_NoScrollbar );
            {
                ImDui::CheckBox( _( "name" ), &settings::esp::name );
                ImDui::CheckBox( _( "gun" ), &settings::esp::gun );
                ImDui::CheckBox( _( "distance" ), &settings::esp::distance );
                ImDui::CheckBox(_("boxes"), &settings::esp::boxes);
                if (settings::esp::boxes)
                {
                    ImDui::SameLine();
                    ImDui::CheckBox(_("cornered"), &(*(bool*)&settings::esp::box_type)); //imdui combo when????
                }
                ImDui::CheckBox(_("head circle"), &settings::esp::head_circle);
                ImDui::CheckBox(_("health bar"), &settings::esp::health_bar);
                ImDui::CheckBox(_("shield bar"), &settings::esp::shield_bar);
                ImDui::CheckBox(_("aimbot"), &settings::aimbot::enabled);
                //ImDui::SameLine( );
                //ImDui::CheckBox( _( "use mpv bone" ), &settings::aimbot::mpv_bone );
                //ImDui::SameLine( );
                ImDui::CheckBox( _( "draw fov" ), &settings::aimbot::draw_fov );
                //ImDui::CheckBox( _( "silent aimbot" ), &settings::aimbot::silent_aim );
                ImDui::SliderFloat(_("aimbot fov"), &settings::aimbot::fov, 1.0f, 1000.0f);
                ImDui::SliderFloat(_("aimbot smoothness"), &settings::aimbot::smoothness, 1.0f, 15.0f);
                //ImDui::SliderFloat( _( "silent aimbot delay" ), &settings::aimbot::shot_delay, 1.0f, 1000.0f );

                ImDui::EndWindow();
            }
        }

        renderer->begin();

        float screen_width = GetSystemMetrics(SM_CXSCREEN);
        float screen_height = GetSystemMetrics(SM_CYSCREEN);

        //static auto last_id = 0;
        //auto local_inventory = local_actor->inventory()->current_equippable()->id();
        //renderer->text({ 20.f, 50.f }, { 66, 101, 199 }, 15.f, false, true, _(L"%d"), local_inventory);
        //if(last_id != local_inventory)
        //    printf("%d\n", local_inventory);

        //last_id = local_inventory;

        //static auto last_id = 0;
        //auto local_actor_id = local_actor->id();
        ////renderer->text({ 20.f, 50.f }, { 66, 101, 199 }, 15.f, false, true, _(L"%d"), local_actor_id);
        //if (last_id != local_actor_id) {
        //    printf("%d\n", local_actor_id);
        //}

        //last_id = local_actor_id;

        //Vector2 best_target = Vector2::Zero();
        //auto best_fov = settings::aimbot::fov;

        for (const auto& actor : player_list)
        {
            //const auto actor = reinterpret_cast<APawn*>(actor_ptr);
            if (!actor->is_dormant()) continue;

            const auto mesh = actor->mesh();
            if (!mesh) continue;

            const auto root_component = actor->root_component();
            if (!root_component) continue;


            const auto damage_controller = actor->damage_controller();
            if (!damage_controller) continue;
            if (damage_controller->health() <= 0.0f) continue;

            Vector3 head_position = mesh->get_bone_position(USkeletalMeshComponent::Bone::GenericHead);
            Vector3 root_position = root_component->position();

            root_position = mesh->get_bone_position(1);

            Vector2 root_screen;
            Vector2 head_screen;


            /// testy vesty code
            //const auto [bone_array, bone_count] = mesh->bones();

            //for (int i = 0; i < bone_count; i++)
            //{
            //    Vector3 bone_pos = mesh->get_bone_position(i);
            //    if (bone_pos == Vector3::Zero()) continue;

            //    Vector2 bone_screen;
            //    if (camera_manager->world_to_screen(bone_pos, bone_screen))
            //        renderer->text(bone_screen, Color3(255, 255, 255), 15.0f, true, true, _(L"id: %d"), i);
            //}

            //if (camera_manager->world_to_screen(head_position, head_screen))
            //{
            //    Vector2 angle = (head_screen - renderer->screen_center);
            //    const auto fov = std::hypot(angle.x, angle.y);

            //    if (fov < best_fov) {
            //        best_fov = fov;
            //        best_target = head_screen;
            //    }
            //}

            if (camera_manager->world_to_screen(Vector3(head_position.x, head_position.y, head_position.z), head_screen) &&
                camera_manager->world_to_screen(root_position, root_screen))
            {
                const auto [shield, max_shield] = damage_controller->shield();
                float health = damage_controller->health();

                float distance = camera_manager->position().distance(head_position) / 100.0f;
                float distance_modifier =  camera_manager->position().distance(head_position) * 0.00064F;
                auto bounds = mesh->get_bounds(2.5f);
                float box_width = bounds.right - bounds.left;
                float box_height = bounds.bottom - bounds.top;
                Color3 box_col = Color3(255, 255, 255);
                float size = (7.0f * camera_manager->zoom()) / distance_modifier;

                if( settings::esp::head_circle)
                    renderer->circle(head_screen, box_col, size, 1.0f);

                if ( settings::esp::name )
                    renderer->boldtext( { root_screen.x, bounds.bottom + 10.0f }, box_col, 14.0f, true, true, _( L"%s" ), actor->name().c_str() /*, relative_scale.x, relative_scale.y, relative_scale.z*/);

                if ( settings::esp::distance )
                    renderer->boldtext( { root_screen.x, bounds.bottom + 40.0f }, box_col, 14.0f, true, true, _( L"%.0fm" ), distance );

                if (settings::esp::boxes && settings::esp::box_type == settings::esp::BoxType::Corner)
                {
                    renderer->line({ bounds.left, bounds.top }, { bounds.left + (box_width / 3.5f), bounds.top }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.top }, { bounds.right - (box_width / 3.5f), bounds.top }, box_col, true, 1.5f);

                    renderer->line({ bounds.left, bounds.bottom }, { bounds.left + (box_width / 3.5f), bounds.bottom }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.bottom }, { bounds.right - (box_width / 3.5f), bounds.bottom }, box_col, true, 1.5f);

                    renderer->line({ bounds.left, bounds.top }, { bounds.left, bounds.top + (box_width / 3.5f) }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.top }, { bounds.right, bounds.top + (box_width / 3.5f) }, box_col, true, 1.5f);

                    renderer->line({ bounds.left, bounds.bottom }, { bounds.left, bounds.bottom - (box_width / 3.5f) }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.bottom }, { bounds.right, bounds.bottom - (box_width / 3.5f) }, box_col, true, 1.5f);
                }
                else if (settings::esp::boxes && settings::esp::box_type == settings::esp::BoxType::Full)
                {
                    renderer->line({ bounds.left, bounds.top }, { bounds.right, bounds.top }, box_col, true, 1.5f);
                    renderer->line({ bounds.left, bounds.top }, { bounds.left, bounds.bottom }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.top }, { bounds.right, bounds.bottom }, box_col, true, 1.5f);
                    renderer->line({ bounds.right, bounds.bottom }, { bounds.left , bounds.bottom }, box_col, true, 1.5f);
                }

                if( settings::esp::gun )
                    if (const auto inventory = actor->inventory())
                        if (const auto current_equipable = inventory->current_equippable())
                           renderer->boldtext({ root_screen.x, bounds.bottom + 25.0f }, box_col, 14.0f, true, true, _(L"%s"), current_equipable->name( ).c_str( )/*, relative_scale.x, relative_scale.y, relative_scale.z*/ );

               
                if (settings::esp::health_bar)
                {
                    renderer->rectangle({ bounds.left - 10.0f, bounds.top }, { 5, box_height + 1 }, Color3(0, 0, 0));
                    renderer->rectangle_filled({ bounds.left - 10.0f, bounds.bottom }, { 4, -(box_height * (health / 100.0f)) }, Color3(255 - (health * 2.55f), health * 2.55f, 0, 255));
                }

                if (shield > 0 && max_shield > 0 && settings::esp::shield_bar)
                {
                    renderer->rectangle({ bounds.left - 20.0f, bounds.top }, { 5, box_height + 1 }, Color3(0, 0, 0));
                    renderer->rectangle_filled({ bounds.left - 20.0f, bounds.bottom }, { 4, -(box_height * (shield / max_shield)) }, Color3(77, 133, 255));
                }
            }
        }

        renderer->end();
	}
}