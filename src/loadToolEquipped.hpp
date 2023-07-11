#ifndef LOADTOOLEQUIPPED_HPP
#define LOADTOOLEQUIPPED_HPP
#include "Chef.hpp"
#include "Values.hpp"
#include "Types.hpp"
#include <fstream>
#include <vector>
#include <exception>
// #include <Windows.h>
using namespace std;
class CSVWarning {
  public:
    bool missingRarity3;
    void operator+=(CSVWarning w) {
        this->missingRarity3 = this->missingRarity3 || w.missingRarity3;
    }
    CSVWarning() : missingRarity3(false) {}
};
struct ToolInfo {
    string name;
    CookAbility ability;
    AbilityBuff abilityBuff;
    FlavorBuff flavorBuff;
    MaterialCategoryBuff materialBuff;
};
typedef vector<string> ToolInfoRow;
struct ToolInfoBlock {
    vector<ToolInfoRow> info;
    int row;
};
typedef vector<ToolInfoBlock> ToolInfoFile;
typedef vector<ToolInfo> Tools;
static ToolInfoBlock *tibptr;

ToolFileType loadToolFile() {

    auto dirs = {"./", "../data/"};

    auto fname = "toolEquipped.csv";
    ifstream toolFile;
    for (const std::string &dir : dirs) {
        toolFile.open(dir + fname, ios::in);
        if (toolFile.good()) {
            break;
        }
        toolFile.close();
    }
    if (!toolFile.good()) {
        tibptr = NULL;
        return NO_FILE__NO_TOOL;
    }
    string line;
    ToolInfoFile tif;
    ToolInfoBlock tib;
    tib.row = 2;
    ToolInfoRow tir;

    string word;
    getline(toolFile, line);
    istringstream sin;
    int lineNum = 1;
    while (getline(toolFile, line)) {
        lineNum++;
        sin.clear();
        sin.str(line);
        getline(sin, word, ',');
        if (word == "") {
            tif.push_back(tib);
            tib.info.clear();
            tib.row = lineNum + 1;
            continue;
        }
        tir.push_back(word);
        while (getline(sin, word, ',')) {
            tir.push_back(word);
        }
        if (line[line.size() - 1] == ',')
            tir.push_back("");
        tib.info.push_back(tir);
        tir.clear();
    }
    if (tib.info.size() != 0)
        tif.push_back(tib);
    toolFile.close();
    if (tif.size() == 0) {
        tibptr = NULL;
        return EMPTY_FILE__NOT_EQUIPPED;
    } else if (tif.size() == 1) {
        tibptr = new ToolInfoBlock(tif[0]);
        return CUSTOMIZE_TOOL;
    } else {
        cout << "发现多个配置，请输入希望使用的配置编号。" << endl;
        for (unsigned int i = 0; i < tif.size(); i++) {
            cout << i << ": ";
            for (unsigned int j = 0; j < tif[i].info.size(); j++) {
                cout << tif[i].info[j][0] << " ";
            }
            cout << endl;
        }
        cout << "请输入0到" << tif.size() - 1 << "之间的整数：";
        int i = -1;
        cin >> i;

        if (!(i >= 0 && i < int(tif.size()))) {
            cout << "输入的不是0到" << tif.size() - 1 << "之间的整数。" << endl;
            exit(1);
        } else {
            cout << "使用配置" << i << "。" << endl;
        }
        tibptr = new ToolInfoBlock(tif[i]);

        return CUSTOMIZE_TOOL;
    }
}

