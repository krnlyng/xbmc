#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <atomic>
#include <utility>
#include "cores/IPlayer.h"
#include "cores/VideoPlayer/VideoRenderers/RenderManager.h"
#include "threads/Thread.h"
#include "IVideoPlayer.h"
#include "DVDMessageQueue.h"
#include "DVDClock.h"
#include "VideoPlayerVideo.h"
#include "VideoPlayerSubtitle.h"
#include "VideoPlayerTeletext.h"
#include "VideoPlayerRadioRDS.h"
#include "Edl.h"
#include "FileItem.h"
#include "system.h"
#include "threads/SystemClock.h"
#include "threads/Thread.h"
#include "utils/StreamDetails.h"
#include "guilib/DispResource.h"

#ifdef HAS_OMXPLAYER
#include "OMXCore.h"
#include "OMXClock.h"
#include "linux/RBP.h"
#else


// dummy class to avoid ifdefs where calls are made
class OMXClock
{
public:
  bool OMXInitialize(CDVDClock *clock) { return false; }
  void OMXDeinitialize() {}
  bool OMXIsPaused() { return false; }
  bool OMXStop(bool lock = true) { return false; }
  bool OMXStep(int steps = 1, bool lock = true) { return false; }
  bool OMXReset(bool has_video, bool has_audio, bool lock = true) { return false; }
  double OMXMediaTime(bool lock = true) { return 0.0; }
  double OMXClockAdjustment(bool lock = true) { return 0.0; }
  bool OMXMediaTime(double pts, bool lock = true) { return false; }
  bool OMXPause(bool lock = true) { return false; }
  bool OMXResume(bool lock = true) { return false; }
  bool OMXSetSpeed(int speed, bool lock = true, bool pause_resume = false) { return false; }
  bool OMXFlush(bool lock = true) { return false; }
  bool OMXStateExecute(bool lock = true) { return false; }
  void OMXStateIdle(bool lock = true) {}
  bool HDMIClockSync(bool lock = true) { return false; }
  void OMXSetSpeedAdjust(double adjust, bool lock = true) {}
};
#endif

struct SOmxPlayerState
{
  OMXClock av_clock;              // openmax clock component
  EDEINTERLACEMODE current_deinterlace; // whether deinterlace is currently enabled
  EINTERLACEMETHOD interlace_method; // current deinterlace method
  bool bOmxWaitVideo;             // whether we need to wait for video to play out on EOS
  bool bOmxWaitAudio;             // whether we need to wait for audio to play out on EOS
  bool bOmxSentEOFs;              // flag if we've send EOFs to audio/video players
  float threshold;                // current fifo threshold required to come out of buffering
  unsigned int last_check_time;   // we periodically check for gpu underrun
  double stamp;                   // last media timestamp
};

class CDVDInputStream;

class CDVDDemux;
class CDemuxStreamVideo;
class CDemuxStreamAudio;
class CStreamInfo;
class CDVDDemuxCC;
class CVideoPlayer;

namespace PVR
{
  class CPVRChannel;
}

#define DVDSTATE_NORMAL           0x00000001 // normal dvd state
#define DVDSTATE_STILL            0x00000002 // currently displaying a still frame
#define DVDSTATE_WAIT             0x00000003 // waiting for demuxer read error
#define DVDSTATE_SEEK             0x00000004 // we are finishing a seek request

class CCurrentStream
{
public:
  int64_t demuxerId; // demuxer's id of current playing stream
  int id;     // id of current playing stream
  int source;
  double dts;    // last dts from demuxer, used to find disncontinuities
  double dur;    // last frame expected duration
  int dispTime; // display time from input stream
  CDVDStreamInfo hint;   // stream hints, used to notice stream changes
  void* stream; // pointer or integer, identifying stream playing. if it changes stream changed
  int changes; // remembered counter from stream to track codec changes
  bool inited;
  unsigned int packets;
  IDVDStreamPlayer::ESyncState syncState;
  double starttime;
  double cachetime;
  double cachetotal;
  const StreamType type;
  const int player;
  // stuff to handle starting after seek
  double startpts;
  double lastdts;

