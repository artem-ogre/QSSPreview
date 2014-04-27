#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private:
	void connectSignalsToSlots();
	private slots:
	void displayUI();
	void applyQSS();
	QWidget * widgetFromUI( QString filename );
	QStringList qrcListFromUI( QString filename );
	void compileQRC( QString filename );

private:
	Ui::MainWindow m_ui;
	static const QString m_tempRCCFileName;
	static const QString m_uiFileName;
	static const QString m_qssFileName;
	QWidget * m_testWidget;
};

#endif
