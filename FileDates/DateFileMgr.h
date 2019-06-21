#ifndef FILEMGR_H
#define FILEMGR_H
///////////////////////////////////////////////////////////////////////
// DateFileMgr.h - Find dates of files matching specified patterns   //
// Ver 1.1                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018           //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * DateFileMgr uses the services of FileSystem to find files and stores its findings
 * efficiently in DataStore.
 * - Finds all files, matching a set of patterns, along with their paths.
 * - Stores each fully qualified filename and last modified date, using DataStore.
 *
 * Required Files:
 * ---------------
 * DateFileMgr.h, DateFileMgr.cpp
 * FileSystem.h, FileSystem.cpp,
 *
 * Maintenance History:
 * --------------------
 * Ver 1.1 : 10 Aug 2018
 * - added reverse display
 * - refactored and cleaned up code
 * Ver 1.0 : 09 Aug 2018
 * - first release
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

class DateFileMgr
{
public:
  using Path = std::string;
  using Pattern = std::string;
  using Patterns = std::vector<Pattern>;
  using File = std::string;
  using Date = std::string;
  using DataStore = std::multimap<Date, File, std::greater<Date>>;
  using DataItem = DataStore::value_type;
  using const_iterator = DataStore::const_iterator;
  using const_reverse_iterator = DataStore::const_reverse_iterator;

  DateFileMgr(const Path& path);
  bool processCmdLine(int argc, char** argv);
  void path(const Path& path);
  Path path();
  void recursive(bool recurse);
  bool recursive();
  void reverse(bool reverse);
  bool reverse();
  void numFiles(size_t num);
  size_t numFiles();
  void addPattern(const std::string& patt);
  void search();
  void find(const Path& path);
  void display();
  const_iterator begin();
  const_iterator end();
  const_reverse_iterator rbegin();
  const_reverse_iterator rend();
private:
  void forwardDisplay();
  void reverseDisplay();
  void showProcessed();
  Date reformatDate(const Date& date);
  Path path_;
  Patterns patterns_;
  DataStore store_;
  bool recursive_ = false;
  bool reverse_ = false;
  size_t numFiles_ = 0;
  Date largest_ = "";
  Date smallest_ = "2020/12/31";  // storage format
};

inline void DateFileMgr::path(const Path& path)
{
  path_ = path;
}

inline DateFileMgr::Path DateFileMgr::path()
{
  return path_;
}

inline void DateFileMgr::numFiles(size_t num) 
{ 
  numFiles_ = num; 
}

inline size_t DateFileMgr::numFiles() 
{ 
  return numFiles_; 
}

inline void DateFileMgr::recursive(bool recurse)
{
  recursive_ = recurse;
}

inline bool DateFileMgr::recursive()
{
  return recursive_;
}

inline void DateFileMgr::reverse(bool reverse)
{
  reverse_ = reverse;
}

inline bool DateFileMgr::reverse()
{
  return reverse_;
}

inline DateFileMgr::const_iterator DateFileMgr::begin() 
{ 
  return store_.begin(); 
}

inline DateFileMgr::const_iterator DateFileMgr::end() 
{ 
  return store_.end(); 
}

inline DateFileMgr::const_reverse_iterator DateFileMgr::rbegin() 
{ 
  return store_.rbegin(); 
}

inline DateFileMgr::const_reverse_iterator DateFileMgr::rend() 
{ 
  return store_.rend(); 
}

#endif
