
//#include"../log/log.hpp"
#include<iostream>
#include<fstream>
#include<boost/algorithm/string.hpp>

#pragma once

//LOG tolog;

class Common
{
  public:
    static bool ReadFile(const std::string& file_path, std::string* content)
    {
      std::ifstream inputFile(file_path.c_str());
      if(!inputFile.is_open())
      {
        //tolog << "ifstream error!";
        std::cout << "ifstream error"<<std::endl; 
        return false;
      }

      std::string line;
      while(std::getline(inputFile, line))
      {
        *content += line + "\n";
      }
      inputFile.close();
      return true;
    }

    static bool WriteFile(const std::string& file_path, const std::string& content)
    {
      std::ofstream outputFile(file_path.c_str());
      if(!outputFile.is_open())
      {
        //tolog << "ofstream error!";
        std::cout << "ofstream error!"  << std::endl; 
        return false;
      }
      outputFile.write(content.c_str(), content.length()); 

      outputFile.close();
      return true;
    }

    static void split(const std::string& input, std::vector<std::string>* output, const std::string& sp_char)
    {
      boost::split(*output,input, boost::is_any_of(sp_char),boost::token_compress_off);
    }
  private:
};







