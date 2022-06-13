/******************************************************************************
    Copyright (C) 2013-2014 by Hugh Bailey <obs.jim@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#pragma once

#include <QBuffer>
#include <QAction>
#include <QWidgetAction>
#include <QSystemTrayIcon>
#include <QStyledItemDelegate>
#include <QProcess>
#include <obs.hpp>
#include <vector>
#include <memory>
#include <qstackedlayout.h>
#include "window-main.hpp"
#include "window-basic-interaction.hpp"
#include "window-basic-properties.hpp"
#include "window-basic-transform.hpp"
#include "window-basic-adv-audio.hpp"
#include "window-basic-filters.hpp"
#include "window-projector.hpp"
#include "window-basic-about.hpp"
#include "auth-base.hpp"
#include "log-viewer.hpp"
#include "qstomp.h"
#include <obs-frontend-internal.hpp>

#include <util/platform.h>
#include <util/threading.h>
#include <util/util.hpp>
#include "url-tab-widget.h"
#include <QPointer>
#include "font-set.h"
#include "msgbox.h"
class QMessageBox;
class QListWidgetItem;
class VolControl;
class OBSBasicStats;
//add by cz
class TopWidget;
class BottomCtrls;
class LeftWidget;
class CustomSlider;
class RecordSaveTip;
class UserInfoWidget;
class QStompClient;
class IMWidget;
class SacrougeDialog;
class SettingDialog;
class RedDetailsDialog;
class WebBrowserWidget;

#include "base-define.h"
#include "asyn-request.h"

#include "ui_OBSBasic.h"
#include "ui_ColorSelect.h"

#define DESKTOP_AUDIO_1 Str("DesktopAudioDevice1")
#define DESKTOP_AUDIO_2 Str("DesktopAudioDevice2")
#define AUX_AUDIO_1 Str("AuxAudioDevice1")
#define AUX_AUDIO_2 Str("AuxAudioDevice2")
#define AUX_AUDIO_3 Str("AuxAudioDevice3")
#define AUX_AUDIO_4 Str("AuxAudioDevice4")

#define SIMPLE_ENCODER_X264 "x264"
#define SIMPLE_ENCODER_X264_LOWCPU "x264_lowcpu"
#define SIMPLE_ENCODER_QSV "qsv"
#define SIMPLE_ENCODER_NVENC "nvenc"
#define SIMPLE_ENCODER_AMD "amd"

#define PREVIEW_EDGE_SIZE 0

struct BasicOutputHandler;

enum class QtDataRole {
	OBSRef = Qt::UserRole,
	OBSSignals,
};

struct SavedProjectorInfo {
	ProjectorType type;
	int monitor;
	std::string geometry;
	std::string name;
	bool alwaysOnTop;
	bool alwaysOnTopOverridden;
};

struct QuickTransition {
	QPushButton *button = nullptr;
	OBSSource source;
	obs_hotkey_id hotkey = OBS_INVALID_HOTKEY_ID;
	int duration = 0;
	int id = 0;
	bool fadeToBlack = false;

	inline QuickTransition() {}
	inline QuickTransition(OBSSource source_, int duration_, int id_,
			       bool fadeToBlack_ = false)
		: source(source_),
		  duration(duration_),
		  id(id_),
		  fadeToBlack(fadeToBlack_),
		  renamedSignal(std::make_shared<OBSSignal>(
			  obs_source_get_signal_handler(source), "rename",
			  SourceRenamed, this))
	{
	}

private:
	static void SourceRenamed(void *param, calldata_t *data);
	std::shared_ptr<OBSSignal> renamedSignal;
};

class ColorSelect : public QWidget {

public:
	explicit ColorSelect(QWidget *parent = 0);

private:
	std::unique_ptr<Ui::ColorSelect> ui;
};

class OBSBasic : public OBSMainWindow {
	Q_OBJECT
	Q_PROPERTY(QIcon imageIcon READ GetImageIcon WRITE SetImageIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon colorIcon READ GetColorIcon WRITE SetColorIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon slideshowIcon READ GetSlideshowIcon WRITE
			   SetSlideshowIcon DESIGNABLE true)
	Q_PROPERTY(QIcon audioInputIcon READ GetAudioInputIcon WRITE
			   SetAudioInputIcon DESIGNABLE true)
	Q_PROPERTY(QIcon audioOutputIcon READ GetAudioOutputIcon WRITE
			   SetAudioOutputIcon DESIGNABLE true)
	Q_PROPERTY(QIcon desktopCapIcon READ GetDesktopCapIcon WRITE
			   SetDesktopCapIcon DESIGNABLE true)
	Q_PROPERTY(QIcon windowCapIcon READ GetWindowCapIcon WRITE
			   SetWindowCapIcon DESIGNABLE true)
	Q_PROPERTY(QIcon gameCapIcon READ GetGameCapIcon WRITE SetGameCapIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon cameraIcon READ GetCameraIcon WRITE SetCameraIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon textIcon READ GetTextIcon WRITE SetTextIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon mediaIcon READ GetMediaIcon WRITE SetMediaIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon browserIcon READ GetBrowserIcon WRITE SetBrowserIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon groupIcon READ GetGroupIcon WRITE SetGroupIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon sceneIcon READ GetSceneIcon WRITE SetSceneIcon
			   DESIGNABLE true)
	Q_PROPERTY(QIcon defaultIcon READ GetDefaultIcon WRITE SetDefaultIcon
			   DESIGNABLE true)

	friend class OBSAbout;
	friend class OBSBasicPreview;
	friend class OBSBasicStatusBar;
	friend class OBSBasicSourceSelect;
	friend class OBSBasicSettings;
	friend class Auth;
	friend class AutoConfig;
	friend class AutoConfigStreamPage;
	friend class RecordButton;
	friend class ReplayBufferButton;
	friend class ExtraBrowsersModel;
	friend class ExtraBrowsersDelegate;
	friend class DeviceCaptureToolbar;
	friend class DeviceToolbarPropertiesThread;
	friend struct BasicOutputHandler;
	friend struct OBSStudioAPI;

	enum class MoveDir { Up, Down, Left, Right };

	enum DropType {
		DropType_RawText,
		DropType_Text,
		DropType_Image,
		DropType_Media,
		DropType_Html,
		DropType_Url,
	};

private:
	obs_frontend_callbacks *api = nullptr;

	std::shared_ptr<Auth> auth;

	std::vector<VolControl *> volumes;

	std::vector<OBSSignal> signalHandlers;

	QList<QPointer<QDockWidget>> extraDocks;

	bool loaded = false;
	long disableSaving = 1;
	bool projectChanged = false;
	bool previewEnabled = true;

	std::list<const char *> copyStrings;
	const char *copyFiltersString = nullptr;
	bool copyVisible = true;

	bool closing = false;
	QScopedPointer<QThread> devicePropertiesThread;
	QScopedPointer<QThread> whatsNewInitThread;
	QScopedPointer<QThread> updateCheckThread;
	QScopedPointer<QThread> introCheckThread;
	QScopedPointer<QThread> logUploadThread;

	QPointer<OBSBasicInteraction> interaction;
	QPointer<OBSBasicProperties> properties;
	QPointer<OBSBasicTransform> transformWindow;
	QPointer<OBSBasicAdvAudio> advAudioWindow;
	QPointer<OBSBasicFilters> filters;
	QPointer<QDockWidget> statsDock;
	QPointer<OBSAbout> about;

	OBSLogViewer *logView = nullptr;

	QPointer<QTimer> cpuUsageTimer;
	QPointer<QTimer> diskFullTimer;

	os_cpu_usage_info_t *cpuUsageInfo = nullptr;

	OBSService service;
	std::unique_ptr<BasicOutputHandler> outputHandler;
	bool streamingStopping = false;
	bool recordingStopping = false;
	bool replayBufferStopping = false;

	gs_vertbuffer_t *box = nullptr;
	gs_vertbuffer_t *boxLeft = nullptr;
	gs_vertbuffer_t *boxTop = nullptr;
	gs_vertbuffer_t *boxRight = nullptr;
	gs_vertbuffer_t *boxBottom = nullptr;
	gs_vertbuffer_t *circle = nullptr;

	bool sceneChanging = false;

	int previewX = 0, previewY = 0;
	int previewCX = 0, previewCY = 0;
	float previewScale = 0.0f;

	ConfigFile basicConfig;

	std::vector<SavedProjectorInfo *> savedProjectorsArray;
	std::vector<OBSProjector *> projectors;

	QPointer<QWidget> stats;
	QPointer<QWidget> remux;
	QPointer<QWidget> extraBrowsers;
	QPointer<QWidget> importer;

	QPointer<QMenu> startStreamMenu;

	QPointer<QPushButton> transitionButton;
	QPointer<QPushButton> replayBufferButton;
	QPointer<QHBoxLayout> replayLayout;
	QScopedPointer<QPushButton> pause;
	QScopedPointer<QPushButton> replay;

	QPointer<QPushButton> vcamButton;
	bool vcamEnabled = false;

	QSystemTrayIcon* trayIcon;
	QPointer<QAction> sysTrayStream;
	QPointer<QAction> sysTrayRecord;
	QPointer<QAction> sysTrayReplayBuffer;
	QPointer<QAction> showHide;
	QPointer<QAction> exit;
	QPointer<QMenu> trayMenu;
	QPointer<QMenu> previewProjector;
	QPointer<QMenu> studioProgramProjector;
	QPointer<QMenu> multiviewProjectorMenu;
	QPointer<QMenu> previewProjectorSource;
	QPointer<QMenu> previewProjectorMain;
	QPointer<QMenu> sceneProjectorMenu;
	QPointer<QMenu> sourceProjector;
	QPointer<QMenu> scaleFilteringMenu;
	QPointer<QMenu> colorMenu;
	QPointer<QWidgetAction> colorWidgetAction;
	QPointer<ColorSelect> colorSelect;
	QPointer<QMenu> deinterlaceMenu;
	QPointer<QMenu> perSceneTransitionMenu;
	QPointer<QObject> shortcutFilter;
	QPointer<QAction> renameScene;
	QPointer<QAction> renameSource;

	QPointer<QWidget> programWidget;
	QPointer<QVBoxLayout> programLayout;
	QPointer<QLabel> programLabel;

	QScopedPointer<QThread> patronJsonThread;
	std::string patronJson;

	void UpdateMultiviewProjectorMenu();

	void DrawBackdrop(float cx, float cy);

	void SetupEncoders();

	void CreateFirstRunSources();
	void CreateDefaultScene(bool firstStart);

	void UpdateVolumeControlsDecayRate();
	void UpdateVolumeControlsPeakMeterType();
	void ClearVolumeControls();

	void UploadLog(const char *subdir, const char *file, const bool crash);

	void Save(const char *file);
	void Load(const char *file);

	void InitHotkeys();
	void CreateHotkeys();
	void ClearHotkeys();

	bool InitService();

	bool InitBasicConfigDefaults();
	void InitBasicConfigDefaults2();
	bool InitBasicConfig();

	void InitOBSCallbacks();

	void InitPrimitives();

	void OnFirstLoad();

	OBSSceneItem GetSceneItem(QListWidgetItem *item);
	OBSSceneItem GetCurrentSceneItem();

	bool QueryRemoveSource(obs_source_t *source);

	void TimedCheckForUpdates();
	void CheckForUpdates(bool manualUpdate);

	void GetFPSCommon(uint32_t &num, uint32_t &den) const;
	void GetFPSInteger(uint32_t &num, uint32_t &den) const;
	void GetFPSFraction(uint32_t &num, uint32_t &den) const;
	void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;
	void GetConfigFPS(uint32_t &num, uint32_t &den) const;

	void UpdatePreviewScalingMenu();

	void LoadSceneListOrder(obs_data_array_t *array);
	obs_data_array_t *SaveSceneListOrder();
	void ChangeSceneIndex(bool relative, int idx, int invalidIdx);

	void TempFileOutput(const char *path, int vBitrate, int aBitrate);
	void TempStreamOutput(const char *url, const char *key, int vBitrate,
			      int aBitrate);

	void CloseDialogs();
	void ClearSceneData();
	void ClearProjectors();

	void Nudge(int dist, MoveDir dir);

	OBSProjector *OpenProjector(obs_source_t *source, int monitor,
				    ProjectorType type);

	void GetAudioSourceFilters();
	void GetAudioSourceProperties();
	void VolControlContextMenu();
	void ToggleVolControlLayout();
	void ToggleMixerLayout(bool vertical);

	void RefreshSceneCollections();
	void ChangeSceneCollection();
	void LogScenes();

	void LoadProfile();
	void ResetProfileData();
	bool AddProfile(bool create_new, const char *title, const char *text,
			const char *init_text = nullptr, bool rename = false);
	void DeleteProfile(const char *profile_name, const char *profile_dir);
	void RefreshProfiles();
	void ChangeProfile();
	void CheckForSimpleModeX264Fallback();

	void SaveProjectNow();

	int GetTopSelectedSourceItem();

	QModelIndexList GetAllSelectedSourceItems();

	obs_hotkey_pair_id streamingHotkeys, recordingHotkeys, pauseHotkeys,
		replayBufHotkeys, vcamHotkeys, togglePreviewHotkeys,
		contextBarHotkeys;
	obs_hotkey_id forceStreamingStopHotkey;

	void InitDefaultTransitions();
	void InitTransition(obs_source_t *transition);
	obs_source_t *FindTransition(const char *name);
	OBSSource GetCurrentTransition();
	obs_data_array_t *SaveTransitions();
	void LoadTransitions(obs_data_array_t *transitions);

	obs_source_t *fadeTransition;
	obs_source_t *cutTransition;

	void CreateProgramDisplay();
	void CreateProgramOptions();
	int TransitionCount();
	int AddTransitionBeforeSeparator(const QString &name,
					 obs_source_t *source);
	void AddQuickTransitionId(int id);
	void AddQuickTransition();
	void AddQuickTransitionHotkey(QuickTransition *qt);
	void RemoveQuickTransitionHotkey(QuickTransition *qt);
	void LoadQuickTransitions(obs_data_array_t *array);
	obs_data_array_t *SaveQuickTransitions();
	void ClearQuickTransitionWidgets();
	void RefreshQuickTransitions();
	void DisableQuickTransitionWidgets();
	void EnableTransitionWidgets(bool enable);
	void CreateDefaultQuickTransitions();

	QMenu *CreatePerSceneTransitionMenu();

	QuickTransition *GetQuickTransition(int id);
	int GetQuickTransitionIdx(int id);
	QMenu *CreateTransitionMenu(QWidget *parent, QuickTransition *qt);
	void ClearQuickTransitions();
	void QuickTransitionClicked();
	void QuickTransitionChange();
	void QuickTransitionChangeDuration(int value);
	void QuickTransitionRemoveClicked();

	void SetPreviewProgramMode(bool enabled);
	void ResizeProgram(uint32_t cx, uint32_t cy);
	void SetCurrentScene(obs_scene_t *scene, bool force = false);
	static void RenderProgram(void *data, uint32_t cx, uint32_t cy);

	std::vector<QuickTransition> quickTransitions;
	QPointer<QWidget> programOptions;
	QPointer<OBSQTDisplay> program;
	OBSWeakSource lastScene;
	OBSWeakSource swapScene;
	OBSWeakSource programScene;
	bool editPropertiesMode = false;
	bool sceneDuplicationMode = true;
	bool swapScenesMode = true;
	volatile bool previewProgramMode = false;
	obs_hotkey_id togglePreviewProgramHotkey = 0;
	obs_hotkey_id transitionHotkey = 0;
	obs_hotkey_id statsHotkey = 0;
	obs_hotkey_id screenshotHotkey = 0;
	obs_hotkey_id sourceScreenshotHotkey = 0;
	int quickTransitionIdCounter = 1;
	bool overridingTransition = false;

	int programX = 0, programY = 0;
	int programCX = 0, programCY = 0;
	float programScale = 0.0f;

	int disableOutputsRef = 0;

	inline void OnActivate();
	inline void OnDeactivate();

	void AddDropSource(const char *file, DropType image);
	void AddDropURL(const char *url, QString &name, obs_data_t *settings,
			const obs_video_info &ovi);
	void ConfirmDropUrl(const QString &url);
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	void ReplayBufferClicked();

	bool sysTrayMinimizeToTray();

	void EnumDialogs();

	QList<QDialog *> visDialogs;
	QList<QDialog *> modalDialogs;
	QList<QMessageBox *> visMsgBoxes;

	QList<QPoint> visDlgPositions;

	QByteArray startingDockLayout;

	obs_data_array_t *SaveProjectors();
	void LoadSavedProjectors(obs_data_array_t *savedProjectors);

	void ReceivedIntroJson(const QString &text);
	void ShowWhatsNew(const QString &url);

#ifdef BROWSER_AVAILABLE
	QList<QSharedPointer<QDockWidget>> extraBrowserDocks;
	QList<QSharedPointer<QAction>> extraBrowserDockActions;
	QStringList extraBrowserDockTargets;

	void ClearExtraBrowserDocks();
	void LoadExtraBrowserDocks();
	void SaveExtraBrowserDocks();
	void ManageExtraBrowserDocks();
	void AddExtraBrowserDock(const QString &title, const QString &url,
				 bool firstCreate);
#endif

	QIcon imageIcon;
	QIcon colorIcon;
	QIcon slideshowIcon;
	QIcon audioInputIcon;
	QIcon audioOutputIcon;
	QIcon desktopCapIcon;
	QIcon windowCapIcon;
	QIcon gameCapIcon;
	QIcon cameraIcon;
	QIcon textIcon;
	QIcon mediaIcon;
	QIcon browserIcon;
	QIcon groupIcon;
	QIcon sceneIcon;
	QIcon defaultIcon;

	QIcon GetImageIcon() const;
	QIcon GetColorIcon() const;
	QIcon GetSlideshowIcon() const;
	QIcon GetAudioInputIcon() const;
	QIcon GetAudioOutputIcon() const;
	QIcon GetDesktopCapIcon() const;
	QIcon GetWindowCapIcon() const;
	QIcon GetGameCapIcon() const;
	QIcon GetCameraIcon() const;
	QIcon GetTextIcon() const;
	QIcon GetMediaIcon() const;
	QIcon GetBrowserIcon() const;
	QIcon GetDefaultIcon() const;

	QSlider *tBar;
	bool tBarActive = false;

	OBSSource GetOverrideTransition(OBSSource source);
	int GetOverrideTransitionDuration(OBSSource source);

	void UpdateProjectorHideCursor();
	void UpdateProjectorAlwaysOnTop(bool top);
	void ResetProjectors();

	QPointer<QObject> screenshotData;

public slots:
	void DeferSaveBegin();
	void DeferSaveEnd();

	void DisplayStreamStartError();

	void StartStreaming();
	void StopStreaming();
	void ForceStopStreaming();

	void StreamDelayStarting(int sec);
	void StreamDelayStopping(int sec);

	void StreamingStart();
	void StreamStopping();
	void StreamingStop(int errorcode, QString last_error);

	void StartRecording();
	void StopRecording();

	void RecordingStart();
	void RecordStopping();
	void RecordingStop(int code, QString last_error);

	void ShowReplayBufferPauseWarning();
	void StartReplayBuffer();
	void StopReplayBuffer();

	void ReplayBufferStart();
	void ReplayBufferSave();
	void ReplayBufferSaved();
	void ReplayBufferStopping();
	void ReplayBufferStop(int code);

	void StartVirtualCam();
	void StopVirtualCam();

	void OnVirtualCamStart();
	void OnVirtualCamStop(int code);

	void SaveProjectDeferred();
	void SaveProject();

	void SetTransition(OBSSource transition);
	void OverrideTransition(OBSSource transition);
	void TransitionToScene(OBSScene scene, bool force = false);
	void TransitionToScene(OBSSource scene, bool force = false,
			       bool quickTransition = false,
			       int quickDuration = 0, bool black = false,
			       bool manual = false);
	void SetCurrentScene(OBSSource scene, bool force = false);

	bool AddSceneCollection(bool create_new,
				const QString &name = QString());

	void UpdatePatronJson(const QString &text, const QString &error);

	void ShowContextBar();
	void HideContextBar();
	void PauseRecording();
	void UnpauseRecording();

private slots:
	void AddSceneItem(OBSSceneItem item);
	void AddScene(OBSSource source);
	void RemoveScene(OBSSource source);
	void RenameSources(OBSSource source, QString newName, QString prevName);

	void ActivateAudioSource(OBSSource source);
	void DeactivateAudioSource(OBSSource source);

	void DuplicateSelectedScene();
	void RemoveSelectedScene();
	void RemoveSelectedSceneItem();

	void ToggleAlwaysOnTop();

	void ReorderSources(OBSScene scene);
	void RefreshSources(OBSScene scene);

	void ProcessHotkey(obs_hotkey_id id, bool pressed);

	void AddTransition(QString id);
	void RenameTransition();
	void TransitionClicked();
	void TransitionStopped();
	void TransitionFullyStopped();
	void TriggerQuickTransition(int id);

	void SetDeinterlacingMode();
	void SetDeinterlacingOrder();

	void SetScaleFilter();

	void IconActivated(QSystemTrayIcon::ActivationReason reason);
	void SetShowing(bool showing);

	void ToggleShowHide();

	void HideAudioControl();
	void UnhideAllAudioControls();
	void ToggleHideMixer();

	void MixerRenameSource();

	void on_vMixerScrollArea_customContextMenuRequested();
	void on_hMixerScrollArea_customContextMenuRequested();

	void on_actionCopySource_triggered();
	void on_actionPasteRef_triggered();
	void on_actionPasteDup_triggered();

	void on_actionCopyFilters_triggered();
	void on_actionPasteFilters_triggered();

	void ColorChange();

	SourceTreeItem *GetItemWidgetFromSceneItem(obs_sceneitem_t *sceneItem);

	void on_actionShowAbout_triggered();

	void AudioMixerCopyFilters();
	void AudioMixerPasteFilters();

	void EnablePreview();
	void DisablePreview();

	void SceneCopyFilters();
	void ScenePasteFilters();

	void CheckDiskSpaceRemaining();
	void OpenSavedProjector(SavedProjectorInfo *info);

	void ScenesReordered();

	void ResetStatsHotkey();

	void SetImageIcon(const QIcon &icon);
	void SetColorIcon(const QIcon &icon);
	void SetSlideshowIcon(const QIcon &icon);
	void SetAudioInputIcon(const QIcon &icon);
	void SetAudioOutputIcon(const QIcon &icon);
	void SetDesktopCapIcon(const QIcon &icon);
	void SetWindowCapIcon(const QIcon &icon);
	void SetGameCapIcon(const QIcon &icon);
	void SetCameraIcon(const QIcon &icon);
	void SetTextIcon(const QIcon &icon);
	void SetMediaIcon(const QIcon &icon);
	void SetBrowserIcon(const QIcon &icon);
	void SetGroupIcon(const QIcon &icon);
	void SetSceneIcon(const QIcon &icon);
	void SetDefaultIcon(const QIcon &icon);

	void TBarChanged(int value);
	void TBarReleased();

	void LockVolumeControl(bool lock);

private:
	/* OBS Callbacks */
	static void SceneReordered(void *data, calldata_t *params);
	static void SceneRefreshed(void *data, calldata_t *params);
	static void SceneItemAdded(void *data, calldata_t *params);
	static void SourceCreated(void *data, calldata_t *params);
	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceActivated(void *data, calldata_t *params);
	static void SourceDeactivated(void *data, calldata_t *params);
	static void SourceAudioActivated(void *data, calldata_t *params);
	static void SourceAudioDeactivated(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);

	void ResizePreview(uint32_t cx, uint32_t cy);

	// add by cjf
	void AddSource(const char *id);
	QMenu *CreateAddSourcePopupMenu();
	void AddSourcePopupMenu(const QPoint &pos);
	void copyActionsDynamicProperties();

	static void HotkeyTriggered(void *data, obs_hotkey_id id, bool pressed);

	void AutoRemux();

	void UpdatePause(bool activate = true);
	void UpdateReplayBuffer(bool activate = true);

	bool OutputPathValid();
	void OutputPathInvalidMessage();

	bool LowDiskSpace();
	void DiskSpaceMessage();

	OBSSource prevFTBSource = nullptr;

