#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include "ui_outputwidget.h"

#include "Renderer/mediainfo.h"

// BLOCKS
#include "UI/Blocks/blockbasewidget.h"
#include "UI/Blocks/blockresize.h"
#include "UI/Blocks/blockframerate.h"
#include "UI/Blocks/blockvideocodec.h"
#include "UI/Blocks/blockvideobitrate.h"
#include "UI/Blocks/blockvideoprofile.h"
#include "UI/Blocks/blockloops.h"
#include "UI/Blocks/blockstartnumber.h"
#include "UI/Blocks/blockalpha.h"
#include "UI/Blocks/blockpixformat.h"
#include "UI/Blocks/blocksampling.h"
#include "UI/Blocks/blockaudiocodec.h"
#include "UI/Blocks/blockaudiobitrate.h"
#include "UI/Blocks/blockcustom.h"
#include "UI/Blocks/blockcolor.h"
#include "UI/Blocks/blockmapping.h"

// OTHER UI
#include "UI/dropshadow.h"

#include <QFileDialog>
#include <QSettings>
#include <QStringList>
#include <QMenu>

class OutputWidget : public QWidget, private Ui::OutputWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(FFmpeg *ff, int id, MediaList *inputMedias, QWidget *parent = nullptr);
    MediaInfo *getMediaInfo();
    void setMediaInfo(MediaInfo *mediaInfo);

signals:
    void checkSaveOutputPath(QString, OutputWidget*);
    /**
     * @brief console general messages to be displayed in the UI by MainWindow
     */
    void newLog( QString, LogUtils::LogType lt = LogUtils::Information );

public slots:
    void ffmpeg_init();
    void ffmpeg_loadMuxers();
    void loadPresets();

private slots:
    void mediaInfoChanged();
    void inputMediaChanged();
    void inputChanged();
    void customParamActivated(bool activated);

    void on_videoTranscodeButton_clicked(bool checked);
    void on_audioTranscodeButton_clicked(bool checked);
    void on_outputBrowseButton_clicked();
    void on_formatsBox_currentIndexChanged(int index);
    void on_formatsFilterBox_currentIndexChanged(int index);
    void on_presetsBox_currentIndexChanged(int index);
    void on_presetsFilterBox_activated(QString arg1);

    void on_videoButton_clicked(bool checked);
    void on_audioButton_clicked(bool checked);

    void on_actionAddCustom_triggered();
    void on_actionSavePreset_triggered();
    void on_actionOpenPreset_triggered();
    void on_actionDefaultPreset_triggered(bool checked);

private:
    // BLOCKS
    BlockBaseWidget *addBlock(BlockContentWidget *content, QAction *action);
    BlockBaseWidget *blockResize;
    BlockBaseWidget *blockFrameRate;
    BlockBaseWidget *blockVideoCodec;
    BlockBaseWidget *blockVideoBitrate;
    BlockBaseWidget *blockVideoProfile;
    BlockBaseWidget *blockLoops;
    BlockBaseWidget *blockStartNumber;
    BlockBaseWidget *blockAlpha;
    BlockBaseWidget *blockPixFormat;
    BlockBaseWidget *blockSampling;
    BlockBaseWidget *blockAudioCodec;
    BlockBaseWidget *blockAudioBitrate;
    BlockBaseWidget *blockColor;
    BlockBaseWidget *blockMap;

    // MENUS
    QMenu *blocksMenu;
    QMenu *presetsMenu;

    void updateOutputExtension(QString outputPath);
    void addNewParam(QString name = "",QString value = "");

    void selectDefaultPreset();

    QSettings settings;
    FFmpeg *_ffmpeg;
    MediaInfo *_mediaInfo;
    QList<BlockBaseWidget *> _customParams;
    MediaList *_inputMedias;

    bool _freezeUI;
    bool _loadingPreset;

    int _index;
};

#endif // OUTPUTWIDGET_H