/*
 * This file is part of Hootenanny.
 *
 * Hootenanny is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --------------------------------------------------------------------
 *
 * The following copyright notices are generated automatically. If you
 * have a new notice to add, please use the format:
 * " * @copyright Copyright ..."
 * This will properly maintain the copyright information. DigitalGlobe
 * copyrights will be updated automatically.
 *
 * @copyright Copyright (C) 2015 DigitalGlobe (http://www.digitalglobe.com/)
 */

#ifndef HDFS_H
#define HDFS_H

// Hadoop
#include <hdfs.h>

// Qt
#include <QStringList>

// Standard
#include <iostream>
#include <string>
#include <vector>

namespace pp
{
using namespace std;

class FileStatus
{
public:
  FileStatus() {}

  FileStatus(const FileStatus& in)
  {
    _accessTime = in._accessTime;
    _modTime = in._modTime;
    _len = in._len;
    _path = in._path;
    _kind = in._kind;
  }

  long getAccessTime() { return _accessTime; }

  long getBlockSize() { return _blockSize; }

  long getLen() { return _len; }

  long getModificationTime() { return _modTime; }

  const string& getPath() { return _path; }

  bool isDir() { return _kind == kObjectKindDirectory; }

  bool isFile() { return _kind == kObjectKindFile; }

protected:
  long _accessTime;
  long _len;
  long _modTime;
  long _blockSize;
  string _path;
  tObjectKind _kind;

  friend class Hdfs;

  void _setAccessTime(long t) { _accessTime = t; }

  void _setBlockSize(long bs) { _blockSize = bs; }

  void _setLen(long len) { _len = len; }

  void _setModificationTime(long t) { _modTime = t; }

  void _setKind(tObjectKind kind) { _kind = kind; }

  void _setPath(const string& s) { _path = s; }
};

/**
 * All methods throw an exception on failure.
 */
class Hdfs
{
public:

  /**
   * Create an HDFS connector. The actual connection is delayed until needed. In most cases
   * the default arguments should be sufficient.
   */
  Hdfs(std::string host = "default", int port = 0);

  void copyFromLocal(string src, string dst);

  /**
   * Opens a new ostream and returns ownership to the caller. An exception is thrown on failure.
   * Seeking is not supported on the output stream.
   */
  ostream* create(string path);

  /**
   * Equivalent to Hadoop's FileSystem::delete.
   * @param recursive Only true is supported at this time.
   */
  bool deletePath(string path, bool recursive);

  /**
   * Returns true if the specified path exists.
   */
  bool exists(string path);

  /**
   * Given a path, calculate the absolute path and return it. If the path is already absolute
   * then no change is made. The path does not have to exist for this to work.
   */
  string getAbsolutePath(const std::string& path);

  /**
   * Returns a status object describing path.
   */
  FileStatus getFileStatus(string path);

  /**
   * Returns a list of data nodes that have a copy of the given file segment.
   */
  QStringList getLocations(string path, long start, long length);

  /**
   * Return the file status of all the files/directories in the given path.
   */
  std::vector<FileStatus> listStatus(string path);

  /**
   * Creates a directory and all the necessary parent directories with default permissions.
   */
  void mkdirs(string path);

  /**
   * Opens a new istream and returns ownership to the caller. An exception is thrown on failure.
   */
  istream* open(std::string path);

  /**
   * Deletes the specified path.
   */
  void remove(string path);

  /**
   * Move file from one location to another.
   */
  void rename(const string& from, const string& to);

private:

  string _host;
  int _port;

  hdfsFS _fs;

  hdfsFS _getFs();
};

}

#endif // HDFS_H
