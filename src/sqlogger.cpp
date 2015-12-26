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
 * \file sqlogger.cpp
 * \author Carlos Nihelton <carlosnsoliveira@gmail.com> (C) 2015
 * 
 * It contains definition of the SQLogger class and its interfaces.
 * 
 */

#include <sqlogger.h>
#include <iostream>

namespace sqlogger{
  
  SQLogger::SQLogger(const std::string& file) : created(false)
  {
    if(sqlite3_open(file.c_str(), &dbHandle) == SQLITE_OK)  {
      fileName = file;
    }
    else {
      throw std::runtime_error(sqlite3_errmsg(dbHandle));
    }
  }
  
  SQLogger::~SQLogger()
  {
    sqlite3_close(dbHandle);
  }


  //bool SQLogger::log(const std::unique_ptr<Record> rec)
  bool SQLogger::log(Record* rec)
  {
    sqlite3_stmt* stmt;
    bool logged{false};
    int error=SQLITE_OK;
    
    if(!created){
      std::string schema=rec->getSchema();
      if(!schema.empty()) {
	error = sqlite3_prepare_v2(dbHandle, schema.c_str(), -1, &stmt, nullptr);
	if(error == SQLITE_OK) {
	  error = sqlite3_step(stmt);
	  if(error == SQLITE_OK || error == SQLITE_DONE) created = true;
	}
	sqlite3_finalize(stmt);
      }
    }
    
    std::string query=rec->writeQuery();
    if(!query.empty() && created) {
      error = sqlite3_prepare_v2(dbHandle, query.c_str(), -1, &stmt, nullptr);
      if(error == SQLITE_OK) {
	error = sqlite3_step(stmt);
	if(error == SQLITE_OK || error == SQLITE_DONE) logged = true;
      }
      sqlite3_finalize(stmt);
    }
    return logged;
  }

  //Strong guarantee exception safe -- See addField member function.
  Record::Record()
  {
    addField("MOMENT", "TEXT", std::bind(&Record::getTime, this));
  }

  //Strong guarantee exception safe
  void Record::addField(const std::string& fieldName, const std::string& typeDesc, std::function< const std::string(void)> callback)
  {
    if(fieldName.empty()) throw std::invalid_argument("Record::addField -> fieldName must not be empty");
    if(typeDesc.empty()) throw std::invalid_argument("Record::addField -> typeDesc must not be empty");
    //if(!std::is_function<decltype(callback)>::value) throw std::invalid_argument("Record::addField -> callback must be a callable object");
    
    //------------- Strong guarantee barrier ------------------
    
    fields.emplace_back(std::make_tuple(fieldName, typeDesc, callback));
    updateSchema();
  }

  void Record::setTableName(const std::string& tblName) noexcept
  {
    tableName = tblName;
    updateSchema();
  }


  //No-throw guarrantee.
  void Record::updateSchema() noexcept
  {
    if(!(tableName.empty() || fields.empty()))
    {
      schema = "CREATE TABLE IF NOT EXISTS " + tableName + "(";
      for(const auto& f : fields) {
	schema += std::get<0>(f) + ' ' + std::get<1>(f) + ',';    
      }
      schema.back()=')'; //Replacing last comma with parenthesis.
    } else {
      schema.clear();
    }
  }

  const std::string Record::getTime()
  {
    std::stringstream helper;
    std::time_t local = std::time(nullptr);
    std::tm tm = *std::localtime(&local);
    helper << std::put_time(&tm, "%Y-%m-%d %H-%M-%S"); //ISO-8601 Format.
    return helper.str();
  }

  const std::string Record::writeQuery()
  {
    std::string query;
    if(!schema.empty()) {
      query = "INSERT INTO " + tableName + " (";
      
      for(const auto& f : fields) {
	query += std::get<0>(f) + ',';
      }
      
      query.back()=')';
      query += " VALUES (";
      
      for(const auto& f : fields) {
	auto callback = std::get<2>(f);
	query += '\'' + callback() + "\',";
      }
      query.back()=')';
    }
    return query;
  }
 
 
  Record::~Record()
  {

  }

  
}
