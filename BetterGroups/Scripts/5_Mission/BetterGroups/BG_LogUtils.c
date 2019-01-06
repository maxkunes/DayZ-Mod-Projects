enum BetterGroupsLogFlags {
	LogFlags_NONE,
	LogFlags_SERVER_NO_LOG, 
	LogFlags_CLIENT_NO_LOG,
	LogFlags_DEBUG_TO_CHAT,
}

enum BetterGroupsLogServerity {
	LogServerity_NONE, 
	LogServerity_DEBUG, // Just print to console. Basic debug info.
	LogServerity_WARN, // Something is wrong, probably not critical though. 
	LogServerity_ERROR // Something is wrong that will probably cause a system to break and possible crash. 
}

string ServityToString(BetterGroupsLogServerity severity) {

	switch(severity)
	{
		case BetterGroupsLogServerity.LogServerity_DEBUG:
			return "Debug";
		case BetterGroupsLogServerity.LogServerity_WARN:
			return "Warning";
		case BetterGroupsLogServerity.LogServerity_ERROR:
			return "Error";
	}	
		
	return "";
}

class BetterGroupsLog {
	static ref map<BetterGroupsLogFlags, bool> m_Flags = new ref map<BetterGroupsLogFlags, bool>();
	
	static void SetFlag(BetterGroupsLogFlags changeFlag, bool value) {
		m_Flags.Insert(changeFlag, value);
	}

	static bool CheckFlag(BetterGroupsLogFlags flag) {
		return m_Flags.Get(flag);
	}

	static void InternalLog(string message, BetterGroupsLogServerity severity) {
		
		bool bIsClient = Utilities.IsClient();
		bool bIsServer = Utilities.IsServer();
		
		if(bIsClient && CheckFlag(BetterGroupsLogFlags.LogFlags_CLIENT_NO_LOG)) {
			return;
		}
		
		if(bIsServer && CheckFlag(BetterGroupsLogFlags.LogFlags_SERVER_NO_LOG)) {
			return;
		}

		Print(message);
		
		switch(severity) {
			case BetterGroupsLogServerity.LogServerity_DEBUG:
			{
				if(CheckFlag(BetterGroupsLogFlags.LogFlags_DEBUG_TO_CHAT)) {
					GetGame().ChatPlayer(CCDirect, message);
				}
			}
			break;
			case BetterGroupsLogServerity.LogServerity_WARN:
			{
				GetGame().ChatPlayer(CCAdmin, message);
			}
			break;
			case BetterGroupsLogServerity.LogServerity_ERROR:
			{
				GetGame().ChatPlayer(CCSystem, message);
			}
			break;
			default:
			{

			}
		}

	}

	static void LogPrint(string message, BetterGroupsLogServerity severity) {
		string formattedMessage = string.Format("[%1]:[%2] %3", Utilities.GetClientTypeString(), ServityToString(severity), message);

		InternalLog("~~~~~~ BetterGroups ~~~~~~", severity);
		InternalLog(formattedMessage, severity);	
		InternalLog("~~~~~~ BetterGroups ~~~~~~", severity);
	}

	static void DebugLog(string message) {
		LogPrint(message, BetterGroupsLogServerity.LogServerity_DEBUG);
	}

	static void WarnLog(string message) {
		LogPrint(message, BetterGroupsLogServerity.LogServerity_WARN);
	}

	static void ErrorLog(string message) {
		LogPrint(message, BetterGroupsLogServerity.LogServerity_ERROR);
	}

}