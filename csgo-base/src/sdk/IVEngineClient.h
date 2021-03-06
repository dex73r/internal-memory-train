#pragma once
#include <Windows.h>
#include "../vector.h"
#include "../util/interface_utils.h"

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define MAX_FLOWS 2 // in & out

class INetChannelInfo
{
  public:
	enum
	{
		GENERIC = 0,  // must be first and is default group
		LOCALPLAYER,  // bytes for local player entity update
		OTHERPLAYERS, // bytes for other players update
		ENTITIES,	 // all other entity bytes
		SOUNDS,		  // game sounds
		EVENTS,		  // event messages
		USERMESSAGES, // user messages
		ENTMESSAGES,  // entity messages
		VOICE,		  // voice data
		STRINGTABLE,  // a stringtable update
		MOVE,		  // client move cmds
		STRINGCMD,	// string command
		SIGNON,		  // various signondata
		TOTAL,		  // must be last and is not a real group
	};

	virtual const char *GetName(void) const = 0;	// get channel name
	virtual const char *GetAddress(void) const = 0; // get channel IP address as string
	virtual float GetTime(void) const = 0;			// current net time
	virtual float GetTimeConnected(void) const = 0; // get connection time in seconds
	virtual int GetBufferSize(void) const = 0;		// netchannel packet history size
	virtual int GetDataRate(void) const = 0;		// send data rate in byte/sec

	virtual bool IsLoopback(void) const = 0;  // true if loopback channel
	virtual bool IsTimingOut(void) const = 0; // true if timing out
	virtual bool IsPlayback(void) const = 0;  // true if demo playback

	virtual float GetLatency(int flow) const = 0;								   // current latency (RTT), more accurate but jittering
	virtual float GetAvgLatency(int flow) const = 0;							   // average packet latency in seconds
	virtual float GetAvgLoss(int flow) const = 0;								   // avg packet loss[0..1]
	virtual float GetAvgChoke(int flow) const = 0;								   // avg packet choke[0..1]
	virtual float GetAvgData(int flow) const = 0;								   // data flow in bytes/sec
	virtual float GetAvgPackets(int flow) const = 0;							   // avg packets/sec
	virtual int GetTotalData(int flow) const = 0;								   // total flow in/out in bytes
	virtual int GetSequenceNr(int flow) const = 0;								   // last send seq number
	virtual bool IsValidPacket(int flow, int frame_number) const = 0;			   // true if packet was not lost/dropped/chocked/flushed
	virtual float GetPacketTime(int flow, int frame_number) const = 0;			   // time when packet was send
	virtual int GetPacketBytes(int flow, int frame_number, int group) const = 0;   // group size of this packet
	virtual bool GetStreamProgress(int flow, int *received, int *total) const = 0; // TCP progress if transmitting
	virtual float GetTimeSinceLastReceived(void) const = 0;						   // get time since last recieved packet in seconds
	virtual float GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float GetTimeoutSeconds() const = 0;
};

// EngineClient
class IVEngineClient
{
  public:
	int GetLocalBase()
	{
		return util::getvfunc<int(__thiscall *)(void *)>(this, 12)(this);
	}
	void GetViewAngles(Vector &v)
	{ // get angle
		return util::getvfunc<void(__thiscall *)(void *, Vector &)>(this, 18)(this, v);
	}
	void SetViewAngles(Vector &v)
	{
		return util::getvfunc<void(__thiscall *)(void *, Vector &)>(this, 19)(this, v);
	}
	int GetMaxClients()
	{
		return util::getvfunc<int(__thiscall *)(void *)>(this, 20)(this);
	}
	bool IsInGame()
	{
		return util::getvfunc<bool(__thiscall *)(void *)>(this, 26)(this);
	}
	bool IsConnected()
	{
		return util::getvfunc<bool(__thiscall *)(void *)>(this, 27)(this);
	}

	typedef struct player_info_s
	{
		unsigned long long unknown;
		unsigned long long xuid;
		char name[128];
		int userID;
		char guid[32 + 1];
		unsigned int friendsID;
		char friendsName[128];
		bool fakeplayer;
		bool ishltv;
		unsigned long customFiles[4];
		unsigned char filesDownloaded;
	} player_info_t;

	void GetScreenSize(int &width, int &height)
	{
		typedef void(__thiscall * OriginalFn)(void *, int &, int &);
		return util::getvfunc<OriginalFn>(this, 5)(this, width, height);
	}
	void ClientCmd(const char *szCmdString)
	{
		typedef void(__thiscall * OriginalFn)(void *, const char *);
		util::getvfunc<OriginalFn>(this, 7)(this, szCmdString);
	}
	bool isinGame(void)
	{
		typedef bool(__thiscall * OriginalFn)(void *);
		return util::getvfunc<OriginalFn>(this, 26)(this);
	}
	bool GetPlayerInfo(int iIndex, player_info_t *pInfo)
	{
		typedef bool(__thiscall * OriginalFn)(void *, int, player_info_t *);
		return util::getvfunc<OriginalFn>(this, 8)(this, iIndex, pInfo);
	}
	int GetLocalPlayer(VOID)
	{
		typedef int(__thiscall * OriginalFn)(void *);
		return util::getvfunc<OriginalFn>(this, 12)(this);
	}
	float GetLastTimeStamp(VOID)
	{
		typedef float(__thiscall * OriginalFn)(void *);
		return util::getvfunc<OriginalFn>(this, 14)(this);
	}
	//virtual const VMatrix&        WorldToScreenMatrix() = 0;
	const matrix3x4_t &WorldToScreenMatrix(void)
	{
		typedef const matrix3x4_t &(__thiscall * w2sFn)(void *);
		return util::getvfunc<w2sFn>(this, 37)(this);
	}
	void ExecuteClientCmd(char const *szCommand)
	{
		typedef void(__thiscall * OriginalFn)(void *, char const *szCommand);
		util::getvfunc<OriginalFn>(this, 108)(this, szCommand);
	}
	void GetServerTick(void)
	{
		typedef void(__thiscall * OriginalFn)(void *);
		return util::getvfunc<OriginalFn>(this, 208)(this);
	}
};

extern IVEngineClient *g_pEngine;