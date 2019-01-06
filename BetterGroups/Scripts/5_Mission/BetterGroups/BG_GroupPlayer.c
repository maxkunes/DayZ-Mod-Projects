class BG_GroupPlayer {
	string m_UID = BETTERGROUPS_INVALID_STRING;
	string m_Name = BETTERGROUPS_INVALID_STRING;
	vector m_Position = Vector(0,0,0);
	PlayerBase m_Player = NULL;
	bool m_bInNetworkBubble = false;
	bool m_bInMyGroup = false;
	bool m_bInvitedMe = false;
	bool m_bSentInviteTo = false;
	bool m_bGroupOwner = false;


	void BG_GroupPlayer(ref PlayerBase player) {
		PlayerIdentity identity = player.GetIdentity();
		m_UID = identity.GetId();
		m_Name = identity.GetName();
		m_Position = player.GetPosition();
		m_Player = player;
	}

	void RemoveFromGroupLocal() {
		m_bInMyGroup = false;
		m_bSentInviteTo = false;
		m_bInvitedMe = false;
		m_bGroupOwner = false;
	}




/*
	bool InNetworkBubble() {
		if(!m_Player)
			return false;

		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());

		if(!localPlayer)
			return false;

		if(vector.Distance(localPlayer.GetPosition(), m_Player.GetPosition()) < 1000)
			return true; // 1000m network bubble.

		return false;
	}	


	float DistanceFromLocalPlayer() {
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());

		if(!localPlayer)
			return -1;

		return vector.Distance(localPlayer.GetPosition(), m_Player.GetPosition());
	}
*/

}