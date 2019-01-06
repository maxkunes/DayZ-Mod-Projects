class BetterGroups
{
	protected bool m_isServer;
	ref map<string, ref BG_GroupPlayer> m_BGPlayers;
	ref array<ref BG_Invite> m_GroupInvites;
	ref array<ref BG_Group> m_Groups;
	// 		Inviter Invitee


	void BetterGroups()
	{
		BetterGroupsLog.SetFlag(BetterGroupsLogFlags.LogFlags_DEBUG_TO_CHAT, true);
		BetterGroupsLog.DebugLog("BetterGroups()");
		m_isServer = GetGame().IsServer();

		InitRPCS();

		m_BGPlayers = new ref map<string, ref BG_GroupPlayer>;
		
		if ( m_isServer )
			InitServer();
		else
			InitClient();
	}
	
	bool IsServer() {
		return m_isServer;
	}

	bool IsClient() {
		return !m_isServer;
	}

	void ~BetterGroups()
	{

	}



	void InitServer()
	{
		m_GroupInvites = new ref array<ref BG_Invite>();
		m_Groups = new ref array<ref BG_Group>();
		return;
	}

	void InitClient()
	{
	
		return;
	}

	void InitRPCS()
	{
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnPlayerJoined", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnPlayerDisconnect", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnInvitePlayer", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnAcceptInvite", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnGroupUpdate", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnRejectInvite", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnCancelInvite", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnKickPlayer", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnLeaveGroup", this, SingeplayerExecutionType.Both );
	}
	
	void BetterGroupsPrint(string text) {
		Print("~~~~~~~~~ Better Groups ~~~~~~~~~");
		Print(text);
		Print("~~~~~~~~~ Better Groups ~~~~~~~~~");
		GetGame().ChatPlayer(1, text);
	}
	
	void OnUpdate(float timeslice) {

	}

	ref BG_GroupPlayer GetPlayerByName(string playerName) {

		foreach(ref BG_GroupPlayer player : m_BGPlayers) {
			if(player.m_Name == playerName) {
				return player;
			}
		}
		return NULL;
	}

	bool RemovePlayerFromGroup(string UID) {
		ref BG_Group group = GetGroup(UID);

		if(group) {
			BetterGroupsLog.WarnLog(string.Format("Disconnected player %1 was in a group. The owner is %2.",UID, group.m_Owner));

			if(group.m_Owner == UID) {
				// The player who disconnected is the owner. We need to give someone else the owner role.
				if(group.m_Members.Count() == 1) {
					BetterGroupsLog.ErrorLog(string.Format("Disconnected player owns a group and is the only person in it, removing the group.", UID));
					m_Groups.Remove(m_Groups.Find(group));
				}
				else {
					group.m_Owner = group.m_Members.Get(1); // Make owner the next person to join the group after the owner;
					group.m_Members.Remove(group.m_Members.Find(UID)); // Remove the owner from the group completely.
				}

			}
			else {
				// The player who disconnected was just a member. We are only going to remove them.

				if(group.m_Members.Count() == 1) {
					BetterGroupsLog.ErrorLog(string.Format("Disconnected player %1 is not the owner of the group but is the only person in it.", UID));
					m_Groups.Remove(m_Groups.Find(group));
				}
				else {
					group.m_Members.Remove(group.m_Members.Find(UID));
				}
			}

			group.OnGroupUpdate();

			if(GetGroup(UID))
				return false; // If they are still in a group, something failed here.
			
			return true;
		}
		return true;
	}

	void OnPlayerDisconnected(string UID) {
		ref BG_Group group = GetGroup(UID);

		if(group) {
			if(!RemovePlayerFromGroup(UID)) {
				BetterGroupsLog.ErrorLog(string.Format("RemovePlayerFromGroup failed for %1.", UID));
			}
			else
				BetterGroupsLog.DebugLog(string.Format("Removed disconnected player %1 from their group.", UID));
		}
		else {
			BetterGroupsLog.DebugLog(string.Format("OnPlayerDisconnected player %1 was not in a group, no need to do anything.", UID));
		}
	}

	ref BG_Group GetGroup(string UID) {
		foreach(ref BG_Group group : m_Groups) {
			if(group.m_Owner == UID)
				return group;

			foreach(string member : group.m_Members) {
				if(member == UID)
					return group;
			}
		}
		return NULL;
	}

	void DebugGroups() {
		foreach(ref BG_Group group : m_Groups) {
			BetterGroupsLog.WarnLog(string.Format("Debugging group with owner %1", group.m_Owner));

			foreach(string member : group.m_Members) {
				BetterGroupsLog.WarnLog(string.Format("Group has member %1", member));
			}
		}
	}

	int GetInviteIndex(string inviter, string invitee) {
		for(int i = 0; i < m_GroupInvites.Count(); i++) {
			ref BG_Invite invite = m_GroupInvites.Get(i);

			if(invite.m_Inviter == inviter && invite.m_Invitee == invitee) {
				return i;
			}
		}
		return INDEX_NOT_FOUND;
	}

	ref BG_Invite GetInvite(string inviter, string invitee) {
		for(int i = 0; i < m_GroupInvites.Count(); i++) {
			ref BG_Invite invite = m_GroupInvites.Get(i);

			if(invite.m_Inviter == inviter && invite.m_Invitee == invitee) {
				invite;
			}
		}
		return NULL;
	}

	void OnInvitePlayerServer(ref Param1< string > serverData, ref PlayerIdentity sender) {
		string invitee = serverData.param1;
    	string inviter = sender.GetId();

    	if(invitee == inviter) {
    		BetterGroupsLog.WarnLog(string.Format("Inviter %1 just tried to invite themselves to a group.", invitee));
    		return;
    	}

    	DebugGroups();

    	if(GetGroup(invitee)) {
    		BetterGroupsLog.WarnLog(string.Format("Cannot invite %1 as they are already in a group.", invitee));
    		return;
    	}

    	m_GroupInvites.Insert(new BG_Invite(inviter, invitee));

    	Utilities.SendRPCSafe("RPC_OnInvitePlayer", new Param1< string >( inviter ), invitee);
	}

	void OnCancelInviteServer(ref Param1< string > serverData, ref PlayerIdentity sender) {
		string inviter = sender.GetId();
		string invitee = serverData.param1;

		ref BG_Invite invite = GetInvite(inviter, invitee);

		if(!invite) {
			BetterGroupsLog.WarnLog(string.Format("OnCancelInviteServer no invite exist with inviter %1 and invitee %2.", inviter, invitee));
    		return;
		}

		m_GroupInvites.Remove(m_GroupInvites.Find(invite));

		BetterGroupsLog.DebugLog(string.Format("OnCancelInviteServer : Removing invite with inviter %1 and invitee %2.", inviter, invitee));

		Utilities.SendRPCSafe("RPC_OnCancelInvite", new Param1< string >( inviter ), invitee);
	}

	void OnRejectInviteServer(ref Param1< string > serverData, ref PlayerIdentity sender) {

		string inviter = serverData.param1;
		string invitee = sender.GetId();

		ref BG_Invite invite = GetInvite(inviter, invitee);

		if(!invite) {
			BetterGroupsLog.WarnLog(string.Format("OnRejectInviteServer no invite exist with inviter %1 and invitee %2.", inviter, invitee));
    		return;
		}

		m_GroupInvites.Remove(m_GroupInvites.Find(invite));

		BetterGroupsLog.DebugLog(string.Format("OnRejectInviteServer : Removing invite with inviter %1 and invitee %2.", inviter, invitee));

		Utilities.SendRPCSafe("RPC_OnRejectInvite", new Param1< string >( invitee ), inviter);
	}

	void OnKickPlayerServer(ref Param1< string > serverData, ref PlayerIdentity sender) {
		string kickee = serverData.param1;
    	string kicker = sender.GetId();

    	if(kickee == kicker) {
    		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 is trying to kick themselves.", kicker));
    		return;
    	}

    	ref BG_Group kickerGroup = GetGroup(kicker);
    	ref BG_Group kickeeGroup = GetGroup(kickee);

    	if(!kickerGroup) {
    		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 is trying to kick %2, but the kicker isn't in a group.", kicker, kickee));
    		return;
    	}

    	if(!kickeeGroup) {
    		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 is trying to kick %2, but the kickee isn't in a group.", kicker, kickee));
    		return;
    	}

    	if(kickerGroup != kickeeGroup) {
    		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 is trying to kick %2, but they are not in the same group.", kicker, kickee));
    		return;
    	}

    	if(kickerGroup.m_Owner != kicker) {
    		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 is trying to kick %2, but the kicker does not own the group.", kicker, kickee));
    		return;
    	}


    	if(RemovePlayerFromGroup(kickee)) {
    		BetterGroupsLog.DebugLog(string.Format("OnKickPlayerServer success : %1 has kicked %2.", kicker, kickee));
    	}
    	else {
    		BetterGroupsLog.DebugLog(string.Format("OnKickPlayerServer failed for %1 trying to kick %2.", kicker, kickee));
    	}

    	Utilities.SendRPCSafe("RPC_OnKickPlayer", new Param1< string >( kicker ), kickee);
	}

	ref BG_GroupPlayer GetSelf()  {
		return m_BGPlayers.Get(GetGame().GetPlayer().GetIdentity().GetId());
	}

	void RPC_OnKickPlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
		BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;

    		OnKickPlayerServer(serverData, sender);
		}
		else if(type == CallType.Client) {
			ref Param1< string > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		string kicker = clientData.param1;

    		if(!m_BGPlayers.Get(kicker)) {
    			BetterGroupsLog.WarnLog(string.Format("RPC_OnKickPlayer UID : %1 who is trying to kick me doesn't exist in the m_BGPlayers map.", kicker));
    			return;
    		}
    		m_BGPlayers.Get(kicker).RemoveFromGroupLocal();

    		//I'm listed as in the group. We need to fix this. 
    		GetSelf().RemoveFromGroupLocal();
		}
	}

	void OnLeaveGroupServer(ref Param1< string > serverData, ref PlayerIdentity sender) {
		string leaver = sender.GetId();

		ref BG_Group leaverGroup = GetGroup(leaver);

		if(!leaverGroup) {
			BetterGroupsLog.WarnLog(string.Format("OnLeaveGroupServer UID : %1 is trying to leave a group, but they are not in one.", leaver));
    		return;
		}

		if(!RemovePlayerFromGroup(leaver)) {
			BetterGroupsLog.WarnLog(string.Format("RemovePlayerFromGroup UID : %1 failed to remove player from group when they tried to leave it.", leaver));
			return
		}
		else {
			BetterGroupsLog.DebugLog(string.Format("OnLeaveGroupServer success! %1 has left the group they were in.", leaver));
		}

		Utilities.SendRPCSafe("RPC_OnLeaveGroup", new Param1< string >( leaver ), leaver);
	}

	void RPC_OnLeaveGroup( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
		BetterGroupsLog.WarnLog(string.Format("RPC_OnLeaveGroup on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;
    		OnLeaveGroupServer(serverData, sender);
		}
		else if (type == CallType.Client) {
			ref Param1< string > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		// I need to leave any group I'm in.
    		foreach(ref BG_GroupPlayer player : m_BGPlayers) {
    			// This also remove me from the group. Which is good.
    			player.RemoveFromGroupLocal();
    		}
		}
 	}

	void RPC_OnRejectInvite( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
		BetterGroupsLog.WarnLog(string.Format("RPC_OnRejectInvite on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;

    		OnRejectInviteServer(serverData, sender);
		}
		else {
			ref Param1< string > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		string invitee = clientData.param1;

    		if(!m_BGPlayers.Get(invitee)) {
    			BetterGroupsLog.WarnLog(string.Format("RPC_OnRejectInvite UID : %1 who I invited m_BGPlayers map.", invitee));
    			return;
    		}

    		m_BGPlayers.Get(invitee).m_bSentInviteTo = false;
		}
	}

	void RPC_OnCancelInvite( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
		BetterGroupsLog.WarnLog(string.Format("RPC_OnCancelInvite on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;


    		OnInvitePlayerServer(serverData, sender);
    		
		}
		else {
			ref Param1< string > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		string inviter = clientData.param1;

    		if(!m_BGPlayers.Get(inviter)) {
    			BetterGroupsLog.WarnLog(string.Format("RPC_OnCancelInvite UID : %1 who invited me doesn't exist in the m_BGPlayers map.", inviter));
    			return;
    		}

    		m_BGPlayers.Get(inviter).m_bInvitedMe = false;
		}
	}
	
	void RPC_OnInvitePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
		BetterGroupsLog.WarnLog(string.Format("RPC_OnInvitePlayer on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;


    		OnInvitePlayerServer(serverData, sender);
    		
		}
		else {
			ref Param1< string > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		string inviter = clientData.param1;

    		if(!m_BGPlayers.Get(inviter)) {
    			BetterGroupsLog.WarnLog(string.Format("RPC_OnInvitePlayer UID : %1 who invited me doesn't exist in the m_BGPlayers map.", inviter));
    			return;
    		}

    		m_BGPlayers.Get(inviter).m_bInvitedMe = true;
		}
	}


	void RPC_OnGroupUpdate( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
		BetterGroupsLog.WarnLog(string.Format("RPC_OnGroupUpdate on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Client) {
			
			ref Param1< ref BG_Group > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		ref BG_Group group = clientData.param1;

    		array<string> groupMembers = new array<string>();
    		groupMembers.Copy(group.m_Members);

    		foreach(string member : groupMembers) {
    			ref BG_GroupPlayer groupPlayer = m_BGPlayers.Get(member);

    			if(groupPlayer) {
    				groupPlayer.m_bInMyGroup = true;
    				groupPlayer.m_bInvitedMe = false;
    				groupPlayer.m_bSentInviteTo = false;

    				if(member == group.m_Owner) {
    					groupPlayer.m_bGroupOwner = true;
    				}
    			}
    		}
			
			foreach(ref BG_GroupPlayer player : m_BGPlayers) {
				//If player is not in our group.
				if(groupMembers.Find(player.m_UID) == INDEX_NOT_FOUND) {
					player.RemoveFromGroupLocal();
				}
			}
    		
		}
	}

	void OnAcceptInviteServer(ref Param1< string > serverData, ref PlayerIdentity sender) {
		string inviter = serverData.param1;
    	string invitee = sender.GetId();

    	int inviteIndex = GetInviteIndex(inviter, invitee);

    	if(inviteIndex == INDEX_NOT_FOUND) {
    		BetterGroupsLog.WarnLog(string.Format("OnAcceptInviteServer invitee %1 just tried to accept an invite that doesn't exist.", invitee));
    		return;
    	}

    	m_GroupInvites.Remove(inviteIndex);

    	ref BG_Group group = GetGroup(inviter);

    	BetterGroupsLog.WarnLog(string.Format("OnAcceptInviteServer GetGroup(inviter) returned %1.", group));

    	if(!group) {
    		BetterGroupsLog.WarnLog(string.Format("%1 did not have a group, creating one.", inviter));

    		group = new BG_Group();
    		group.m_Owner = inviter;
    		group.m_Members.Insert(inviter);
    		m_Groups.Insert(group);
    	}

    	group.m_Members.Insert(invitee);
    	group.OnGroupUpdate();
	}

	void RPC_OnAcceptInvite( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
		BetterGroupsLog.WarnLog(string.Format("RPC_OnAcceptInvite on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;

    		OnAcceptInviteServer(serverData, sender);
		}
	}

	void RPC_OnPlayerJoined( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
		BetterGroupsLog.WarnLog(string.Format("RPC_OnPlayerJoined on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Client) {
			
			ref Param1< ref BG_GroupPlayer > data;
    		if ( !ctx.Read( data ) ) return;

    		ref BG_GroupPlayer groupPlayer = data.param1;

    		m_BGPlayers.Insert(groupPlayer.m_UID, groupPlayer);

    		BetterGroupsLog.DebugLog(string.Format("RPC_OnPlayerJoined Player name %1, Player Position %2.", groupPlayer.m_Name, groupPlayer.m_Position));
    		BetterGroupsLog.DebugLog(string.Format("m_BGPlayers map has grown. Current size %1.", m_BGPlayers.Count()));
		}
	}


	void RPC_OnPlayerDisconnect( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
		BetterGroupsLog.WarnLog(string.Format("RPC_OnPlayerDisconnect on %1", Utilities.GetClientTypeString()));
		if(type == CallType.Client) {
			
			ref Param1< string > data;
    		if ( !ctx.Read( data ) ) return;

    		string uidOfDisconnect = data.param1;

    		if(!m_BGPlayers.Get(uidOfDisconnect)) {
    			BetterGroupsLog.WarnLog(string.Format("RPC_OnPlayerDisconnect m_BGPlayers does not have an entry with uid of %1.", uidOfDisconnect));
    			return;
    		}

    		m_BGPlayers.Remove(uidOfDisconnect);

    		BetterGroupsLog.DebugLog(string.Format("m_BGPlayers map has shrunk. Current size %1.", m_BGPlayers.Count()));

		}
	}



}



ref BetterGroups g_BetterGroups;

BetterGroups GetBetterGroups() {
	if(!g_BetterGroups) {
		g_BetterGroups = new BetterGroups();
	}
	return g_BetterGroups;
}