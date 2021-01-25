#include "blockstartnumber.h"

BlockStartNumber::BlockStartNumber(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
#ifdef QT_DEBUG
    qDebug() << "Create Start Number block";
#endif
    setType(Type::Video);
    setupUi(this);
}

void BlockStartNumber::activate(bool activate)
{
    if (activate)
    {
        _mediaInfo->setStartNumber( startNumberEdit->value() );
    }
    else
    {
        _mediaInfo->setStartNumber( 0 );
    }
}

void BlockStartNumber::update()
{
    if (!_mediaInfo->isSequence())
    {
        emit blockEnabled(false);
        return;
    }

    startNumberEdit->setValue( _mediaInfo->startNumber() );
}

void BlockStartNumber::on_startNumberEdit_valueChanged(int arg1)
{
    if (_freezeUI) return;
    _mediaInfo->setStartNumber( arg1 );
}
