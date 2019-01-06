class CfgPatches
{
	class bettergroups_scripts
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"RPC_Scripts", "JM_Permissions_Framework_Scripts", "DZ_Data"
		};
	};
};

class CfgMods
{
	class bettergroups
	{
		
		dir = "BetterGroups";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "Better Groups";
		credits = "0";
		author = "0";
		authorID = "0"; 
		version = ""; 
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};
		
		class defs
		{	
			class gameScriptModule
			{
				value = "";
				files[] = {"BetterGroups/Scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"BetterGroups/Scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"BetterGroups/Scripts/5_Mission"};
			};
		};
	};
};