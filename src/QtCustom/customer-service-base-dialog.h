#pragma once

#include "qtcustom_global.h"
#include "move-dialog.h"
#include "asyn-request.h"
#include "util/config-file.h"
#include "obs.h"
#include "obs-properties.h"
#include "named-btn.h"
#include "base-define.h"
#include "webengine-widget.h"

class QPushButton;
class QVBoxLayout;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QButtonGroup;
class QPropertyAnimation;
class QHBoxLayout;
class QScrollArea;
class QLineEdit;
class QRadioButton;
class QTimerEvent;
class QTextEdit;
class QGridLayout;
class NoneStateLabel;
class UrlRadiusLabel;
class QCheckBox;
class FilterItem;
class MaskFilterLabel;

enum QUALITY_TYPE {
	QUALITY_HIGH = 0,
	QUALITY_STANDARD
};

/*暂不使用*/
//class QTCUSTOM_EXPORT ConfirmDialog : public MoveDialog {
//	Q_OBJECT
//public:
//	explicit ConfirmDialog(QWidget *parent = nullptr);
//	virtual ~ConfirmDialog() override;
//};

class QTCUSTOM_EXPORT TipsDialog : public MoveDialog {
	Q_OBJECT
public:
	explicit TipsDialog(const QString &info, QWidget *parent = nullptr);
	virtual ~TipsDialog() override;
};

class QTCUSTOM_EXPORT CustomerServiceBaseDialog : public MoveDialog {
	Q_OBJECT
public:
	explicit CustomerServiceBaseDialog(bool bConfirm, const QString &strTitle, QWidget *parent = nullptr);
	virtual ~CustomerServiceBaseDialog() override;

	virtual bool OnConfirmClick() = 0;

protected:
	QVBoxLayout *m_pContentLayout;
	QPushButton *m_pCancelBtn;
	QPushButton *m_pConfirmBtn;
};

class QTCUSTOM_EXPORT FinishLiveDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit FinishLiveDialog(QWidget *parent = nullptr);
	virtual ~FinishLiveDialog() override;

	virtual bool OnConfirmClick() override;
};

class QTCUSTOM_EXPORT ExitDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit ExitDialog(QWidget *parent = nullptr);
	virtual ~ExitDialog() override;

	virtual bool OnConfirmClick() override;
};

class QTCUSTOM_EXPORT EffectDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit EffectDialog(const std::string &token, BtnTypes type, QWidget *parent = nullptr);
	virtual ~EffectDialog() override;

	void AddNoneItem(int index);
	void AddItem(int index, const QString &path, int type);

	virtual bool OnConfirmClick() override;

private:
	void GetMaskList(const std::string &token);
	void GetBkgroundList(const std::string &token);

signals:
	void ChangeFilterPath(int type, std::string &path);

public slots:
	void OnReadyData(const STRspData &stRsp);
	void OnNoneClicked(NoneStateLabel *label);
	void OnClicked(UrlRadiusLabel *label);

	//void OnItemClicked(QListWidgetItem *item);

private:
	QScrollArea *m_pArea;
	QGridLayout *m_pGrid;
	std::string m_strPath;
	BtnTypes m_type;
	int m_nPosType;
};

class QTCUSTOM_EXPORT MaskDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit MaskDialog(const std::string &token, QWidget *parent = nullptr);
	virtual ~MaskDialog() override;

	void AddMosaicItem(int index);
	void AddNoneItem(int index);
	void AddItem(int index);
	void GetData(Mask_Type type, QString &rpath, QString &img_dir1, QString &img_dir2);

	virtual bool OnConfirmClick() override;

signals:
	void ChangeMaskPath(std::string path1, std::string path2);

public slots:
	void OnNoneClicked(NoneStateLabel *label);
	void OnClicked(MaskFilterLabel *label);

private:
	QScrollArea *m_pArea;
	QGridLayout *m_pGrid;
	std::string m_strPath;
	QString m_dir1;
	QString m_dir2;
	int m_index = -2;
};

