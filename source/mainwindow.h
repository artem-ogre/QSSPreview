#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"
#include <QFileSystemWatcher>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private:
	void connectSignalsToSlots();
	
private slots:
	void loadWidgetFromUI();
	void loadAndApplyQSS();
	QString readStylesheetFromQSS( QString const& ) const;
	void applyStylesheetToWidget( QString const & style, QWidget * widget );
	QWidget * widgetFromUI( QString filename ) const;
	QStringList qrcListFromUI( QString filename ) const;
	void compileQRC( QString filename ) const;
	void initFileWatcher();
	void watchedFileChanged( const QString & path );

private:
	Ui::MainWindow m_ui;
	static const QString m_tempRCCFileName;
	static const QString m_uiFileName;
	static const QString m_qssFileName;
	QWidget * m_testWidget;
	QString m_style;
	QFileSystemWatcher m_fileWatcher;

};

#endif