  enum
  {
    AV_SYNC_NONE,
    AV_SYNC_CHECK,
    AV_SYNC_CONT,
    AV_SYNC_FORCE
  } avsync;

  CCurrentStream(StreamType t, int i)
    : type(t)
    , player(i)
  {
    Clear();
  }

  void Clear()
  {
    id = -1;
    demuxerId = -1;
    source = STREAM_SOURCE_NONE;
    dts = DVD_NOPTS_VALUE;
    dur = DVD_NOPTS_VALUE;
    hint.Clear();
    stream = NULL;
    changes = 0;
    inited = false;
    packets = 0;
    syncState = IDVDStreamPlayer::SYNC_STARTING;
    starttime = DVD_NOPTS_VALUE;
    startpts = DVD_NOPTS_VALUE;
    lastdts = DVD_NOPTS_VALUE;
    avsync = AV_SYNC_FORCE;
  }

  double dts_end()
  {
    if(dts == DVD_NOPTS_VALUE)
      return DVD_NOPTS_VALUE;
    if(dur == DVD_NOPTS_VALUE)
      return dts;
    return dts + dur;
  }
};

typedef struct SelectionStream
{
  StreamType   type = STREAM_NONE;
  int          type_index = 0;
  std::string  filename;
  std::string  filename2;  // for vobsub subtitles, 2 files are necessary (idx/sub)
  std::string  language;
  std::string  name;
  CDemuxStream::EFlags flags = CDemuxStream::FLAG_NONE;
  int          source = 0;
  int          id = 0;
  int64_t      demuxerId = -1;
  std::string  codec;
  int          channels = 0;
  int          bitrate = 0;
  int          width = 0;
  int          height = 0;
  CRect        SrcRect;
  CRect        DestRect;
  std::string  stereo_mode;
  float        aspect_ratio = 0.0f;
} SelectionStream;

typedef std::vector<SelectionStream> SelectionStreams;

class CSelectionStreams
{
  SelectionStream  m_invalid;
public:
  CSelectionStreams()
  {
    m_invalid.id = -1;
    m_invalid.source = STREAM_SOURCE_NONE;
    m_invalid.type = STREAM_NONE;
  }
  std::vector<SelectionStream> m_Streams;
  CCriticalSection m_section;

  int              IndexOf (StreamType type, int source, int64_t demuxerId, int id) const;
  int              IndexOf (StreamType type, const CVideoPlayer& p) const;
  int              Count   (StreamType type) const { return IndexOf(type, STREAM_SOURCE_NONE, -1, -1) + 1; }
  int              CountSource(StreamType type, StreamSource source) const;
  SelectionStream& Get     (StreamType type, int index);
  bool             Get     (StreamType type, CDemuxStream::EFlags flag, SelectionStream& out);

  /**
   * Note: calling this method makes the whole instance thread-unsafe.
   */
  SelectionStreams Get(StreamType type);

  /**
   * Note: calling this method makes the whole instance thread-unsafe.
   */
  template<typename Compare> SelectionStreams Get(StreamType type, Compare compare)
  {
    SelectionStreams streams = Get(type);
    std::stable_sort(streams.begin(), streams.end(), compare);
    return streams;
  }

  void             Clear   (StreamType type, StreamSource source);
  int              Source  (StreamSource source, std::string filename);

  void             Update  (SelectionStream& s);
  void             Update  (CDVDInputStream* input, CDVDDemux* demuxer, std::string filename2 = "");
};

class CProcessInfo;

class CVideoPlayer : public IPlayer, public CThread, public IVideoPlayer, public IDispResource, public IRenderMsg
{
public:
  CVideoPlayer(IPlayerCallback& callback);
  virtual ~CVideoPlayer();
  bool OpenFile(const CFileItem& file, const CPlayerOptions &options) override;
  bool CloseFile(bool reopen = false) override;
  bool IsPlaying() const override;
  void Pause() override;
  bool IsPaused() const override;
  bool HasVideo() const override;
  bool HasAudio() const override;
  bool HasRDS() const override;
  bool IsPassthrough() const override;
  bool CanSeek() override;
  void Seek(bool bPlus, bool bLargeStep, bool bChapterOverride) override;
  bool SeekScene(bool bPlus = true) override;
  void SeekPercentage(float iPercent) override;
  float GetPercentage() override;
  float GetCachePercentage() override;

