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

#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules; // Applied in order (a later rule overrides an earlier one)

    // Adds a rule highlighting the (whole) [words] with [format]
    void addKeywords(const QStringList &words, const QTextCharFormat &format, bool caseInsensitive = false);

    // Comments spanning several lines (/* ... */, for ex.); not highlighted if [commentStart] is not set
    QRegularExpression commentStart;
    QRegularExpression commentEnd;
    QTextCharFormat commentFormat;
};

class FsmSyntaxHighlighter : public SyntaxHighlighter
{
public:
  FsmSyntaxHighlighter(QTextDocument *parent = 0);
};

class CTaskSyntaxHighlighter : public SyntaxHighlighter
{
public:
  CTaskSyntaxHighlighter(QTextDocument *parent = 0);
};

class SystemCSyntaxHighlighter : public SyntaxHighlighter
{
public:
  SystemCSyntaxHighlighter(QTextDocument *parent = 0);
};

class VhdlSyntaxHighlighter : public SyntaxHighlighter
{
public:
  VhdlSyntaxHighlighter(QTextDocument *parent = 0);
};
