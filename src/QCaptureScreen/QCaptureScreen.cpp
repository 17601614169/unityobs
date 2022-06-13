#include "QCaptureScreen.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QDebug>
#include <json/json.h>

#ifdef _WINDOWS
#include <Windows.h>
#endif // _WINDOWS

#define PADDING 3

QCaptureScreen::QCaptureScreen(QWidget *parent)
	: QDialog(parent)
	, m_beginPoint(QPoint(0,0))
	, m_endPoint(QPoint(0,0))
{
	ui.setupUi(this);
	initWindow();
	loadBackgroundPixmap();
}

QCaptureScreen::~QCaptureScreen() {}

void QCaptureScreen::initWindow() {
	this->setMouseTracking(true);
	this->setWindowFlags(Qt::FramelessWindowHint);
	setWindowState(Qt::WindowFullScreen);
}

void QCaptureScreen::loadBackgroundPixmap() {
	m_loadPixmap = QPixmap::grabWindow(QApplication::desktop()->winId()); //抓取当前屏幕的图片;
	m_screenwidth = m_loadPixmap.width();
	m_screenheight = m_loadPixmap.height();
}

QRect QCaptureScreen::getRect(const QPoint &beginPoint, const QPoint &endPoint) {
	int x, y, width, height;
	width = qAbs(beginPoint.x() - endPoint.x());
	height = qAbs(beginPoint.y() - endPoint.y());
	x = beginPoint.x() < endPoint.x() ? beginPoint.x() : endPoint.x();
	y = beginPoint.y() < endPoint.y() ? beginPoint.y() : endPoint.y();

	QRect selectedRect = QRect(x, y, width, height);
	// 避免宽或高为零时拷贝截图有误;
	// 可以看QQ截图，当选取截图宽或高为零时默认为2;
	if (selectedRect.width() == 0) {
		selectedRect.setWidth(1);
	}
	if (selectedRect.height() == 0) {
		selectedRect.setHeight(1);
	}

	return selectedRect;
}

bool QCaptureScreen::getCurrentWindowFromCursor(QRect &out_rect)
{
#ifdef _WINDOWS
	::EnableWindow((HWND)winId(), FALSE);

	HWND hwnd;
	POINT pt;
	// 获得当前鼠标位置
	::GetCursorPos(&pt);
	// 获得当前位置桌面上的子窗口
	hwnd = ::ChildWindowFromPointEx(::GetDesktopWindow(), pt, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);
	if (hwnd != NULL) {
		HWND temp_hwnd;
		temp_hwnd = hwnd;
		while (true) {
			::GetCursorPos(&pt);
			::ScreenToClient(temp_hwnd, &pt);
			temp_hwnd = ::ChildWindowFromPointEx(temp_hwnd, pt, CWP_SKIPINVISIBLE);
			if (temp_hwnd == NULL || temp_hwnd == hwnd)
				break;
			hwnd = temp_hwnd;
		}
		RECT temp_window;
		::GetWindowRect(hwnd, &temp_window);
		out_rect.setRect(temp_window.left,temp_window.top, temp_window.right - temp_window.left, temp_window.bottom - temp_window.top);
		::EnableWindow((HWND)winId(), TRUE);
		return true;
	}
	::EnableWindow((HWND)winId(), TRUE);
#endif // _WINDOWS

	return false;
}

void QCaptureScreen::WriteProcess(QRect rect) {
	Json::FastWriter writer;
	Json::Value root;
	root["x1"] = Json::Value(rect.topLeft().x());
	root["y1"] = Json::Value(rect.topLeft().y());
	root["x2"] = Json::Value(rect.bottomRight().x());
	root["y2"] = Json::Value(rect.bottomRight().y());
	std::string str = writer.write(root);

	QFile file;
	file.open(stdout, QIODevice::WriteOnly);
	file.write(str.c_str());
	file.close();
}

void QCaptureScreen::mouseMoveEvent(QMouseEvent *event) {
	QRect rc;
	bool bResult = getCurrentWindowFromCursor(rc);
	if (bResult) {
		m_beginPoint = rc.topLeft();
		m_endPoint = rc.bottomRight();
		update();
	}

	return QWidget::mouseMoveEvent(event);
}

void QCaptureScreen::mouseReleaseEvent(QMouseEvent *event) {
	QWidget::mouseReleaseEvent(event);

	QPoint pt = event->pos();
	QRect rc(m_beginPoint, m_endPoint);
	if (rc.contains(pt)) {
		WriteProcess(rc);
		close();
	}
}

void QCaptureScreen::keyPressEvent(QKeyEvent *event) {
	// Esc 键退出截图;
	if (event->key() == Qt::Key_Escape) {
		close();
	}
	// Eeter键完成截图;
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
		WriteProcess(QRect(m_beginPoint, m_endPoint));
		close();
	}
}

void QCaptureScreen::paintEvent(QPaintEvent *event) {
	m_painter.begin(this);          //进行重绘;

	QColor shadowColor = QColor(0, 0, 0, 100);                      //阴影颜色设置;
	m_painter.setPen(QPen(Qt::blue, PADDING, Qt::SolidLine, Qt::FlatCap));    //设置画笔;
	m_painter.drawPixmap(0, 0, m_loadPixmap);                       //将背景图片画到窗体上;
	m_painter.fillRect(m_loadPixmap.rect(), shadowColor);           //画影罩效果;

	m_beginPoint.setX(m_beginPoint.x() < PADDING ? PADDING : m_beginPoint.x());
	m_beginPoint.setY(m_beginPoint.y() < PADDING ? PADDING : m_beginPoint.y());
	m_endPoint.setX(m_endPoint.x() > m_screenwidth - PADDING ? m_screenwidth - PADDING : m_endPoint.x());
	m_endPoint.setY(m_endPoint.y() > m_screenheight ? m_screenheight : m_endPoint.y());

	QRect selectedRect = getRect(m_beginPoint, m_endPoint);
	QPixmap capturePixmap = m_loadPixmap.copy(selectedRect);
	m_painter.drawPixmap(selectedRect.topLeft(), capturePixmap);
	m_painter.drawRect(selectedRect);

	m_painter.end();  //重绘结束;
}
