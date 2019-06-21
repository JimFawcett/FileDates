///////////////////////////////////////////////////////////////////////
// DateFileMgr.cpp - Find dates of files matching specified patterns //
// Ver 1.1                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018           //
///////////////////////////////////////////////////////////////////////

#include "DateFileMgr.h"
#include "FileSystem.h"
#define STATIC_LIB
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <algorithm>

void usage()
{
  std::cout << "\n  FileDates version 1.1, 10 Aug 2018";
  std::cout << "\n  usage: FileDates path [/s] [pattern]* [numFiles]";
  std::cout << "\n    path = relative or absolute path of starting directory";
  std::cout << "\n    /s for recursive search";
  std::cout << "\n    pattern is a pattern string of the form *.h, *.log, etc.";
  std::cout << "\n    numFiles is the number of files displayed";
  std::cout << "\n  Example: FileDates ../.. /s *.h *.cpp *.txt 25";
  std::cout << "\n";
}
bool DateFileMgr::processCmdLine(int argc, char** argv)
{
  if (argc < 2)
  {
    usage();
    return false;
  }
  path_ = FileSystem::Path::getFullFileSpec(argv[1]);
  if (!FileSystem::Directory::exists(path_))
  {
    std::cout << "\n  " << path_ << " does not exist\n";
    return false;
  }
  std::cout << "\n  FileDates";
  std::cout << "\n    " << path_;
  for (int i = 2; i < argc; ++i)
  {
    std::cout << ", " << argv[i];
    if (std::string(argv[i]) == "/s")
    {
      recursive_ = true;
    }
    else if (std::string(argv[i]) == "/r")
    {
      reverse_ = true;
    }
    else if (argv[i][0] == '/')
    {
      std::cout << "\n  unknown option";
    }
    else
    {
      size_t numFiles = atoi(argv[i]);
      if (numFiles == 0)
      {
        addPattern(argv[i]);
      }
      else
      {
        numFiles_ = numFiles;
      }
    }
  }
  std::cout << "\n";
  return true;
}

DateFileMgr::DateFileMgr(const Path& path = "") : path_(path)
{
  patterns_.push_back("*.*");
}

void DateFileMgr::addPattern(const std::string& patt)
{
  if (patterns_.size() == 1 && patterns_[0] == "*.*")
    patterns_.pop_back();
  patterns_.push_back(patt);
}

DateFileMgr::Date DateFileMgr::reformatDate(const Date& datetime)
{
  size_t pos = datetime.find(' ');
  if (pos >= datetime.size())
    throw "bad date";
  std::string date = datetime.substr(0, pos);    // month/day/year
  std::string time = datetime.substr(pos + 1);   // hr:min:sec

  pos = date.find_last_of('/');
  if (pos >= date.size())
    throw "bad date";
  std::string temp = date.substr(0, pos);    // month/day
  std::string year = date.substr(pos + 1);   // year

  pos = temp.find_last_of('/');
  if (pos >= date.size())
    throw "bad date";
  std::string month = temp.substr(0, pos);   // month
  std::string day = temp.substr(pos + 1);    // day

  std::string reformattedDate = year + "/" + month + "/" + day;
  std::string reformattedDateTime = reformattedDate + " " + time;
  return reformattedDateTime;
}

void DateFileMgr::search()
{
  std::string fullPath = FileSystem::Path::getFullFileSpec(path_);
  if (recursive_)
    find(fullPath);
  else
  {
    for (auto patt : patterns_)
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fullPath, patt);
      for (auto file : files)
      {
        std::string fileSpec = fullPath + "\\" + file;
        FileSystem::FileInfo fi(fileSpec);
        Date date = reformatDate(fi.date());
        DataItem item(date, fileSpec);
        //std::pair<Date, std::string> item = { date, fileSpec };
        store_.insert(item);
      }
    }
  }
}

void DateFileMgr::find(const Path& path)
{
  for (auto patt : patterns_)
  {
    //std::cout << "\n-- " << path;
    std::vector<std::string> files = FileSystem::Directory::getFiles(path, patt);
    for (auto f : files)
    {
      std::string fileSpec = path + "\\" + f;
      FileSystem::FileInfo fi(fileSpec);
      Date date = reformatDate(fi.date());
      if (date > largest_) largest_ = date;
      if (date < smallest_) smallest_ = date;
      //std::pair<Date, std::string> item = { date, fileSpec };
      DataItem item(date, fileSpec);
      store_.insert(item);
    }
  }
  std::vector<std::string> subdirs = FileSystem::Directory::getDirectories(path);
  for (auto d : subdirs)
  {
    if (d != "." && d != "..")
    {
      find(path + "\\" + d);
    }
  }
}

void DateFileMgr::showProcessed()
{
  std::cout << "\n\n    Processed " << store_.size() << " files";
  size_t numDisplayed = std::min<size_t>(numFiles_, store_.size());
  if (numDisplayed == 0)
    numDisplayed = store_.size();
  std::cout << "\n    Displayed " << numDisplayed << " files";
  std::cout << "\n    largest date  = " << largest_;
  std::cout << "\n    smallest date = " << smallest_;
}
void DateFileMgr::forwardDisplay()
{
  size_t count = 0;
  for (auto iter = store_.begin(); iter != store_.end(); ++iter)
  {
    ++count;
    if (count > numFiles_ && numFiles_ > 0)
      break;
    std::cout << "\n " << std::setw(20) << iter->first << " " << iter->second;
  }
  showProcessed();
  std::cout << "\n";
}

void DateFileMgr::reverseDisplay()
{
  size_t count = 0;
  for (auto iter = store_.rbegin(); iter != store_.rend(); ++iter)
  {
    ++count;
    if (count > numFiles_ && numFiles_ > 0)
      break;
    std::cout << "\n " << std::setw(20) << iter->first << " " << iter->second;
  }
  showProcessed();
  std::cout << "\n";
}

void DateFileMgr::display()
{
  if (reverse_)
    reverseDisplay();
  else
    forwardDisplay();
}


#ifdef TEST_DATEFILEMGR

#include <map>
#include <iostream>
#include <functional>
#include "../Utilities/CodeUtilities/CodeUtilities.h"
#include "../Utilities/StringUtilities/StringUtilities.h"

using Date = std::string;
using Path = std::string;
using File = std::string;

int main(int argc, char* argv[])
{
  DateFileMgr fm;
  if (!fm.processCmdLine(argc, argv))
  {
    std::cout << "\n    command line parsing failed\n\n";
    return 1;
  }

  fm.search();
  fm.display();

  std::cout << "\n\n";
  return 0;
}

#endif
