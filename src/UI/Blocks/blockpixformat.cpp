#include "blockpixformat.h"

BlockPixFormat::BlockPixFormat(MediaInfo *mediaInfo, QWidget *parent) :
    BlockContentWidget(mediaInfo,parent)
{
    setupUi(this);
    _freezeUI = true;
    listPixFormats();
    _freezeUI = false;
}

void BlockPixFormat::activate(bool activate)
{
    _freezeUI = true;

    if (activate && pixFmtFilterBox->currentIndex() != 0 )
    {
        _mediaInfo->setPixFormat( pixFmtBox->currentData(Qt::UserRole).toString() );
    }
    else
    {
        _mediaInfo->setPixFormat( nullptr );
    }

    _freezeUI = false;
}

void BlockPixFormat::update()
{
    if (_freezeUI) return;

    if (!_mediaInfo->hasVideo())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    VideoInfo *stream = _mediaInfo->videoStreams()[0];
    if(stream->isCopy())
    {
        emit blockEnabled(false);
        _freezeUI = false;
        return;
    }
    emit blockEnabled(true);

    listPixFormats( );

    _freezeUI = true;

    filterPixFormats();

    _freezeUI = true;

    FFPixFormat *pf = stream->pixFormat();
    if ( pf == nullptr )
    {
        setDefaultPixFormat();
    }
    else
    {
        setPixFormat( pf->name() );
    }

    _freezeUI = false;
}

void BlockPixFormat::listPixFormats()
{
    _freezeUI = true;

    QString prevFilter = pixFmtFilterBox->currentText();
    QString prevFormat = pixFmtBox->currentData().toString();
    pixFmtBox->clear();
    pixFmtFilterBox->clear();
    _pixFormats.clear();

    if (!_mediaInfo->hasVideo())
    {
        _freezeUI = false;
        return;
    }

    FFCodec *vc = _mediaInfo->videoStreams()[0]->codec();
    if ( vc == nullptr ) vc = _mediaInfo->defaultVideoCodec();
    if ( vc == nullptr )
    {
        _freezeUI = false;
        return;
    }

    _pixFormats = vc->pixFormats();

    QStringList filters = QStringList();
    foreach( FFPixFormat *p, vc->pixFormats() )
    {
        QString filter = QString::number(p->bitsPerPixel()) + " bits - " + QString::number(p->numComponents()) + " channels" ;
        if (!filters.contains(filter)) filters << filter;
    }

    filters.sort();
    pixFmtFilterBox->addItem("Default");
    pixFmtFilterBox->addItem("All bit depths");
    foreach(QString filter,filters)
    {
        pixFmtFilterBox->addItem(filter);
    }

    // reselect previous
    setFilter( prevFilter );
    setPixFormat( prevFormat );

    _freezeUI = false;
}

void BlockPixFormat::filterPixFormats()
{
    _freezeUI = true;

    QString prevFormat = pixFmtBox->currentData().toString();
    pixFmtBox->clear();

    bool alpha = _mediaInfo->hasAlpha();
    QString filter = pixFmtFilterBox->currentText();
    foreach( FFPixFormat *p, _pixFormats )
    {
        if ( p->hasAlpha() == alpha && ( p->prettyName().indexOf( filter ) > 0 || pixFmtFilterBox->currentIndex() <= 1 ) )
        {
            pixFmtBox->addItem(p->prettyName(), p->name() );
        }
    }

    setPixFormat( prevFormat, false );

    _freezeUI = false;
}

void BlockPixFormat::setDefaultPixFormat()
{
    _freezeUI = true;

    pixFmtFilterBox->setCurrentIndex( 0 );
    filterPixFormats();

    _freezeUI = true;

    if (!_mediaInfo->hasVideo())
    {
        _freezeUI = false;
        return;
    }

    FFPixFormat *pf = _mediaInfo->videoStreams()[0]->defaultPixFormat();
    if (pf == nullptr) pf = _mediaInfo->defaultPixFormat();

    if (pf == nullptr)
    {
        pixFmtBox->setCurrentIndex( -1 );
    }
    else
    {
        setPixFormat( pf->name() );
    }

    pixFmtBox->setEnabled( false );

    _freezeUI = false;
}

void BlockPixFormat::setPixFormat(QString name, bool tryWithoutFilter )
{
    _freezeUI = true;

    for(int i = 0; i < pixFmtBox->count(); i++)
    {
        if (pixFmtBox->itemData( i ).toString() == name )
        {
            pixFmtBox->setCurrentIndex( i );
            _freezeUI = false;
            return;
        }
    }

    //try again without filter
    if ( tryWithoutFilter )
    {
        pixFmtFilterBox->setCurrentIndex( 0 );
        filterPixFormats();
        setPixFormat( name, false );
    }
    else
    {
        pixFmtBox->setCurrentIndex( -1 );
    }

    _freezeUI = false;
}

void BlockPixFormat::setFilter(QString name)
{
    _freezeUI = true;

    pixFmtFilterBox->setCurrentText( name );
    filterPixFormats();

    _freezeUI = false;
}

void BlockPixFormat::on_pixFmtFilterBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    if ( index == 0 )
    {
        _mediaInfo->setPixFormat( nullptr );
    }
    else
    {
        filterPixFormats();
        pixFmtBox->setEnabled(true);
    }

}

void BlockPixFormat::on_pixFmtBox_currentIndexChanged(int index)
{
    if (_freezeUI) return;
    _mediaInfo->setPixFormat( pixFmtBox->itemData(index, Qt::UserRole).toString() );
}