  void SetVolume(float nVolume) override { m_VideoPlayerAudio->SetVolume(nVolume); }
  void SetMute(bool bOnOff) override { m_VideoPlayerAudio->SetMute(bOnOff); }
  void SetDynamicRangeCompression(long drc) override { m_VideoPlayerAudio->SetDynamicRangeCompression(drc); }
  bool CanRecord() override;
  bool IsRecording() override;
  bool CanPause() override;
  bool Record(bool bOnOff) override;
  void SetAVDelay(float fValue = 0.0f) override;
  float GetAVDelay() override;
  bool IsInMenu() const override;
  bool HasMenu() const override;

  void SetSubTitleDelay(float fValue = 0.0f) override;
  float GetSubTitleDelay() override;
  int GetSubtitleCount() override;
  int GetSubtitle() override;
  void GetSubtitleStreamInfo(int index, SPlayerSubtitleStreamInfo &info) override;
  void SetSubtitle(int iStream) override;
  bool GetSubtitleVisible() override;
  void SetSubtitleVisible(bool bVisible) override;
  void AddSubtitle(const std::string& strSubPath) override;

  int GetAudioStreamCount() override;
  int GetAudioStream() override;
  void SetAudioStream(int iStream) override;

  int GetVideoStream() const override;
  int GetVideoStreamCount() const override;
  void GetVideoStreamInfo(int streamId, SPlayerVideoStreamInfo &info) override;
  void SetVideoStream(int iStream) override;

  TextCacheStruct_t* GetTeletextCache() override;
  void LoadPage(int p, int sp, unsigned char* buffer) override;

  std::string GetRadioText(unsigned int line) override;

  int  GetChapterCount() override;
  int  GetChapter() override;
  void GetChapterName(std::string& strChapterName, int chapterIdx=-1) override;
  int64_t GetChapterPos(int chapterIdx=-1) override;
  int  SeekChapter(int iChapter) override;

  void SeekTime(int64_t iTime) override;
  bool SeekTimeRelative(int64_t iTime) override;
  int64_t GetTime() override;
  int64_t GetTotalTime() override;
  void ToFFRW(int iSpeed) override;
  bool OnAction(const CAction &action) override;

  int GetSourceBitrate() override;
  bool GetStreamDetails(CStreamDetails &details) override;
  void GetAudioStreamInfo(int index, SPlayerAudioStreamInfo &info) override;

  std::string GetPlayerState() override;
  bool SetPlayerState(const std::string& state) override;

  std::string GetPlayingTitle() override;

  bool SwitchChannel(const PVR::CPVRChannelPtr &channel) override;

  void FrameMove() override;
  bool HasFrame() override;
  void Render(bool clear, uint32_t alpha = 255, bool gui = true) override;
  void FlushRenderer() override;
  void SetRenderViewMode(int mode) override;
  float GetRenderAspectRatio() override;
  void TriggerUpdateResolution() override;
  bool IsRenderingVideo() override;
  bool IsRenderingGuiLayer() override;
  bool IsRenderingVideoLayer() override;
  bool Supports(EDEINTERLACEMODE mode) override;
  bool Supports(EINTERLACEMETHOD method) override;
  bool Supports(ESCALINGMETHOD method) override;
  bool Supports(ERENDERFEATURE feature) override;

  unsigned int RenderCaptureAlloc() override;
  void RenderCapture(unsigned int captureId, unsigned int width, unsigned int height, int flags) override;
  void RenderCaptureRelease(unsigned int captureId) override;
  bool RenderCaptureGetPixels(unsigned int captureId, unsigned int millis, uint8_t *buffer, unsigned int size) override;

  // IDispResource interface
  void OnLostDisplay() override;
  void OnResetDisplay() override;

  enum ECacheState
  { CACHESTATE_DONE = 0
  , CACHESTATE_FULL     // player is filling up the demux queue
  , CACHESTATE_INIT     // player is waiting for first packet of each stream
  , CACHESTATE_PLAY     // player is waiting for players to not be stalled
  , CACHESTATE_FLUSH    // temporary state player will choose startup between init or full
  };

