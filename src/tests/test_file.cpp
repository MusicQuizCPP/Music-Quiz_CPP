#include <chrono>
#include <iostream>
#include <thread>

#include <QApplication>
#include <QDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

#include "common/Log.hpp"

class VideoWidget : public QWidget {
public:
    VideoWidget(QWidget* parent = 0) : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        player              = new QMediaPlayer(this);
        QVideoWidget* vw    = new QVideoWidget;
        layout->addWidget(vw);
        player->setVideoOutput(vw);
        player->setMedia(QUrl::fromLocalFile("C:/Users/Ztaal/Desktop/video.mp4"));
        player->setVolume(100);
    }

    void play() { player->play(); }

    QMediaPlayer* player = nullptr;
};

int main(int argc, char* argv[])
{
    std::cout << "Hello MusicQuiz World!" << std::endl;

    do {
        std::cout << '\n' << "Press the Enter key to continue.";
    } while (std::cin.get() != '\n');

    /** Create QApplication */
    QApplication app(argc, argv);

    /*QMediaPlayer* player = new QMediaPlayer;
    //connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    player->setMedia(QUrl::fromLocalFile("C:/Users/Ztaal/Desktop/test.wav"));
    player->setVolume(50);
    player->setPosition(2000);
    player->play();*/

    /*QMediaPlaylist* playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl("C:/Users/Ztaal/Desktop/video.mp4"));
    //playlist->addMedia(QUrl("http://example.com/movie2.mp4"));
    //playlist->addMedia(QUrl("http://example.com/movie3.mp4"));
    playlist->setCurrentIndex(1);

    QMediaPlayer* player = new QMediaPlayer;
    player->setPlaylist(playlist);

    QVideoWidget* videoWidget = new QVideoWidget;
    player->setVideoOutput(videoWidget);
    videoWidget->show();
    player->play();*/

    QDialog*     dialog      = new QDialog;
    QVBoxLayout* layout      = new QVBoxLayout(dialog);
    VideoWidget* videoWidget = new VideoWidget;
    dialog->resize(400, 300);
    layout->addWidget(videoWidget);
    dialog->setLayout(layout);

    videoWidget->show();
    videoWidget->play();

    std::this_thread::sleep_for(std::chrono::seconds(2));
    dialog->exec();

    app.exec();
    do {
        std::cout << '\n' << "Press the Enter key to continue.";
    } while (std::cin.get() != '\n');

    return 0;
}
