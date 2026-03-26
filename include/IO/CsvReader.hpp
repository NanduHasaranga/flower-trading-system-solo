#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <utility>

class CsvReader
{
public:
    std::vector<std::string> parseLine(const std::string &line) const;

    template <typename Callback>
    void parseFile(std::ifstream &file, Callback &&callback) const
    {
        std::string line;
        std::getline(file, line);
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;
            std::forward<Callback>(callback)(parseLine(line));
        }
    }
};

#endif