  bool IsCaching() const override { return m_caching > CACHESTATE_DONE && m_caching < CACHESTATE_PLAY; }
  int GetCacheLevel() const override;

  int OnDVDNavResult(void* pData, int iMessage) override;

  bool ControlsVolume() override {return m_omxplayer_mode;}

protected:
  friend class CSelectionStreams;

  void OnStartup() override;
  void OnExit() override;
  void Process() override;
  void VideoParamsChange() override;
  void GetDebugInfo(std::string &audio, std::string &video, std::string &general) override;

  void CreatePlayers();
  void DestroyPlayers();

  bool OpenStream(CCurrentStream& current, int64_t demuxerId, int iStream, int source, bool reset = true);
  bool OpenAudioStream(CDVDStreamInfo& hint, bool reset = true);
  bool OpenVideoStream(CDVDStreamInfo& hint, bool reset = true);
  bool OpenSubtitleStream(CDVDStreamInfo& hint);
  bool OpenTeletextStream(CDVDStreamInfo& hint);
  bool OpenRadioRDSStream(CDVDStreamInfo& hint);

  /** \brief Switches forced subtitles to forced subtitles matching the language of the current audio track.
  *          If these are not available, subtitles are disabled.
  *   \return true if the subtitles were changed, false otherwise.
  */
  bool AdaptForcedSubtitles();
  bool CloseStream(CCurrentStream& current, bool bWaitForBuffers);

  bool CheckIsCurrent(CCurrentStream& current, CDemuxStream* stream, DemuxPacket* pkg);
  void ProcessPacket(CDemuxStream* pStream, DemuxPacket* pPacket);
  void ProcessAudioData(CDemuxStream* pStream, DemuxPacket* pPacket);
  void ProcessVideoData(CDemuxStream* pStream, DemuxPacket* pPacket);
  void ProcessSubData(CDemuxStream* pStream, DemuxPacket* pPacket);
  void ProcessTeletextData(CDemuxStream* pStream, DemuxPacket* pPacket);
  void ProcessRadioRDSData(CDemuxStream* pStream, DemuxPacket* pPacket);

  bool ShowPVRChannelInfo();

  int  AddSubtitleFile(const std::string& filename, const std::string& subfilename = "");
  void SetSubtitleVisibleInternal(bool bVisible);

  /**
   * one of the DVD_PLAYSPEED defines
   */
  void SetPlaySpeed(int iSpeed);
  int GetPlaySpeed()                                                { return m_playSpeed; }
  void SetCaching(ECacheState state);

  int64_t GetTotalTimeInMsec();

  double GetQueueTime();
  bool GetCachingTimes(double& play_left, double& cache_left, double& file_offset);


  void FlushBuffers(bool queued, double pts = DVD_NOPTS_VALUE, bool accurate = true, bool sync = true);

  void HandleMessages();
  void HandlePlaySpeed();
  bool IsInMenuInternal() const;

  void SynchronizePlayers(unsigned int sources);
  void SynchronizeDemuxer(unsigned int timeout);
  void CheckAutoSceneSkip();
  bool CheckContinuity(CCurrentStream& current, DemuxPacket* pPacket);
  bool CheckSceneSkip(CCurrentStream& current);
  bool CheckPlayerInit(CCurrentStream& current);
  void UpdateCorrection(DemuxPacket* pkt, double correction);
  void UpdateTimestamps(CCurrentStream& current, DemuxPacket* pPacket);
  IDVDStreamPlayer* GetStreamPlayer(unsigned int player);
  void SendPlayerMessage(CDVDMsg* pMsg, unsigned int target);

  bool ReadPacket(DemuxPacket*& packet, CDemuxStream*& stream);
  bool IsValidStream(CCurrentStream& stream);
  bool IsBetterStream(CCurrentStream& current, CDemuxStream* stream);
  void CheckBetterStream(CCurrentStream& current, CDemuxStream* stream);
  void CheckStreamChanges(CCurrentStream& current, CDemuxStream* stream);
  bool CheckDelayedChannelEntry(void);

  bool OpenInputStream();
  bool OpenDemuxStream();
  void OpenDefaultStreams(bool reset = true);

