/***********************************************************************/
/*                                                                     */
/*       This file is part of the Grasp software package               */
/*                                                                     */
/*  Copyright (c) 2019-present, Jocelyn SEROT (jocelyn.serot@uca.fr)   */
/*                       All rights reserved.                          */
/*                                                                     */
/*    This source code is licensed under the license found in the      */
/*      LICENSE file in the root directory of this source tree.        */
/*                                                                     */
/***********************************************************************/

#include "fragment.h"

#include <QString>
#include <QPair>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

QJsonObject Fragment::toJson() const
{
        QJsonObject json;
        json["inps"] = mapToJsonArray(ctx.inps);
        json["outps"] = mapToJsonArray(ctx.outps);
        json["vars"] = mapToJsonArray(ctx.vars);
        json["obj"] = obj;
        return json;
}

Fragment Fragment::fromJson(const QJsonObject &json)
{
        return Fragment(
            jsonArrayToMap(json["inps"].toArray()),
            jsonArrayToMap(json["outps"].toArray()),
            jsonArrayToMap(json["vars"].toArray()),
            json["obj"].toString()
        );
}

// QJsonArray Fragment::listToJsonArray(const QList<QPair<QString, QString>> &list)
// {
//         QJsonArray arr;
//         for (const auto &p : list) {
//             QJsonObject o;
//             o["id"] = p.first;
//             o["type"] = p.second;
//             arr.append(o);
//         }
//         return arr;
// }

// QList<QPair<QString, QString>> Fragment::jsonArrayToList(const QJsonArray &arr)
// {
//         QList<QPair<QString, QString>> list;
//         for (const auto &val : arr) {
//             QJsonObject o = val.toObject();
//             list.append(qMakePair(o["id"].toString(), o["type"].toString()));
//         }
//         return list;
// }

QJsonArray Fragment::mapToJsonArray(const QMap<QString, QString> &map)
{
        QJsonArray arr;
        for (auto i = map.cbegin(), end = map.cend(); i != end; ++i) {
            QJsonObject o;
            o["id"] = i.key();
            o["type"] = i.value();
            arr.append(o);
        }
        return arr;
}

QMap<QString,QString> Fragment::jsonArrayToMap(const QJsonArray &arr)
{
        QMap<QString,QString> r;
        for (const auto &val : arr) {
            QJsonObject o = val.toObject();
            r.insert(o["id"].toString(), o["type"].toString());
        }
        return r;
}
