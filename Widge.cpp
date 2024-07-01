#include "Widge.h"
#include "ui_Widge.h"
#include <QPixmap>
#include <QPalette>
#include <QDesktopServices> //打开文件夹
#include <QMessageBox>   //消息提示
#include <QFileDialog>  //操作文件
#include <QDir>         //操作路径
#include <dwmapi.h>
#include <windowsx.h>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAction>
#include <QMouseEvent>
#include <QList>
#include <QUrl>
#include <QStringList>
#include <QPaintEvent>    //头文件是把我知道的全加上了（
#include <QRandomGenerator>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTimer>
#include <QTextEdit>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

/*
    注：D:\\mypvz()代指音乐文件路径；
    我自己加入了listWidget_3 作用是搜索时弹出符合条件的目标
        加入了pushButton_6  作用是点击使listWidge_3隐藏      因为不会布局，所以就随便找了个地方放  pushButton_6最好紧贴listWidge_3
    暂用listWidget显示播放列表  添加歌曲使用pushButtun_5(反正全用的右边的
    pushButton_5用以添加指定文件夹中的所有mp3文件
*/

Widget::Widget(QWidget *parent)
    : QWidget(parent),
     ui(new Ui::Widget),
    list_vi(false),
    currentIndex(0),
    playbackMode(0),
    player(nullptr),
    isPlaying(false),
    flag1(false),
    isMute(false),
    pre_volume(30.0),
    Delete(true),
    nonlrc(false)
{
    ui->setupUi(this);
    //ui->pushButton_6->setVisible(false);
    //ui->listWidget_3->setVisible(false);
    ui->Box_Speed->setVisible(false);
    ui->Button_PlayPause->setIcon(QIcon(":/new/prefix1/res/Icon_Pause.png"));
    ui->Button_Volume->setIcon(QIcon(":/new/prefix1/res/Icon_Volume.png"));
    ui->Button_Volume->setIconSize(QSize(40,40));
    // this->setStyleSheet("QWidget { background-image: url(:/new/prefix1/res/background.png); }");
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    QPushButton *Button_PlayPause = new QPushButton(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0);
    Button_PlayPause->setIcon(QIcon(":/new/prefix1/res/Icon_Pause.png"));
    setupConnections(); //connect
    clickTimer = new QTimer(this);
    clickTimer->setSingleShot(true);
    clickTimer->setInterval(200); // 300毫秒用于区分单击和双击
    ui->Text_Lyric->setReadOnly(false);
    ui->Text_Lyric->setText("这个程序没有bug");
    connect(clickTimer, &QTimer::timeout, this, &Widget::Timeout);
    connect(ui->Button_AddToList,&QPushButton::clicked,this,&Widget::add_music);
    //connect(ui->pushButton_6,&QPushButton::clicked,this,&Widget::change_run);
    // connect(ui->Search_Lib,&QLineEdit::textChanged,this,&Widget::select_music);
    connect(ui->Search_Lib,SIGNAL(returnPressed()),this,SLOT(Func_searchlib()));
    //connect(ui->pushButton_6,&QPushButton::clicked,this,&Widget::change_run);
    connect(ui->Button_AddToLib,&QPushButton::clicked,this,&Widget::open_music_file);
}
//connect(ui->listWidget_2,&QPushButton::clicked,this,&music::open_music_file);
Widget::~Widget()
{
    delete ui;
    if(player){
        player->stop();
        delete player;
    }
    delete audioOutput;
    delete clickTimer;
}

/*void Widget::change_vi_li(){
    list_vi=!list_vi;
    ui->listWidget_3->setVisible(list_vi);
}*/

void Widget::open_music_file(){            //这里会先清空列表
    SongLib.clear();
    //QMessageBox::information(this,"文件","打开文件");
    auto path = QFileDialog::getExistingDirectory(this,"选择音乐所在的路径","D:\\mypvz()");
    QDir dir(path);
    auto musiclist=dir.entryList(QStringList()<<"*.mp3");
    ui->List_Lib->clear();
    ui->List_Lib->addItems(musiclist);
    //默认选择第一首音乐
    ui->List_Lib->setCurrentRow(0);
    //把音乐完整路径保存起来
    for(auto file : musiclist)
        SongLib.append(QUrl::fromLocalFile(path+"\\"+file));  //添加
    //change_vi_li();

}

void Widget::add_music(){

}

