
#include"../log/log.hpp"

#pragma once

LOG tolog;

class Common
{
  public:
    static bool ReadFile(const std::string& file_path, std::string* content)
    {
      std::ifstream inputFile(file_path.c_str());
      if(!inputFile.is_open())
      {
        tolog << "ifstream error!";
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
        tolog << "ofstream error!";
        return false;
      }
      outputFile.write(content.c_str(), content.length()); 

      outputFile.close();
      return true;
    }
  private:
}







