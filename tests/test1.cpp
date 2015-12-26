/* \file test1.cpp
 * \author Carlos Nihelton <carlosnsoliveira@gmail.com> (C) 2015
 * 
 * It contains declaration of the SQLogger class and its interfaces and private members.
 * ------------------------------------------------------------------------------------
 * This code is part of SQLogger, a simple utility to log stuff into a very small SQLite database.
 * This code is licensed under GNU LGPL v2.1 license.
 * See <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html> for more datails.
 * 
 */

#include <iostream>
#include <cstdlib>
#include <thread>
#include <gtest/gtest.h>
#include <sqlogger.h>

class Teste1 : public sqlogger::Record
{
private:
  std::string username;
  std::string message;
  
public:
  Teste1();
  //getters
  const std::string user(){return username;};
  const std::string msg(){return message;};
  
  //setters
  void setMsg(const std::string& Message){message=Message;};
};

Teste1::Teste1(){
    username = std::getenv("USER");
    setTableName("hello");
    addField("USER", "TEXT", std::bind(&Teste1::user, this));
    addField("MSG", "TEXT", std::bind(&Teste1::msg, this));
}

TEST(SQLogger, creation)
{
  Teste1 var;
  var.setMsg("Hello, World!");
  ASSERT_TRUE(sqlogger::SQLogger::instance().log(&var));
}



TEST(SQLogger, thread)
{
  auto f = [](){
    Teste1 var;
    std::ostringstream oss;
    oss << "Hello from thread " << std::this_thread::get_id();
    var.setMsg(oss.str());
    ASSERT_TRUE(sqlogger::SQLogger::instance().log(&var));
  };
  
  std::vector<std::thread> thread_pool;
  for (int i=0; i<40; i++) thread_pool.emplace_back(std::thread{f});
  for(auto &t : thread_pool) t.join();
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc,argv);
  return RUN_ALL_TESTS();
}