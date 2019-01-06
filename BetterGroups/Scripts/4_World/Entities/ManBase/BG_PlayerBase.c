modded class PlayerBase {
	string m_GroupUID = "";

	bool InNetworkBubble() {
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());

		if(!localPlayer)
			return false;

		if(vector.Distance(localPlayer.GetPosition(), GetPosition()) < 1000)
			return true; // 1000m network bubble.

		return false;
	}	

	float DistanceFromLocalPlayer() {
		PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());

		if(!localPlayer)
			return -1;

		return vector.Distance(localPlayer.GetPosition(), GetPosition());
	}

}