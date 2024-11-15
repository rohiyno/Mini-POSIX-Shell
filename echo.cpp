#include "headers.h"

// string getContentInQuotes(const string &input)
// {
//     int firstQuote = input.find('"');
//     int lastQuote = input.rfind('"');
//     if (firstQuote != string::npos && lastQuote != string::npos && firstQuote < lastQuote)
//     {
//         return input.substr(firstQuote, lastQuote - firstQuote + 1);
//     }
//     return "";
// }

// void echo(vector<string> args, string input)
// {
//     string contentInQuotes = getContentInQuotes(input);
//     if (!contentInQuotes.empty())
//     {
//         cout << contentInQuotes << endl;
//         return;
//     }
//     else
//     {
//         args.erase(args.begin());
//         for (string &arg : args)
//         {
//             cout << arg << ' ';
//         }
//         cout << endl;
//     }
// }

void echo(vector<string> args)
{
    for (int i = 1; i < args.size(); i++)
    {
        cout << args[i] << ' ';
    }
    cout << endl;
}