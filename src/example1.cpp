/***************************************************************************
 *   (C) Carlos Nihelton (carlosnsoliveira@gmail.com) 2015                 *
 *                                                                         *
 *   This file is part of SQLogger C++11 data logger facility.             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   SQLogger is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this code; if not, write to the Free Software      *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Carlos Nihelton 2015                                                  *
 ***************************************************************************/

/**
 * \file 	example1.h
 * \author 	Carlos Nihelton <carlosnsoliveira@gmail.com>
 * \details	Demonstrates how to use this library. 
 */

#include <iostream>
#include <sqlogger.h>
#include <thread>

using namespace sqlogger;

class LogRec : public Record
{
//Encapsulated data.These member values will be put into the database using its getters, which must supply string return values.  
private:
  std::string m_user;
  std::thread::id m_threadId;
  std::string m_message;
  
public:
  LogRec();
  ~LogRec()=default;
  
  //getters: Note that they must return const std::string and take no parameters.
  const std::string userName(){return m_user;};
  const std::string Msg(){return m_message;};
  const std::string threadId();  
  
  //setter to allow passing messages to this object on the fly.
  void setMsg(const std::string& msg){m_message = msg;};
};

//In the constructor we setup the table name and the user name.
//Must important: we setup the field mapping by calling addField.
LogRec::LogRec()
{
  m_user = std::getenv("USER");
  m_threadId = std::this_thread::get_id();
  
  setTableName("LogRecExample");
  //field mapping.
  addField("USERNAME", "TEXT", std::bind(&LogRec::userName, this));
  addField("THREAD_ID", "TEXT", std::bind(&LogRec::threadId, this));
  addField("MESSAGE", "TEXT", std::bind(&LogRec::Msg, this));
}

//Returning this thread id as string.
const std::string LogRec::threadId()
{
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  return oss.str();
}

//Main Function - Demonstrates this logger capabilities inside a multithread environment.
int main(int argc, char* argv[]){
  //Functor to spawn in threads.
  auto functor = [](){
    LogRec tmp;
    tmp.setMsg("Hello, World! Logging from spawn thread from main");
    SQLogger::instance().log(&tmp);
  };
  LogRec fromMain;
  fromMain.setMsg("Hello, World! Loggin from main.");
  SQLogger::instance().log(&fromMain);
  
  std::vector<std::thread> thread_pool;
  for(int i=0; i<40; ++i) thread_pool.emplace_back(std::thread{functor});
  for(auto& f : thread_pool) f.join();  
}

