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

		/*
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnPlayerJoined", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnPlayerDisconnect", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnAcceptInvite", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnRejectInvite", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnInvitePlayer", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnCancelInvitePlayer", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnUpdateGroupPlayerData", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_TeleportAllPlayers", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_ChangeUpdateRate", this, SingeplayerExecutionType.Both );
		GetRPCManager().AddRPC( "BetterGroups", "RPC_OnPlayerJoinedEntity", this, SingeplayerExecutionType.Both );
		*/
	}
	
	void BetterGroupsPrint(string text) {
		Print("~~~~~~~~~ Better Groups ~~~~~~~~~");
		Print(text);
		Print("~~~~~~~~~ Better Groups ~~~~~~~~~");
		GetGame().ChatPlayer(1, text);
	}
	
	void OnUpdate(float timeslice) {

	}

	void OnPlayerDisconnected(string UID) {
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
		}
		else {
			BetterGroupsLog.WarnLog(string.Format("Disconnected player %1 was not in a group, no need to do anything.", UID));
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
		BetterGroupsLog.WarnLog(string.Format("GetInviteIndex looking for inviter %1, and invitee %2", inviter, invitee));
		for(int i = 0; i < m_GroupInvites.Count(); i++) {
			BG_Invite invite = m_GroupInvites.Get(i);

			m_GroupInvites.Debug();
			BetterGroupsLog.WarnLog(string.Format("Invite %1", invite));


			BetterGroupsLog.WarnLog(string.Format("Invite %1, Inviter %2, Invitee %3", invite.ToString(), invite.m_Inviter, invite.m_Invitee));
			if(invite.m_Inviter == inviter && invite.m_Invitee == invitee) {
				return i;
			}
		}
		return INDEX_NOT_FOUND;
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
	
	void RPC_OnInvitePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )

	{ 
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
		if(type == CallType.Client) {
			
			ref Param1< ref BG_Group > clientData;
    		if ( !ctx.Read( clientData ) ) return;

    		ref BG_Group group = clientData.param1;

    		array<string> groupMembers = new array<string>();
    		groupMembers.Copy(group.m_Members);

    		foreach(string member : groupMembers) {
    			ref BG_GroupPlayer player = m_BGPlayers.Get(member);

    			if(player) {
    				player.m_bInMyGroup = true;
    				player.m_bInvitedMe = false;
    				player.m_bSentInviteTo = false;

    				if(member == group.m_Owner) {
    					player.m_bGroupOwner = true;
    				}
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
		if(type == CallType.Server) {
			
			ref Param1< string > serverData;
    		if ( !ctx.Read( serverData ) ) return;

    		OnAcceptInviteServer(serverData, sender);
		}
	}

	void RPC_OnPlayerJoined( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
	{ 
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