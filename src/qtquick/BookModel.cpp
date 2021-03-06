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

#include "BookModel.h"

struct BookPage {
    BookPage() {}
    QString url;
    QString title;
};

class BookModel::Private {
public:
    Private() {}
    QString filename;
    QString author;
    QString publisher;
    QList<BookPage*> entries;
};

BookModel::BookModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

BookModel::~BookModel()
{
    delete d;
}

QHash<int, QByteArray> BookModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UrlRole] = "url";
    roles[TitleRole] = "title";
    return roles;
}

QVariant BookModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const BookPage* entry = d->entries[index.row()];
        switch(role)
        {
            case UrlRole:
                result.setValue(entry->url);
                break;
            case TitleRole:
                result.setValue(entry->title);
                break;
            default:
                result.setValue(QString("Unknown role"));
                break;
        }
    }
    return result;
}

int BookModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void BookModel::addPage(QString url, QString title)
{
    BookPage* page = new BookPage();
    page->url = url;
    page->title = title;

    beginInsertRows(QModelIndex(), d->entries.count(), d->entries.count());
    d->entries.append(page);
    endInsertRows();
}

QString BookModel::filename() const
{
    return d->filename;
}

void BookModel::setFilename(QString newFilename)
{
    d->filename = newFilename;
    emit filenameChanged();
}

QString BookModel::author() const
{
    return d->author;
}

void BookModel::setAuthor(QString newAuthor)
{
    d->author = newAuthor;
    emit authorChanged();
}

QString BookModel::publisher() const
{
    return d->publisher;
}

void BookModel::setPublisher(QString newPublisher)
{
    d->publisher = newPublisher;
    emit publisherChanged();
}
