#ifndef WIDGE_H
#define WIDGE_H

#include <QWidget>
#include <QMediaPlayer>
#include <QStringList>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QComboBox>
#include <QProgressBar>
#include <QAudio>
#include <QAudioOutput>
#include <QList>
#include <QUrl>
#include <QString>
#include <QtMultimedia>
#include <QListWidgetItem>
#include <QListWidget>
#include <QTimer>
#include <QTextEdit>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class LrcLine {
public:
    qint64 time; // 歌词对应的时间（毫秒）
    QString text; // 歌词文本

    LrcLine(qint64 time, QString text) : time(time), text(text) {}
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //void select_music(const QString &text);    //就读个信息
    //void f_music();         //正经查找
    void open_music_file(); //这个只是播放列表
    void add_music();       //只是打开文件夹（
    //void change_vi_li();     //打开关闭播放列表
    void change_run();       //关闭搜索框
    void Func_searchlib();
    //播放控制相关函数
    void pause();
    void play();
    void playpause();
    void next();
    void previous();
    //音量、播放速率、播放模式设置
    void setVolume(int volume);
    void setPlaybackRate(int index);
    void setPlaybackMode(int mode);
    //声音，倍速的可视化
    void Func_Mute();
    void showRateSlider();
    //播放选定
    void playSelectedURL(int index);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_Button_LibClear_clicked();
    void on_Button_ListClear_clicked();
    void Func_AddToList();
    void Func_LibClick(QListWidgetItem *item);
    void Func_ListClick(QListWidgetItem *item);
    void Func_DeleteLib(QListWidgetItem *item);
    void Func_DeleteList(QListWidgetItem *item);
    void Timeout();
    void on_List_ToPlay_itemDoubleClicked(QListWidgetItem *item);
    void Func_ClickPlay();
    void updateLyrics(qint64 position);

private:
    Ui::Widget *ui;
    QString m_searchText;   //查找时输入的文本（
    QList<QString> searchlist;
    QList<QUrl> playlist;   //列表
    QList<QUrl> SongLib;    // 曲库
    QList<LrcLine> Lyric;
    bool list_vi;           //控制播放列表的可视
    int currentIndex;//当前文件索引
    int playbackMode;//播放模式：0 - 循环，1 - 顺序，2 - 随机
    QMediaPlayer *player;//媒体播放器指针
    void playCurrent();//播放当前选定的文件
    void setupConnections();//设置信号槽连接
    void createMediaPlayer();//创建新房QMediaPlayer实例
    bool isPlaying;//判断是否在播放
    QAudioOutput *audioOutput;
    bool flag1,isMute,Delete,nonlrc;
    int pre_volume;
    QTimer* clickTimer;
    QListWidgetItem *tempitem;
    QString htmlContent;
};


#endif // WIDGE_H
