#include "cef-view-widget.h"
#include "QCefViewSDK/include/QCefView.h"
#include <QHBoxLayout> 
CefViewWidget::CefViewWidget(const QString &url,QWidget *parent)
	: QDialog(parent), m_pCefView(nullptr), m_pLayout(nullptr)
{
	m_pLayout = new QHBoxLayout;
	m_pLayout->setContentsMargins(0, 0, 0, 0);
	m_pLayout->setSpacing(0);
	m_pCefView = new QCefView(url, parent,true);
	m_pLayout->addWidget(m_pCefView);
	this->setLayout(m_pLayout);
}

CefViewWidget::~CefViewWidget() {

}


void CefViewWidget::Load(const QString &url) {
	if (m_pCefView != nullptr) {
		m_pCefView->navigateToUrl(url);
	}
}
