#include <QtGui>
#include <QtUiTools>
#include <QXmlQuery>

#include "MainWindow.h"
#include "common.h"

const QString MainWindow::m_tempRCCFileName = "tempRCC";
const QString MainWindow::m_uiFileName = "C:/Artem/Work/iAnalyse_GIT/iAnalyseSrc/iAnalyseToolkit/Dreamcaster/dream_caster4.ui";//"C:/Artem/Work/iAnalyse_GIT/iAnalyseSrc/iAnalyseGUI/Mainwindow.ui";
const QString MainWindow::m_qssFileName = "C:/Users/astra/Dropbox/PyQSS/dark.qss";

MainWindow::MainWindow( QWidget *parent )
: QMainWindow( parent ), m_testWidget( 0 )
{
	m_ui.setupUi( this );
	connectSignalsToSlots();
}

void MainWindow::connectSignalsToSlots()
{
	connect( m_ui.pushButton_DisplayUI, SIGNAL( clicked() ), this, SLOT( displayUI() ) );
	connect( m_ui.pushButton_ApplyQSS, SIGNAL( clicked() ), this, SLOT( applyQSS() ) );
}

void MainWindow::displayUI()
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
		if( m_testWidget )
			delete m_testWidget;
		m_testWidget = widgetFromUI( m_uiFileName );
		m_testWidget->show();
	}
	catch( ... )
	{
		return;
	}
}

QWidget * MainWindow::widgetFromUI( QString filename )
{
	QUiLoader loader;
	QFile file( filename );
	if( !file.open( QFile::ReadOnly ) )
	{
		qDebugWithInfo() << "Cannot open the UI file.";
		throw std::exception( "Cannot open the UI file." );
	}
	QWidget * widget = loader.load( &file, this );
	return widget;
}

QStringList MainWindow::qrcListFromUI( QString filename )
{
	//Open file
	QFile file( filename );
	if( !file.open( QFile::ReadOnly ) )
	{
		qDebugWithInfo() << "Cannot open the UI file.";
		throw std::exception( "Cannot open the UI file." );
	}

	//Query resources in a string
	QXmlQuery xmlQuery;
	xmlQuery.bindVariable( "myDocument", &file );
	xmlQuery.setQuery( "doc($myDocument)/ui/resources/include/data(@location)" );
	if( !xmlQuery.isValid() )
	{
		qDebugWithInfo() << "XmlQuery is not valid";
		throw std::exception( "XmlQuery is not valid" );
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

void MainWindow::compileQRC( QString filename )
{
	//Check if file exists
	if( !QFile::exists( filename ) )
	{
		qDebugWithInfo() << "QRC file does not exist:" << filename;
		throw std::exception( "QRC file does not exist." );
	}

	// Run rcc on qrc
	const QString command = "C:/Tools/Qt-5/qtbase/bin/rcc";
	QStringList params; params << "-binary" << filename << "-o" << m_tempRCCFileName;
	QProcess process;
	process.start( command, params );
	if( !process.waitForFinished() ) {
		const QString path = QString::fromLocal8Bit( qgetenv( "PATH" ) );
		QString message = QString( "'%1' could not be found when run from '%2'. Path: '%3' " ).arg( command, QDir::currentPath(), path );
		qDebugWithInfo() << message;
		throw std::exception( message.toLocal8Bit().data() );
	}
	const QChar cr = QLatin1Char( '\r' );
	const QString err = QString::fromLocal8Bit( process.readAllStandardError() ).remove( cr );
	if( !err.isEmpty() )
	{
		qDebugWithInfo() << "unexpected stderr contents: " << err;
		throw std::exception( "unexpected stderr contents" );
	}
}

void MainWindow::applyQSS()
{
	// Load an application style
	QFile styleFile( m_qssFileName );
	if( !styleFile.open( QFile::ReadOnly ) )
	{
		qDebugWithInfo() << "Cannot open the QSS file.";
		throw std::exception( "Cannot open the QSS file." );
	}
	QTextStream styleIn( &styleFile );
	QString style = styleIn.readAll();
	styleFile.close();
	m_testWidget->setStyleSheet( style );
}