bool isFloat(string s) {
    bool hasDot = false;
    if (s.size() == 0)
        return true;
    if (s[0] != '-' && !(s[0] >= '0' && s[0] <= '9'))
        return false;
    for (unsigned int i = 1; i < s.size(); i++) {
        if (s[i] == '.') {
            if (hasDot)
                return false;
            else
                hasDot = true;
            continue;
        }
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    if (hasDot)
        return true;
    else {
        throw runtime_error("Float");
    }
}
double str2f(string s) {
    if (isFloat(s))
        return atof(s.c_str());
    else
        throw runtime_error("Float");
}
bool isInt(string s) {
    if (s.size() == 0)
        return true;
    if (s[0] != '-' && !(s[0] >= '0' && s[0] <= '9'))
        return false;
    for (unsigned int i = 1; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}
int str2i(string s) {
    if (isInt(s))
        return atoi(s.c_str());
    else
        throw runtime_error("Int");
}

CSVWarning loadToolFromFile(Chef *chef, ToolFileType t) {
    CSVWarning w;
    if (t == EMPTY_FILE__NOT_EQUIPPED) {
        return w;
    }
    if (t == NO_FILE__NO_TOOL) {
        chef->NoTool();
        return w;
    }

    auto lineN = tibptr->row - 1;
    auto tools = (*tibptr).info;
    for (unsigned int i = 0; i < tools.size(); i++) {
        lineN = lineN + 1;
        auto tool = tools[i];
        if (!(tool[0] == chef->name)) {
            continue;
        }
        chef->NoTool();
        unsigned int j = 1;
        auto skill = &chef->skill;
        auto ability = &skill->ability;
        auto abilityBuff = &skill->abilityBuff;
        auto flavorBuff = &skill->flavorBuff;
        auto materialBuff = &skill->materialBuff;
        try {
            if (tool[j][0] == '*') {
                ability->multiply(str2f(tool[j++].substr(1)));
            } else {
                ability->add(str2i(tool[j++]));
            }
            if (tool[j][0] == '*') {
                ability->bake = int(ability->bake * str2f(tool[j++].substr(1)));
            } else {
                ability->bake += str2i(tool[j++]);
            }
            if (tool[j][0] == '*') {
                ability->knife =
                    int(ability->knife * str2f(tool[j++].substr(1)));
            } else {
                ability->knife += str2i(tool[j++]);
            }
            if (tool[j][0] == '*') {
                ability->stirfry =
                    int(ability->stirfry * str2f(tool[j++].substr(1)));
            } else {
                ability->stirfry += str2i(tool[j++]);
            }
            if (tool[j][0] == '*') {
                ability->fry = int(ability->fry * str2f(tool[j++].substr(1)));
            } else {
                ability->fry += str2i(tool[j++]);
            }
            if (tool[j][0] == '*') {
                ability->steam =
                    int(ability->steam * str2f(tool[j++].substr(1)));
            } else {
                ability->steam += str2i(tool[j++]);
            }
            if (tool[j][0] == '*') {
                ability->boil = int(ability->boil * str2f(tool[j++].substr(1)));
            } else {
                ability->boil += str2i(tool[j++]);
            }
            skill->coinBuff += str2i(tool[j++]);
            abilityBuff->bake += str2i(tool[j++]);
            abilityBuff->knife += str2i(tool[j++]);
            abilityBuff->stirfry += str2i(tool[j++]);
            abilityBuff->fry += str2i(tool[j++]);
            abilityBuff->steam += str2i(tool[j++]);
            abilityBuff->boil += str2i(tool[j++]);
            flavorBuff->sweet += str2i(tool[j++]);
            flavorBuff->spicy += str2i(tool[j++]);
            flavorBuff->salty += str2i(tool[j++]);
            flavorBuff->tasty += str2i(tool[j++]);
            flavorBuff->sour += str2i(tool[j++]);
            flavorBuff->bitter += str2i(tool[j++]);
            materialBuff->vegetable += str2i(tool[j++]);
            materialBuff->meat += str2i(tool[j++]);
            materialBuff->fish += str2i(tool[j++]);
            materialBuff->creation += str2i(tool[j++]);
            if (tool.size() == j) {
                w.missingRarity3 = true;
            } else if (tool.size() == j + 1) {
                skill->rarityBuff[3] += str2i(tool[j++]);
            } else {
                throw runtime_error("Too many columns");
            }
        } catch (runtime_error e) {
            j -= 1;
            if (e.what() == string("Float")) {

                cout << "厨具配置文件第" << lineN << "行第" << j + 1
                     << "列应当是一个小数，实际上却是" << tool[j] << "。"
                     << endl;
            } else if (e.what() == string("Int")) {
                cout << "厨具配置文件第" << lineN << "行第" << j + 1
                     << "列应当是一个整数，实际上却是" << tool[j] << "。"
                     << endl;
            } else if (e.what() == string("Too many columns")) {
                cout << "厨具配置文件的列数过多，请对照样例检查哪里多了一列。"
                     << endl;
            } else {
                j -= 1;
                cout << "厨具配置文件第" << lineN << "行第" << j + 1
                     << "列出现无法识别的内容" << tool[j] << "。" << endl;
            }
            exit(0);
        }
    }
    return w;
}

#endif