public:
	OBSSource GetProgramSource();
	OBSScene GetCurrentScene();

	void SysTrayNotify(const QString &text, QSystemTrayIcon::MessageIcon n);

	inline OBSSource GetCurrentSceneSource()
	{
		OBSScene curScene = GetCurrentScene();
		return OBSSource(obs_scene_get_source(curScene));
	}

	obs_service_t *GetService();
	void SetService(obs_service_t *service);

	int GetTransitionDuration();

	inline bool IsPreviewProgramMode() const
	{
		return os_atomic_load_bool(&previewProgramMode);
	}

	inline bool VCamEnabled() const { return vcamEnabled; }

	bool StreamingActive() const;
	bool Active() const;

	void ResetUI();
	int ResetVideo();
	bool ResetAudio();

	void AddVCamButton();
	void ResetOutputs();

	void ResetAudioDevice(const char *sourceId, const char *deviceId,
			      const char *deviceDesc, int channel);

	void NewProject();
	void LoadProject();

	inline void GetDisplayRect(int &x, int &y, int &cx, int &cy)
	{
		x = previewX;
		y = previewY;
		cx = previewCX;
		cy = previewCY;
	}

	inline bool SavingDisabled() const { return disableSaving; }

	inline double GetCPUUsage() const
	{
		return os_cpu_usage_info_query(cpuUsageInfo);
	}

	void SaveService();
	bool LoadService();

	inline Auth *GetAuth() { return auth.get(); }

	inline void EnableOutputs(bool enable)
	{
		if (enable) {
			if (--disableOutputsRef < 0)
				disableOutputsRef = 0;
		} else {
			disableOutputsRef++;
		}
	}

	QMenu *AddDeinterlacingMenu(QMenu *menu, obs_source_t *source);
	QMenu *AddScaleFilteringMenu(QMenu *menu, obs_sceneitem_t *item);
	QMenu *AddBackgroundColorMenu(QMenu *menu, QWidgetAction *widgetAction,
				      ColorSelect *select,
				      obs_sceneitem_t *item);
	void CreateSourcePopupMenu(int idx, bool preview);

	void UpdateTitleBar();
	void UpdateSceneSelection(OBSSource source);

	void SystemTrayInit();
	void SystemTray(bool firstStarted);

	void OpenSavedProjectors();

	void CreateInteractionWindow(obs_source_t *source);
	void CreatePropertiesWindow(obs_source_t *source);
	void CreateFiltersWindow(obs_source_t *source);
	//add by huchenzhi
	bool CreateSecretWindow(obs_source_t *source);
	bool CreateCameraWindow(obs_source_t *source);

	QAction *AddDockWidget(QDockWidget *dock);

	static OBSBasic *Get();

	const char *GetCurrentOutputPath();

	void DeleteProjector(OBSProjector *projector);
	void AddProjectorMenuMonitors(QMenu *parent, QObject *target,
				      const char *slot);

	QIcon GetSourceIcon(const char *id) const;
	QIcon GetGroupIcon() const;
	QIcon GetSceneIcon() const;

	OBSWeakSource copyFilter = nullptr;

