#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_MainWindow.h"
#include <QFileSystemWatcher>
#include <QSettings>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );

private:
	void connectSignalsToSlots();
	void initUIStates();
	void updateUIStates();
	void updateFileWatcher();
	void loadSettings();
	void saveSettings();
	bool openUIFile();
	bool openQSSFile();
	void applyStylesheetToWidget( QString const & style, QWidget * widget );
	QWidget * widgetFromUI( QString filename ) const;
	QStringList qrcListFromUI( QString filename ) const;
	void compileQRC( QString filename ) const;
	QString readStylesheetFromQSS( QString const& ) const;
	void loadUI();
	void loadQSS();

protected:
	void dropEvent( QDropEvent * event );
	void dragEnterEvent ( QDragEnterEvent * event );
	void dragLeaveEvent ( QDragLeaveEvent * event );
	
private slots:
	void loadUIFromFile();
	void loadLastUI();
	void loadQSSFromFile();
	void loadLastQSS();
	void watchedFileChanged( const QString & path );

private:
	Ui::MainWindow m_ui;
	static QString m_tempRCCFileName;
	static QString m_uiFileName;
	static QString m_qssFileName;
	static QSettings m_settings;
	QWidget * m_testWidget;
	QString m_style;
	QFileSystemWatcher m_fileWatcher;

};

#endif
