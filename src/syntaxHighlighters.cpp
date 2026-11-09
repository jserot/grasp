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

void SyntaxHighlighter::addKeywords(const QStringList &words, const QTextCharFormat &format, bool caseInsensitive)
{
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b(" + words.join("|") + ")\\b",
                                      caseInsensitive ? QRegularExpression::CaseInsensitiveOption
                                                      : QRegularExpression::NoPatternOption);
    rule.format = format;
    highlightingRules.append(rule);
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Each rule is applied to all its occurrences in the line (only the first one was highlighted)
    foreach (const HighlightingRule &rule, highlightingRules) {
        Q_ASSERT(rule.pattern.isValid());
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while ( it.hasNext() ) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
    }
    // Multi-line comments ; block state 1 means that the previous line ended inside a comment
    setCurrentBlockState(0);
    if ( commentStart.pattern().isEmpty() ) return;
    int start = 0;
    if ( previousBlockState() != 1 ) {
        QRegularExpressionMatch m = commentStart.match(text);
        start = m.hasMatch() ? m.capturedStart() : -1;
        }
    while ( start >= 0 ) {
        QRegularExpressionMatch end = commentEnd.match(text, start);
        int length;
        if ( end.hasMatch() )
            length = end.capturedEnd() - start;
        else {
            setCurrentBlockState(1);
            length = text.length() - start;
            }
        setFormat(start, length, commentFormat);
        QRegularExpressionMatch next = commentStart.match(text, start + length);
        start = next.hasMatch() ? next.capturedStart() : -1;
        }
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
    rule.pattern = QRegularExpression("\\bwait_ev\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bwait_evs\\b"); highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bnotify_ev\\b"); highlightingRules.append(rule);
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

// The formats follow the conventions of the FSM and CTask highlighters : blue for the structure, dark green for the
// control flow, dark magenta for the types, dark yellow for the library functions, gray italics for the comments

SystemCSyntaxHighlighter::SystemCSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    QTextCharFormat f;

    f.setForeground(Qt::blue);
    addKeywords({ "SC_MODULE", "SC_CTOR", "SC_THREAD", "SC_METHOD", "SC_HAS_PROCESS", "sensitive",
                  "typedef", "struct", "enum", "class", "void", "static", "const", "return" }, f);
    f.setForeground(Qt::darkGreen);
    addKeywords({ "while", "for", "do", "if", "else", "switch", "case", "default", "break", "continue" }, f);
    HighlightingRule rule;
    rule.format = f;
    rule.pattern = QRegularExpression("^\\s*#\\s*[a-z]+\\b"); highlightingRules.append(rule); // #include, ...
    f.setForeground(Qt::darkMagenta);
    addKeywords({ "int", "bool", "char", "float", "double", "unsigned", "long", "short",
                  "sc_in", "sc_out", "sc_inout", "sc_signal", "sc_int", "sc_uint", "sc_bigint", "sc_biguint",
                  "sc_bv", "sc_lv", "sc_logic", "sc_bit", "sc_event", "sc_time", "sc_clock",
                  "sc_trace_file", "true", "false" }, f);
    f.setForeground(Qt::darkYellow);
    addKeywords({ "wait", "notify", "read", "write", "posedge_event", "negedge_event", "posedge", "negedge",
                  "sc_start", "sc_stop", "sc_time_stamp", "sc_main", "sc_create_vcd_trace_file",
                  "sc_close_vcd_trace_file", "sc_trace", "SC_NS", "SC_US", "SC_MS", "SC_SEC", "SC_PS", "SC_FS",
                  "SC_ZERO_TIME" }, f);
    f.setForeground(Qt::gray);
    f.setFontItalic(true);
    rule.format = f;
    rule.pattern = QRegularExpression("//[^\n]*"); highlightingRules.append(rule);
    commentStart = QRegularExpression("/\\*");
    commentEnd = QRegularExpression("\\*/");
    commentFormat = f;
}

VhdlSyntaxHighlighter::VhdlSyntaxHighlighter(QTextDocument *parent)
    : SyntaxHighlighter(parent)
{
    // VHDL is not case sensitive
    QTextCharFormat f;

    f.setForeground(Qt::blue);
    addKeywords({ "library", "use", "entity", "architecture", "component", "package", "body", "configuration",
                  "is", "of", "begin", "end", "port", "generic", "map", "process", "function", "procedure",
                  "return", "signal", "variable", "constant", "type", "subtype", "record", "array", "range",
                  "others", "all", "to", "downto", "in", "out", "inout", "buffer", "open", "work" }, f, true);
    f.setForeground(Qt::darkGreen);
    addKeywords({ "if", "then", "elsif", "else", "case", "when", "loop", "for", "while", "wait", "until",
                  "after", "exit", "next", "null", "report", "assert", "severity",
                  "and", "or", "not", "xor", "nand", "nor", "xnor", "mod", "rem", "abs",
                  "sll", "srl", "sla", "sra", "rol", "ror" }, f, true);
    f.setForeground(Qt::darkMagenta);
    addKeywords({ "std_logic", "std_logic_vector", "std_ulogic", "std_ulogic_vector", "integer", "natural",
                  "positive", "boolean", "bit", "bit_vector", "time", "unsigned", "signed", "string",
                  "character", "real", "true", "false", "ns", "us", "ms", "ps", "fs", "sec" }, f, true);
    f.setForeground(Qt::darkYellow);
    addKeywords({ "rising_edge", "falling_edge", "shift_left", "shift_right", "to_integer", "to_unsigned",
                  "to_signed", "resize", "now" }, f, true);
    f.setForeground(Qt::gray);
    f.setFontItalic(true);
    HighlightingRule rule;
    rule.format = f;
    rule.pattern = QRegularExpression("--[^\n]*"); highlightingRules.append(rule);
}
