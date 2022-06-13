#pragma once

#include <QtWidgets/QDialog>
#include <QPainter>
#include "ui_QCaptureScreen.h"

class QCaptureScreen : public QDialog
{
    Q_OBJECT

public:
    QCaptureScreen(QWidget *parent = Q_NULLPTR);
    ~QCaptureScreen();

private:
    void initWindow();
    void loadBackgroundPixmap();
    QRect getRect(const QPoint &beginPoint, const QPoint &endPoint);
    bool getCurrentWindowFromCursor(QRect &out_rect);
    void WriteProcess(QRect rect);

    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::QCaptureScreenClass ui;

    QPixmap m_loadPixmap;
    int m_screenwidth;
    int m_screenheight;
    QPoint m_beginPoint, m_endPoint;
    QPainter m_painter;
};
