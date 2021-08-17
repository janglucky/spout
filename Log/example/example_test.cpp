#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Policy
{
public:
    virtual void inputPassword()
    {
        cout << "输入密码" << endl;
    }

    virtual void auth() = 0;
    virtual void inputAmount() = 0;
    virtual void enter() = 0;
    virtual void exit() = 0;

    void putMoney()
    {
        inputPassword();
        auth();
        inputAmount();
        enter();
        exit();
    }
};

class Cash : public Policy
{
public:
    void auth()
    {
        cout << "插卡" << endl;
    }

    void inputAmount()
    {
        cout << "atm 输入金额" << endl;
    }

    void enter()
    {
        cout << "按下确认按钮" << endl;
    }

    void exit()
    {
        cout << "拔卡" << endl;
    }
};

class Online : public Policy
{
public:
    void inputPassword()
    {
        cout << "人脸免密" << endl;
    }
    void auth()
    {
        cout << "键盘输入密码" << endl;
    }

    void inputAmount()
    {
        cout << "键盘输入金额" << endl;
    }

    void enter()
    {
        cout << "按下回车按钮" << endl;
    }

    void exit()
    {
        cout << "拔卡" << endl;
    }
};

int main()
{


    string pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";

    int i = 0;
    vector<string> res;

    map<string, string> formateItem;

    while (pattern.size() > 0)
    {
        if (pattern[i] == 'd')
        {
            string datetime;
            int j = i + 1;
            bool start = false;

            for (; j < pattern.size(); j++)
            {
                if (pattern[j] == '{')
                {
                    start = true;
                }
                else if (pattern[j] == '}')
                {
                    cout << j << endl;
                    break;
                }
                else if (start)
                {
                    datetime.append(1, pattern[j]);
                }
            }

            cout << "datetime: " << datetime << endl;
            cout << i - 1 << "," << j - i + 1 << endl;
            pattern = pattern.substr(j + 1);
            cout << "pattern: " << pattern << endl;
            formateItem["d"] = datetime;
        }
        else if (isalpha(pattern[i]))
        {
            formateItem["" + pattern[i]] = pattern[i];
        }
        else if (pattern[i] != '%')
        {
            formateItem["str"] = pattern[i];
        }
        else if (pattern[i] == '%' && i + 1 < pattern.size() && pattern[i] == '%')
        {
            formateItem["str"] = "%";
        }
    }

    return 0;
}