protected:
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void changeEvent(QEvent *event) override;
	virtual bool event(QEvent *event) override;
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private slots:
	void on_actionFullscreenInterface_triggered();

	void on_actionShow_Recordings_triggered();
	void on_actionRemux_triggered();
	void on_action_Settings_triggered();
	void on_actionAdvAudioProperties_triggered();
	void on_advAudioProps_clicked();
	void on_advAudioProps_destroyed();
	void on_actionShowLogs_triggered();
	void on_actionUploadCurrentLog_triggered();
	void on_actionUploadLastLog_triggered();
	void on_actionViewCurrentLog_triggered();
	void on_actionCheckForUpdates_triggered();

	void on_actionShowCrashLogs_triggered();
	void on_actionUploadLastCrashLog_triggered();

	void on_actionEditTransform_triggered();
	void on_actionCopyTransform_triggered();
	void on_actionPasteTransform_triggered();
	void on_actionRotate90CW_triggered();
	void on_actionRotate90CCW_triggered();
	void on_actionRotate180_triggered();
	void on_actionFlipHorizontal_triggered();
	void on_actionFlipVertical_triggered();
	void on_actionFitToScreen_triggered();
	void on_actionStretchToScreen_triggered();
	void on_actionCenterToScreen_triggered();
	void on_actionVerticalCenter_triggered();
	void on_actionHorizontalCenter_triggered();

	void on_customContextMenuRequested(const QPoint &pos);

	void on_scenes_currentItemChanged(QListWidgetItem *current,
					  QListWidgetItem *prev);
	void on_scenes_customContextMenuRequested(const QPoint &pos);
	void on_actionGridMode_triggered();
	void on_actionAddScene_triggered();
	void on_actionRemoveScene_triggered();
	void on_actionSceneUp_triggered();
	void on_actionSceneDown_triggered();
	void on_sources_customContextMenuRequested(const QPoint &pos);
	void on_scenes_itemDoubleClicked(QListWidgetItem *item);
	void on_actionAddSource_triggered();
	void on_actionRemoveSource_triggered();
	void on_actionInteract_triggered();
	void on_actionSourceProperties_triggered();
	void on_actionSourceUp_triggered();
	void on_actionSourceDown_triggered();

	void on_actionMoveUp_triggered();
	void on_actionMoveDown_triggered();
	void on_actionMoveToTop_triggered();
	void on_actionMoveToBottom_triggered();

	void on_actionLockPreview_triggered();

	void on_scalingMenu_aboutToShow();
	void on_actionScaleWindow_triggered();
	void on_actionScaleCanvas_triggered();
	void on_actionScaleOutput_triggered();

	void on_streamButton_clicked();
	void on_recordButton_clicked();
	void VCamButtonClicked();
	void on_settingsButton_clicked();
	void Screenshot(OBSSource source_ = nullptr);
	void ScreenshotSelectedSource();
	void ScreenshotProgram();
	void ScreenshotScene();

	void on_actionHelpPortal_triggered();
	void on_actionWebsite_triggered();
	void on_actionDiscord_triggered();

	void on_preview_customContextMenuRequested(const QPoint &pos);
	void on_program_customContextMenuRequested(const QPoint &pos);
	void PreviewDisabledMenu(const QPoint &pos);

	void on_actionNewSceneCollection_triggered();
	void on_actionDupSceneCollection_triggered();
	void on_actionRenameSceneCollection_triggered();
	void on_actionRemoveSceneCollection_triggered();
	void on_actionImportSceneCollection_triggered();
	void on_actionExportSceneCollection_triggered();

	void on_actionNewProfile_triggered();
	void on_actionDupProfile_triggered();
	void on_actionRenameProfile_triggered();
	void on_actionRemoveProfile_triggered();
	void on_actionImportProfile_triggered();
	void on_actionExportProfile_triggered();

	void on_actionShowSettingsFolder_triggered();
	void on_actionShowProfileFolder_triggered();

	void on_actionAlwaysOnTop_triggered();

	void on_toggleListboxToolbars_toggled(bool visible);
	void on_toggleContextBar_toggled(bool visible);
	void on_toggleStatusBar_toggled(bool visible);
	void on_toggleSourceIcons_toggled(bool visible);

	void on_transitions_currentIndexChanged(int index);
	void on_transitionRemove_clicked();
	void on_transitionProps_clicked();
	void on_transitionDuration_valueChanged(int value);

	void on_modeSwitch_clicked();

	// Source Context Buttons
	void on_sourcePropertiesButton_clicked();
	void on_sourceFiltersButton_clicked();

	void on_autoConfigure_triggered();
	void on_stats_triggered();

	void on_resetUI_triggered();
	void on_lockUI_toggled(bool lock);

	void PauseToggled();

	void logUploadFinished(const QString &text, const QString &error);
	void crashUploadFinished(const QString &text, const QString &error);
	void openLogDialog(const QString &text, const bool crash);

	void updateCheckFinished();

	void AddSourceFromAction();

	void MoveSceneToTop();
	void MoveSceneToBottom();

	void EditSceneName();
	void EditSceneItemName();

	void SceneNameEdited(QWidget *editor,
			     QAbstractItemDelegate::EndEditHint endHint);

	void OpenSceneFilters();
	void OpenFilters();

	void EnablePreviewDisplay(bool enable);
	void TogglePreview();

	void NudgeUp();
	void NudgeDown();
	void NudgeLeft();
	void NudgeRight();

	void OpenStudioProgramProjector();
	void OpenPreviewProjector();
	void OpenSourceProjector();
	void OpenMultiviewProjector();
	void OpenSceneProjector();

	void OpenStudioProgramWindow();
	void OpenPreviewWindow();
	void OpenSourceWindow();
	void OpenMultiviewWindow();
	void OpenSceneWindow();

	void DeferredSysTrayLoad(int requeueCount);

	void StackedMixerAreaContextMenuRequested();

	void ResizeOutputSizeOfSource();
	
