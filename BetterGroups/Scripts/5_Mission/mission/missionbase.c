modded class MissionBase
{
    override UIScriptedMenu CreateScriptedMenu(int id)
    {
        UIScriptedMenu menu = NULL;

		if(id == MENU_BETTERGROUPS) {
			menu = new BetterGroupsUI();
		}
		else {
        	menu = super.CreateScriptedMenu(id);
		}

		menu.SetID(id);
		
        return menu;
    }
}