/*
 Copyright Artem Amirkhanov 2014
 Distributed under the MIT Software License (See accompanying file LICENSE.txt)
 Contact the author: artem.ogre@gmail.com
*/

#include <QApplication>
#include <QFileInfo>
#include "mainwindow.h"

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	MainWindow mainwindow;

	//If ui or qss files are passed with command line, pre-load them
	if( argc > 1 )
	{
		//load .ui
		bool isUILoaded = false;
		for( int i = 1; i < argc; ++i )
		{
			QFileInfo fi( argv[i] );
			if( !fi.exists() )
				continue;
			if( 0 == fi.suffix().compare( "ui", Qt::CaseInsensitive ) )
			{
				mainwindow.loadUIFromFile( fi.absoluteFilePath() );
				isUILoaded = true;
			}
		}
		//if ui is loaded look for qss and load it too
		if( isUILoaded )
			for( int i = 1; i < argc; ++i )
			{
				QFileInfo fi( argv[i] );
				if( !fi.exists() )
					continue;
				if( 0 == fi.suffix().compare( "qss", Qt::CaseInsensitive ) )
						mainwindow.loadQSSFromFile( fi.absoluteFilePath() );
			}
	}

	mainwindow.show();
	return app.exec();
}
