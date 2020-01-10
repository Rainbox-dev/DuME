#include "blockvideoprofile.h"

BlockVideoProfile::BlockVideoProfile(MediaInfo *mediaInfo, QWidget *parent) :
    UIBlockContent(mediaInfo,parent)
{
    setupUi(this);
}

void BlockVideoProfile::activate(bool activate)
{
    _freezeUI = true;
    if (activate)
    {
        _mediaInfo->setVideoProfile( videoProfileBox->currentData().toString() );
    }
    else
    {
        _mediaInfo->setVideoProfile( "" );
    }
    _freezeUI = false;
}

void BlockVideoProfile::update()
{
    if(_freezeUI) return;
    _freezeUI = true;

    //Update profiles list
    videoProfileBox->clear();

    FFCodec *c = _mediaInfo->videoCodec();
    if ( c == nullptr ) c = _mediaInfo->defaultVideoCodec();
    if ( c == nullptr )
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    if ( c->name().startsWith("prores") )
    {
        videoProfileBox->addItem("Proxy", "0");
        videoProfileBox->addItem("LT", "1");
        videoProfileBox->addItem("Normal", "2");
        videoProfileBox->addItem("HQ", "3");
        emit blockEnabled(true);
    }
    else if ( c->name() == "h264" )
    {
        videoProfileBox->addItem("Baseline", "baseline");
        videoProfileBox->addItem("Main", "main");
        videoProfileBox->addItem("High", "high");
        emit blockEnabled(true);
    }
    else
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }

    for (int i = 0; i < videoProfileBox->count(); i++)
    {
        if ( _mediaInfo->videoProfile() == videoProfileBox->itemData(i).toString() )
        {
            videoProfileBox->setCurrentIndex(i);
            break;
        }
    }
    _freezeUI = false;
}

void BlockVideoProfile::on_videoProfileBox_currentIndexChanged(int index)
{
    if(_freezeUI) return;
    _freezeUI = true;
    _mediaInfo->setVideoProfile( videoProfileBox->itemData(index).toString() );
    _freezeUI = false;
}
