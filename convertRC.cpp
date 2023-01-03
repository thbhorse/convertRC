#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

int read_line(istream &is, string &str)
{
    str = "";
    char c = is.get();
    int count = 0;
    while (c != '\n' && !is.eof() && c != '\r')
    {
        str += c;
        c = is.get();
        count++;
    }
    return count;
}

void add_line_to_list(list<string> &words, const string &str)
{
    string word;
    words.clear();
    const char *cstr = str.c_str();
    const char *beg = cstr;
    while (*beg == ' ' || *beg == '\t')
        beg++;
    const char *end = beg;
    do
    {
        while (*end && *end != ' ' && *end != '\t')
            end++;
        if (end > beg)
        {
            word.assign(beg, end - beg);
            words.push_back(word);
        }
        beg = end;
        while (*beg == ' ' || *beg == '\t')
            beg++;
        end = beg;
    } while (*beg);
}

bool write_to_file(string &line, ifstream &is, ofstream &os, const list<string> &words, const char *key, const char *key2, const char *sub)
{
    bool success = false;
    if (words.size() == 2 && words.back() == key)
    {
        string bmpfile = string(sub) + "\\" + words.front() + key2;
        ofstream bmpf(bmpfile, ios::binary);
        read_line(is, line); //  {
        read_line(is, line);
        do
        {
            const char *beg = line.c_str();
            while (*beg == ' ')
                beg++;
            if (*beg == '\'')
            {
                beg++;
                const char *end = beg;
                while (*end && *end != '\'')
                    end++;
                do
                {
                    char b[3];
                    b[0] = *beg;
                    b[1] = beg[1];
                    b[2] = 0;
                    int a = strtol(b, 0, 16);
                    bmpf.write((char *)&a, 1);
                    beg += 3;
                } while (beg < end);
            }
            read_line(is, line);
        } while (line.find_first_of('}') == string::npos);
        os << words.front() << " " << key << " \"" << words.front() << key2 << "\"\n";
        success = true;
    }
    return success;
}

int main(int argc, char *argv[], char *env[])
{
    int retval = 0;
    if (argc == 3)
    {
        ifstream is(argv[1]);
        ofstream os(string(argv[2]) + "\\" + argv[1]);
        if (is.good() && os.good())
        {
            list<string> words;
            int count = 0;
            string line;
            while (!is.eof())
            {
                read_line(is, line);
                int len = line.length();
                add_line_to_list(words, line);
                int i;
                const char *str = line.c_str();
                int beg = line.find_first_of('{');
                int end = line.find_first_of('}');
                if (beg != string::npos || end != string::npos)
                {
                    if (beg != string::npos)
                    {
                        for (i = 0; i < len && (str[i] == ' ' || str[i] == '{'); i++)
                            ;
                        if (i == len)
                            line.replace(beg, 1, "BEGIN");
                    }
                    if (end != string::npos)
                    {
                        for (i = 0; i < len && (str[i] == ' ' || str[i] == '}'); i++)
                            ;
                        if (i == len)
                            line.replace(end, 1, "END");
                    }
                    os << line << endl;
                }
                else if (write_to_file(line, is, os, words, "BITMAP", ".BMP", argv[2]))
                    ;
                else if (write_to_file(line, is, os, words, "CURSOR", ".CUR", argv[2]))
                    ;
                else if (write_to_file(line, is, os, words, "ICON", ".ICO", argv[2]))
                    ;
                else
                    os << line << endl;

                count++;
            }
            cout << "success\n写入文件 ";
            cout << count << " 行。\n";
        }
        else
        {
            cout << "error: 文件无法打开\n";
            retval = 1;
        }
    }
    else
    {
        cout << "error: 参数错误, 用法：";
        cout << argv[0] << " rc文件名 输出目录" << endl;
        retval = 2;
    }

    return retval;
}
