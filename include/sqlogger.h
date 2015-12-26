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
 * \file 	sqlogger.h
 * \author 	Carlos Nihelton <carlosnsoliveira@gmail.com>
 * \details	It contains declaration of the SQLogger class and its interfaces. 
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
/**
* \class 	sqlogger::Record
* \brief 	A base class proiding the interfaces required for the logger class.
* \details 	The user must derive from this class and customizes his log database using the Record::addField member function. 
* 		The interfaces provided by this base class will be used by the SQLogger.
*/
  class Record
  {
  public:
    ///\name 	Only destructor is public.
    ///\details This class must not be directly used. That's why no public interfaces are offered.
    ///\{
    virtual ~Record();
    ///\}
    
  protected:
    ///name 	Protected methods invoked by SQLogger when logging back user's stuff.
    ///\{
    const std::string writeQuery();
    const std::string& getSchema(){updateSchema(); return schema;};
    ///\}
    
    ///\name 	protected member functions that must be used by the inherited classes.
    ///\{
    /**
     * This function ensures the database schema is updated when the table name is set.
     * It's declared protected to avoid the end user to call it more than once.
     * It is recommended to be called only during inherited object construction.
     * \param tblName	A std::string object holding the table name.
     */
    void setTableName(const std::string& tblName) noexcept; 
    /**
     * This function updates database schema based on fields description and log table name.
     * \param void
     */
    void updateSchema() noexcept;
    /**
     * This function holds the fields description and the getter member functions for the data members representing the fields in the log table.
     * \param fieldName		The name of the field (aka column) in the log table. std::string.
     * \param typeDesc		Any valid data type description as per SQLite 3 specification.
     * \param callback		A callable object returning a const std::string object with no parameters.
     * 				It is recommended to be a member function of the derived class which converts the data member into string.
     */
    void addField(const std::string& fieldName, const std::string& typeDesc, std::function<const std::string(void)> callback);
    ///\}  
    ///A default constructor providing the most basic information for the logging system. Only accessible to the inherited classes.
    Record();
    
  private:
    /**
     * \name Protected data members altered by derived classes.
     * \{ */
      std::vector<std::tuple<std::string, std::string, std::function<const std::string(void)>>> fields;
      std::string schema;
      std::string tableName;
    ///\}
      
    /**
     * \name Member function the get the current time in ISO-8601 format string.
     * \details The minimum data provided by this base class is the time when the user performs a log call.
     */
    const std::string getTime();
    
      ///\name Declaring SQLogger as friend allows access to the protected member functions.
    friend class SQLogger;
  };
  
/**
 * \class SQLogger
 * \brief A Singleton to log stuff into a SQLite database.
 * \details The user must provide the scheme for the database and a mimic class for the records and this class must have derive from Record class.
 */
  class SQLogger
  {
  public:
    //bool log(std::unique_ptr<Record> rec);
    bool log(Record* rec);
    
    /**
     * Meyers-Singleton design.
     * 
     * This method is the only way the user can create and/or access <b> SQLogger instance</b>.
     * \param file 	A std::string object holding the desired name for the SQLite3 file into which the log will be written.
     * 			If not provided, the default ./log.db will be used.
     * \return A static const reference to the logger object created.
     */
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