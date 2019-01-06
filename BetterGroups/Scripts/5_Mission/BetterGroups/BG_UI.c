class BetterGroupsUI extends UIScriptedMenu
{
    private Widget m_GUIRoot;
	private TextListboxWidget m_PlayerList;
	private TextListboxWidget m_GroupList;

	//buttons
	private ButtonWidget m_AcceptButton;
	private ButtonWidget m_RejectButton;
	private ButtonWidget m_InviteButton;
	private ButtonWidget m_CancelInvite;
	private ButtonWidget m_CancelAllInvites;
	private ButtonWidget m_KickButton;
	private ButtonWidget m_LeaveButton;

	void BetterGroupsUI()
	{
	}
	
	void ~BetterGroupsUI()
	{
		
	}

	override Widget Init()
	{
		m_GUIRoot = GetGame().GetWorkspace().CreateWidgets("BetterGroups\\Scripts\\5_Mission\\BetterGroups\\Layouts\\BetterGroupUI.layout");

		m_PlayerList = TextListboxWidget.Cast(m_GUIRoot.FindAnyWidget("PlayerListbox"));
		m_GroupList = TextListboxWidget.Cast(m_GUIRoot.FindAnyWidget("GroupListbox"));

		m_AcceptButton = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("AcceptInviteButton"));
		m_RejectButton = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("RejectInviteButton"));
		m_InviteButton = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("InviteButton"));
		m_CancelInvite = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("CancelInvite"));
		m_CancelAllInvites = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("CancelAllInvites"));
		m_KickButton = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("KickButton"));
		m_LeaveButton = ButtonWidget.Cast(m_GUIRoot.FindAnyWidget("LeaveButton"));

		return m_GUIRoot;
	}

	void UpdatePlayerList() {

		m_PlayerList.ClearItems();
		m_GroupList.ClearItems();

		for(int i = 0; i < GetBetterGroups().m_BGPlayers.Count(); i++) {
			ref BG_GroupPlayer groupPlayer = GetBetterGroups().m_BGPlayers.GetElement(i);

			if(groupPlayer) {

				m_PlayerList.AddItem(groupPlayer.m_Name, NULL, 0);

				if(groupPlayer.m_bInMyGroup) {
					int row = m_GroupList.AddItem(groupPlayer.m_Name, NULL, 0);

					if(groupPlayer.m_bGroupOwner)
						m_GroupList.SetItem(row, "Group Owner", NULL, 1);
				}

				if(groupPlayer.m_bInvitedMe) {
					m_PlayerList.SetItem(i, "Invite Pending", NULL, 1);
				}
				else if(groupPlayer.m_bSentInviteTo) {
					m_PlayerList.SetItem(i, "Invite Sent", NULL, 1);
				}

			}

		}

	}

	override void OnShow()
	{
		UpdatePlayerList();
		super.OnShow();
		PPEffects.SetBlurMenu(0.5);
		PlayerBase player = g_Game.GetPlayer();
		player.GetInputController().SetDisabled(true);
		GetGame().GetMission().PlayerControlDisable();
		GetGame().GetUIManager().ShowUICursor(true);
        GetGame().GetUIManager().ShowCursor(true);
        GetGame().GetInput().ChangeGameFocus( 1 );
		SetFocus( m_GUIRoot );
	}



	override void OnHide()
	{
		super.OnHide();
		
		PPEffects.SetBlurMenu(0);

		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		player.GetInputController().SetDisabled(false);

		GetGame().GetUIManager().ShowCursor(false);
        GetGame().GetUIManager().ShowUICursor(false);
        GetGame().GetInput().ResetGameFocus();
		GetGame().GetMission().PlayerControlEnable();
		
		GetGame().GetUIManager().Back();
		Close();

	}
	

	ref BG_GroupPlayer GetSelectedPlayer_PlayerList() {
		int selectedRow = m_PlayerList.GetSelectedRow();

		if(selectedRow < 0)
			return NULL;

		return GetBetterGroups().m_BGPlayers.GetElement(selectedRow);
	}

	
	override bool OnClick(Widget w, int x, int y, int button)
    {
    	super.OnClick(w, x,y, button);
 
 		ButtonWidget buttonInst = ButtonWidget.Cast(w);

 		if(buttonInst) {
 			GetGame().ChatPlayer(1, "OnClick : " + buttonInst.GetName());
 		}

		
 		ref BG_GroupPlayer playerListSelectedPlayer = GetSelectedPlayer_PlayerList();

    	if(w == m_AcceptButton) {
    		if(playerListSelectedPlayer)
    			Utilities.SendRPCSafe("RPC_OnAcceptInvite", new Param1< string >( playerListSelectedPlayer.m_UID ));
    	}
    	else if(w == m_RejectButton) {
    		if(playerListSelectedPlayer)
    			Utilities.SendRPCSafe("RPC_OnRejectInvite", new Param1< string >( playerListSelectedPlayer.m_UID ));
    	}
    	else if(w == m_InviteButton) {
    		if(playerListSelectedPlayer) {
				playerListSelectedPlayer.m_bSentInviteTo = true;
				Utilities.SendRPCSafe("RPC_OnInvitePlayer", new Param1< string >( playerListSelectedPlayer.m_UID ));
			}
    	}
    	else if(w == m_CancelInvite) {
    		if(playerListSelectedPlayer) {
				playerListSelectedPlayer.m_bSentInviteTo = false;
				Utilities.SendRPCSafe("RPC_OnCancelInvitePlayer", new Param1< string >( playerListSelectedPlayer.m_UID ));
			}
    	}
    	else if(w == m_CancelAllInvites) {
    		for(int i = 0; i < GetBetterGroups().m_BGPlayers.Count(); i++) {
				ref BG_GroupPlayer groupPlayer = GetBetterGroups().m_BGPlayers.GetElement(i);
				groupPlayer.m_bSentInviteTo = false;
				Utilities.SendRPCSafe("RPC_OnCancelInvitePlayer", new Param1< string >( groupPlayer.m_UID ));
			}
    	}
    	else if(w == m_KickButton) {

    	}
    	else if(w == m_LeaveButton) {

    	}

 
        return false;
    }

}
