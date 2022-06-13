#ifndef MSG_BOX_H
#define MSG_BOX_H
#include "move-dialog.h"
#include "qtcustom_global.h"
enum MSGBTN {
	ENONE = 0,
	EOK = 1,
	ECANCLE=2,
	EOK_CANCLE = 4,
};

class QTCUSTOM_EXPORT MsgBox : public MoveDialog{
	Q_OBJECT
public:
	explicit MsgBox(QWidget *parent, const QString &title=QString(""),const QString &text=QString(""), MSGBTN buttons = EOK);
	~MsgBox();

	static MSGBTN information(QWidget *parent, const QString &title,const QString &text, MSGBTN buttons = EOK);
	static QString Tips() {return QString(tr("tips"));}
	int GetMsgValue();
signals:
	void sigClose();
private:
	QLabel* m_pTitle;
	QPushButton* m_pClose;
	QLabel* m_pContent;


	QPushButton* m_pOk;
	QPushButton* m_pCancle;
	MSGBTN m_eBtn;
};

#endif