  void UpdateApplication(double timeout);
  void UpdatePlayState(double timeout);
  void UpdateStreamInfos();
  void GetGeneralInfo(std::string& strVideoInfo);

  double m_UpdateApplication;

  bool m_players_created;
  bool m_bAbortRequest;

  ECacheState  m_caching;
  XbmcThreads::EndTime m_cachingTimer;
  CFileItem    m_item;
  XbmcThreads::EndTime m_ChannelEntryTimeOut;
  CProcessInfo *m_processInfo;

  CCurrentStream m_CurrentAudio;
  CCurrentStream m_CurrentVideo;
  CCurrentStream m_CurrentSubtitle;
  CCurrentStream m_CurrentTeletext;
  CCurrentStream m_CurrentRadioRDS;

  CSelectionStreams m_SelectionStreams;

  int m_playSpeed;
  int m_streamPlayerSpeed;
  struct SSpeedState
  {
    double  lastpts;  // holds last display pts during ff/rw operations
    int64_t lasttime;
    int lastseekpts;
    double  lastabstime;
  } m_SpeedState;

  int m_errorCount;
  double m_offset_pts;

  CDVDMessageQueue m_messenger;     // thread messenger

  IDVDStreamPlayerVideo *m_VideoPlayerVideo; // video part
  IDVDStreamPlayerAudio *m_VideoPlayerAudio; // audio part
  CVideoPlayerSubtitle *m_VideoPlayerSubtitle; // subtitle part
  CDVDTeletextData *m_VideoPlayerTeletext; // teletext part
  CDVDRadioRDSData *m_VideoPlayerRadioRDS; // rds part

  CDVDClock m_clock;                // master clock
  CDVDOverlayContainer m_overlayContainer;

  CDVDInputStream* m_pInputStream;  // input stream for current playing file
  CDVDDemux* m_pDemuxer;            // demuxer for current playing file
  CDVDDemux* m_pSubtitleDemuxer;
  CDVDDemuxCC* m_pCCDemuxer;

  CRenderManager m_renderManager;

  struct SDVDInfo
  {
    void Clear()
    {
      state                =  DVDSTATE_NORMAL;
      iSelectedSPUStream   = -1;
      iSelectedAudioStream = -1;
      iSelectedVideoStream = -1;
      iDVDStillTime        =  0;
      iDVDStillStartTime   =  0;
      syncClock = false;
    }

    int state;                // current dvdstate
    bool syncClock;
    unsigned int iDVDStillTime;      // total time in ticks we should display the still before continuing
    unsigned int iDVDStillStartTime; // time in ticks when we started the still
    int iSelectedSPUStream;   // mpeg stream id, or -1 if disabled
    int iSelectedAudioStream; // mpeg stream id, or -1 if disabled
    int iSelectedVideoStream; // mpeg stream id or angle, -1 if disabled
  } m_dvd;

  friend class CVideoPlayerVideo;
  friend class CVideoPlayerAudio;
#ifdef HAS_OMXPLAYER
  friend class OMXPlayerVideo;
  friend class OMXPlayerAudio;
#endif

  SPlayerState m_State;
  CCriticalSection m_StateSection;
  XbmcThreads::EndTime m_syncTimer;

  CEvent m_ready;

  CEdl m_Edl;
  bool m_SkipCommercials;

  struct SEdlAutoSkipMarkers {

    void Clear()
    {
      cut = -1;
      commbreak_start = -1;
      commbreak_end = -1;
      seek_to_start = false;
      mute = false;
    }

    int cut;              // last automatically skipped EDL cut seek position
    int commbreak_start;  // start time of the last commercial break automatically skipped
    int commbreak_end;    // end time of the last commercial break automatically skipped
    bool seek_to_start;   // whether seeking can go back to the start of a previously skipped break
    bool mute;            // whether EDL mute is on

  } m_EdlAutoSkipMarkers;

  CPlayerOptions m_PlayerOptions;

  bool m_HasVideo;
  bool m_HasAudio;

  std::atomic<bool> m_displayLost;

  // omxplayer variables
  struct SOmxPlayerState m_OmxPlayerState;
  bool m_omxplayer_mode;            // using omxplayer acceleration

  XbmcThreads::EndTime m_player_status_timer;
};
