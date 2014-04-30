#include <QtGui>
#include <QtUiTools>
#include <QXmlQuery>
#include <QFileDialog>

#include "MainWindow.h"
#include "common.h"

QString MainWindow::m_tempRCCFileName = "tempRCC";
QString MainWindow::m_uiFileName = "";
QString MainWindow::m_qssFileName = "";
QSettings MainWindow::m_settings("ArtemAmirkhanov", "QSS");

MainWindow::MainWindow( QWidget *parent )
: QMainWindow( parent ), m_fileWatcher( this ), m_testWidget( 0 )
{
	m_ui.setupUi( this );
	connectSignalsToSlots();
	loadSettings();
	updateFileWatcher();
	initUIStates();
}

void MainWindow::connectSignalsToSlots()
{
	connect( m_ui.pushButton_ui, SIGNAL( clicked() ), this, SLOT( loadUIFromFile() ) );
	connect( m_ui.pushButton_qss, SIGNAL( clicked() ), this, SLOT( loadQSSFromFile() ) );
	connect( m_ui.pushButton_recentUI, SIGNAL( clicked() ), this, SLOT( loadLastUI() ) );
	connect( m_ui.pushButton_recentQSS, SIGNAL( clicked() ), this, SLOT( loadLastQSS() ) );
	connect( &m_fileWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( watchedFileChanged( QString ) ) );
}

void MainWindow::loadUIFromFile()
{
	if( !openUIFile() )
		return;
	loadUI();
}

void MainWindow::loadLastUI()
{
	loadSettings();
	loadUI();
}

void MainWindow::loadUI()
{
	try
	{
		//Dynamically loaded resources attached to the UI file
		QStringList resoruces = qrcListFromUI( m_uiFileName );
		if( 0 != resoruces.size() )
		{
			QString qrcFileName = QFileInfo( m_uiFileName ).absolutePath() + "/" + resoruces.at( 0 );
			compileQRC( qrcFileName );
			bool result = QResource::registerResource( m_tempRCCFileName );
			QFile::remove( m_tempRCCFileName );
		}

		//Instantiate from UI
		QWidget * newTestWidget = widgetFromUI( m_uiFileName );
		if( m_testWidget )

		{
			newTestWidget->restoreGeometry( m_testWidget->saveGeometry() );
			delete m_testWidget;
		}
		m_testWidget = newTestWidget;
		if( !m_style.isEmpty() )
			applyStylesheetToWidget( m_style, m_testWidget );
		m_testWidget->show();
		this->activateWindow();
		
		m_ui.lineEdit_ui->setText( QFileInfo( m_uiFileName ).fileName() );
		m_ui.pushButton_qss->setEnabled( true );
		saveSettings();
		updateUIStates();
		updateFileWatcher();
	}
	catch( ... )
	{
		return;
	}
}

void MainWindow::loadQSSFromFile()
{
	if( !openQSSFile() )
		return;
	loadQSS();
}

void MainWindow::loadLastQSS()
{
	loadSettings();
	loadQSS();
}

void MainWindow::loadQSS()
{
	try
	{
		m_style = readStylesheetFromQSS( m_qssFileName );
		applyStylesheetToWidget( m_style, m_testWidget );

		m_ui.lineEdit_qss->setText( QFileInfo( m_qssFileName ).fileName() );
		m_ui.pushButton_done->setEnabled( true );
		saveSettings();
		updateUIStates();
		updateFileWatcher();
	}
	catch( ... )
	{
		return;
	}
}

QWidget * MainWindow::widgetFromUI( QString filename ) const
{
	QUiLoader loader;
	QFile file( filename );
	if( !file.open( QFile::ReadOnly ) )
	{
		LogErrorAndThrowException( "Cannot open the UI file." );
	}
	QWidget * widget = loader.load( &file );
	return widget;
}

QStringList MainWindow::qrcListFromUI( QString filename ) const
{
	//Open file
	QFile file( filename );
	if( !file.open( QFile::ReadOnly ) )
	{
		LogErrorAndThrowException( "Cannot open the UI file." );
	}

	//Query resources in a string
	QXmlQuery xmlQuery;
	xmlQuery.bindVariable( "documentName", &file );
	xmlQuery.setQuery( "doc($documentName)/ui/resources/include/data(@location)" );
	if( !xmlQuery.isValid() )
	{
		LogErrorAndThrowException( "XmlQuery is not valid" );
	}
	QString res;
	xmlQuery.evaluateTo( &res );
	file.close();

	//Filter qrc resources only
	QStringList resList = res.split( QRegExp( "[\r\n]" ), QString::SkipEmptyParts );
	QStringList qrcList;
	for( int i = 0; i < resList.size(); ++i )
	{
		QFileInfo fi( resList.at( i ) );
		if( 0 == fi.suffix().compare( "qrc", Qt::CaseInsensitive ) )
		{
			qrcList << resList.at( i );
		}
	}
	return qrcList;
}

