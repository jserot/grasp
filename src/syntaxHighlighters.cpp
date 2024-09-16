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


#include "syntaxHighlighters.h"
#include <QRegularExpression>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpression re(rule.pattern);
        Q_ASSERT(re.isValid());
        QRegularExpressionMatch match = re.match(text);
        int index = match.capturedStart();
        int length = match.capturedLength();
        setFormat(index, length, rule.format);
    }
    setCurrentBlockState(0);
}

FsmSyntaxHighlighter::FsmSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightingRule rule;

    rule.format.setForeground(Qt::blue);
    //rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\btype\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bfsm\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bmodel\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\binput\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\boutput\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bshared\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\s->\\s"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bon\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bwhen\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bwith\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\|\\s"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("!\\s"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::darkGreen);
    rule.format.setFontWeight(QFont::Normal);
    //rule.pattern = QRegularExpression("#[a-z]+\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bin\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bout\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\binout\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bstates\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bvars\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\btrans\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bitrans\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bsporadic\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bperiodic\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bvalue_changes\\b"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bevent\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bint\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bbool\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\barray\\b"); highlightingRules.append(rule);
    //rule.pattern = QRegularExpression("[A-Za-z]+\\s*:"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::gray);
    rule.format.setFontItalic(true);
    rule.pattern = QRegularExpression("-- [^\n]*"); highlightingRules.append(rule);
}

CTaskSyntaxHighlighter::CTaskSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    HighlightingRule rule;

    rule.format.setForeground(Qt::blue);
    rule.format.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression("\\btask\\b"); highlightingRules.append(rule);
    rule.format.setFontWeight(QFont::Normal);
    rule.pattern = QRegularExpression("\\bin\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bout\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\binout\\b"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::darkYellow);
    rule.pattern = QRegularExpression("\\wait_ev\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\wait_evs\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\notify_ev\\b"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("#[a-z]+\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bwhile\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bswitch\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bcase\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bbreak\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bif\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\belse\\b"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bevent\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bint\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bbool\\b"); highlightingRules.append(rule);
    //rule.pattern = QRegularExpression("[A-Za-z]+\\s*:"); highlightingRules.append(rule);
    rule.format.setForeground(Qt::gray);
    rule.format.setFontItalic(true);
    rule.pattern = QRegularExpression("//[^\n]*"); highlightingRules.append(rule);
}
