#ifndef ALTER_PASSWORD_H
#define ALTER_PASSWORD_H
#include "move-dialog.h"
#include "qtcustom_global.h"
#include "base-define.h"

class QLabel;
class QPushButton;
class QLineEdit;
class QTCUSTOM_EXPORT AlterPassword : public MoveDialog {
    Q_OBJECT
public:
    AlterPassword(STUser stUser, QWidget* widget=0);
    ~AlterPassword();
public slots:
    void OnSaveBtnClicked() ;

private:
    bool IsEnglish(const QString &str);
private:
    QPushButton* m_pCloseBtn;
    QLabel* m_pTitle;

    QLabel* m_pUserName;
    QLineEdit* m_pUserNameEdit;

    QLabel* m_pOldPassword;
    QLineEdit* m_pOldPasswordEdit;

    QLabel* m_pNewPassword;
    QLineEdit* m_pNewPasswordEdit;

    QLabel* m_pNewConfirmPassword;
    QLineEdit* m_pNewConfirmPasswordEdit;
    QPushButton* m_pSaveBtn;
};

#endif // !ALTER_PASSWORD_H
