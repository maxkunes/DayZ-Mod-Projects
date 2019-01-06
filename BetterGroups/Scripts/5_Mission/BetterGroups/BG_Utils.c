
class Utilities {

	static bool IsLoadedIn() {
		return GetGame().IsClient() && GetGame().GetPlayer() != NULL;
	}
	
	static bool IsClient() {
		return GetGame().IsClient();
	}
	
	static bool IsServer() {
		return GetGame().IsServer();
	}

	static string GetLocalUID() {
		return GetGame().GetPlayer().GetIdentity().GetId();
	}
	
	static string GetClientTypeString() {
		bool bIsClient = Utilities.IsClient();
		bool bIsServer = Utilities.IsServer();
		
		string clientType = "Client";
		
		if(bIsClient)
			clientType = "Client";
		else if(bIsServer)
			clientType = "Server";
		else 
			clientType = "Unknown";
		
		return clientType;
	}
	
	static PlayerIdentity GetIdentityFromUID(string UID) {
		array<Man> igPlayers = new array<Man>();


		if(!igPlayers)
			return NULL;

		GetGame().GetPlayers( igPlayers );


		foreach(Man playerEntry : igPlayers) {
			PlayerIdentity identityEntry = playerEntry.GetIdentity();


			if(identityEntry.GetId() == UID)
				return identityEntry;
		}
		return NULL;
	}

	static void SendRPCSafe(string funcName, ref Param params, string toSendUID = "") 
    {
    	string clientServerString = "";

    	if(IsClient()) {
			BetterGroupsLog.DebugLog(string.Format("Client sending %1 rpc to server.", funcName));
    		GetRPCManager().SendRPC( BETTERGROUPS_NAMESPACE , funcName, params, true);
    		return;
    	}
    	else {

    		if(toSendUID != "") {
				BetterGroupsLog.DebugLog(string.Format("Server sending %1 rpc to client with UID %2. Params : %3", funcName, toSendUID, params.ToString()));
    			PlayerIdentity identity = GetIdentityFromUID(toSendUID);

				if(!identity) {
					BetterGroupsLog.WarnLog(string.Format("SendRPCSafe failed to lookup identity with UID %1.", toSendUID));
					return;
				}


			 	GetRPCManager().SendRPC( BETTERGROUPS_NAMESPACE, funcName, params, true, identity);
			}
			else {
				BetterGroupsLog.DebugLog(string.Format("Server sending %1 rpc to all clients.", funcName));
				GetRPCManager().SendRPC( BETTERGROUPS_NAMESPACE , funcName, params, true);
			}
    	}
    }


	
}
