#include <QtGui>
#include <QtUiTools>
#include <QXmlQuery>

#include "MainWindow.h"
#include "common.h"

const QString MainWindow::m_tempRCCFileName = "tempRCC";
const QString MainWindow::m_uiFileName = "C:/Artem/Work/iAnalyse_GIT/iAnalyseSrc/iAnalyseToolkit/Dreamcaster/dream_caster4.ui";//"C:/Artem/Work/iAnalyse_GIT/iAnalyseSrc/iAnalyseGUI/Mainwindow.ui";
const QString MainWindow::m_qssFileName = "C:/Users/astra/Dropbox/PyQSS/dark.qss";

MainWindow::MainWindow( QWidget *parent )
: QMainWindow( parent ), m_fileWatcher( this ), m_testWidget( 0 )
{
	m_ui.setupUi( this );
	connectSignalsToSlots();
	initFileWatcher();
}

void MainWindow::connectSignalsToSlots()
{
	connect( m_ui.pushButton_UI, SIGNAL( clicked() ), this, SLOT( loadWidgetFromUI() ) );
	connect( m_ui.pushButton_QSS, SIGNAL( clicked() ), this, SLOT( loadAndApplyQSS() ) );
	connect( &m_fileWatcher, SIGNAL( fileChanged( QString ) ), this, SLOT( watchedFileChanged( QString ) ) );
}

void MainWindow::loadWidgetFromUI()
{
	try
	{
		//Dynamically loaded resources attached to the UI file
		QStringList resoruces = qrcListFromUI( m_uiFileName );
		QFileInfo fi( m_uiFileName );
		QString qrcFileName = fi.absolutePath() + "/" + resoruces.at( 0 );
		compileQRC( qrcFileName );
		bool result = QResource::registerResource( m_tempRCCFileName );
		QFile::remove( m_tempRCCFileName );

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
	}
	catch( ... )
	{
		return;
	}
}

void MainWindow::loadAndApplyQSS()
{
	try
	{
		m_style = readStylesheetFromQSS( m_qssFileName );
		applyStylesheetToWidget( m_style, m_testWidget );
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
	const QString command = "C:/Tools/Qt-5/qtbase/bin/rcc";
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

void MainWindow::initFileWatcher()
{
	m_fileWatcher.addPath( m_qssFileName );
	m_fileWatcher.addPath( m_uiFileName );
}

void MainWindow::watchedFileChanged( const QString & path )
{
	qDebug() << "fileChanged: " << path;
	m_fileWatcher.addPath( path ); //not to loose the track of a file
	if( path == m_uiFileName )
		loadWidgetFromUI();
	if( path == m_qssFileName )
		loadAndApplyQSS();
}
