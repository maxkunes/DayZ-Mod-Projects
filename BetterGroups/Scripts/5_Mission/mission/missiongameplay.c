modded class MissionGameplay
{
	override void OnMissionStart()
	{
		super.OnMissionStart();

		Print("OnMissionStart()");

		if ( GetGame().IsClient() )
			GetBetterGroups();
	}

	override void OnMissionFinish()
	{
		super.OnMissionFinish();

	}



	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		GetBetterGroups().OnUpdate(timeslice);
	}

	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		Print("OnKeyPress()");
		if ( key == KeyCode.KC_6 )
		{
			if (GetGame().GetUIManager().IsMenuOpen(MENU_BETTERGROUPS))
			{
				GetGame().GetUIManager().CloseMenu(MENU_BETTERGROUPS);
			}
			else {
				GetGame().GetUIManager().EnterScriptedMenu(MENU_BETTERGROUPS, NULL);
			}
		}

		if(key == KeyCode.KC_5) {
			Utilities.SendRPCSafe("RPC_TeleportAllPlayers", new Param1< string >( "test" ));
		}

		if(key == KeyCode.KC_NUMPAD8) {
			//Increase update rate
			Utilities.SendRPCSafe("RPC_ChangeUpdateRate", new Param1< float >( 0.05 ));
		}

		if(key == KeyCode.KC_NUMPAD5) {
			//Dec update rate
			Utilities.SendRPCSafe("RPC_ChangeUpdateRate", new Param1< float >( -0.05 ));
		}
		
	}



};