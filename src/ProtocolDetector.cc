/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "ProtocolDetector.h"

#include <cstring>
#include <iomanip>

#include "Request.h"
#include "File.h"
#include "util.h"
#include "RecoverableException.h"
#include "uri.h"
#include "BufferedFile.h"
#ifdef ENABLE_BITTORRENT
#  include "bittorrent_helper.h"
#endif // ENABLE_BITTORRENT
#ifdef ENABLE_CONTROL_FILE
# include "DefaultBtProgressInfoFile.h"
#endif // ENABLE_CONTROL_FILE

namespace aria2 {

ProtocolDetector::ProtocolDetector() = default;

ProtocolDetector::~ProtocolDetector() = default;

bool ProtocolDetector::isStreamProtocol(const std::string& uri) const
{
  return uri_split(nullptr, uri.c_str()) == 0;
}

bool ProtocolDetector::guessTorrentFile(const std::string& uri) const
{
  BufferedFile fp(uri.c_str(), BufferedFile::READ);
  if (fp) {
    char head[1];
    if (fp.read(head, sizeof(head)) == sizeof(head)) {
      return head[0] == 'd';
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

bool ProtocolDetector::guessTorrentMagnet(const std::string& uri) const
{
#ifdef ENABLE_BITTORRENT
  try {
    bittorrent::parseMagnet(uri);
    return true;
  }
  catch (RecoverableException& e) {
    return false;
  }
#else  // !ENABLE_BITTORRENT
  return false;
#endif // !ENABLE_BITTORRENT
}

bool ProtocolDetector::guessAria2ControlFile(const std::string& uri) const
{
#ifdef ENABLE_CONTROL_FILE
  File control_file(uri);

  if(!control_file.isFile())
  {
      return false;
  }

  return control_file.getExtension() == DefaultBtProgressInfoFile::getSuffix();
#else //  !ENABLE_CONTROL_FILE
  return false;
#endif // !ENABLE_CONTROL_FILE
}

bool ProtocolDetector::guessMetalinkFile(const std::string& uri) const
{
  BufferedFile fp(uri.c_str(), BufferedFile::READ);
  if (fp) {
    char head[5];
    if (fp.read(head, sizeof(head)) == sizeof(head)) {
      return memcmp(head, "<?xml", 5) == 0;
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

} // namespace aria2
