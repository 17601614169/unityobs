#include "TestUrlWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

TestUrlWidget::TestUrlWidget(QWidget *parent) : QWidget(parent)
{
	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setPlaceholderText("input url");
	m_pLineEdit->setFixedHeight(80);
	m_pLoadBtn = new QPushButton("Load url",this);
	m_pLoadBtn->setFixedSize(200,80);
	QWidget *pWidget = new QWidget(this);
	m_pYlUrlEdit = new QTextEdit(pWidget);
	m_pYlUrlEdit->setFixedWidth(400);
	m_pExampleUrlEdit = new QTextEdit(pWidget);
	m_pExampleUrlEdit->setFixedWidth(400);
	m_pBrowser=new WebBrowserWidget(this);
	m_pBrowser->SetMinimumSize(600,400);
	auto LoadSlot = [&]() {
		QString url = m_pLineEdit->text();
		this->LoadUrl(url);
	};
	connect(m_pLoadBtn,&QPushButton::clicked,LoadSlot);
	QHBoxLayout *hlay = new QHBoxLayout;
	hlay->setContentsMargins(0,0,0,0);
	hlay->setSpacing(0);

	hlay->addWidget(m_pLineEdit);
	hlay->addSpacing(10);
	hlay->addWidget(m_pLoadBtn);
	hlay->addSpacing(20);

	QVBoxLayout* vText = new QVBoxLayout(pWidget);
	vText->setContentsMargins(0,0,0,0);
	vText->setSpacing(0);
	vText->addWidget(m_pYlUrlEdit);
	vText->addSpacing(10);
	vText->addWidget(m_pExampleUrlEdit);
	vText->addSpacing(20);

	QHBoxLayout* hBottom = new QHBoxLayout;
	hBottom->setContentsMargins(0,0,0,0);
	hBottom->setSpacing(0);
	hBottom->addWidget(m_pBrowser);
	hBottom->addSpacing(10);
	hBottom->addWidget(pWidget);
	QVBoxLayout* vMain = new QVBoxLayout;
	vMain->setContentsMargins(0,0,0,0);
	vMain->setSpacing(0);
	vMain->addLayout(hlay);
	vMain->setSpacing(10);
	vMain->addLayout(hBottom);
	setLayout(vMain);
}


TestUrlWidget::~TestUrlWidget()
{

}

void TestUrlWidget::LoadUrl(const QString& url)
{
	if (!url.isEmpty())
		m_pBrowser->Load(url);
}

void TestUrlWidget::AppendYlUrl(const QString& url)
{
	m_pYlUrlEdit->append(url);
	m_pYlUrlEdit->append("\n");
}
