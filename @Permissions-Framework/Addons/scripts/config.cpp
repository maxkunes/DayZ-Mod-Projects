class CfgPatches
{
    class JM_Permissions_Framework_Scripts
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
            "RPC_Scripts", "DZ_Data"
        };
    };
};

class CfgMods
{
    class JM_Permissions_Framework
    {
        dir = "JM";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Permissions Framework";
        credits = "Jacob_Mango";
        author = "Jacob_Mango";
        authorID = "0"; 
        version = "1.0"; 
        extra = 0;
        type = "mod";
        
        dependencies[] = {"Game", "World", "Mission"};
        
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"JM/PermissionsFramework/Scripts/3_Game"};
            };

            class worldScriptModule
            {
                value = "";
                files[] = {"JM/PermissionsFramework/Scripts/4_World"};
            };

            class missionScriptModule
            {
                value = "";
                files[] = {"JM/PermissionsFramework/Scripts/5_Mission"};
            };
        }
    };
};