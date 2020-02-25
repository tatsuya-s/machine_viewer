#include <iostream>
#include <string>
#include <utility>
#include <vector>

void replace_solution(std::string& str) {
    const std::vector<std::pair<std::string, std::string>> dict = {
        {"F'", "4j"}, 
        {"R'", "4k"}, 
        {"U'", "4l"}, 
        {"B'", "1u"}, 
        {"L'", "1i"}, 
        {"D'", "1o"}, 
        {"Fw'", "34j"}, 
        {"Rw'", "34k"}, 
        {"Uw'", "34l"}, 
        {"Bw'", "12u"}, 
        {"Lw'", "12i"}, 
        {"Dw'", "12o"}, 
        {"Fw2", "34uu"}, 
        {"Rw2", "34ii"}, 
        {"Uw2", "34oo"}, 
        {"Bw2", "12jj"}, 
        {"Lw2", "12kk"}, 
        {"Dw2", "12ll"}, 
        {"Fw", "34u"}, 
        {"Rw", "34i"}, 
        {"Uw", "34o"}, 
        {"Bw", "12j"}, 
        {"Lw", "12k"}, 
        {"Dw", "12l"}, 
        {"F2", "4uu"}, 
        {"R2", "4ii"}, 
        {"U2", "4oo"}, 
        {"B2", "1jj"}, 
        {"L2", "1kk"}, 
        {"D2", "1ll"}, 
        {"F", "4u"}, 
        {"R", "4i"}, 
        {"U", "4o"}, 
        {"B", "1j"}, 
        {"L", "1k"}, 
        {"D", "1l"}
    };

    for (auto &pair : dict) {
        std::string::size_type pos = str.find(pair.first);
        while (pos = str.find(pair.first, pos), pos != std::string::npos) {
            str.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
}

int main() {
    std::string str = "U2 Fw D' Fw2 Uw L R2 Uw Rw' Uw L Fw2 F2 Rw2 B L' F B' U F Lw2 U B' L2 Lw2 F U2 Fw2 Lw2 U2 Fw2 U2 F L D2 B' L' F' B' U' F D2 R2 U' L2 U L2 D' F2 D F2";
    std::cout << str << std::endl;
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    replace_solution(str);
    std::cout << str << std::endl;
}