/*void Widget::select_music(const QString &text){
    m_searchText = text;
    f_music();
}
*/
/* void Widget::Func_searchlib(){
//     QString searchTarget = ui->Search_Lib->text();
//     if(searchTarget.isEmpty())
//         return;
//     searchlist.clear();
//     QString searchTarget_wildcard;
//     searchTarget_wildcard.clear();
//     searchTarget_wildcard.push_back('*');
//     for (int i = 0; i < (int)searchTarget.size(); i++)
//     {
//         searchTarget_wildcard.push_back(searchTarget[i]);
//         searchTarget_wildcard.push_back('*');
//     }
//     QList<QListWidgetItem*> match = ui->List_Lib->findItems(searchTarget_wildcard, Qt::MatchWildcard);
//     ui->List_Lib->clear();
//     for (int i = 0; i < (int)match.size(); i++)
//         ui->List_Lib->addItem(match[i]->text());
// }
*/

void Widget::Func_searchlib() {
    QString searchTarget = ui->Search_Lib->text().trimmed();
    if(searchTarget.isEmpty()) {
        ui->List_Lib->clear(); // 如果搜索框为空，则清空列表显示
        for (const QUrl &url : qAsConst(SongLib)) {
            ui->List_Lib->addItem(url.fileName()); // 使用fileName()而不是toString()
        }
        return;
    }

    QList<QUrl> searchResults;
    for (const QUrl &url : qAsConst(SongLib)) {
        if (url.toString().contains(searchTarget, Qt::CaseInsensitive)) { // 使用contains进行模糊匹配
            searchResults.append(url);
        }
    }

    ui->List_Lib->clear(); // 清空当前列表
    for (const QUrl &url : qAsConst(searchResults)) {
        ui->List_Lib->addItem(url.fileName()); // 使用fileName()而不是toString()
    }
}

/*void Widget::f_music(){
    ui->pushButton_6->setVisible(true);
    ui->listWidget_3->setVisible(true);
    searchlist.clear();
    ui->listWidget_3->clear();
    if(!m_searchText.isEmpty()){
    bool f1=false;
    for(auto file : playlist){
        QString str=file.toString().mid(8);
        for(int i=0;i<str.length();i++){
            bool f=true;
            for(int j=0;j<m_searchText.length();j++){
                if(str[i+j]!=m_searchText[j]){
                    f=false;
                    break;
                }
            }
            if(f){
                f1=true;
                searchlist.append(str);
            }
        }
    }
    if(!f1){
        searchlist.append("there is nothing :p");  //字符串内容可以改
    }
    }
    if(m_searchText.isEmpty()){
        searchlist.append("这里没有bug :)");        //字符串内容可以改
    }
    ui->listWidget_3->addItems(searchlist);
}*/

void Widget::change_run(){
    //ui->pushButton_6->setVisible(false);
    //ui->listWidget_3->setVisible(false);
    ui->Search_Lib->clear();
}

void Widget::playCurrent() //目前状态函数
{
    if (!playlist.isEmpty() && currentIndex >= 0 && currentIndex < playlist.size()) {
        connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
                next();
            }
        });
        player->setSource(playlist[currentIndex]);
        player->play();

        QString lyricPath = playlist[currentIndex].toLocalFile(); // 获取本地文件路径
        if (lyricPath.length() >= 3) {
            lyricPath = lyricPath.left(lyricPath.length() - 3) + "lrc";
        }

        QFile lyricFile(lyricPath);
        ui->Text_Lyric->setText(lyricPath);
        if (lyricFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&lyricFile);
            QString lyrics = in.readAll(); // 读取整个歌词文件
            ui->Text_Lyric->setPlainText(lyrics); // 将歌词显示在QTextEdit中
            lyricFile.close();
        } else {
            ui->Text_Lyric->setPlainText("纯音乐"); // 如果找不到歌词文件，显示提示信息
            Lyric.clear();
        }
    }
}

void Widget::play() //播放函数
{
    // 如果已有播放器实例，则继续播放当前媒体，否则播放当前选定的媒体文件
    if (player) {
        player->play();
    } else {
        playCurrent();
    }
    ui->Button_PlayPause->setIcon(QIcon(":/new/prefix1/res/Icon_Pause.png"));
}

void Widget::pause() //暂停函数
{
    // 如果有播放器实例，则暂停播放
    if (player) {
        player->pause();
        ui->Button_PlayPause->setIcon(QIcon(":/new/prefix1/res/Icon_Play.png"));
    }
}

void Widget::playpause() //播放和暂停切换函数
{
    if (isPlaying) {
        pause();
        // ui->pushButton->setText("播放"); // 或者更新图标为播放图标
    } else {
        play();
        // ui->pushButton->setText("暂停"); // 或者更新图标为暂停图标
    }
    isPlaying = !isPlaying;
}


void Widget::next() //下一首
{
    if (playbackMode == 2){
        currentIndex = currentIndex;
    }else if (playbackMode == 1) { // 随机模式
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    } else { //顺序模式
        currentIndex = (currentIndex + 1) % playlist.size();
    }
    playCurrent();
}

