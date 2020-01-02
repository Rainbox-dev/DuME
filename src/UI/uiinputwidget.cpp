#include "uiinputwidget.h"

#ifdef QT_DEBUG
#include <QtDebug>
#endif

UIInputWidget::UIInputWidget(FFmpeg *ff, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _mediaInfo = new MediaInfo( ff, this);

    // CREATE MENUS
    blocksMenu = new QMenu(this);
    blocksMenu->setTearOffEnabled(true);
    addBlockButton->setMenu( blocksMenu );

    // CREATE BLOCKS
    blockFrameRateContent = new BlockFrameRate( _mediaInfo );
    blockFrameRate = addBlock( blockFrameRateContent, actionFramerate );
    blockColorContent = new BlockColor( _mediaInfo );
    blockColor = addBlock( blockColorContent, actionColor );
    blockEXRContent = new BlockEXR( _mediaInfo );
    blockEXR = addBlock( blockEXRContent, actionEXR );
    blockAECompContent = new BlockAEComp( _mediaInfo );
    blockAEComp = addBlock( blockAECompContent, actionAfter_Effects_composition);
    blockAEThreadsContent = new BlockAEThreads( _mediaInfo );
    blockAEThreads = addBlock( blockAEThreadsContent, actionAfter_Effects_threads);
    blocksMenu->addAction( actionAddCustom );

    //TODO Fix connections between input and output

    updateOptions();
}

MediaInfo *UIInputWidget::getMediaInfo()
{
    //ADD CUSTOM PARAMS
    foreach ( UIBlockWidget *b, _customParams )
    {
        BlockCustom *bc = static_cast<BlockCustom *>( b->content() );
        QString param = bc->value();
        if (param != "")
        {
            QStringList option(param);
            option << bc->param();
            _mediaInfo->addFFmpegOption(option);
        }
    }

    return _mediaInfo;
}

void UIInputWidget::openFile(QString file)
{
    QSettings settings;
    if (file == "") return;
    file = QDir::toNativeSeparators( file );

    QFileInfo fileInfo(file);
    _mediaInfo->updateInfo( fileInfo );

    //keep in settings
    settings.setValue("input/path", fileInfo.path() );

    //update UI
    inputEdit->setText( file );

    QString mediaInfoString = "Media information\n\n";
    mediaInfoString += _mediaInfo->getDescription();

    mediaInfosText->setText(mediaInfoString);

    updateOptions();

    emit newMediaLoaded(_mediaInfo);
}

void UIInputWidget::openFile(QUrl file)
{
    openFile(file.toLocalFile());
}

void UIInputWidget::on_inputBrowseButton_clicked()
{
    QSettings settings;
    QString inputPath = QFileDialog::getOpenFileName(this,"Select the media file to transcode",settings.value("input/path","").toString());
    if (inputPath == "") return;
    openFile(inputPath);
}

void UIInputWidget::on_inputEdit_editingFinished()
{
    //check if file exists, try to read url
    QUrl test(inputEdit->text());
    if (!test.isEmpty())
    {
        if (test.isValid())
        {
            inputEdit->setText(test.toLocalFile());
        }
    }

    openFile(inputEdit->text());
}

UIBlockWidget *UIInputWidget::addBlock(UIBlockContent *content, QAction *action)
{
    // create block
    UIBlockWidget *b = new UIBlockWidget( action->text(), content, blocksWidget);
    blocksLayout->addWidget( b );
    //add and connect action
    blocksMenu->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), b, SLOT( setVisible(bool) ) );
    connect( b, SIGNAL( activated(bool) ), action, SLOT( setChecked( bool ) ) );

    return b;
}

void UIInputWidget::addNewParam(QString name, QString value)
{
    //add a param and a value
    qDebug() << "New Custom param: " + name + " " + value;
    BlockCustom *block = new BlockCustom( _mediaInfo, name, value );
    UIBlockWidget *bw = new UIBlockWidget( "FFmpeg parameter", block, blocksWidget );
    blocksLayout->addWidget( bw );
    bw->show();
    connect( bw, SIGNAL(activated(bool)), this, SLOT(customParamActivated(bool)));
    //add to list
    _customParams << bw;
}

void UIInputWidget::on_actionAddCustom_triggered()
{
    addNewParam();
}

void UIInputWidget::updateOptions()
{
    //get media default extension (needed to adjust some options)
    QString extension = "";
    if (_mediaInfo->extensions().count() > 0) extension = _mediaInfo->extensions()[0];

    // framerate buttons
    if (_mediaInfo->isImageSequence() || _mediaInfo->isAep())
    {
        actionFramerate->setVisible( true );
        blockFrameRate->show();
    }
    else
    {
        actionFramerate->setVisible( false );
        blockFrameRate->hide();
    }

    //exr prerender
    actionEXR->setVisible( extension == "exr_pipe" );
    blockEXR->hide();

    if (_mediaInfo->isAep())
    {
        actionAfter_Effects_composition->setVisible(true);
        blockAEComp->show();
        actionAfter_Effects_threads->setVisible(true);
    }
    else
    {
        actionAfter_Effects_composition->setVisible(false);
        blockAEComp->hide();
        actionAfter_Effects_threads->setVisible(false);
    }
    blockAEThreads->hide();

    emit newMediaLoaded(_mediaInfo);
}