public slots:
	void on_actionResetTransform_triggered();

	bool StreamingActive();
	bool RecordingActive();
	bool ReplayBufferActive();

	void ClearContextBar();
	void UpdateContextBar();

	//add by huchenzhi
	void OnRecordSlot(bool start,int total);
	void OnAddSource(QString str);
	void OnMoveSourceItem(void *scene, int move);
	void OnSelectSourceItem(void *currentScene, void *previousScene);
	void OnEditClicked(void *scene);
	void OnDeleteClicked(void *scene);
	void OnRecvNetSlot(const STRspData& stRsp);
	void OnSettingButtonClicked(int id);
	void OnSysButtonClicked(int id);
	void OnCloseWindow();
	void OnExitWindow();
	void OnShowRedEnvelops();

	void OnSocketConnected();
	void OnSocketDisConnected();
	//void OnMessageFrame(const QStompResponseFrame &frame);
	void OnOpenSecret();
	void OnCloseSecret();
	void OnReadStandardOutput();
	void UrlTabSlot(const TabType& type);
	void OnEnterRoolSlot(const QString & roomid,const QString & liveId);
	void OnLiveDetailSlot(const QString & anchorName,const QString& openTimeStart,const QString& openTimeEnd);
	void OnQueryAnchorSlot(const QString & strQueryId);
	void ForBackSlot(const TabType& type);
	void OnCloseRoom();
	void OnUpdateLoginInfo(STCsUser csuser);
	void OnAddFontTxt(const STTxtSet & set);
	void OnFilterChange(int type);
	void OnBeautyFaceChange(int type, int value);
 public:
	explicit OBSBasic(QWidget *parent = 0);
	virtual ~OBSBasic();

	virtual void OBSInit() override;

	virtual config_t *Config() const override;

	virtual int GetProfilePath(char *path, size_t size,
				   const char *file) const override;

	static void InitBrowserPanelSafeBlock();
	//add by hcz
	void SetOriginCtrlsVisible(bool visible);
	/**
	@brief 自定义控件初始化
	@param 无
	@return 无
	*/
	void InitCustomCtrls();
	/**
	@brief 麦克风,音频滑块
	@param 无
	@return 无
	*/
	void InitSlider();
	/**
	@brief 初始化麦克风id,桌面音频id
	@param 无
	@return 无
	*/
	void InitMicAudioId();
	/**
	@brief 停止录制后的提示信息
	@param 无
	@return 无
	*/
	void InitRecordTip();
	/**
	@brief 设置推流地址和直播码
	@param url:推流地址
	@param key:直播码
	@return 无
	*/
	void SetRtmp(const char* url,const char* key);
	/**
	@brief 全屏拉伸
	@param OBSSceneItem item
	@return 无
	*/
	void StretchToScreen(OBSSceneItem item);
	/**
	@brief 移除lefteidget中的item项
	@param OBSSceneItem item
	@return 无
	*/
	void RemoveLeftWidgetItem(OBSSceneItem item);
	/**
	@brief 开始直播和结束直播文本切换
	@param QString
	@return 无
	*/
	void SetLiveBtnText(STRINDEX str, bool enable);
	/**
	@brief 初始化stomp client
	@param 无
	@return 无
	*/
	void InitStompClient();

	/**
	@brief 通用连接绑定
	@param 无
	@return 无
	*/
	void InitConnections();
	/**
	@brief 直播/项目分类
	@param 无
	@return 无
	*/
	void GetLiveList();
	/**
	@brief 开始直播
	@param 无
	@return 无
	*/
	void BeginLive();
	/**
	@brief 连接im-ws
	@param 无
	@return 无
	*/
	void ConnectWebSocket();
	/**
	@brief 订阅
	@param bool
	@return 无
	*/
	void SubscribeChat(bool sub);
	void SendChat();
	/**
	@brief 直播结束关注,收入,总人数信息
	@param void
	@return 无
	*/
	void ShowEndLiveInfo();
	/**
	@brief 结束直播
	@param void
	@return 无
	*/
	void FinishLive();
	/**
	@brief 房间信息
	@param void
	@return 无
	*/
	void GetRoomInfo();
	/**
	@brief 获取礼物列表
	@param void
	@return 无
	*/
	void GetGiftList();
	/**
	@brief stomp messge
	@param void
	@return 无
	*/
	void NotifyMessageBody(const STMessageBody & stbody);
	/**
	@brief 选中lefteidget中的item项
	@param OBSSceneItem item，bool select
	@return 无
	*/
	void SelectLeftWidgetItem(OBSSceneItem item, bool select);
	/**
	@brief 主播是否实名认证
	@param void
	@return void
	*/
	void CheckAnchorPermission();
	/**
	@brief 将开播流程改为有权限开播之后执行
	@param void
	@return void
	*/
	void PermissionStream();
	/**
	@brief 初始化url列表
	@param void
	@return void
	*/
	void InitUrlTab() ;
	/**
	@brief 获取IM token
	@param void
	@return void
	*/
	void GetIMToken();
	/**
	@brief 签出
	@param void
	@return void
	*/
	void DropOut();
	/**
	@brief 初始化url列表对应的url
	@param void
	@return void
	*/
	void InitUrlWebView() ;
	void LoadWebUrl();
	/**
	@brief 暂停直播
	@param void
	@return void
	*/
	void PauseLive() ;
	/**
	@brief 暂停直播
	@param void
	@return void
	*/
	void RePauseLive() ;
	/**
	@brief 设置进入房间状态
	@param bool 
	@return void
	*/
	void SetRoomState(bool enter) ;
	/**
	* 获取直播列表
	*/
	void GetRoomList() ;
	//
	void CleanSource();
	/**
	* 心跳检测
	*/
	void CheckHearBeat() ;
	/**
	* 迁入心跳检测
	*/
	void StartSignInHearBeat();
	void CheckSignInHearBeat() ;
	void MoveItem(obs_sceneitem_t *item);
	/**
	* 移除除摄像头之外的所有source
	*/
	void RemoveNonDShowSources();
	/**
	* 开始直播,结束直播
	*/
	void StartStreamTimer();
	/**
	* s0001弹框
	*/
	void ShowInvalidDialog();
	/**
	* 禁用摄像头
	*/
	void SetCameraEnabled(bool enabled);

 Q_SIGNALS:
	void StreamStateSignal(const StreamState& state);
 private:
	std::unique_ptr<Ui::OBSBasic> ui;
	//added by hcz
	TopWidget *m_pTopWidget;
	BottomCtrls *m_pBottomCtrl;
	LeftWidget *m_pLeftWidget;
	QSlider *m_pMicSlider;
	QSlider *m_pAudioSlider;
	RecordSaveTip *m_pRecordSaveTip;
	std::string m_strMicId;
	std::string m_strAudioId;
	bool m_bMicAudioCheck;
	QTranslator m_customTranslator;
	UserInfoWidget *m_pUserInfoWidget;
	bool m_bStartStream;
	//QSharedPointer<QStompClient> m_pStompClient;
	IMWidget *m_pIMWidget;
	SettingDialog *m_pSettingDialog;
	SacrougeDialog *m_pSacrougeDialog;
	RedDetailsDialog *m_pRedDetailsDialog;
	QProcess *m_pProcess;
	//web url
	QStackedLayout* m_pStackedLayout;
	UrlTabWidget* m_pUrlTabWidget;
	WebBrowserWidget* m_pChatWebWidget;
	bool m_bEnterRoom ;
	QString m_strRoomId;
	QString m_strLiveId;
	WebBrowserWidget* m_pRoomListWebWidget;
	WebBrowserWidget* m_pExchangeWebWidget;
	WebBrowserWidget* m_pRedPacketWebWidget;
	WebBrowserWidget* m_pWebData;
	WebBrowserWidget* m_pWebDetail;
	WebBrowserWidget *m_pWebEvaluate;
	WebBrowserWidget* m_pKnowledgeWebWidget;
	WebBrowserWidget* m_pQuickReplyWebWidget;
	QTimer* m_pCheckHeartbeat;
	QTimer* m_pCheckInHeartbeat;
	QTimer* m_pTimerPreviewPos;
	bool m_bStreamOperate;
	QTimer* m_pStreamTimer;
	MsgBox* m_pInvalidDlg;
	int m_nMask3dIndex = -2;

public:
	std::string m_strCloseName;
	obs_sceneitem_t *m_curSelectItem;
};

class SceneRenameDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:
	SceneRenameDelegate(QObject *parent);
	virtual void setEditorData(QWidget *editor,
				   const QModelIndex &index) const override;

protected:
	virtual bool eventFilter(QObject *editor, QEvent *event) override;
};