class QTCUSTOM_EXPORT MaskDialog3D : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit MaskDialog3D(int idx, QWidget *parent = nullptr);
	virtual ~MaskDialog3D() override;
	
	virtual bool OnConfirmClick() override;

	void AddNoneItem(int index);
	void AddItem(int index);
	void GetData(Mask3d_Type type, QString &path); 

public slots:
	void OnNoneClicked(NoneStateLabel *label);
	void OnClicked(UrlRadiusLabel *label);

signals:
	void ChangeIndex(int index);

private:
	QScrollArea *m_pArea;
	QGridLayout *m_pGrid;
	int m_index = -2;
};

class QTCUSTOM_EXPORT DeviceDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
	using properties_delete_t = decltype(&obs_properties_destroy);
	using properties_t =
		std::unique_ptr<obs_properties_t, properties_delete_t>;
public:
	explicit DeviceDialog(obs_source_t *camera_source, obs_source_t* mic_source, QWidget *parent = nullptr);
	virtual ~DeviceDialog() override;

	void InitCamera(obs_source_t *source);
	void InitMicro(obs_source_t *source);

	virtual bool OnConfirmClick() override;

public slots:
	void OnCameraIndexChanged(int index);

protected:
	virtual void timerEvent(QTimerEvent *event) override;  

private:
	QComboBox *m_pCameraCombo;
	QComboBox *m_pMicroCombo;

	properties_t m_cameraProperties;
	properties_t m_microProperties;
	int m_nTimerId;

	obs_source_t *m_pCameraSource;
	obs_source_t *m_pMicSource;
};

class QTCUSTOM_EXPORT QualityDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	explicit QualityDialog(config_t *config, QWidget *parent = nullptr);
	virtual ~QualityDialog() override;

	std::string GetOutputResolut();

	virtual bool OnConfirmClick() override;

private:
	QButtonGroup *m_pQualityGroup;
	config_t *m_pConfig;
};

class QTCUSTOM_EXPORT NotificationButton : public QPushButton {
	Q_OBJECT
public:
	explicit NotificationButton(const QString &strtext, QWidget *parent = nullptr);
	virtual ~NotificationButton() override;

	void SetNoteCount(int count);

private:
	QLabel *m_pCount;
};

class QTCUSTOM_EXPORT NoteButtonGroup : public QWidget {
	Q_OBJECT
public:
	explicit NoteButtonGroup(QWidget *parent = nullptr);
	virtual ~NoteButtonGroup() override;

	void AddButton(QPushButton* btn, int id);
	QPushButton *GetButton(int id);

protected:
	virtual void paintEvent(QPaintEvent *event) override;

signals:
	void ButtonClicked(int id);

public slots:
	void OnButtonClicked(int index);

private:
	QButtonGroup *m_btnGroup;
	int m_curIndex;
	int m_lineHeight;
	QColor m_lineColor;
	QSize m_btnSize;
};

class QTCUSTOM_EXPORT NoteTypeItem : public QWidget {
	Q_OBJECT
public:
	explicit NoteTypeItem(const STInfoClass &info, QWidget *parent = nullptr);
	virtual ~NoteTypeItem() override;

	void SetNoteCount(int count);
	int GetClassId();

private:
	QLabel *m_pCountLabel;
	STInfoClass m_infoClass;
};

class NoteTitleWidget : public QWidget {
	Q_OBJECT
public:
	NoteTitleWidget(const STPageData &note, QWidget *parent = nullptr);
	~NoteTitleWidget();

	void SetCheck(bool check);
	bool GetCheck();

protected:
	virtual void mouseReleaseEvent(QMouseEvent *event) override;

signals:
	void CheckNote(bool check);

private:
	QRadioButton *m_pRadioButton;
	//QCheckBox *m_pCheckBox;
};

