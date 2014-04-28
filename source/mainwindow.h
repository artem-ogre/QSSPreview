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
	void displayUI();
	void loadAndApplyQSS();
	QString readStylesheetFromQSS( QString const& ) const;
	void applyStylesheetToWidget( QString const & style, QWidget * widget );
	QWidget * widgetFromUI( QString filename );
	QStringList qrcListFromUI( QString filename );
	void compileQRC( QString filename );
	void initFileWatcher();

private:
	Ui::MainWindow m_ui;
	static const QString m_tempRCCFileName;
	static const QString m_uiFileName;
	static const QString m_qssFileName;
	QWidget * m_testWidget;
	QString m_style;
	QFileSystemWatcher m_fileWatcher;

private slots:
	void onFileChanged( const QString & path );
};

#endif
