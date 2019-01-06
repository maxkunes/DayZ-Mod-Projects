modded class MissionServer
{
	ref array<string> connectedUIDs;

	void MissionServer()
	{
		Print("Server Start");
		GetBetterGroups();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MonitorPlayers, 1000, true);
		connectedUIDs = new ref array<string>();
	}

	//Necessary as the engine functions are buggy as hell. Not efficient at all though.

	void MonitorPlayers() {
		//Idividual functions as scope doesn't work properly right now.
		CheckForConnections();
		CheckForDisconnections();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		
		float curTimeSeconds = GetGame().GetTime() / 1000;
		static float lastTime = 0;

		if(curTimeSeconds - lastTime > POSITION_UPDATE_RATE) {
			lastTime = curTimeSeconds;
			//GetBetterGroups().FastUpdate();
		}
	}


	void CheckForConnections() {
		array<Man> players = new array<Man>();
		if(!players)
			return;

		GetGame().GetPlayers( players );
		//Check for new connections!
		foreach(ref Man player : players) {
			PlayerIdentity identity = player.GetIdentity();

			if(connectedUIDs.Find(identity.GetId()) < 0) {
				// UID is missing, this player must have just connected
				BetterGroups_OnPlayerConnected(player, identity);
				connectedUIDs.Insert(identity.GetId());
			}

		}
	}

	void CheckForDisconnections() {
		array<Man> players = new array<Man>();
		if(!players)
			return;

		GetGame().GetPlayers( players );

		array<string> UIDsToErase = new array<string>();

		//Check for disconnections!
		foreach (string UID : connectedUIDs) {
			bool foundUID = false;

			//forloop as I'm  not sure if break works with foreach
			for(int i = 0; i < players.Count(); i++) {
				Man player = players.Get(i);
				PlayerIdentity identity = player.GetIdentity();

				if(identity.GetId() == UID) {
					foundUID = true;
					break;
				}
			}

			if(!foundUID) {
					// UID is missing from the player array, player must have just disconnected
				BetterGroups_OnPlayerDisconnected(UID);
				UIDsToErase.Insert(UID);
			}

		}

		foreach (string UIDToErase : UIDsToErase) {

			int indexToErase = connectedUIDs.Find(UIDToErase);


			if(indexToErase >= 0)
				connectedUIDs.Remove(indexToErase);
			else {
				BetterGroupsLog.WarnLog(string.Format("Failed to remove player with UID %1 from the connectedUIDs array!", UIDToErase));
			}
		}

	}

	void BetterGroups_OnPlayerConnected(ref Man playerMan, ref PlayerIdentity identity) {
		BetterGroupsLog.DebugLog(string.Format("Player connected with name %1 and uid %2!", identity.GetName(), identity.GetId()));

		PlayerBase player = PlayerBase.Cast(playerMan);

		if(!player) {
			BetterGroupsLog.WarnLog(string.Format("Failed to cast player with UID %1 to a PlayerBase!", identity.GetId()));
			return;
		}

		array<Man> igPlayers = new array<Man>();
		if(!igPlayers)
			return;

		string inputUID = identity.GetId();
		string inputName = identity.GetName();

		GetGame().GetPlayers( igPlayers );

		foreach(Man playerEntry : igPlayers) {
			PlayerIdentity identityEntry = playerEntry.GetIdentity();

			ref BG_GroupPlayer groupEntryPlayer = new BG_GroupPlayer(PlayerBase.Cast(playerEntry));

			if(identityEntry.GetId() != inputUID) {
				Utilities.SendRPCSafe("RPC_OnPlayerJoined", new Param1< ref BG_GroupPlayer >( groupEntryPlayer ), inputUID); 
			} 
		}

		ref BG_GroupPlayer newGroupPlayer = new BG_GroupPlayer(player);

		Utilities.SendRPCSafe("RPC_OnPlayerJoined", new Param1< ref BG_GroupPlayer >( newGroupPlayer ));
		BetterGroupsLog.WarnLog("RPC_OnPlayerJoined Global");
	}

	void BetterGroups_OnPlayerDisconnected(string UID) {
		GetBetterGroups().OnPlayerDisconnected(UID);
		BetterGroupsLog.DebugLog(string.Format("Player disconnected with uid %1!", UID));
		Utilities.SendRPCSafe("RPC_OnPlayerDisconnect", new Param1<string>( UID ));
	}

};