#pragma once
#include <fstream>
#include <iostream>
#include <string>

class Logger{
    public:
        static Logger& instance(){
            static Logger logger("debug.log");
            return logger;
        }
        template <typename... Args>
        void log(const std::string& file,int line,const std::string& func,Args&&... args){
            ofs << "["
                << file << ":" <<line << "(" << func << ")]";
                (ofs << ... << args) << std::endl;
        }
        private:
            std::ofstream ofs;
            Logger(const std::string& filename){
                ofs.open(filename,std::ios::trunc);
                if(!ofs){
                    std::cerr << "can not open log file: " << filename << std::endl;
                }
            }
            ~Logger(){
                ofs.close();
            }
            Logger(const Logger&)=delete;
            Logger& operator=(const Logger&)=delete;
};

#define DEBUG_LOG(...) \
Logger::instance().log(__FILE__,__LINE__,__func__,__VA_ARGS__)