#include <iostream>

#include <QApplication>
#include <QTextCodec>
#include <QDir>
#include <QDebug>

#ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#undef MAC_OS_X_VERSION_MIN_REQUIRED
// We need to do this because of Qt: https://bugreports.qt-project.org/browse/QTBUG-22154
#define MAC_OS_X_VERSION_MIN_REQUIRED __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#endif // MAC_OS_X_VERSION_MIN_REQUIRED

#include <SDL.h>

#include "maindialog.hpp"

int main(int argc, char *argv[])
{
    try
    {
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        SDL_SetMainReady();
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            qDebug() << "SDL_Init failed: " << QString::fromStdString(SDL_GetError());
            return 0;
        }

        QApplication app(argc, argv);

        // Now we make sure the current dir is set to application path
        QDir dir(QCoreApplication::applicationDirPath());

        #ifdef Q_OS_MAC
        if (dir.dirName() == "MacOS") {
            dir.cdUp();
            dir.cdUp();
            dir.cdUp();
        }

        // force Qt to load only LOCAL plugins, don't touch system Qt installation
        QDir pluginsPath(QCoreApplication::applicationDirPath());
        pluginsPath.cdUp();
        pluginsPath.cd("Plugins");

        QStringList libraryPaths;
        libraryPaths << pluginsPath.path() << QCoreApplication::applicationDirPath();
        app.setLibraryPaths(libraryPaths);
        #endif

        QDir::setCurrent(dir.absolutePath());

        // Support non-latin characters
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

        Launcher::MainDialog mainWin;

        if (!mainWin.showFirstRunDialog())
            return 0;

    //    if (!mainWin.setup()) {
    //        return 0;
    //    }

        mainWin.show();

        int returnValue = app.exec();
        SDL_Quit();
        return returnValue;
    }
    catch (std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 0;
    }
}
