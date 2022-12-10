#pragma once

#include "Common/dataStructs/NxArray.h"
#include <assert.h>

#define NX_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // To apply printf-style warnings to our functions.

struct NxTextBuffer
{
    NxArray<char> Buf;
    static char EmptyString[1];
    
    inline char         operator[](int i) const { assert(Buf.data() != NULL); return Buf[i]; }
    const char*         begin() const           { return Buf.data() ? &Buf.front() : EmptyString; }
    const char*         end() const             { return Buf.data() ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
    int                 size() const            { return Buf.size() ? Buf.size() - 1 : 0; }
    bool                empty() const           { return Buf.size() <= 1; }
    void                clear()                 { Buf.clear(); }
    void                reserve(int capacity)   { Buf.reserve(capacity); }
    const char*         c_str() const           { return Buf.data() ? Buf.data() : EmptyString; }
    void      append(const char* str, const char* str_end = NULL);
    void      appendf(const char* fmt, ...); //NX_FMTARGS(2);
    void      appendfv(const char* fmt, va_list args); //NX_FMTARGS(2);
};

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct NxTextFilter
{
              NxTextFilter(const char* default_filter = "");
    bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);  // Helper calling InputText+Build
    bool      PassFilter(const char* text, const char* text_end = NULL) const;
    void      Build();
    void                Clear()          { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }
    
    struct NxTextRange
    {
        const char*     b;
        const char*     e;

        NxTextRange()                                { b = e = NULL; }
        NxTextRange(const char* _b, const char* _e)  { b = _b; e = _e; }
        bool            empty() const                   { return b == e; }
        void  split(char separator, NxArray<NxTextRange>* out) const;
    };
    char                    InputBuf[256];
    NxArray<NxTextRange>Filters;
    int                     CountGrep;
};


// Usage:
//  static NxAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
class NxAppLog
{
public:
    NxAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void AddLog(const char* fmt, ...); //NX_FMTARGS(2)

    void Draw(const char* title, bool* p_open = NULL);

private:
    NxTextBuffer     Buf;
    NxTextFilter     Filter;
    NxArray<int>     LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool             AutoScroll;  // Keep scrolling if already at the bottom.
};