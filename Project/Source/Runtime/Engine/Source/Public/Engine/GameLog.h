#pragma once

struct PLog
{
	PLog() = default;

	PLog(std::string InString)
		: LogString(InString)
	{
	}

	PLog(std::string InString, float InExpireTime)
		: LogString(InString)
		, ExpireTime(InExpireTime)
	{
		LogHash = std::hash<std::string>()(InString);
	}

public:
	bool IsLogFinished(float CurrentTime) const;

	std::string LogString;
	std::size_t LogHash = Math::InvalidHash;
	float ExpireTime = -1.0f;
};

struct PLogCompare
{
	bool operator()(const PLog& a, const PLog& b) {
		return a.ExpireTime > b.ExpireTime;
	}
};

struct PLogTime
{
	PLogTime() = default;

	PLogTime(float CurrTime)
	{
		double intTime = 0.0f;
		double floatTime = 0.0f;
		floatTime = std::modf(CurrTime/1000, &intTime);
		
		min = static_cast<int> (static_cast<int>(intTime) / 60);
		sec = static_cast<int> (static_cast<int>(intTime) % 60);
		dotsec = static_cast<int> (std::floor(floatTime * 100));
	}

	const std::string to_string()
	{
		std::string Return;
		if (min < 10)
		{
			Return += "0";
		}
		Return += std::to_string(min);
		Return += ":";

		if (sec < 10)
		{
			Return += "0";
		}
		Return += std::to_string(sec);
		Return += ".";

		if (dotsec < 10)
		{
			Return += "0";
		}
		Return += std::to_string(dotsec);

		return Return;
	}

	int min = 0;
	int sec = 0;
	int dotsec = 0;
};

class PLogs
{
public:
	PLogs()
		: LastFrameTime(0.0f)
		, LogHeader("PLOG")
		, LogColor(LinearColor::Black)
		, FilePath("C:\\Work\\PGameEngine\\Output\\Logs\\Log.txt")
	{}

	void SetLogPath(std::string NewFilePath);
	void SetLogHeader(std::string NewLogHeader);
	void SetLogColor(LinearColor NewColor);

	void PushLog(const std::string& LogString, bool bShowScreen);
	void PushLog(const std::string& LogString, float LifeTime);

	void MakeLogOutString(const PLog& TargetLog, std::string& LogOutString, float CurrentTime);

	void WriteToFile(const std::string& LogString);

	void UpdateLogs(float CurrentTime, std::vector<std::pair<std::size_t, std::string>>& LogsToAdd,  std::vector<std::size_t>& LogHashToDelete);

private:
	bool IsFirstLog = true;
	float LastFrameTime;
	std::string LogHeader;
	LinearColor LogColor;
	std::string FilePath;

	std::priority_queue<PLog, std::vector<PLog>, PLogCompare> LogsPQ;
	
	std::vector<std::pair<std::size_t, std::string>> LogsToUpdate;
};