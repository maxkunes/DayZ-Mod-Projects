class BG_Group {
	string m_Owner = BETTERGROUPS_INVALID_STRING;
	ref array<string> m_Members;

	void BG_Group() {
		m_Members = new ref array<string>();
	}

	void OnGroupUpdate() {
		foreach(string member : m_Members) {
			Utilities.SendRPCSafe("RPC_OnGroupUpdate", new Param1< ref BG_Group >( this ), member);
		}
	}

}

class BG_Invite {
	string m_Inviter = BETTERGROUPS_INVALID_STRING;
	string m_Invitee = BETTERGROUPS_INVALID_STRING;

	void BG_Invite(string inviter, string invitee) {
		m_Inviter = inviter;
		m_Invitee = invitee;
	}
}