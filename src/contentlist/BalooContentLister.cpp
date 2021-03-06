/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "BalooContentLister.h"
#include <QFileInfo>

#include <Baloo/IndexerConfig>
#include <Baloo/File>
#include <kfilemetadata/propertyinfo.h>

#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QThreadPool>
#include <QList>

class BalooContentLister::Private
{
public:
    Private() {}
    QList<QString> locations;
    QString searchString;
    QList<Baloo::QueryRunnable*> queries;
};

BalooContentLister::BalooContentLister(QObject* parent)
    : ContentListerBase(parent)
    , d(new Private)
{
}

BalooContentLister::~BalooContentLister()
{
    delete d;
}

bool BalooContentLister::balooEnabled() const
{
    Baloo::IndexerConfig config;
    bool result = config.fileIndexingEnabled();

    if(result)
    {
        // It would be terribly nice with a bit of baloo engine exporting, so
        // we can ask the database about whether or not it is accessible...
        // But, this is a catch-all check anyway, so we get a complete "everything's broken"
        // result if anything is broken... guess it will do :)
        QProcess statuscheck;
        statuscheck.start("balooctl", QStringList() << "status");
        statuscheck.waitForFinished();
        QString output = statuscheck.readAll();
        qDebug() << "Baloo status check says:" << output;
        if(statuscheck.exitStatus() == QProcess::CrashExit || statuscheck.exitCode() != 0)
        {
            result = false;
        }
    }

    return result;
}

void BalooContentLister::addLocation(QString path)
{
    d->locations.append(path);
}

void BalooContentLister::addMimetype(QString mimetype)
{
    Q_UNUSED(mimetype)
    // yes, unsatisfactory... we're using this to find comic books for now, and their mimetypes are terrible
}

void BalooContentLister::setSearchString(const QString& searchString)
{
    d->searchString = searchString;
}

void BalooContentLister::startSearch()
{
    Q_FOREACH(const QString& location, d->locations)
    {
        Baloo::Query query;
        query.setSearchString(d->searchString);
        query.setIncludeFolder(location);

        Baloo::QueryRunnable *runnable = new Baloo::QueryRunnable(query);
        connect(runnable, SIGNAL(queryResult(Baloo::QueryRunnable*, QString)),
                this, SLOT(queryResult(Baloo::QueryRunnable*, QString)), Qt::QueuedConnection);
        connect(runnable, SIGNAL(finished(Baloo::QueryRunnable*)),
                this, SLOT(queryCompleted(Baloo::QueryRunnable*)));

        d->queries.append(runnable);
    }

    if(!d->queries.empty())
    {
        QThreadPool::globalInstance()->start(d->queries.first());
    }
}

void BalooContentLister::queryCompleted(Baloo::QueryRunnable* query)
{
    d->queries.removeAll(query);
    if(d->queries.empty())
    {
        emit searchCompleted();
    }
    else
    {
        QThreadPool::globalInstance()->start(d->queries.first());
    }
}

void BalooContentLister::queryResult(Baloo::QueryRunnable* query, QString file)
{
    Q_UNUSED(query)
    QVariantHash metadata;

    Baloo::File balooFile(file);
    balooFile.load();
    KFileMetaData::PropertyMap properties = balooFile.properties();
    KFileMetaData::PropertyMap::const_iterator it = properties.constBegin();
    for (; it != properties.constEnd(); it++)
    {
        KFileMetaData::PropertyInfo propInfo(it.key());
        metadata[propInfo.name()] = it.value();
//             qDebug() << KFileMetaData::PropertyInfo(it.key()).name() << " --> "
//                 << it.value().toString() << " (" << it.value().typeName() << ")\n";
    }
    QFileInfo info(file);
    metadata["lastModified"] = info.lastModified();
    metadata["created"] = info.created();
    metadata["lastRead"] = info.lastRead();
    emit fileFound(file, metadata);
}
