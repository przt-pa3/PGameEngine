#include "Precompiled.h"

bool PLog::IsLogFinished(float CurrentTime) const
{
	return CurrentTime >= ExpireTime;
}

void PLogs::SetLogPath(std::string NewFilePath)
{
	FilePath = NewFilePath;
}

void PLogs::SetLogHeader(std::string NewLogHeader)
{
	LogHeader = NewLogHeader;
}

void PLogs::SetLogColor(LinearColor NewColor)
{
	LogColor = NewColor;
}

void PLogs::PushLog(const std::string& LogString, bool bShowScreen)
{
	PLog NewLog(LogString);
	std::string LogOutString;
	MakeLogOutString(NewLog, LogOutString, LastFrameTime);
	WriteToFile(LogOutString);

	if (bShowScreen)
	{
		LogsToUpdate.push_back(std::make_pair(NewLog.LogHash, LogOutString));
	}
}

void PLogs::PushLog(const std::string& LogString, float LifeTime)
{
	PLog NewLog(LogString, LastFrameTime + LifeTime);
	std::string LogOutString;
	MakeLogOutString(NewLog, LogOutString, LastFrameTime);
	WriteToFile(LogOutString);

	if (LifeTime <= 500.0f)
	{
		return;
	}
	
	LogsPQ.push(NewLog);
	LogsToUpdate.push_back(std::make_pair(NewLog.LogHash, LogOutString));
}

void PLogs::MakeLogOutString(const PLog& TargetLog, std::string& LogOutString, float CurrentTime)
{
	LogOutString.clear();
	
	PLogTime InLogTime(CurrentTime);

	LogOutString = InLogTime.to_string() + ") " + LogHeader + " ||  " + TargetLog.LogString;
}

void PLogs::WriteToFile(const std::string& LogString)
{
	//ofsLog << LogString << std::endl;
	std::ofstream ofs("C:\\Work\\PGameEngine\\Output\\Logs\\Log.txt", std::ios::out | std::ios::app);
	if (IsFirstLog)
	{
		ofs << "=========================================" << std::endl;
		IsFirstLog = false;
	}
	ofs << LogString << std::endl;
}



void PLogs::UpdateLogs(float CurrentTime, std::vector<std::pair<std::size_t, std::string>>& LogsToAdd, std::vector<std::size_t>& LogHashToDelete)
{
	LastFrameTime = CurrentTime;

	for (auto& LogToAdd : LogsToUpdate)
	{
		LogsToAdd.push_back(LogToAdd);
	}

	while (!LogsPQ.empty())
	{
		if (LogsPQ.top().IsLogFinished(CurrentTime))
		{
			LogHashToDelete.push_back(LogsPQ.top().LogHash);
			LogsPQ.pop();
		}
		else
		{
			break;
		}
	}

	LogsToUpdate.clear();
}