void Widget::previous() //上一首
{
    if (playbackMode == 2){
        currentIndex = currentIndex;
    }else if (playbackMode == 1) { // 随机模式
        currentIndex = QRandomGenerator::global()->bounded(playlist.size());
    } else { //顺序模式
        currentIndex = (currentIndex - 1 + playlist.size()) % playlist.size();
    }
    playCurrent();
}

void Widget::setVolume(int volume) //音量调节
{
    if (player) {
        audioOutput->setVolume(volume/100.0);
        if(volume == 0){
            isMute = true;
            ui->Button_Volume->setIcon(QIcon(":/new/prefix1/res/Icon_Mute.png"));
        }
        else
            pre_volume = volume;
    }
}

void Widget::setPlaybackRate(int index) //速率调节
{
    qreal rate=1.0;
    switch(index){
        case 0:
        rate=0.5;break;
        case 1:
            rate=1.0;break;
        case 2:
            rate=1.5;break;
        case 3:
            rate=2.0;break;
    }
    if (player) {
        player->setPlaybackRate(rate);
    }
}
//点按音量键可以切换是否静音
void Widget::Func_Mute()
{
    if(isMute){
        audioOutput->setVolume(pre_volume);
        ui->Button_Volume->setIcon(QIcon(":/new/prefix1/res/Icon_Volume.png"));
    } else{
        audioOutput->setVolume(0.0);
        ui->Button_Volume->setIcon(QIcon(":/new/prefix1/res/Icon_Mute.png"));
    }
    isMute=!isMute;
}

void Widget::showRateSlider()
{
    if(flag1){
        ui->Box_Speed->setVisible(true);
    } else{
        ui->Box_Speed->setVisible(false);
    }
    flag1=!flag1;
}

void Widget::setPlaybackMode(int mode) //模式选择
{
    playbackMode = mode;
}

void Widget::playSelectedURL(int index) //播放选择的内容
{
    if (index >= 0 && index < playlist.size()) {
        player->setSource(playlist[index]);
        play();
        // ui->pushButton->setText("Pause");

        QString lyricPath = playlist[index].toLocalFile(); // 获取本地文件路径
        if (lyricPath.length() >= 3) {
            lyricPath = lyricPath.left(lyricPath.length() - 3) + "lrc";
        }

        QFile lyricFile(lyricPath);
        if (lyricFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&lyricFile);
            QString lyrics = in.readAll(); // 读取整个歌词文件
            // ui->Text_Lyric->setPlainText(lyrics); // 将歌词显示在QTextEdit中
            QRegularExpression re("\\[(\\d{2}):(\\d{2})\\.(\\d{2})\\](.*)");
            QStringList lines = lyrics.split("\n");
            Lyric.clear();
            for (const QString &line : lines) {
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch()) {
                    int minutes = match.captured(1).toInt();
                    int seconds = match.captured(2).toInt();
                    int centiseconds = match.captured(3).toInt();
                    QString text = match.captured(4).trimmed();
                    qint64 time = (minutes * 60 * 1000) + (seconds * 1000) + (centiseconds * 10);
                    Lyric.append(LrcLine(time, text));
                }
            }
            for (const LrcLine &line : Lyric) {
                htmlContent += QString("<p id='%1'>%2</p>").arg(line.time).arg(line.text);
            }
            ui->Text_Lyric->clear();
            ui->Text_Lyric->setHtml(htmlContent);
            nonlrc = false;
            lyricFile.close();
        } else {
            ui->Text_Lyric->setPlainText("纯音乐"); // 如果找不到歌词文件，显示提示信息
            nonlrc = true;
        }

    }
}

void Widget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        next();
    }
}

void Widget::setupConnections()
{
    connect(ui->Button_PlayPause, &QPushButton::clicked, this, &Widget::playpause);
    connect(ui->Button_Next, &QPushButton::clicked, this, &Widget::next);
    connect(ui->Button_Previous, &QPushButton::clicked, this, &Widget::previous);
    connect(ui->Slider_Volume, &QSlider::valueChanged, this, &Widget::setVolume);
    connect(ui->Box_Speed, QOverload<int>::of(&QComboBox::activated), this, &Widget::setPlaybackRate);
    connect(ui->Box_Mode, QOverload<int>::of(&QComboBox::activated), this, &Widget::setPlaybackMode);
    connect(ui->Button_Volume, &QPushButton::clicked, this, &Widget::Func_Mute);
    connect(ui->Button_Speed, &QPushButton::clicked, this, &Widget::showRateSlider);
    connect(ui->List_ToPlay, &QListWidget::itemClicked, this, &Widget::Func_ListClick);
    //播放与进度条的同步
    connect(player, &QMediaPlayer::durationChanged, ui->Slider_Process, [&](qint64 duration){
        ui->Slider_Process->setRange(0, duration);
    });
    connect(player, &QMediaPlayer::positionChanged, ui->Slider_Process, [&](qint64 position){
        if (!ui->Slider_Process->isSliderDown()) {
            ui->Slider_Process->setValue(position);
        }
        ui->label->setText(QTime::fromMSecsSinceStartOfDay(position).toString());
        ui->label_4->setText(QTime::fromMSecsSinceStartOfDay(player->duration()).toString());
    });
    connect(ui->Slider_Process, &QSlider::sliderReleased, this, [&](){
        player->setPosition(ui->Slider_Process->value());
    });
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChanged);
    connect(ui->List_Lib, &QListWidget::itemClicked, this, &Widget::Func_LibClick);
    connect(ui->List_Lib, &QListWidget::itemDoubleClicked, this, &Widget::Func_DeleteLib);
    connect(ui->List_ToPlay, &QListWidget::itemDoubleClicked, this, &Widget::Func_DeleteList);
    connect(player, &QMediaPlayer::positionChanged, this, &Widget::updateLyrics);
}

