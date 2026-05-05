#include <filewriter.hpp>
#include <fstream>

void FileWriter::write(const std::string &filename, const std::vector<int> &data)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        return;
    }
    for (size_t i = 0; i < data.size(); i++) 
    {
        outFile << i << " " << data[i] << "\n";
    }
}