/* \file sqlogger.h
 * \author Carlos Nihelton <carlosnsoliveira@gmail.com> (C) 2015
 * 
 * It contains declaration of the SQLogger class and its interfaces and private members.
 * ------------------------------------------------------------------------------------
 * This code is part of SQLogger, a simple utility to log stuff into a very small SQLite database.
 * This code is licensed under GNU LGPL v2.1 license.
 * See <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html> for more datails.
 * 
 */

#ifndef SQLOGGER_H
#define SQLOGGER_H


#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <functional>
#include <tuple>
#include <memory>
#include <vector>
#include <stdexcept>

#include <sqlite/sqlite3.h>

namespace sqlogger {
  
  
  /* \class Record
* \brief A base class proiding the interfaces required for the logger class.
* \details The user must derive a class, which customizes its log database. The interfaces provided by this base class will be used by the SQLogger.
*/
  class Record
  {
  public:
    ///Public methods invoked by the SQLogger when logging back user's stuff.
    const std::string writeQuery();
    const std::string& getSchema(){updateSchema(); return schema;};
    const std::string& getTime();
    void setTableName(const std::string& tblName) noexcept; ///We need to update schema when the tableName is setup.
    
  protected:
    std::vector<std::tuple<std::string, std::string, std::function<const std::string&(void)>>> fields;
    std::string schema;
    
    void updateSchema() noexcept;
    void addField(const std::string& fieldName, const std::string& typeDesc, std::function<const std::string&(void)> callback);
      
    ///A default constructor providing the most basic information for the logging system.
    Record();
    
  private:
    std::string tableName;
    std::string localTime;
  };
  
/* \class SQLogger
 * \brief A Singleton to log stuff into a SQLite database.
 * \details The user must provide the scheme for the database and a mimic class for the records and this class must have derive from Record class.
 */
  class SQLogger
  {
  public:
    //bool log(const std::unique_ptr<Record> rec);
    bool log(Record* rec);
    
    static SQLogger& instance(const std::string& file=std::string{"log.db"}) {
      static SQLogger theLogger(file);
      return theLogger;
    }
    
  private:
    SQLogger(const std::string& file);
    SQLogger(SQLogger const&)=delete;
    SQLogger& operator=(SQLogger const&)=delete;
    virtual ~SQLogger();
    
  private:
    std::string fileName;
    sqlite3* dbHandle;
    bool created;
  };
  
}
 
#endif