void Widget::updateLyrics(qint64 position) {
    // 找到当前时间最接近的歌词行
    if(nonlrc)
        return;
    int closestIndex = -1;
    qint64 closestTimeDiff = std::numeric_limits<qint64>::max();

    for (int i = 0; i < Lyric.size(); ++i) {
        qint64 timeDiff = std::abs(Lyric[i].time - position);
        if (timeDiff < closestTimeDiff) {
            closestTimeDiff = timeDiff;
            closestIndex = i;
        } else {
            // 因为Lyric是按时间排序的，所以一旦开始增加，就可以停止搜索
            break;
        }
    }

    if (closestIndex != -1) {
        // 构建新的HTML内容，高亮显示当前行
        QString htmlContent;
        for (int i = 0; i < Lyric.size(); ++i) {
            if (i == closestIndex) {
                htmlContent += QString("<p id='%1' style='color:red;'>%2</p>").arg(Lyric[i].time).arg(Lyric[i].text);
            } else {
                htmlContent += QString("<p id='%1'>%2</p>").arg(Lyric[i].time).arg(Lyric[i].text);
            }
        }
        ui->Text_Lyric->clear();
        ui->Text_Lyric->setHtml(htmlContent);

        // 滚动到当前歌词行
        if(closestIndex <= 3)
            ui->Text_Lyric->scrollToAnchor(QString::number(Lyric[0].time));
        else
            ui->Text_Lyric->scrollToAnchor(QString::number(Lyric[closestIndex-3].time));
    }
}

void Widget::on_Button_LibClear_clicked()
{
    ui->List_Lib->clear();
}

void Widget::on_Button_ListClear_clicked()
{
    ui->List_ToPlay->clear();
}

void Widget::Func_LibClick(QListWidgetItem *item){
    if (!clickTimer->isActive()) {
        Delete = true;
        clickTimer->start();
        tempitem = item;
    }

}

void Widget::Func_AddToList()
{
    int index = ui->List_Lib->row(tempitem); // 获取被点击项的索引
    if (index < 0 || index >= SongLib.size()) return; // 确保索引有效

    QUrl songUrl = SongLib.at(index); // 从源曲库中获取对应的 QUrl
    playlist.append(songUrl); // 将 QUrl 添加到目标曲库中

    // 在目标 QListWidget 中显示歌曲名称
    ui->List_ToPlay->addItem(tempitem->text());
}

void Widget::Func_DeleteLib(QListWidgetItem *item)
{
    clickTimer->stop();
    int index = ui->List_Lib->row(item); // 获取被双击项的索引
    if (index < 0 || index >= SongLib.size()) return; // 确保索引有效
    SongLib.removeAt(index); // 从曲库中删除对应的 QUrl
    delete ui->List_Lib->takeItem(index); // 从 QListWidget 中移除项
}

void Widget::Func_DeleteList(QListWidgetItem *item)
{
    clickTimer->stop();
    int index = ui->List_ToPlay->row(item); // 获取被双击项的索引
    if (index < 0 || index >= playlist.size()) return; // 确保索引有效
    playlist.removeAt(index); // 从曲库中删除对应的 QUrl
    delete ui->List_ToPlay->takeItem(index); // 从 QListWidget 中移除项

}

void Widget::Timeout() {
    // 定时器到期，处理单击事件
    if(Delete)
        Func_AddToList();
    else
        Func_ClickPlay();
}

void Widget::Func_ListClick(QListWidgetItem *item)
{
    if (!clickTimer->isActive()) {
        Delete = false;
        clickTimer->start();
        tempitem = item;
    }
}

void Widget::Func_ClickPlay(){
    int index = ui->List_ToPlay->row(tempitem);
    playSelectedURL(index);
}

void Widget::on_List_ToPlay_itemDoubleClicked(QListWidgetItem *item){};
