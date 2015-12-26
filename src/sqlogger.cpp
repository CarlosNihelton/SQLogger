/* \file sqlogger.cpp
 * \author Carlos Nihelton <carlosnsoliveira@gmail.com> (C) 2015
 * 
 * It contains definition of the SQLogger class and its interfaces and private members.
 * ------------------------------------------------------------------------------------
 * This code is part of SQLogger, a simple utility to log stuff into a very small SQLite database.
 * This code is licensed under GNU LGPL v2.1 license.
 * See <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html> for more datails.
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

  //bool SQLogger::log(const std::unique_ptr<Record> rec)
  bool SQLogger::log(Record* rec)
  {
    sqlite3_stmt* stmt;
    bool logged{false};
    
    if(!created)
    {
      std::string schema=rec->getSchema();
      if(!schema.empty()){
	if(sqlite3_prepare_v2(dbHandle, schema.c_str(), -1, &stmt, nullptr) == SQLITE_OK){
	  int step = sqlite3_step(stmt);
	  if( step == SQLITE_OK || step == SQLITE_DONE) created = true;
	  else throw std::runtime_error(sqlite3_errmsg(dbHandle));	
	}else throw std::runtime_error(sqlite3_errmsg(dbHandle));
      }
    }
    
    std::string query=rec->writeQuery();
    if(!query.empty()){
      if(sqlite3_prepare_v2(dbHandle, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK){
	  int step = sqlite3_step(stmt);
	  if( step == SQLITE_OK || step == SQLITE_DONE) logged = true;
	  else throw std::runtime_error(sqlite3_errmsg(dbHandle));	
	}else throw std::runtime_error(sqlite3_errmsg(dbHandle));
    }
    return logged;
  }

sqlogger::SQLogger::~SQLogger()
{

}


  //Strong guarantee exception safe -- See addField member function.
  Record::Record()
  {
    addField("MOMENT", "TEXT", std::bind(&Record::getTime, this));
  }

  //Strong guarantee exception safe
  void Record::addField(const std::string& fieldName, const std::string& typeDesc, std::function<const std::string&(void)> callback)
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

  const std::string& Record::getTime()
  {
    std::stringstream helper;
    std::time_t local = std::time(nullptr);
    std::tm tm = *std::localtime(&local);
    helper << std::put_time(&tm, "%Y-%m-%d %H-%M-%S"); //ISO-8601 Format.
    localTime = helper.str();
    return localTime;
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
  
}