void MainWindow::compileQRC( QString filename ) const
{
	//Check if file exists
	if( !QFile::exists( filename ) )
	{
		QString message( "QRC file does not exist: " + filename );
		LogErrorAndThrowException( message.toLocal8Bit().constData() );
	}

	// Run rcc on qrc
	const QString command = "rcc";
	QStringList params; params << "-binary" << filename << "-o" << m_tempRCCFileName;
	QProcess process;
	process.start( command, params );
	if( !process.waitForFinished() ) {
		const QString path = QString::fromLocal8Bit( qgetenv( "PATH" ) );
		QString message = QString( "'%1' could not be found when run from '%2'. Path: '%3' " ).arg( command, QDir::currentPath(), path );
		LogErrorAndThrowException( message.toLocal8Bit().constData() );
	}
	const QChar cr = QLatin1Char( '\r' );
	const QString err = QString::fromLocal8Bit( process.readAllStandardError() ).remove( cr );
	if( !err.isEmpty() )
	{
		QString message( "Unexpected stderr contents: " + err );
		LogErrorAndThrowException( message.toLocal8Bit().constData() );
	}
}

QString MainWindow::readStylesheetFromQSS(QString const& qssFile) const
{
	// Load an application style
	QFile styleFile( qssFile );
	if( !styleFile.open( QFile::ReadOnly ) )
	{
		LogErrorAndThrowException( "Cannot open the QSS file." );
	}
	QTextStream styleIn( &styleFile );
	QString res = styleIn.readAll();
	styleFile.close();
	return res;
}

void MainWindow::applyStylesheetToWidget( QString const & style, QWidget * widget )
{
	if( style.isEmpty() )
	{
		LogErrorAndThrowException( "Stylesheet is empty." );
	}
	else if( !widget )
	{
		LogErrorAndThrowException( "Stylesheet is loaded but not applied. Widget is null. Display UI first?" );
	}
	widget->setStyleSheet( style );
}

void MainWindow::updateFileWatcher()
{
	if( !m_qssFileName.isEmpty() )
		m_fileWatcher.addPath( m_qssFileName );
	if( !m_uiFileName.isEmpty() )
		m_fileWatcher.addPath( m_uiFileName );
}

void MainWindow::watchedFileChanged( const QString & path )
{
	qDebug() << "fileChanged: " << path;
	m_fileWatcher.addPath( path ); //not to loose the track of a file
	if( path == m_uiFileName )
		loadUI();
	if( path == m_qssFileName )
		loadQSS();
}

void MainWindow::initUIStates()
{
	m_ui.pushButton_qss->setEnabled( false );
	m_ui.pushButton_done->setEnabled( false );
	m_ui.pushButton_recentUI->setEnabled( false );
	m_ui.pushButton_recentQSS->setEnabled( false );
	updateUIStates();
}

void MainWindow::updateUIStates()
{
	m_ui.pushButton_recentUI->setEnabled( !m_uiFileName.isEmpty() );
	m_ui.pushButton_recentQSS->setEnabled( ( !m_qssFileName.isEmpty() ) && m_ui.pushButton_qss->isEnabled() );
}

bool MainWindow::openUIFile()
{
	QString fileName = QFileDialog::getOpenFileName( QApplication::activeWindow(), 
													 "Open UI File", 
													 QFileInfo( m_uiFileName ).absolutePath(),
													 "UI files (*.ui);;" );
	if( fileName.isEmpty() )
	{
		return false;
	}
	m_uiFileName = fileName;
	return true;
}

bool MainWindow::openQSSFile()
{
	QString fileName = QFileDialog::getOpenFileName(
		QApplication::activeWindow(),
		tr( "Open QSS File" ),
		QFileInfo( m_qssFileName ).absolutePath(),
		tr( "QSS files (*.qss);;" )
		);
	if( fileName.isEmpty() )
	{
		return false;
	}
	m_qssFileName = fileName;
	return true;
}

void MainWindow::loadSettings()
{
	m_uiFileName = m_settings.value( "Settings/FileNameUI", "" ).toString();
	m_qssFileName = m_settings.value( "Settings/FileNameQSS", "" ).toString();
}

void MainWindow::saveSettings()
{
	m_settings.setValue( "Settings/FileNameUI", m_uiFileName );
	m_settings.setValue( "Settings/FileNameQSS", m_qssFileName );
}