using resize_callback = std::function<void(int)>;
using click_callback = std::function<void(const STPageData &note)>;
class QTCUSTOM_EXPORT NotificationItem : public QWidget {
	Q_OBJECT
public:
	explicit NotificationItem(const STPageData &note, const resize_callback &func, const click_callback &func1, QWidget *parent = nullptr);
	virtual ~NotificationItem() override;

	void SetCheck(bool check);
	bool GetCheck();
	int GetNoteId();

signals:
	void ReadNote(int id);

private:
	NoteTitleWidget *m_pWidget;
	int m_nNoteId;
};

class QTCUSTOM_EXPORT PageWidget : public QWidget {
	Q_OBJECT
public:
	explicit PageWidget(QWidget *parent = nullptr);
	virtual ~PageWidget() override;

	void SetPageCount(int count);
	void SetCurrentCount(int current);
	void SetCheckIndex(int index);

private:
	void DeleteLayout();

signals:
	void ButtonClicked(int page);

public slots:
	void OnButtonClicked(int id);

private:
	QButtonGroup *m_pPageGroup;
	int m_nShowCount;
	int m_nAllCount;
	int m_nFirstIndex;
};

class QTCUSTOM_EXPORT NotificationDialog : public CustomerServiceBaseDialog {
	Q_OBJECT
public:
	enum STATE_TYPE {
		STATE_UNREAD = 0,
		STATE_READ,
		STATE_ALL,
	};

	explicit NotificationDialog(const std::string &token, QWidget *parent = nullptr);
	virtual ~NotificationDialog() override;

	void AddListItem(const STInfoClass &info);
	void UpdateListItem(const STInfoClass &info);
	void AddNoteItem(const STPageData &note);
	void UpdateSize(int diffheight);
	void ClickNote(const STPageData &note);
	void DeleteLayout(QLayout *layout);

	void GetInfoClass();
	void GetPageList();
	void GetPageListLimit();
	void GetUnreadCount();
	void GetCurrentPageList();

	virtual bool OnConfirmClick() override;

signals:
	void OperationMessage();

public slots:
	void OnListItemClicked(QListWidgetItem *item);
	void OnPageComboChange(const QString &text);
	void OnOperationClicked();
	void OnSelectRead();
	void OnSelectDelete();
	void OnAllRead();
	void OnAllDelete();
	void OnReadyData(const STRspData &stRsp);
	void OnReadNote(int id);

private:
	NoteButtonGroup *m_pStateGroup;
	QListWidget *m_pListWidget;
	QScrollArea *m_pScrollArea;
	QLabel *m_pPageLabel;
	PageWidget *m_pPageWidget;
	QLineEdit *m_pEdit;
	QWidget *m_pWidget;

	std::string m_strToken;

	int m_classid;
	STATE_TYPE m_stateType;
	int m_nSize;
	int m_nPage;
	int m_nAllPage;
};

class QTCUSTOM_EXPORT UserInfoDialog : public MoveDialog {
	Q_OBJECT
public:
	explicit UserInfoDialog(std::string token, STUser stUser, const QString &head, QWidget *parent = nullptr);
	virtual ~UserInfoDialog() override;
	STCsUser GetCsUser();

private:
	void SetLoginInfo(const STCsUser &csuser);

public slots:
	void OnUploadImage();
	void OnSaveClick();
	void OnResetClick();
	void OnTextChange();

private:
	QLineEdit *m_pChineseName;
	QLineEdit *m_pEnglishName;
	QTextEdit *m_pWelcomeRemark;
	QTextEdit *m_pCloseRemark;
	QPushButton *m_pImageBtn;
	QPushButton *m_pImageBtn2;

	std::string m_strToken;
	std::string m_strImagePath;
	STCsUser m_stCsUser;
	std::string m_strName;
	std::string m_strNameEn;
	std::string m_strWelcome;
	std::string m_strGoodbye;
	QString m_strHead;
};
