modded class MissionGameplay
{
    protected ref PermissionsFramework m_PermissionsFramework;

    void MissionGameplay()
    {        
        m_PermissionsFramework = new ref PermissionsFramework;
    }

    void ~MissionGameplay()
    {
        delete m_PermissionsFramework;
    }

    override void OnInit()
    {
        super.OnInit();
    }

    override void OnMissionStart()
    {
        if ( !GetGame().IsMultiplayer() )
        {
            GetPermissionsManager().PlayerJoined( NULL );
        }

        super.OnMissionStart();
    
        m_PermissionsFramework.OnStart();
    }

    override void OnMissionFinish()
    {
        m_PermissionsFramework.OnFinish();

        GetGame().GetUIManager().CloseMenu( MENU_INGAME );

        super.OnMissionFinish();
    }

    override void OnUpdate( float timeslice )
    {
        super.OnUpdate( timeslice );

        m_PermissionsFramework.Update( timeslice );
    }
}
